/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 2 февр. 2026 г.
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

MTEST_BEGIN("tk.widgets.simple", image)
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

    static status_t slot_submit(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("SUBMIT: %s\n", h->label);

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

        tk::Image * const img = tk::widget_cast<tk::Image>(w);
        if (img != NULL)
        {
            if (hid >= 0) hid = w->slots()->bind(tk::SLOT_SUBMIT, slot_submit, h);
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

        io::Path landscape, portrait;
        MTEST_ASSERT(landscape.fmt("%s/img/landscape-icon.xpm", resources()) > 0);
        MTEST_ASSERT(portrait.fmt("%s/img/portrait-icon.xpm", resources()) > 0);

        tk::Display *dpy = new tk::Display();
        MTEST_ASSERT(dpy != NULL);

        MTEST_ASSERT(dpy->init(0, NULL) == STATUS_OK);

        lltl::parray<tk::Widget> widgets;
        tk::Widget *w = NULL;
        tk::Window *wnd = new tk::Window(dpy);
        tk::Grid *grid = NULL;
        tk::Image *img = NULL;

        // Initialize window
        MTEST_ASSERT(init_widget(wnd, vh, "window") == STATUS_OK);
        MTEST_ASSERT(wnd->title()->set_raw("Test image") == STATUS_OK);
        MTEST_ASSERT(wnd->role()->set_raw("image_test") == STATUS_OK);
        wnd->bg_color()->set_rgb(0, 0.75, 1.0);
        wnd->actions()->set_actions(ws::WA_MOVE | ws::WA_RESIZE | ws::WA_CLOSE);
        wnd->border_style()->set(ws::BS_DIALOG);
        wnd->border_color()->set_rgb(1.0f, 1.0f, 0.0f);
        wnd->constraints()->set(160, 100, 1280, 1024);
        wnd->size()->set(320, 200);
        wnd->slot(tk::SLOT_CLOSE)->bind(slot_close, this);
        wnd->slot(tk::SLOT_KEY_UP)->bind(slot_key_up, this);
        wnd->pointer()->set(ws::MP_TABLE_CELL);
        wnd->layout()->set(0.0f, 0.0f, 1.0f, 1.0f);
        MTEST_ASSERT(widgets.push(wnd));

        // Create Grid
        MTEST_ASSERT(grid = new tk::Grid(dpy));
        MTEST_ASSERT(init_widget(grid, vh, "grid") == STATUS_OK);
        MTEST_ASSERT(widgets.push(grid));
        MTEST_ASSERT(wnd->add(grid) == STATUS_OK);
        grid->bg_color()->set_rgb(1.0f, 1.0f, 1.0f);
        grid->padding()->set(8);
        grid->rows()->set(8);
        grid->columns()->set(6);
        grid->orientation()->set_horizontal();
        grid->hspacing()->set(2);
        grid->vspacing()->set(2);

        {
            // Create alignment and child widget
            LSPString id;

            // Rows 1 and 2
            // Create landscape image (default)
            MTEST_ASSERT(id.fmt_ascii("img-0"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img, 1, 2) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(landscape) == STATUS_OK);
            img->fitting()->set(tk::FIT_FULL);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->proportion()->set(1.0f);
            img->constraints()->set(32, 32, 32, 32);
            img->pointer()->set(ws::MP_HAND);

            // Create landscape image (default)
            MTEST_ASSERT(id.fmt_ascii("img-1"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img, 1, 2) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(landscape) == STATUS_OK);
            img->fitting()->set(tk::FIT_FULL);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->proportion()->set(1.0f);
            img->layout()->set_scale(0.5f, 0.5f);
            img->layout()->set_align(0.0f, 0.0f);
            img->constraints()->set(32, 32, 32, 32);
            img->transparency()->set(0.25f);
            img->pointer()->set(ws::MP_HAND);

            // Create portrait image (default)
            MTEST_ASSERT(id.fmt_ascii("img-2"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img, 2, 1) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(portrait) == STATUS_OK);
            img->fitting()->set(tk::FIT_FULL);
            img->border_color()->set_rgb24(0x0000ff);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->proportion()->set(1.0f);
            img->constraints()->set(32, 32, 32, 32);
            img->pointer()->set(ws::MP_HAND);

            // Create portrait image (default)
            MTEST_ASSERT(id.fmt_ascii("img-3"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img, 2, 1) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(portrait) == STATUS_OK);
            img->fitting()->set(tk::FIT_FULL);
            img->border_color()->set_rgb24(0x0000ff);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->proportion()->set(1.0f);
            img->layout()->set_scale(0.5f, 0.5f);
            img->layout()->set_align(0.0f, 0.0f);
            img->constraints()->set(32, 32, 32, 32);
            img->transparency()->set(0.25f);
            img->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(id.fmt_ascii("img-4"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(landscape) == STATUS_OK);
            img->fitting()->set(tk::FIT_NONE);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->proportion()->set(1.0f);
            img->layout()->set_scale(0.75f, 0.75f);
            img->layout()->set_align(0.5f, 0.0f);
            img->constraints()->set(64, 72, 64, 72);
            img->transparency()->set(0.25f);
            img->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(id.fmt_ascii("img-5"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(portrait) == STATUS_OK);
            img->fitting()->set(tk::FIT_NONE);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->proportion()->set(1.0f);
            img->layout()->set_scale(0.75f, 0.75f);
            img->layout()->set_align(0.5f, -0.5f);
            img->constraints()->set(72, 64, 72, 64);
            img->transparency()->set(0.25f);
            img->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(id.fmt_ascii("img-6"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(landscape) == STATUS_OK);
            img->fitting()->set(tk::FIT_WIDTH);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->proportion()->set(3.0f / 4.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(id.fmt_ascii("img-7"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(portrait) == STATUS_OK);
            img->fitting()->set(tk::FIT_HEIGHT);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->proportion()->set(4.0f / 3.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);

            // Row 3
            MTEST_ASSERT(id.fmt_ascii("img-8"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(landscape) == STATUS_OK);
            img->fitting()->set(tk::FIT_COVER);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->proportion()->set(4.0f / 3.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(id.fmt_ascii("img-9"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(landscape) == STATUS_OK);
            img->fitting()->set(tk::FIT_COVER);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->proportion()->set(3.0f / 4.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(id.fmt_ascii("img-10"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(portrait) == STATUS_OK);
            img->fitting()->set(tk::FIT_COVER);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->proportion()->set(4.0f / 3.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(id.fmt_ascii("img-11"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(portrait) == STATUS_OK);
            img->fitting()->set(tk::FIT_COVER);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->proportion()->set(3.0f / 4.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(id.fmt_ascii("img-12"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(landscape) == STATUS_OK);
            img->fitting()->set(tk::FIT_CONTAIN);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->proportion()->set(4.0f / 3.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(id.fmt_ascii("img-13"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(landscape) == STATUS_OK);
            img->fitting()->set(tk::FIT_CONTAIN);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->proportion()->set(3.0f / 4.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);

            // Row 4
            MTEST_ASSERT(id.fmt_ascii("img-14"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(portrait) == STATUS_OK);
            img->fitting()->set(tk::FIT_CONTAIN);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->proportion()->set(4.0f / 3.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(id.fmt_ascii("img-15"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(portrait) == STATUS_OK);
            img->fitting()->set(tk::FIT_CONTAIN);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->proportion()->set(3.0f / 4.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(id.fmt_ascii("img-16"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(landscape) == STATUS_OK);
            img->fitting()->set(tk::FIT_HSTRETCH);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->proportion()->set(5.0f / 4.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(id.fmt_ascii("img-17"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(landscape) == STATUS_OK);
            img->fitting()->set(tk::FIT_HSTRETCH);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->proportion()->set(4.0f / 5.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(id.fmt_ascii("img-18"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(portrait) == STATUS_OK);
            img->fitting()->set(tk::FIT_HSTRETCH);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->proportion()->set(5.0f / 4.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(id.fmt_ascii("img-19"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(portrait) == STATUS_OK);
            img->fitting()->set(tk::FIT_HSTRETCH);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->proportion()->set(4.0f / 5.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);

            // Row 6
            MTEST_ASSERT(id.fmt_ascii("img-20"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(landscape) == STATUS_OK);
            img->fitting()->set(tk::FIT_VSTRETCH);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->proportion()->set(4.0f / 3.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(id.fmt_ascii("img-21"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(landscape) == STATUS_OK);
            img->fitting()->set(tk::FIT_VSTRETCH);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->proportion()->set(3.0f / 4.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(id.fmt_ascii("img-22"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(portrait) == STATUS_OK);
            img->fitting()->set(tk::FIT_VSTRETCH);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->proportion()->set(5.0f / 4.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(id.fmt_ascii("img-23"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(portrait) == STATUS_OK);
            img->fitting()->set(tk::FIT_VSTRETCH);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->proportion()->set(4.0f / 5.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(id.fmt_ascii("img-24"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(landscape) == STATUS_OK);
            img->fitting()->set(tk::FIT_STRETCH);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->proportion()->set(5.0f / 4.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(id.fmt_ascii("img-25"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(landscape) == STATUS_OK);
            img->fitting()->set(tk::FIT_STRETCH);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->proportion()->set(4.0f / 5.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);

            // Row 7
            MTEST_ASSERT(id.fmt_ascii("img-26"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(portrait) == STATUS_OK);
            img->fitting()->set(tk::FIT_STRETCH);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->proportion()->set(5.0f / 4.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(id.fmt_ascii("img-27"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(portrait) == STATUS_OK);
            img->fitting()->set(tk::FIT_STRETCH);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->proportion()->set(4.0f / 5.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(id.fmt_ascii("img-28"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(landscape) == STATUS_OK);
            img->fitting()->set(tk::FIT_STRETCH);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(id.fmt_ascii("img-29"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(landscape) == STATUS_OK);
            img->fitting()->set(tk::FIT_STRETCH);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(id.fmt_ascii("img-30"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(portrait) == STATUS_OK);
            img->fitting()->set(tk::FIT_STRETCH);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(id.fmt_ascii("img-31"));
            MTEST_ASSERT(img = new tk::Image(dpy));
            MTEST_ASSERT(init_widget(img, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(img));
            MTEST_ASSERT(grid->add(img) == STATUS_OK);

            MTEST_ASSERT(img->bitmap()->load(portrait) == STATUS_OK);
            img->fitting()->set(tk::FIT_STRETCH);
            img->border_color()->set_rgb24(0xff0000);
            img->border_hover_color()->set_rgb24(0x00ff00);
            img->color()->set_rgb24(0xeeeeee);
            img->hover_color()->set_rgb24(0xffffff);
            img->border_width()->set(2);
            img->layout()->set_align(0.0f, 0.0f);
            img->constraints()->set(64, 64, -1, -1);
            img->pointer()->set(ws::MP_HAND);
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









