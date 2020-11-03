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

    STYLE_INITIALIZER_DEF(TestBase,);
    STYLE_INITIALIZER_DEF(TestChild1, TestBase);
    STYLE_INITIALIZER_DEF(TestChild2, TestBase);
    STYLE_INITIALIZER_DEF(TestChild3, TestBase);

    STYLE_INITIALIZER_BEGIN(TestBase,)
        prop::Integer::init("int", style, 440);
        prop::Float::init("float", style, 3.14f);
        prop::Color::init("color", style, "#112233");
    STYLE_INITIALIZER_END(TestBase, "TestBase");

    STYLE_INITIALIZER_BEGIN(TestChild1, TestBase)
    STYLE_INITIALIZER_END(TestChild1, "TestChild1");

    STYLE_INITIALIZER_BEGIN(TestChild2, TestBase)
    STYLE_INITIALIZER_END(TestChild2, "TestChild2");

    STYLE_INITIALIZER_BEGIN(TestChild3, TestBase)
    STYLE_INITIALIZER_END(TestChild3, "TestChild3");

    static StyleInitializer *init_list[] =
    {
        &STYLE_INITIALIZER_INSTANCE(TestBase),
        &STYLE_INITIALIZER_INSTANCE(TestChild1),
        &STYLE_INITIALIZER_INSTANCE(TestChild2),
        &STYLE_INITIALIZER_INSTANCE(TestChild3)
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

            prop::Integer       sInt;
            prop::Float         sFloat;
            prop::Color         sColor;
            prop::Float         sScaling;

        public:
            StyleClient(const char *style, Schema *schema):
                sListener(this),
                sStyle(schema),
                sInt(&sListener),
                sFloat(&sListener),
                sColor(&sListener),
                sScaling(&sListener)
            {
                pSchema     = schema;
                pParent     = style;
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
            }

        public:
            LSP_TK_PROPERTY(Integer,        ivalue,         &sInt);
            LSP_TK_PROPERTY(Float,          fvalue,         &sFloat);
            LSP_TK_PROPERTY(tk::Color,      color,          &sColor);
            LSP_TK_PROPERTY(Float,          scaling,        &sScaling);
    };

    UTEST_MAIN
    {
        Atoms sAtoms;
        Schema sSchema(&sAtoms);
        StyleSheet sSheet;
        io::Path path;

        // Create default styles
        UTEST_ASSERT(sSchema.init(init_list, sizeof(init_list)/sizeof(StyleInitializer *)) == STATUS_OK);

        // Load style sheet and apply
        UTEST_ASSERT(path.fmt("%s/schema/nesting.xml", resources()) > 0);
        UTEST_ASSERT(sSheet.parse_file(&path) == STATUS_OK);
        UTEST_ASSERT(sSchema.apply(&sSheet) == STATUS_OK);

        // Bind clients
        StyleClient c1("TestChild1", &sSchema);
        StyleClient c2("TestChild2", &sSchema);
        StyleClient c3("TestChild3", &sSchema);

        UTEST_ASSERT(c1.init() == STATUS_OK);
        UTEST_ASSERT(c2.init() == STATUS_OK);
        UTEST_ASSERT(c3.init() == STATUS_OK);

        // Test state of clients
        UTEST_ASSERT(c1.ivalue()->get() == 440);
        UTEST_ASSERT(float_equals_adaptive(c1.fvalue()->get(), 3.14f));
        UTEST_ASSERT(c1.color()->rgb24() == 0x112233);
        UTEST_ASSERT(float_equals_adaptive(c1.scaling()->get(), 1.5f));

        UTEST_ASSERT(c2.ivalue()->get() == 48000);
        UTEST_ASSERT(float_equals_adaptive(c2.fvalue()->get(), 1.41f));
        UTEST_ASSERT(c2.color()->rgb24() == 0xccddee);
        UTEST_ASSERT(float_equals_adaptive(c2.scaling()->get(), 1.5f));

        UTEST_ASSERT(c3.ivalue()->get() == 44100);
        UTEST_ASSERT(float_equals_adaptive(c3.fvalue()->get(), 10.0f));
        UTEST_ASSERT(c3.color()->rgb24() == 0x5a5a5a);
        UTEST_ASSERT(float_equals_adaptive(c3.scaling()->get(), 1.5f));
    }

UTEST_END



