/*
 * schema.cpp
 *
 *  Created on: 13 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/io/Path.h>
#include <lsp-plug.in/stdlib/string.h>

UTEST_BEGIN("tk.style", schema)

    void test_load()
    {
        printf("Testing schema load...\n");

        io::Path path;
        tk::Atoms x;
        tk::Schema sc(&x);
        tk::Style *s;
        lsp::Color *c;
        char buf[32];
        union
        {
            ssize_t     iv;
            float       fv;
            bool        bv;
            const char *sv;
        } v;

        UTEST_ASSERT(path.fmt("%s/schema/parse.xml", resources()) > 0);
        UTEST_ASSERT(sc.parse_file(&path) == STATUS_OK);

        printf("Validating colors...\n");
        UTEST_ASSERT(c = sc.color("color1"));
        UTEST_ASSERT(c->format_rgb(buf, sizeof(buf), 2) > 0);
        UTEST_ASSERT(::strcmp(buf, "#1b1c22") == 0);

        UTEST_ASSERT(c = sc.color("red"));
        UTEST_ASSERT(c->format_hsl(buf, sizeof(buf), 1) > 0);
        UTEST_ASSERT(::strcmp(buf, "@0f8") == 0);

        UTEST_ASSERT(c = sc.color("rgb"));
        UTEST_ASSERT(c->format_rgb(buf, sizeof(buf), 1) > 0);
        UTEST_ASSERT(::strcmp(buf, "#0f0") == 0);

        UTEST_ASSERT(c = sc.color("rgba"));
        UTEST_ASSERT(c->format_rgba(buf, sizeof(buf), 2) > 0);
        UTEST_ASSERT(::strcmp(buf, "#0000ff88") == 0);

        UTEST_ASSERT(c = sc.color("hsl"));
        UTEST_ASSERT(c->format_hsl(buf, sizeof(buf), 2) > 0);
        UTEST_ASSERT(::strcmp(buf, "@112233") == 0);

        UTEST_ASSERT(c = sc.color("hsla"));
        UTEST_ASSERT(c->format_hsla(buf, sizeof(buf), 2) > 0);
        UTEST_ASSERT(::strcmp(buf, "@11223388") == 0);

        UTEST_ASSERT(c = sc.color("ared"));
        UTEST_ASSERT(c->format_hsla(buf, sizeof(buf), 1) > 0);
        UTEST_ASSERT(::strcmp(buf, "@0f84") == 0);

        printf("Validating styles...\n");
        UTEST_ASSERT(s = sc.root());
        UTEST_ASSERT(s->get_bool(x.atom_id("b.value"), &v.bv) == STATUS_OK);
        UTEST_ASSERT(v.bv == true);
        UTEST_ASSERT(s->get_int(x.atom_id("i.value"), &v.iv) == STATUS_OK);
        UTEST_ASSERT(v.iv == 123);
        UTEST_ASSERT(s->get_float(x.atom_id("f.value"), &v.fv) == STATUS_OK);
        UTEST_ASSERT(float_equals_absolute(v.fv, 123.45f, 5e-3f));
        UTEST_ASSERT(s->get_string(x.atom_id("s.value"), &v.sv) == STATUS_OK);
        UTEST_ASSERT(::strcmp(v.sv, "some text") == 0);

        UTEST_ASSERT(s->get_string(x.atom_id("s.b.value"), &v.sv) == STATUS_OK);
        UTEST_ASSERT(::strcmp(v.sv, "true") == 0);
        UTEST_ASSERT(s->get_string(x.atom_id("s.i.value"), &v.sv) == STATUS_OK);
        UTEST_ASSERT(::strcmp(v.sv, "123") == 0);
        UTEST_ASSERT(s->get_string(x.atom_id("s.f.value"), &v.sv) == STATUS_OK);
        UTEST_ASSERT(::strcmp(v.sv, "123.45") == 0);
        UTEST_ASSERT(s->get_float(x.atom_id("f.i.value"), &v.fv) == STATUS_OK);
        UTEST_ASSERT(float_equals_absolute(v.fv, 123.00f, 5e-3f));

        UTEST_ASSERT(s = sc.get("style1"));
        UTEST_ASSERT(s->get_int(x.atom_id("i.value"), &v.iv) == STATUS_OK);
        UTEST_ASSERT(v.iv == 456);
        UTEST_ASSERT(s->get_float(x.atom_id("f.value"), &v.fv) == STATUS_OK);
        UTEST_ASSERT(float_equals_absolute(v.fv, 123.45f, 5e-3f));
        UTEST_ASSERT(s->get_string(x.atom_id("s.value"), &v.sv) == STATUS_OK);
        UTEST_ASSERT(::strcmp(v.sv, "some text") == 0);

        UTEST_ASSERT(s = sc.get("style2"));
        UTEST_ASSERT(s->get_int(x.atom_id("i.value"), &v.iv) == STATUS_OK);
        UTEST_ASSERT(v.iv == 123);
        UTEST_ASSERT(s->get_float(x.atom_id("f.value"), &v.fv) == STATUS_OK);
        UTEST_ASSERT(float_equals_absolute(v.fv, 234.56f, 5e-3f));
        UTEST_ASSERT(s->get_string(x.atom_id("s.value"), &v.sv) == STATUS_OK);
        UTEST_ASSERT(::strcmp(v.sv, "some text") == 0);

        UTEST_ASSERT(s = sc.get("style3"));
        UTEST_ASSERT(s->get_int(x.atom_id("i.value"), &v.iv) == STATUS_OK);
        UTEST_ASSERT(v.iv == 123);
        UTEST_ASSERT(s->get_float(x.atom_id("f.value"), &v.fv) == STATUS_OK);
        UTEST_ASSERT(float_equals_absolute(v.fv, 123.45f, 5e-3f));
        UTEST_ASSERT(s->get_string(x.atom_id("s.value"), &v.sv) == STATUS_OK);
        UTEST_ASSERT(::strcmp(v.sv, "overridden text") == 0);

        UTEST_ASSERT(s = sc.get("compound"));
        UTEST_ASSERT(s->get_int(x.atom_id("i.value"), &v.iv) == STATUS_OK);
        UTEST_ASSERT(v.iv == 123);
        UTEST_ASSERT(s->get_float(x.atom_id("f.value"), &v.fv) == STATUS_OK);
        UTEST_ASSERT(float_equals_absolute(v.fv, 123.45f, 5e-3f));
        UTEST_ASSERT(s->get_string(x.atom_id("s.value"), &v.sv) == STATUS_OK);
        UTEST_ASSERT(::strcmp(v.sv, "overridden text") == 0);
        UTEST_ASSERT(s->get_int(x.atom_id("i.c.value"), &v.iv) == STATUS_OK);
        UTEST_ASSERT(v.iv == 456);
        UTEST_ASSERT(s->get_float(x.atom_id("f.c.value"), &v.fv) == STATUS_OK);
        UTEST_ASSERT(float_equals_absolute(v.fv, 234.56f, 5e-3f));
        UTEST_ASSERT(s->get_string(x.atom_id("s.c.value"), &v.sv) == STATUS_OK);
        UTEST_ASSERT(::strcmp(v.sv, "overridden text") == 0);

        UTEST_ASSERT(s = sc.get("overrides"));
        UTEST_ASSERT(s->get_int(x.atom_id("i.value"), &v.iv) == STATUS_OK);
        UTEST_ASSERT(v.iv == 567);
        UTEST_ASSERT(s->get_float(x.atom_id("f.value"), &v.fv) == STATUS_OK);
        UTEST_ASSERT(float_equals_absolute(v.fv, 345.67f, 5e-3f));
        UTEST_ASSERT(s->get_string(x.atom_id("s.value"), &v.sv) == STATUS_OK);
        UTEST_ASSERT(::strcmp(v.sv, "nested text") == 0);
    }

    UTEST_MAIN
    {
        test_load();
    }

UTEST_END


