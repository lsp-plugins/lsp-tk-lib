/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 4 нояб. 2020 г.
 *
 * lsp-tk-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-tk-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-tk-lib. If not, see <https://www.gnu.org/licenses/>.
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/stdlib/math.h>

using namespace lsp;
using namespace lsp::tk;

namespace
{
    using namespace lsp::tk;

    // TestBase style
    LSP_TK_STYLE_DEF_BEGIN(TestBase, Style)
        prop::Integer       sInt;
        prop::Float         sFloat;
        prop::Color         sColor;
    LSP_TK_STYLE_DEF_END

    LSP_TK_STYLE_IMPL_BEGIN(TestBase, Style)
        // Bind
        sInt.bind("int", this);
        sFloat.bind("float", this);
        sColor.bind("color", this);

        // Init
        sInt.set(440);
        sFloat.set(3.14f);
        sColor.set("#112233");
    LSP_TK_STYLE_IMPL_END

    // TestChild nested style
    LSP_TK_STYLE_DEF_BEGIN(TestChild, TestBase)
    LSP_TK_STYLE_DEF_END

    LSP_TK_STYLE_IMPL_BEGIN(TestChild, TestBase)
    LSP_TK_STYLE_IMPL_END

    StyleFactory<TestBase>  TestBaseFactory("TestBase");
    StyleFactory<TestChild> TestBaseChild1("TestChild1");
    StyleFactory<TestChild> TestBaseChild2("TestChild2");
    StyleFactory<TestChild> TestBaseChild3("TestChild3");

    static IStyleFactory *init_list[] =
    {
        &TestBaseFactory,
        &TestBaseChild1,
        &TestBaseChild2,
        &TestBaseChild3
    };
}

