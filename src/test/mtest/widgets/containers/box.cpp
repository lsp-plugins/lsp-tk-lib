/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 18 мая 2020 г.
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

MTEST_BEGIN("tk.widgets.containers", box)
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

            if ((ev->nCode == '+') || (ev->nCode == ws::WSK_KEYPAD_ADD))
                wnd->style()->schema()->scaling()->add(0.25f);
            else if ((ev->nCode == '-') || (ev->nCode == ws::WSK_KEYPAD_SUBTRACT))
                wnd->style()->schema()->scaling()->sub(0.25f);
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

        tk::handler_id_t hid;
        hid = w->slots()->bind(tk::SLOT_MOUSE_IN, slot_mouse_in, h);
        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_DOWN, slot_mouse_down, h);
        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_MOVE, slot_mouse_move, h);
        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_UP, slot_mouse_up, h);
        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_CLICK, slot_mouse_click, h);
        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_DBL_CLICK, slot_mouse_dbl_click, h);
        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_TRI_CLICK, slot_mouse_tri_click, h);
        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_OUT, slot_mouse_out, h);

        if (hid < 0)
            res = -hid;

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

    MTEST_MAIN
    {
        lltl::parray<handler_t> vh;

        tk::Display *dpy = new tk::Display();
        MTEST_ASSERT(dpy != NULL);

        MTEST_ASSERT(dpy->init(0, NULL) == STATUS_OK);

        lltl::parray<tk::Widget> widgets;
        tk::Widget *w = NULL;
        tk::Window *wnd = new tk::Window(dpy);
        tk::Void *wv = NULL;
        tk::Box *box = NULL;
        tk::WidgetContainer *parent = NULL;

        // Initialize window
        MTEST_ASSERT(init_widget(wnd, vh, "window") == STATUS_OK);
        MTEST_ASSERT(wnd->title()->set_raw("Test box") == STATUS_OK);
        MTEST_ASSERT(wnd->role()->set_raw("box_test") == STATUS_OK);
        wnd->bg_color()->set_rgb(0, 0.75, 1.0);
        wnd->actions()->set_actions(ws::WA_MOVE | ws::WA_RESIZE | ws::WA_CLOSE);
        wnd->border_style()->set(ws::BS_DIALOG);
//        wnd->border_size()->set(2);
        wnd->border_color()->set_rgb(1.0f, 1.0f, 0.0f);
        wnd->constraints()->set(160, 100, 640, 400);
        wnd->size()->set(320, 200);
        wnd->slot(tk::SLOT_CLOSE)->bind(slot_close, this);
        wnd->slot(tk::SLOT_KEY_UP)->bind(slot_key_up, this);
        wnd->pointer()->set(ws::MP_TABLE_CELL);
        wnd->layout()->set(-0.5, 0.5, 0.5, 0.5);
//        wnd->padding()->set_all(16);
        MTEST_ASSERT(widgets.push(wnd));

        // Create vertical box
        MTEST_ASSERT(box = new tk::Box(dpy));
        MTEST_ASSERT(init_widget(box, vh, "vbox") == STATUS_OK);
        MTEST_ASSERT(widgets.push(box));
        MTEST_ASSERT(wnd->add(box) == STATUS_OK);
        box->bg_color()->set_rgb(1.0f, 1.0f, 1.0f);
        box->orientation()->set_vertical();
        box->spacing()->set(16);
        box->padding()->set_all(8);
        box->pointer()->set(ws::MP_IBEAM);
        parent = box;

        // Create horizontal box (1)
        MTEST_ASSERT(box = new tk::Box(dpy));
        MTEST_ASSERT(init_widget(box, vh, "hbox1") == STATUS_OK);
        MTEST_ASSERT(widgets.push(box));
        MTEST_ASSERT(parent->add(box) == STATUS_OK);
        box->padding()->set_all(1);
        box->bg_color()->set_rgb(1.0, 0.75, 1.0);
        box->pointer()->set(ws::MP_DEFAULT);
//        box->bg_color()->set_rgb(1.0, 1.0, 1.0);
        box->spacing()->set(8);
        box->orientation()->set_horizontal();
        {
            // Create widget (1)
            MTEST_ASSERT(wv = new tk::Void(dpy));
            MTEST_ASSERT(init_widget(wv, vh, "void1-1") == STATUS_OK);
            MTEST_ASSERT(widgets.push(wv));
            MTEST_ASSERT(box->add(wv) == STATUS_OK);
            wv->constraints()->set(32, 32, -1, -1);
            wv->bg_color()->set_rgb(1.0f, 0.0f, 0.0f);
            wv->allocation()->set_expand();
            wv->pointer()->set(ws::MP_HAND);

            // Create widget (2)
            MTEST_ASSERT(wv = new tk::Void(dpy));
            MTEST_ASSERT(init_widget(wv, vh, "void1-2") == STATUS_OK);
            MTEST_ASSERT(widgets.push(wv));
            MTEST_ASSERT(box->add(wv) == STATUS_OK);
            wv->constraints()->set(32, 32, 32, 32);
            wv->bg_color()->set_rgb(1.0f, 1.0f, 0.0f);
            wv->allocation()->set_fill();

            // Create widget (3)
            MTEST_ASSERT(wv = new tk::Void(dpy));
            MTEST_ASSERT(init_widget(wv, vh, "void1-3") == STATUS_OK);
            MTEST_ASSERT(widgets.push(wv));
            MTEST_ASSERT(box->add(wv) == STATUS_OK);
            wv->constraints()->set(32, 32, 64, 64);
            wv->bg_color()->set_rgb(0.0f, 1.0f, 0.0f);
            wv->allocation()->set_expand();
        }

        // Create horizontal box (2)
        MTEST_ASSERT(box = new tk::Box(dpy));
        MTEST_ASSERT(init_widget(box, vh, "hbox2") == STATUS_OK);
        MTEST_ASSERT(widgets.push(box));
        MTEST_ASSERT(parent->add(box) == STATUS_OK);
//        box->bg_color()->set_rgb(0, 0.75, 1.0);
        box->bg_color()->set_rgb(1.0, 0.75, 1.0);
        box->orientation()->set_horizontal();
        box->homogeneous()->set(true);
        box->spacing()->set(8);
        {
            // Create widget (1)
            MTEST_ASSERT(wv = new tk::Void(dpy));
            MTEST_ASSERT(init_widget(wv, vh, "void2-1") == STATUS_OK);
            MTEST_ASSERT(widgets.push(wv));
            MTEST_ASSERT(box->add(wv) == STATUS_OK);
            wv->constraints()->set(32, 32, -1, -1);
            wv->bg_color()->set_rgb(1.0f, 1.0f, 0.0f);
            wv->allocation()->set_fill();
            wv->padding()->set_all(8);

            // Create widget (2)
            MTEST_ASSERT(wv = new tk::Void(dpy));
            MTEST_ASSERT(init_widget(wv, vh, "void2-2") == STATUS_OK);
            MTEST_ASSERT(widgets.push(wv));
            MTEST_ASSERT(box->add(wv) == STATUS_OK);
            wv->constraints()->set(48, 32, 48, 32);
            wv->bg_color()->set_rgb(0.0f, 1.0f, 1.0f);
            wv->padding()->set_all(0);
            wv->allocation()->set_fill(false);

            // Create widget (3)
            MTEST_ASSERT(wv = new tk::Void(dpy));
            MTEST_ASSERT(init_widget(wv, vh, "void2-3") == STATUS_OK);
            MTEST_ASSERT(widgets.push(wv));
            MTEST_ASSERT(box->add(wv) == STATUS_OK);
            wv->constraints()->set(32, 32, 64, 64);
            wv->bg_color()->set_rgb(0.0f, 0.0f, 1.0f);
            wv->allocation()->set_fill();
            wv->padding()->set_all(8);
        }

        // Create horizontal box (3)
        MTEST_ASSERT(box = new tk::Box(dpy));
        MTEST_ASSERT(init_widget(box, vh, "hbox3") == STATUS_OK);
        MTEST_ASSERT(widgets.push(box));
        MTEST_ASSERT(parent->add(box) == STATUS_OK);
        box->bg_color()->set_rgb(0, 0.75, 0.5);
        box->orientation()->set_horizontal();
        box->border_color()->set_rgb24(0xcccccc);
        box->border()->set(4);
        {
            // Create widget (1)
            MTEST_ASSERT(wv = new tk::Void(dpy));
            MTEST_ASSERT(init_widget(wv, vh, "void3-1") == STATUS_OK);
            MTEST_ASSERT(widgets.push(wv));
            MTEST_ASSERT(box->add(wv) == STATUS_OK);
            wv->constraints()->set(32, 32, -1, -1);
            wv->bg_color()->set_rgb(1.0f, 0.5f, 0.0f);
            wv->allocation()->set_fill(false);

            // Create widget (2)
            MTEST_ASSERT(wv = new tk::Void(dpy));
            MTEST_ASSERT(init_widget(wv, vh, "void3-2") == STATUS_OK);
            MTEST_ASSERT(widgets.push(wv));
            MTEST_ASSERT(box->add(wv) == STATUS_OK);
            wv->constraints()->set(32, 32, 32, 32);
            wv->bg_color()->set_rgb(0.0f, 1.0f, 0.0f);
            wv->allocation()->set_fill(false);

            // Create widget (3)
            MTEST_ASSERT(wv = new tk::Void(dpy));
            MTEST_ASSERT(init_widget(wv, vh, "void3-3") == STATUS_OK);
            MTEST_ASSERT(widgets.push(wv));
            MTEST_ASSERT(box->add(wv) == STATUS_OK);
            wv->constraints()->set(32, 32, 64, 64);
            wv->bg_color()->set_rgb(0.0f, 1.0f, 1.0f);
            wv->allocation()->set_fill(false);
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



