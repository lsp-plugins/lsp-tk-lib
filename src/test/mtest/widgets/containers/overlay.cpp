/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 29 мар. 2025 г.
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

namespace
{
    typedef struct ov_position_t
    {
        float           nLeft;
        float           nTop;
        bool            bHide;
    } ov_position_t;

    static const ov_position_t ov_positions[] =
    {
        { 0.25f, 0.25f, false },
        { 0.25f, 0.75f, false },
        { 0.75f, 0.25f, false },
        { 0.75f, 0.75f, true  },
    };
}

MTEST_BEGIN("tk.widgets.containers", overlay)
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

    static void destroy_handlers(lltl::parray<handler_t> &vh)
    {
        for (size_t i=0, n=vh.size(); i<n; ++i)
        {
            handler_t *h = vh.uget(i);
            ::free(h->label);
            delete h;
        }
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

    status_t create_group(
        tk::WidgetContainer *dst,
        lltl::parray<tk::Widget> & widgets,
        lltl::parray<handler_t> & vh,
        int gid)
    {
        LSPString id;
        tk::Display *dpy = dst->display();

        // Create group
        tk::Group *gr;
        MTEST_ASSERT(id.fmt_ascii("group-%d", gid));
        MTEST_ASSERT(gr = new tk::Group(dpy));
        MTEST_ASSERT(init_widget(gr, vh, id.get_ascii()) == STATUS_OK);
        MTEST_ASSERT(widgets.push(gr));
        MTEST_ASSERT(dst->add(gr) == STATUS_OK);

        MTEST_ASSERT(id.fmt_ascii("Test Group #%d", gid));
        gr->text()->set_raw(&id);
        gr->show_text()->set(true);

        // Create box
        tk::Box *box;
        MTEST_ASSERT(id.fmt_ascii("box-%d", gid));
        MTEST_ASSERT(box = new tk::Box(dpy));
        MTEST_ASSERT(init_widget(box, vh, id.get_ascii()) == STATUS_OK);
        MTEST_ASSERT(widgets.push(box));
        MTEST_ASSERT(gr->add(box) == STATUS_OK);
        box->orientation()->set_horizontal();

        // Create knobs
        for (size_t x=0; x<3; ++x)
        {
            tk::Knob *kn;
            MTEST_ASSERT(id.fmt_ascii("knob-%d-%d", gid, x));
            MTEST_ASSERT(kn = new tk::Knob(dpy));
            MTEST_ASSERT(init_widget(kn, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(kn));
            MTEST_ASSERT(box->add(kn) == STATUS_OK);

            kn->size()->set(48, 64);
            kn->scale_color()->set_rgb24(test::colors[x]);
            kn->scale()->set(4);
            kn->value()->set_all(0.5f, 0.0f, 1.0f);
            kn->step()->set_step(0.001f, 10.0f, 0.1f);
        }

        return STATUS_OK;
    }

    static bool overlay_position_func(ws::rectangle_t *rect, tk::Overlay *overlay, void *data)
    {
        const ov_position_t *pos = static_cast<const ov_position_t *>(data);

        tk::Window *wnd = tk::widget_cast<tk::Window>(overlay->toplevel());
        if (wnd == NULL)
            return false;

        ws::rectangle_t r;
        wnd->get_rectangle(&r);

        rect->nLeft     = r.nWidth * pos->nLeft - rect->nWidth * 0.5f;
        rect->nTop      = r.nHeight * pos->nTop - rect->nHeight * 0.5f;

        return true;
    }

    tk::Overlay *create_overlay(
        tk::Window *wnd,
        lltl::parray<tk::Widget> & widgets,
        lltl::parray<handler_t> & vh,
        int ovid,
        const ov_position_t *position)
    {
        LSPString id;
        tk::Display *dpy = wnd->display();

        tk::Overlay *ov;
        MTEST_ASSERT(id.fmt_ascii("overlay-%d", ovid));
        MTEST_ASSERT(ov = new tk::Overlay(dpy));
        MTEST_ASSERT(init_widget(ov, vh, id.get_ascii()) == STATUS_OK);
        MTEST_ASSERT(widgets.push(ov));
        MTEST_ASSERT(wnd->add(ov) == STATUS_OK);

        MTEST_ASSERT(create_group(ov, widgets, vh, ovid + 9) == STATUS_OK);

        ov->set_position_function(overlay_position_func, const_cast<ov_position_t *>(position));
        ov->transparency()->set(0.1f);
        ov->auto_close()->set(position->bHide);

        return ov;
    }

    static status_t show_overlay(tk::Widget *sender, void *ptr, void *data)
    {
        tk::Overlay *ov = static_cast<tk::Overlay *>(ptr);
        ov->visibility()->set(true);

        return STATUS_OK;
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

        // Initialize window
        MTEST_ASSERT(init_widget(wnd, vh, "window") == STATUS_OK);
        MTEST_ASSERT(wnd->title()->set_raw("Test group 4") == STATUS_OK);
        MTEST_ASSERT(wnd->role()->set_raw("group_test") == STATUS_OK);
        wnd->bg_color()->set_rgb(0, 0.75, 1.0);
        wnd->actions()->set_actions(ws::WA_MOVE | ws::WA_RESIZE | ws::WA_CLOSE);
        wnd->border_style()->set(ws::BS_DIALOG);
        wnd->constraints()->set(160, 100, 960, 600);
        wnd->size()->set(320, 200);
        wnd->slot(tk::SLOT_CLOSE)->bind(slot_close, this);
        wnd->slot(tk::SLOT_KEY_UP)->bind(slot_key_up, this);
        wnd->pointer()->set(ws::MP_TABLE_CELL);
        wnd->layout()->set(-0.5, 0.5, 0.5, 0.5);
        MTEST_ASSERT(widgets.push(wnd));

        // Create grid
        MTEST_ASSERT(grid = new tk::Grid(dpy));
        MTEST_ASSERT(init_widget(grid, vh, "grid") == STATUS_OK);
        MTEST_ASSERT(widgets.push(grid));
        MTEST_ASSERT(wnd->add(grid) == STATUS_OK);
        grid->padding()->set_all(4);
        grid->hspacing()->set(8);
        grid->vspacing()->set(4);
        grid->bg_color()->set_rgb(1.0, 0.75, 1.0);
        grid->rows()->set(4);
        grid->columns()->set(3);

        // Create groups
        for (size_t i=0; i<9; ++i)
            MTEST_ASSERT(create_group(grid, widgets, vh, i) == STATUS_OK);

        tk::Overlay *ov[4];
        MTEST_ASSERT(ov[0] = create_overlay(wnd, widgets, vh, 0, &ov_positions[0]));
        MTEST_ASSERT(ov[1] = create_overlay(wnd, widgets, vh, 1, &ov_positions[1]));
        MTEST_ASSERT(ov[2] = create_overlay(wnd, widgets, vh, 2, &ov_positions[2]));
        MTEST_ASSERT(ov[3] = create_overlay(wnd, widgets, vh, 3, &ov_positions[3]));

        // Add two void widgets
        tk::Void *v;
        for (size_t i=0; i<2; ++i)
        {
            MTEST_ASSERT(v = new tk::Void(dpy));
            MTEST_ASSERT(init_widget(v, vh, "void") == STATUS_OK);
            MTEST_ASSERT(widgets.push(v));
            MTEST_ASSERT(grid->add(v) == STATUS_OK);
        }

        // Add Button for activating overlay
        tk::Button *btn;
        MTEST_ASSERT(btn = new tk::Button(dpy));
        MTEST_ASSERT(init_widget(btn, vh, "button") == STATUS_OK);
        MTEST_ASSERT(widgets.push(btn));
        MTEST_ASSERT(grid->add(btn) == STATUS_OK);

        btn->text()->set_raw("Show overlay");
        btn->slots()->bind(tk::SLOT_SUBMIT, show_overlay, ov[3]);

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



