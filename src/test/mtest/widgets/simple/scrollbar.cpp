/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 05 июн. 2020 г.
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

MTEST_BEGIN("tk.widgets.simple", scrollbar)
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

        ws::event_t *ev = static_cast<ws::event_t *>(data);
        if (ev->nCode == ws::MCB_MIDDLE)
        {
            tk::ScrollBar *sb = tk::widget_cast<tk::ScrollBar>(sender);
            if (sb != NULL)
                sb->active()->toggle();
        }

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

    static status_t slot_begin_edit(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("BEGIN_EDIT: %s\n", h->label);

        return STATUS_OK;
    }

    static status_t slot_end_edit(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("END_EDIT: %s\n", h->label);

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
        if (w->slots()->contains(tk::SLOT_BEGIN_EDIT))
        {
            if (hid >= 0) hid = w->slots()->bind(tk::SLOT_BEGIN_EDIT, slot_begin_edit, h);
            if (hid >= 0) hid = w->slots()->bind(tk::SLOT_END_EDIT, slot_end_edit, h);
        }


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
        tk::Grid *grid = NULL;
        tk::ScrollBar *sb = NULL;

        // Initialize window
        MTEST_ASSERT(init_widget(wnd, vh, "window") == STATUS_OK);
        MTEST_ASSERT(wnd->title()->set_raw("Test scrollbar") == STATUS_OK);
        MTEST_ASSERT(wnd->role()->set_raw("scrollbar_test") == STATUS_OK);
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

        // Create Grid
        MTEST_ASSERT(grid = new tk::Grid(dpy));
        MTEST_ASSERT(init_widget(grid, vh, "grid") == STATUS_OK);
        MTEST_ASSERT(widgets.push(grid));
        MTEST_ASSERT(wnd->add(grid) == STATUS_OK);
        grid->bg_color()->set_rgb(1.0f, 1.0f, 1.0f);
        grid->padding()->set(8);
        grid->rows()->set(3);
        grid->columns()->set(8);
        grid->orientation()->set_horizontal();
        grid->hspacing()->set(2);
        grid->vspacing()->set(2);

        {
            // Create alignment and child widget
            LSPString id;

            // Create horizontal scrollbar
            MTEST_ASSERT(id.fmt_ascii("hscroll-0"));
            MTEST_ASSERT(sb = new tk::ScrollBar(dpy));
            MTEST_ASSERT(init_widget(sb, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(sb));
            MTEST_ASSERT(grid->add(sb, 1, 8) == STATUS_OK);

            sb->orientation()->set_horizontal();
            sb->value()->set(0.5f);

            // Create vertical faders
            for (ssize_t x=0; x<8; ++x)
            {
                // Create alignment widget
                MTEST_ASSERT(id.fmt_ascii("vscroll-%d-0", x));
                MTEST_ASSERT(sb = new tk::ScrollBar(dpy));
                MTEST_ASSERT(init_widget(sb, vh, id.get_ascii()) == STATUS_OK);
                MTEST_ASSERT(widgets.push(sb));
                MTEST_ASSERT(grid->add(sb) == STATUS_OK);

                sb->orientation()->set_vertical();
            }

            // Create horizontal fader
            MTEST_ASSERT(id.fmt_ascii("hscroll-1"));
            MTEST_ASSERT(sb = new tk::ScrollBar(dpy));
            MTEST_ASSERT(init_widget(sb, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(sb));
            MTEST_ASSERT(grid->add(sb, 1, 8) == STATUS_OK);

            sb->orientation()->set_horizontal();
            sb->value()->set(0.5f);
            sb->step()->set(0.005f);
            sb->padding()->set(2);
            sb->bg_color()->set_rgb24(0x1b1c22);
            sb->border_color()->set_rgb24(0xdae0ff);
            sb->border_gap_color()->set_rgb24(0x1b1c22);
            sb->inc_color()->set_rgb24(0x1b1c22);
            sb->dec_color()->set_rgb24(0x1b1c22);
            sb->inc_active_color()->set_rgb24(0x00c0ff);
            sb->dec_active_color()->set_rgb24(0x00c0ff);
            sb->slider_border_color()->set_rgb24(0xdae0ff);
            sb->slider_color()->set_rgb24(0xdae0ff);
            sb->slider_active_color()->set_rgb24(0x00c0ff);
            sb->button_color()->set_rgb24(0xdae0ff);
            sb->button_active_color()->set_rgb24(0x00c0ff);
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
