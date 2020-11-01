/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 1 нояб. 2020 г.
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

#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/io/Path.h>
#include <lsp-plug.in/stdlib/string.h>

UTEST_BEGIN("tk.style", stylesheet)

    bool check_list(lltl::parray<LSPString> *items, const char * const * list)
    {
        size_t n = 0;
        for (const char * const *l=list; *l != NULL; ++l)
            ++n;
        if (items->size() != n)
            return false;

        for (const char * const *l=list; *l != NULL; ++l)
        {
            bool found = false;
            for (size_t j=0, m=items->size(); j<m; ++j)
            {
                LSPString *s = items->uget(j);
                if (s->equals_utf8(*l))
                {
                    found = true;
                    break;
                }
            }

            if (!found)
                return false;
        }

        return true;
    }

    void check_properties(tk::StyleSheet *ss, const char *style, const char * const * list)
    {
        lltl::parray<LSPString> pl;
        printf("Checking properties for style '%s'...\n", style);

        UTEST_ASSERT(ss->enum_properties(style, &pl) == STATUS_OK);

        size_t n = 0;
        for (const char * const *l=list; *l != NULL; l += 2)
            ++n;
        UTEST_ASSERT(pl.size() == n);

        for (const char * const *l=list; *l != NULL; )
        {
            const char *name = *(l++);
            const char *value = *(l++);
            printf("  checking property %s=%s\n", name, value);

            bool found = false;
            for (size_t j=0, m=pl.size(); j<m; ++j)
            {
                LSPString *s = pl.uget(j);
                if (s->equals_utf8(name))
                {
                    found = true;
                    break;
                }
            }

            UTEST_ASSERT(found);
            LSPString v;
            UTEST_ASSERT(ss->get_property(style, name, &v) == STATUS_OK);
            UTEST_ASSERT(v.equals_utf8(value));
        }
    }

    void test_load()
    {
        printf("Testing load of style sheet...\n");
        io::Path path;
        tk::StyleSheet ss;
        lltl::parray<LSPString> vs;
        lsp::Color c;
        const char *root = NULL;
        char buf[32];

        UTEST_ASSERT(path.fmt("%s/schema/parse.xml", resources()) > 0);
        UTEST_ASSERT(ss.parse_file(&path) == STATUS_OK);

        // Check colors
        vs.clear();
        UTEST_ASSERT(ss.enum_colors(&vs) == STATUS_OK);
        static const char *colors[] = { "color1", "red", "rgb", "rgba", "hsl", "hsla", "ared", NULL };
        UTEST_ASSERT(check_list(&vs, colors));

        UTEST_ASSERT(ss.get_color("color1", &c) == STATUS_OK);
        UTEST_ASSERT(c.format_rgb(buf, sizeof(buf), 2) > 0);
        UTEST_ASSERT(::strcmp(buf, "#1b1c22") == 0);

        UTEST_ASSERT(ss.get_color("red", &c) == STATUS_OK);
        UTEST_ASSERT(c.format_hsl(buf, sizeof(buf), 1) > 0);
        UTEST_ASSERT(::strcmp(buf, "@0f8") == 0);

        UTEST_ASSERT(ss.get_color("rgb", &c) == STATUS_OK);
        UTEST_ASSERT(c.format_rgb(buf, sizeof(buf), 1) > 0);
        UTEST_ASSERT(::strcmp(buf, "#0f0") == 0);

        UTEST_ASSERT(ss.get_color("rgba", &c) == STATUS_OK);
        UTEST_ASSERT(c.format_rgba(buf, sizeof(buf), 2) > 0);
        UTEST_ASSERT(::strcmp(buf, "#0000ff88") == 0);

        UTEST_ASSERT(ss.get_color("hsl", &c) == STATUS_OK);
        UTEST_ASSERT(c.format_hsl(buf, sizeof(buf), 2) > 0);
        UTEST_ASSERT(::strcmp(buf, "@112233") == 0);

        UTEST_ASSERT(ss.get_color("hsla", &c) == STATUS_OK);
        UTEST_ASSERT(c.format_hsla(buf, sizeof(buf), 2) > 0);
        UTEST_ASSERT(::strcmp(buf, "@11223388") == 0);

        UTEST_ASSERT(ss.get_color("ared", &c) == STATUS_OK);
        UTEST_ASSERT(c.format_hsla(buf, sizeof(buf), 1) > 0);
        UTEST_ASSERT(::strcmp(buf, "@0f84") == 0);

        // Check style list
        vs.clear();
        UTEST_ASSERT(ss.enum_styles(&vs) == STATUS_OK);
        static const char *styles[] = { "style1", "style2", "style3", "compound", "overrides", NULL };
        UTEST_ASSERT(check_list(&vs, styles));

        // Check parents
        vs.clear();
        UTEST_ASSERT(ss.enum_parents(root, &vs) == STATUS_OK);
        static const char *root_parents[] = { NULL };
        UTEST_ASSERT(check_list(&vs, root_parents));

        vs.clear();
        UTEST_ASSERT(ss.enum_parents("style1", &vs) == STATUS_OK);
        static const char *style1_parents[] = { "root", NULL };
        UTEST_ASSERT(check_list(&vs, style1_parents));

        vs.clear();
        UTEST_ASSERT(ss.enum_parents("style2", &vs) == STATUS_OK);
        static const char *style2_parents[] = { "root", NULL };
        UTEST_ASSERT(check_list(&vs, style2_parents));

        vs.clear();
        UTEST_ASSERT(ss.enum_parents("style3", &vs) == STATUS_OK);
        static const char *style3_parents[] = { "root", NULL };
        UTEST_ASSERT(check_list(&vs, style3_parents));

        vs.clear();
        UTEST_ASSERT(ss.enum_parents("compound", &vs) == STATUS_OK);
        static const char *compound_parents[] = { "style1", "style2", "style3", NULL };
        UTEST_ASSERT(check_list(&vs, compound_parents));

        vs.clear();
        UTEST_ASSERT(ss.enum_parents("overrides", &vs) == STATUS_OK);
        static const char *overrides_parents[] = { "compound", NULL };
        UTEST_ASSERT(check_list(&vs, overrides_parents));

        // Check properties
        static const char *root_properties[] =
        {
            "b.value", "true",
            "i.value", "123",
            "f.value", "123.45",
            "s.value", "some text",
            "s.b.value", "true",
            "s.i.value", "123",
            "s.f.value", "123.45",
            "f.i.value", "123",
            NULL
        };
        check_properties(&ss, root, root_properties);

        static const char *style1_properties[] =
        {
            "i.value", "456",
            "i.c.value", "456",
            NULL
        };
        check_properties(&ss, "style1", style1_properties);

        static const char *style2_properties[] =
        {
            "f.value", "234.56",
            "f.c.value", "234.56",
            NULL
        };
        check_properties(&ss, "style2", style2_properties);

        static const char *style3_properties[] =
        {
            "s.value", "overridden text",
            "s.c.value", "overridden text",
            NULL
        };
        check_properties(&ss, "style3", style3_properties);

        static const char *compound_properties[] =
        {
            NULL
        };
        check_properties(&ss, "compound", compound_properties);

        static const char *overrides_properties[] =
        {
            "i.value", "567",
            "f.value", "345.67",
            "s.value", "nested text",
            NULL
        };
        check_properties(&ss, "overrides", overrides_properties);

    }

    void test_loop()
    {
        printf("Testing different inheritance loops...\n");

        io::Path path;

        {
            tk::StyleSheet ss;
            UTEST_ASSERT(path.fmt("%s/schema/loop1.xml", resources()) > 0);
            UTEST_ASSERT(ss.parse_file(&path) == STATUS_BAD_HIERARCHY);
        }

        {
            tk::StyleSheet ss;
            UTEST_ASSERT(path.fmt("%s/schema/loop2.xml", resources()) > 0);
            UTEST_ASSERT(ss.parse_file(&path) == STATUS_BAD_HIERARCHY);
        }

        {
            tk::StyleSheet ss;
            UTEST_ASSERT(path.fmt("%s/schema/loop3.xml", resources()) > 0);
            UTEST_ASSERT(ss.parse_file(&path) == STATUS_BAD_HIERARCHY);
        }

        {
            tk::StyleSheet ss;
            UTEST_ASSERT(path.fmt("%s/schema/loop4.xml", resources()) > 0);
            UTEST_ASSERT(ss.parse_file(&path) == STATUS_OK);
        }
    }

    UTEST_MAIN
    {
        test_load();
        test_loop();
    }

UTEST_END
