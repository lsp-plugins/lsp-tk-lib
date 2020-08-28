/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 22 авг. 2020 г.
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

#include <lsp-plug.in/test-fw/mtest.h>
#include <lsp-plug.in/tk/tk.h>
#include <private/mtest/tk/common.h>

MTEST_BEGIN("tk.widgets.graph", graph)
    typedef struct handler_t
    {
        test_type_t    *test;
        char           *label;
    } handler_t;

    static status_t slot_close(tk::Widget *sender, void *ptr, void *data)
    {
        sender->display()->quit_main();
        return STATUS_OK;
    }

    static status_t slot_key_up(tk::Widget *sender, void *ptr, void *data)
    {
        tk::Window *wnd = tk::widget_cast<tk::Window>(sender);
        ws::event_t *ev = static_cast<ws::event_t *>(data);
        test_type_t *_this = static_cast<test_type_t *>(ptr);

        if ((wnd != NULL) && (ev != NULL) && (ev->nType == ws::UIE_KEY_UP))
        {
            _this->printf("Key up: %c (0x%x)\n", (char)ev->nCode, int(ev->nCode));

            switch (ev->nCode)
            {
                case '+':
                case ws::WSK_KEYPAD_ADD:
                    wnd->style()->schema()->scaling()->add(0.25f);
                    break;

                case '-':
                case ws::WSK_KEYPAD_SUBTRACT:
                    wnd->style()->schema()->scaling()->sub(0.25f);
                    break;

                default:
                    break;
            }
        }
        return STATUS_OK;
    }

    static status_t slot_mouse_in(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("MOUSE_IN: %s\n", h->label);

        return STATUS_OK;
    }

    static status_t slot_mouse_out(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("MOUSE_OUT: %s\n", h->label);

        return STATUS_OK;
    }

    static status_t slot_mouse_move(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("MOUSE_MOVE: %s\n", h->label);

        return STATUS_OK;
    }

    static status_t slot_mouse_down(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("MOUSE_DOWN: %s\n", h->label);

        return STATUS_OK;
    }

    static status_t slot_mouse_up(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("MOUSE_UP: %s\n", h->label);

        return STATUS_OK;
    }

    static status_t slot_mouse_click(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("MOUSE_CLICK: %s\n", h->label);

        return STATUS_OK;
    }

    static status_t slot_mouse_dbl_click(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("MOUSE_DBL_CLICK: %s\n", h->label);

        return STATUS_OK;
    }

    static status_t slot_mouse_tri_click(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("MOUSE_TRI_CLICK: %s\n", h->label);

        return STATUS_OK;
    }

    status_t init_widget(tk::Widget *w, lltl::parray<handler_t> &vh, const char *label)
    {
        status_t res = w->init();
        if (res != STATUS_OK)
            return res;

        handler_t *h = new handler_t;
        if ((h == NULL) || (!vh.add(h)))
            return STATUS_NO_MEM;
        h->test     = this;
        h->label    = ::strdup(label);

//        tk::handler_id_t hid;
//        hid = w->slots()->bind(tk::SLOT_MOUSE_IN, slot_mouse_in, h);
//        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_DOWN, slot_mouse_down, h);
//        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_MOVE, slot_mouse_move, h);
//        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_UP, slot_mouse_up, h);
//        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_CLICK, slot_mouse_click, h);
//        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_DBL_CLICK, slot_mouse_dbl_click, h);
//        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_TRI_CLICK, slot_mouse_tri_click, h);
//        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_OUT, slot_mouse_out, h);
//
//        if (hid < 0)
//            res = -hid;

        return res;
    }

    static void destroy_handlers(lltl::parray<handler_t> &vh)
    {
        for (size_t i=0, n=vh.size(); i<n; ++i)
        {
            handler_t *h = vh.uget(i);
            ::free(h->label);
            delete h;
        }
    }

    typedef struct marker_t
    {
        float       value;
        uint32_t    color;
    } marker_t;

    MTEST_MAIN
    {
        lltl::parray<handler_t> vh;

        tk::Display *dpy = new tk::Display();
        MTEST_ASSERT(dpy != NULL);

        MTEST_ASSERT(dpy->init(0, NULL) == STATUS_OK);

        lltl::parray<tk::Widget> widgets;
        tk::Widget *w = NULL;
        tk::Window *wnd = new tk::Window(dpy);
        tk::Graph *gr = NULL;

        // Initialize window
        MTEST_ASSERT(init_widget(wnd, vh, "window") == STATUS_OK);
        MTEST_ASSERT(wnd->title()->set_raw("Test graph") == STATUS_OK);
        MTEST_ASSERT(wnd->role()->set_raw("graph_test") == STATUS_OK);
        wnd->bg_color()->set_rgb24(0x1b1c22);
        wnd->actions()->set_actions(ws::WA_MOVE | ws::WA_RESIZE | ws::WA_CLOSE);
        wnd->border_style()->set(ws::BS_DIALOG);
//        wnd->border_size()->set(2);
        wnd->border_color()->set_rgb(1.0f, 1.0f, 0.0f);
        wnd->size_constraints()->set(160, 100, 640, 400);
        wnd->size()->set(320, 200);
        wnd->slot(tk::SLOT_CLOSE)->bind(slot_close, this);
        wnd->slot(tk::SLOT_KEY_UP)->bind(slot_key_up, this);
        wnd->pointer()->set(ws::MP_TABLE_CELL);
        wnd->layout()->set(-0.5, 0.5, 0.5, 0.5);
//        wnd->padding()->set_all(16);
        MTEST_ASSERT(widgets.push(wnd));

        // Create Graph
        MTEST_ASSERT(gr = new tk::Graph(dpy));
        MTEST_ASSERT(init_widget(gr, vh, "graph") == STATUS_OK);
        MTEST_ASSERT(widgets.push(gr));
        MTEST_ASSERT(wnd->add(gr) == STATUS_OK);
        gr->constraints()->set_min(160, 100);
        gr->bg_color()->set_rgb24(0x1b1c22);

        {
            int wid = 0;
            size_t col = 0;
            LSPString id;

            // Create Origins
            tk::GraphOrigin *go;
            static const float coords[] = { -1, -1, 0, 0, -1, 1, 1, 1, 1, -1 };
            for (size_t i=0; i<5; ++i)
            {
                MTEST_ASSERT(go = new tk::GraphOrigin(dpy));
                MTEST_ASSERT(id.fmt_ascii("origin_%d", wid++));
                MTEST_ASSERT(init_widget(go, vh, id.get_ascii()) == STATUS_OK);
                MTEST_ASSERT(widgets.push(go));
                MTEST_ASSERT(gr->add(go) == STATUS_OK);
                go->left()->set(coords[i*2]);
                go->top()->set(coords[i*2+1]);
                go->color()->set_rgb24(next_color(col));
                go->radius()->set(4 + i*2);
                go->smooth()->set(true);
            }

            // Add markers
            tk::GraphMarker *gm;

            // Vertical markers
            static const marker_t vmarkers[] =
            {
                { 20.0f, 0x888800 },
                { 30.0f, 0x888800 },
                { 40.0f, 0x888800 },
                { 50.0f, 0x888800 },
                { 60.0f, 0x888800 },
                { 70.0f, 0x888800 },
                { 80.0f, 0x888800 },
                { 90.0f, 0x888800 },
                { 100.0f, 0x888888 },
                { 200.0f, 0x888800 },
                { 300.0f, 0x888800 },
                { 400.0f, 0x888800 },
                { 500.0f, 0x888800 },
                { 600.0f, 0x888800 },
                { 700.0f, 0x888800 },
                { 800.0f, 0x888800 },
                { 900.0f, 0x888800 },
                { 1000.0f, 0x888888 },
                { 2000.0f, 0x888800 },
                { 3000.0f, 0x888800 },
                { 4000.0f, 0x888800 },
                { 5000.0f, 0x888800 },
                { 6000.0f, 0x888800 },
                { 7000.0f, 0x888800 },
                { 8000.0f, 0x888800 },
                { 9000.0f, 0x888800 },
                { 10000.0f, 0x888888 },
                { 20000.0f, 0x888800 },
                { 24000.0f, 0xcccccc }
            };

            for (size_t i=0; i<sizeof(vmarkers)/sizeof(marker_t); ++i)
            {
                MTEST_ASSERT(gm = new tk::GraphMarker(dpy));
                MTEST_ASSERT(id.fmt_ascii("marker_%d", wid++));
                MTEST_ASSERT(init_widget(gm, vh, id.get_ascii()) == STATUS_OK);
                MTEST_ASSERT(widgets.push(gm));
                MTEST_ASSERT(gr->add(gm) == STATUS_OK);

                gm->origin()->set(0);
                gm->basis()->set(0);
                gm->parallel()->set(1);

                const marker_t *m = &vmarkers[i];
                gm->color()->set_rgb24(m->color);
                gm->value()->set(m->value);
            }

            // Horizontal markers
            static const marker_t hmarkers[] =
            {
                { 12,    0x888800 },
                { 24,    0x888888 },
                { 36,    0x888800 },
                { 48,    0x888888 },
                { 60,    0x888800 },
                { 72,    0x888888 },
                { 84,    0x888800 },
                { 96,    0xcccccc },
                { 108,   0x888800 },
                { 120,   0xcccccc }
            };

            for (size_t i=0; i<sizeof(hmarkers)/sizeof(marker_t); ++i)
            {
                MTEST_ASSERT(gm = new tk::GraphMarker(dpy));
                MTEST_ASSERT(id.fmt_ascii("marker_%d", wid++));
                MTEST_ASSERT(init_widget(gm, vh, id.get_ascii()) == STATUS_OK);
                MTEST_ASSERT(widgets.push(gm));
                MTEST_ASSERT(gr->add(gm) == STATUS_OK);

                gm->origin()->set(0);
                gm->basis()->set(1);
                gm->parallel()->set(0);

                const marker_t *m = &hmarkers[i];
                gm->color()->set_rgb24(m->color);
                gm->value()->set(m->value);
            }

            // Create Editable vertical marker
            MTEST_ASSERT(gm = new tk::GraphMarker(dpy));
            MTEST_ASSERT(id.fmt_ascii("marker_%d", wid++));
            MTEST_ASSERT(init_widget(gm, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(gm));
            MTEST_ASSERT(gr->add(gm) == STATUS_OK);

            gm->origin()->set(0);
            gm->basis()->set(0);
            gm->parallel()->set(1);
            gm->value()->set_all(1000, 10, 24000);
            gm->pointer()->set(ws::MP_HSIZE);
            gm->editable()->set(true);
            gm->left_border()->set(20);
            gm->right_border()->set(20);
            gm->hover_left_border()->set(32);
            gm->hover_right_border()->set(32);
            gm->color()->set_rgb24(0x00cc00);
            gm->width()->set(2);
            gm->hover_width()->set(3);
            gm->hover_color()->set_rgb24(0x00ff00);
            gm->border_left_color()->set_rgba32(0x44cc8800);
            gm->border_right_color()->set_rgba32(0x4488cc00);
            gm->hover_border_left_color()->set_rgba32(0x44ffcc00);
            gm->hover_border_right_color()->set_rgba32(0x44ccff00);

            // Create Editable horizontal marker
            MTEST_ASSERT(gm = new tk::GraphMarker(dpy));
            MTEST_ASSERT(id.fmt_ascii("marker_%d", wid++));
            MTEST_ASSERT(init_widget(gm, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(gm));
            MTEST_ASSERT(gr->add(gm) == STATUS_OK);

            gm->origin()->set(0);
            gm->basis()->set(1);
            gm->parallel()->set(0);
            gm->value()->set_all(60, 0, 120);
            gm->pointer()->set(ws::MP_VSIZE);
            gm->editable()->set(true);
            gm->left_border()->set(20);
            gm->right_border()->set(20);
            gm->hover_left_border()->set(32);
            gm->hover_right_border()->set(32);
            gm->color()->set_rgb24(0x0000cc);
            gm->width()->set(2);
            gm->hover_width()->set(3);
            gm->hover_color()->set_rgb24(0x0000ff);
            gm->border_left_color()->set_rgba32(0x440088cc);
            gm->border_right_color()->set_rgba32(0x4400cc88);
            gm->hover_border_left_color()->set_rgba32(0x4400ccff);
            gm->hover_border_right_color()->set_rgba32(0x4400ffcc);

            // Create axes
            tk::GraphAxis *ga;

            // Horizontal
            MTEST_ASSERT(ga = new tk::GraphAxis(dpy));
            MTEST_ASSERT(id.fmt_ascii("axis_%d", wid++));
            MTEST_ASSERT(init_widget(ga, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(ga));
            MTEST_ASSERT(gr->add(ga) == STATUS_OK);
            ga->min()->set(10);
            ga->max()->set(24000);
            ga->log_scale()->set(true);
            ga->direction()->set_dangle(0);
            ga->origin()->set(0);
            ga->color()->set_rgb24(0xcccccc);

            // Vertical
            MTEST_ASSERT(ga = new tk::GraphAxis(dpy));
            MTEST_ASSERT(id.fmt_ascii("axis_%d", wid++));
            MTEST_ASSERT(init_widget(ga, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(ga));
            MTEST_ASSERT(gr->add(ga) == STATUS_OK);
            ga->min()->set(0); // -96 dB
            ga->max()->set(120); // +24 dB
            ga->log_scale()->set(false);
            ga->direction()->set_dangle(90);
            ga->origin()->set(0);
            ga->color()->set_rgb24(0xcccccc);
        }

        // Show window
        wnd->visibility()->set(true);

        MTEST_ASSERT(dpy->main() == STATUS_OK);

        while ((w = widgets.pop()) != NULL)
        {
            w->destroy();
            delete w;
        }

        dpy->destroy();
        delete dpy;
        destroy_handlers(vh);
    }

MTEST_END