UTEST_BEGIN("tk.style", nesting)
    class StyleClient
    {
        protected:
            class Listener: public prop::Listener
            {
                private:
                    StyleClient *pClient;

                public:
                    explicit Listener(StyleClient *ptr) : pClient(ptr) {}

                public:
                    virtual void notify(Property *prop)
                    {
                        if (pClient != NULL)
                            pClient->property_changed(prop);
                    }
            };

        protected:
            Listener            sListener;
            Schema             *pSchema;
            Style               sStyle;
            const char         *pParent;
            test_type_t        *pTest;

            prop::Integer       sInt;
            prop::Float         sFloat;
            prop::Color         sColor;
            prop::Float         sScaling;

        public:
            StyleClient(test_type_t *test, const char *style, Schema *schema):
                sListener(this),
                sStyle(schema),
                sInt(&sListener),
                sFloat(&sListener),
                sColor(&sListener),
                sScaling(&sListener)
            {
                pSchema     = schema;
                pParent     = style;
                pTest       = test;
            }

            virtual ~StyleClient()
            {
            }

        public:
            status_t init()
            {
                status_t res = sStyle.init();
                if (res != STATUS_OK)
                    return res;

                sInt.bind("int", &sStyle);
                sFloat.bind("float", &sStyle);
                sColor.bind("color", &sStyle);
                sScaling.bind("size.scaling", &sStyle);

                // Bind parent style class
                Style *parent = pSchema->get(pParent);
                if (parent == NULL)
                    return STATUS_CORRUPTED;
                return sStyle.add_parent(parent);
            }

            virtual void property_changed(Property *prop)
            {
                if (sInt.is(prop))
                    pTest->printf("  %s.int -> %d\n", pParent, int(sInt.get()));
                if (sFloat.is(prop))
                    pTest->printf("  %s.float -> %f\n", pParent, sFloat.get());
                if (sColor.is(prop))
                    pTest->printf("  %s.color -> 0x%x\n", pParent, int(sColor.rgba32()));
                if (sScaling.is(prop))
                    pTest->printf("  %s.scaling -> %f\n", pParent, sScaling.get());
            }

        public:
            LSP_TK_PROPERTY(Integer,        ivalue,         &sInt);
            LSP_TK_PROPERTY(Float,          fvalue,         &sFloat);
            LSP_TK_PROPERTY(tk::Color,      color,          &sColor);
            LSP_TK_PROPERTY(Float,          scaling,        &sScaling);
    };

    static bool part_cmp(int a, int b)
    {
        a -= b;
        return (a >= -1) && (a <= 1);
    }

    static bool rgba32_cmp(uint32_t c1, uint32_t c2)
    {
        return
            part_cmp(c1 & 0xff, c2 & 0xff) &&
            part_cmp((c1 >> 8) & 0xff, (c2 >> 8) & 0xff) &&
            part_cmp((c1 >> 16) & 0xff, (c2 >> 16) & 0xff) &&
            part_cmp((c1 >> 24) & 0xff, (c2 >> 24) & 0xff);
    }

    UTEST_MAIN
    {
        Atoms sAtoms;
        Schema sSchema(&sAtoms, NULL);
        StyleSheet sSheet;
        io::Path path;

        // Create default styles
        UTEST_ASSERT(sSchema.init(init_list, sizeof(init_list)/sizeof(StyleInitializer *)) == STATUS_OK);

        // Load style sheet and apply
        UTEST_ASSERT(path.fmt("%s/schema/nesting.xml", resources()) > 0);
        UTEST_ASSERT(sSheet.parse_file(&path) == STATUS_OK);
        UTEST_ASSERT(sSchema.apply(&sSheet) == STATUS_OK);

        // Bind clients
        StyleClient c1(this, "TestChild1", &sSchema);
        StyleClient c2(this, "TestChild2", &sSchema);
        StyleClient c3(this, "TestChild3", &sSchema);

        UTEST_ASSERT(c1.init() == STATUS_OK);
        UTEST_ASSERT(c2.init() == STATUS_OK);
        UTEST_ASSERT(c3.init() == STATUS_OK);

        // Test state of clients
        printf("c1.ivalue = %d\n", int(c1.ivalue()->get()));
        UTEST_ASSERT(c1.ivalue()->get() == 440);
        printf("c1.fvalue = %f\n", c1.fvalue()->get());
        UTEST_ASSERT(float_equals_adaptive(c1.fvalue()->get(), 3.14f));
        printf("c1.rgb24 = 0x%x\n", c1.color()->rgb24());
        UTEST_ASSERT(rgba32_cmp(c1.color()->rgb24(), 0x112233));
        printf("c1.scaling = %f\n", c1.scaling()->get());
        UTEST_ASSERT(float_equals_adaptive(c1.scaling()->get(), 1.5f));

        printf("c2.ivalue = %d\n", int(c2.ivalue()->get()));
        UTEST_ASSERT(c2.ivalue()->get() == 48000);
        printf("c2.fvalue = %f\n", c2.fvalue()->get());
        UTEST_ASSERT(float_equals_adaptive(c2.fvalue()->get(), 1.41f));
        printf("c2.rgb24 = 0x%x\n", c2.color()->rgb24());
        UTEST_ASSERT(rgba32_cmp(c2.color()->rgb24(), 0xccddee));
        printf("c2.scaling = %f\n", c2.scaling()->get());
        UTEST_ASSERT(float_equals_adaptive(c2.scaling()->get(), 1.5f));

        printf("c3.ivalue = %d\n", int(c3.ivalue()->get()));
        UTEST_ASSERT(c3.ivalue()->get() == 44100);
        printf("c3.fvalue = %f\n", c3.fvalue()->get());
        UTEST_ASSERT(float_equals_adaptive(c3.fvalue()->get(), 10.0f));
        printf("c3.rgb24 = 0x%x\n", c3.color()->rgb24());
        UTEST_ASSERT(rgba32_cmp(c3.color()->rgb24(), 0x5a5a5a));
        printf("c3.scaling = %f\n", c3.scaling()->get());
        UTEST_ASSERT(float_equals_adaptive(c3.scaling()->get(), 1.5f));
    }

UTEST_END



