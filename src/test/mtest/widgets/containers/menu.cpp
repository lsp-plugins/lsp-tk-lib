/*
 * menu.cpp
 *
 *  Created on: 20 июн. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/test-fw/mtest.h>
#include <lsp-plug.in/tk/tk.h>
#include <private/mtest/tk/common.h>

MTEST_BEGIN("tk.widgets.containers", menu)
    typedef struct handler_t
    {
        test_type_t        *test;
        tk::Menu           *menu;
        char               *label;
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

        if (h->menu == NULL)
            return STATUS_OK;

        if (h->menu->visibility()->get())
            h->menu->hide();
        else
            h->menu->show();

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

    status_t init_widget(tk::Widget *w, lltl::parray<handler_t> &vh, tk::Menu *menu, const char *label)
    {
        status_t res = w->init();
        if (res != STATUS_OK)
            return res;

        handler_t *h = new handler_t;
        if ((h == NULL) || (!vh.add(h)))
            return STATUS_NO_MEM;
        h->test     = this;
        h->menu     = menu;
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

    tk::MenuItem *add_item(tk::Menu *mw, lltl::parray<handler_t> &vh, size_t &miid, const char *text, tk::menu_item_type_t type)
    {
        LSPString id;
        tk::MenuItem *mi;

        MTEST_ASSERT(id.fmt_ascii("menuitem-%d", int(miid++)));
        MTEST_ASSERT(mi = new tk::MenuItem(mw->display()));
        MTEST_ASSERT(init_widget(mi, vh, NULL, id.get_ascii()) == STATUS_OK);
        MTEST_ASSERT(mw->add(mi) == STATUS_OK);
        mi->text()->set_raw(text);
        mi->type()->set(type);
        return mi;
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
        tk::Menu *mw = NULL;
        tk::Void *vw = NULL;
        tk::MenuItem *mi = NULL;

        // Initialize window
        MTEST_ASSERT(init_widget(wnd, vh, NULL, "window") == STATUS_OK);
        MTEST_ASSERT(wnd->title()->set_raw("Test fader") == STATUS_OK);
        MTEST_ASSERT(wnd->role()->set_raw("fader_test") == STATUS_OK);
        wnd->bg_color()->set_rgb(0, 0.75, 1.0);
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

        // Create Grid
        MTEST_ASSERT(grid = new tk::Grid(dpy));
        MTEST_ASSERT(init_widget(grid, vh, NULL, "grid") == STATUS_OK);
        MTEST_ASSERT(widgets.push(grid));
        MTEST_ASSERT(wnd->add(grid) == STATUS_OK);
        grid->bg_color()->set_rgb(1.0f, 1.0f, 1.0f);
        grid->padding()->set(8);
        grid->rows()->set(3);
        grid->columns()->set(2);
        grid->orientation()->set_horizontal();
        grid->hspacing()->set(4);

        {
            // Create alignment and child widget
            LSPString id;
            size_t col = 0;
            size_t miid = 0;

            // Create menu
            MTEST_ASSERT(id.fmt_ascii("menu-%d", int(0)));
            MTEST_ASSERT(mw = new tk::Menu(dpy));
            MTEST_ASSERT(init_widget(mw, vh, NULL, id.get_ascii()) == STATUS_OK);

            add_item(mw, vh, miid, "Item 1", tk::MI_NORMAL);
            add_item(mw, vh, miid, "Item 2", tk::MI_NORMAL);
            add_item(mw, vh, miid, "Item 3", tk::MI_NORMAL);
            add_item(mw, vh, miid, "-", tk::MI_SEPARATOR);
            mi = add_item(mw, vh, miid, "Check 1", tk::MI_CHECK);
            mi->checked()->set(true);
            add_item(mw, vh, miid, "Check 2", tk::MI_CHECK);
            mi = add_item(mw, vh, miid, "Check 3", tk::MI_CHECK);
            mi->checked()->set(true);
            add_item(mw, vh, miid, "-", tk::MI_SEPARATOR);
            mi = add_item(mw, vh, miid, "Radio 1", tk::MI_RADIO);
            mi->checked()->set(true);
            add_item(mw, vh, miid, "Radio 2", tk::MI_RADIO);
            mi = add_item(mw, vh, miid, "Radio 3", tk::MI_RADIO);
            mi->checked()->set(true);

            // Create void widget
            MTEST_ASSERT(id.fmt_ascii("void-%d", int(0)));
            MTEST_ASSERT(vw = new tk::Void(dpy));
            MTEST_ASSERT(init_widget(vw, vh, mw, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(vw));
            MTEST_ASSERT(grid->add(vw) == STATUS_OK);
            vw->bg_color()->set_rgb24(next_color(col));
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



