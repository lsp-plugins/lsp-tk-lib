/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 4 авг. 2020 г.
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

MTEST_BEGIN("tk.widgets.compound", listbox)
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
            tk::ListBox *lb = tk::widget_cast<tk::ListBox>(wnd->child());

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

                case 'a':
                    if (lb != NULL)
                        lb->hscroll_mode()->set_none();
                    break;
                case 's':
                    if (lb != NULL)
                        lb->hscroll_mode()->set_clip();
                    break;
                case 'd':
                    if (lb != NULL)
                        lb->hscroll_mode()->set_optional();
                    break;
                case 'f':
                    if (lb != NULL)
                        lb->hscroll_mode()->set_always();
                    break;

                case 'z':
                    if (lb != NULL)
                        lb->vscroll_mode()->set_none();
                    break;
                case 'x':
                    if (lb != NULL)
                        lb->vscroll_mode()->set_clip();
                    break;
                case 'c':
                    if (lb != NULL)
                        lb->vscroll_mode()->set_optional();
                    break;
                case 'v':
                    if (lb != NULL)
                        lb->vscroll_mode()->set_always();
                    break;
                case 'm':
                    if (lb != NULL)
                        lb->multi_select()->toggle();
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

        ws::event_t *ev = static_cast<ws::event_t *>(data);
        if (ev->nCode == ws::MCB_MIDDLE)
        {
            tk::ListBox *list = tk::widget_cast<tk::ListBox>(sender);
            if (list != NULL)
            {
                if (ev->nState & ws::MCF_SHIFT)
                    list->active()->toggle();
                else
                {
                    lltl::parray<tk::ListBoxItem> widgets;
                    list->selected()->values(&widgets);
                    for (size_t i=0; i<widgets.size(); ++i)
                        widgets.get(i)->active()->toggle();
                }
            }

            tk::ListBoxItem *item = tk::widget_cast<tk::ListBoxItem>(sender);
            if (item != NULL)
                item->active()->toggle();
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

    MTEST_MAIN
    {
        lltl::parray<handler_t> vh;

        tk::Display *dpy = new tk::Display();
        MTEST_ASSERT(dpy != NULL);

        MTEST_ASSERT(dpy->init(0, NULL) == STATUS_OK);

        lltl::parray<tk::Widget> widgets;
        tk::Widget *w = NULL;
        tk::Window *wnd = new tk::Window(dpy);
        tk::ListBox *lb = NULL;
        tk::ListBoxItem *li = NULL;

        // Initialize window
        MTEST_ASSERT(init_widget(wnd, vh, "window") == STATUS_OK);
        MTEST_ASSERT(wnd->title()->set_raw("Test listbox") == STATUS_OK);
        MTEST_ASSERT(wnd->role()->set_raw("listbox_test") == STATUS_OK);
        wnd->bg_color()->set_rgb(0, 0.75, 1.0);
        wnd->actions()->set_actions(ws::WA_MOVE | ws::WA_RESIZE | ws::WA_CLOSE);
        wnd->border_style()->set(ws::BS_DIALOG);
        wnd->constraints()->set(160, 100, 640, 400);
        wnd->size()->set(320, 200);
        wnd->slot(tk::SLOT_CLOSE)->bind(slot_close, this);
        wnd->slot(tk::SLOT_KEY_UP)->bind(slot_key_up, this);
        wnd->pointer()->set(ws::MP_TABLE_CELL);
        wnd->layout()->set(-0.5, 0.5, 0.5, 0.5);
        MTEST_ASSERT(widgets.push(wnd));

        // Create list box
        MTEST_ASSERT(lb = new tk::ListBox(dpy));
        MTEST_ASSERT(init_widget(lb, vh, "listbox") == STATUS_OK);
        MTEST_ASSERT(widgets.push(lb));
        MTEST_ASSERT(wnd->add(lb) == STATUS_OK);
        lb->hscroll_mode()->set(tk::SCROLL_NONE);
        lb->vscroll_mode()->set(tk::SCROLL_NONE);
        lb->multi_select()->set(true);

        lb->constraints()->set(96, 96, 96, 96);
        {
            LSPString id;
            size_t wid = 0;
            size_t col = 0;

            for (size_t i=0; i<32; ++i)
            {
                MTEST_ASSERT(id.fmt_ascii("item-%d", int(wid++)));
                MTEST_ASSERT(li = new tk::ListBoxItem(dpy));
                MTEST_ASSERT(init_widget(li, vh, id.get_ascii()) == STATUS_OK);
                MTEST_ASSERT(widgets.push(li));
                MTEST_ASSERT(lb->items()->add(li) == STATUS_OK);

                MTEST_ASSERT(id.fmt_ascii("%s %d",
                        (i % 6 == 0) ? "Item with long text" : "Item",
                        int(i)
                    ));
                li->text()->set_raw(&id);

                if (i > 16)
                {
                    li->text_color()->set_rgb24(next_color(col));
                    li->text_selected_color()->set_rgb24(next_color(col));
                    li->bg_color()->set_rgb24(next_color(col));
                    li->bg_selected_color()->set_rgb24(next_color(col));
                }
            }
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





