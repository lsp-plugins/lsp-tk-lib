/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 27 сент. 2020 г.
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

MTEST_BEGIN("tk.widgets.specific", ledmeter)
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

    static void sync_text(tk::LedMeterChannel *lm)
    {
        LSPString tmp;
        tmp.fmt_ascii("%.1f", lm->value()->get());
        lm->text()->set_raw(&tmp);
    }

    static status_t slot_mouse_scroll(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("MOUSE_SCROLL: %s\n", h->label);

        tk::LedMeterChannel *lm = tk::widget_cast<tk::LedMeterChannel>(sender);
        if (lm == NULL)
            return STATUS_OK;

        ws::event_t *ev = static_cast<ws::event_t *>(data);

        float delta = 0.0f;

        if (ev->nCode == ws::MCD_UP)
            delta = 0.05f;
        else if (ev->nCode == ws::MCD_DOWN)
            delta = -0.05f;

        if (ev->nState & (ws::MCF_CONTROL | ws::MCF_ALT))
            delta *= 10.0f;

        if (ev->nState & ws::MCF_SHIFT)
            lm->peak()->add(delta);
        else
        {
            lm->value()->add(delta, false);
            sync_text(lm);
        }

        return STATUS_OK;
    }

    static status_t slot_mouse_click(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("MOUSE_CLICK: %s\n", h->label);

        tk::LedMeter *lm = tk::widget_cast<tk::LedMeter>(sender);
        if (lm == NULL)
        {
            tk::LedMeterChannel *lmc = tk::widget_cast<tk::LedMeterChannel>(sender);
            if (lmc == NULL)
                return STATUS_OK;
            lm = tk::widget_cast<tk::LedMeter>(lmc->parent());
            if (lm == NULL)
                return STATUS_OK;
        }

        ws::event_t *ev = static_cast<ws::event_t *>(data);
        if (ev->nCode == ws::MCB_RIGHT)
            lm->text_visible()->toggle();

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
        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_SCROLL, slot_mouse_scroll, h);
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

    void add_items(tk::Fraction *f, lltl::parray<handler_t> &vh, lltl::parray<tk::Widget> &widgets, size_t &vid, size_t count)
    {
        tk::ListBoxItem *li;
        LSPString text, id;

        for (size_t i=0; i<count; ++i)
        {
            text.fmt_ascii("%d", int(i));
            id.fmt_ascii("fraction-%d", int(vid++));
            MTEST_ASSERT(li = new tk::ListBoxItem(f->display()));
            MTEST_ASSERT(init_widget(li, vh, id.get_ascii()) == STATUS_OK);

            MTEST_ASSERT(li->text()->set_raw(&text) == STATUS_OK);
            MTEST_ASSERT(f->num_items()->add(li) == STATUS_OK);
            MTEST_ASSERT(widgets.push(li));

            if (i == 0)
                f->num_selected()->set(li);
        }

        for (size_t i=1; i<=count; ++i)
        {
            text.fmt_ascii("%d", int(i));
            id.fmt_ascii("fraction-%d", int(vid++));
            MTEST_ASSERT(li = new tk::ListBoxItem(f->display()));
            MTEST_ASSERT(init_widget(li, vh, id.get_ascii()) == STATUS_OK);

            MTEST_ASSERT(li->text()->set_raw(&text) == STATUS_OK);
            MTEST_ASSERT(f->den_items()->add(li) == STATUS_OK);
            MTEST_ASSERT(widgets.push(li));
            if (i == 1)
                f->den_selected()->set(li);
        }
    }

    void add_channels(tk::LedMeter *lm, size_t &vid, lltl::parray<handler_t> &vh, lltl::parray<tk::Widget> &widgets)
    {
        tk::LedMeterChannel *lc = NULL;
        LSPString id, ch_id;

        static const float hue[] = { 0.0f, 0.5f, 0.25f, 0.75f, 0.0f};

        for (size_t i=0; i < sizeof(hue)/sizeof(float); ++i)
        {
            MTEST_ASSERT(id.fmt_ascii("meterchannel-%d", int(vid++)));
            MTEST_ASSERT(lc = new tk::LedMeterChannel(lm->display()));
            MTEST_ASSERT(init_widget(lc, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(lc));
            MTEST_ASSERT(lm->add(lc) == STATUS_OK);

            ch_id.fmt_ascii("ch%d", int(i));

            lc->text_visible()->set(true);
            lc->peak_visible()->set(true);
            lc->header_visible()->set(true);
            lc->value()->set_all(-1.2f, -7.2f, 2.4f);
            lc->peak()->set(0.0f);
            lc->angle()->set(1 + i*2);
            lc->header_value()->set(0.0f);
            lc->constraints()->set_min_height(128);
            lc->header()->set_raw(&ch_id);

            lsp::Color c;
            tk::ColorRange *cr;
            c.set_rgb24(0xff0000);
            c.hue(hue[i]);

            // Color ranges
            cr = lc->value_ranges()->append();
            cr->set_range(0.0f, 2.4f);
            cr->set_color("#ff0000");

            cr = lc->value_ranges()->append();
            cr->set_range(-0.6f, 0.0f);
            cr->set_color("#ffff00");

            cr = lc->value_ranges()->append();
            cr->set_range(-2.4f, 0.0f);
            c.lightness(0.5f);
            cr->set_color(&c);

            cr = lc->value_ranges()->append();
            cr->set_range(-4.8f, -2.4f);
            c.lightness(0.75f * 0.5f);
            cr->set_color(&c);

            cr = lc->value_ranges()->append();
            cr->set_range(-7.2f, -4.8f);
            c.lightness(0.5f * 0.5f);
            cr->set_color(&c);

            // Peak ranges
            cr = lc->peak_ranges()->append();
            cr->set_range(0.0f, 2.4f);
            cr->set_color("#ff0000");

            cr = lc->peak_ranges()->append();
            cr->set_range(-0.6f, 0.0f);
            cr->set_color("#ffff00");

            cr = lc->peak_ranges()->append();
            cr->set_range(-2.4f, 0.0f);
            c.lightness(0.5f);
            cr->set_color(&c);

            cr = lc->peak_ranges()->append();
            cr->set_range(-4.8f, -2.4f);
            c.lightness(0.75f * 0.5f);
            cr->set_color(&c);

            cr = lc->peak_ranges()->append();
            cr->set_range(-7.2f, -4.8f);
            c.lightness(0.5f * 0.5f);
            cr->set_color(&c);

            // Text ranges
            cr = lc->text_ranges()->append();
            cr->set_range(0.0f, 2.4f);
            cr->set_color("#ff0000");

            cr = lc->text_ranges()->append();
            cr->set_range(-0.6f, 0.0f);
            cr->set_color("#ffff00");

            cr = lc->text_ranges()->append();
            cr->set_range(-2.4f, 0.0f);
            c.lightness(0.5f);
            cr->set_color(&c);

            cr = lc->text_ranges()->append();
            cr->set_range(-4.8f, -2.4f);
            c.lightness(0.75f * 0.5f);
            cr->set_color(&c);

            cr = lc->text_ranges()->append();
            cr->set_range(-7.2f, -4.8f);
            c.lightness(0.5f * 0.5f);
            cr->set_color(&c);

            sync_text(lc);
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
        tk::LedMeter *lm = NULL;
        size_t vid = 0;

        // Initialize window
        MTEST_ASSERT(init_widget(wnd, vh, "window") == STATUS_OK);
        MTEST_ASSERT(wnd->title()->set_raw("Test led meter") == STATUS_OK);
        MTEST_ASSERT(wnd->role()->set_raw("ledmeter_test") == STATUS_OK);
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
        grid->rows()->set(5);
        grid->columns()->set(4);
        grid->orientation()->set_horizontal();
        grid->hspacing()->set(2);
        grid->vspacing()->set(2);

        {
            // Create meter channels
            LSPString id;

            // Create horizontal meters with packing
            for (size_t i=0; i<2; ++i)
            {
                MTEST_ASSERT(id.fmt_ascii("meter-%d", int(vid++)));
                MTEST_ASSERT(lm = new tk::LedMeter(dpy));
                MTEST_ASSERT(init_widget(lm, vh, id.get_ascii()) == STATUS_OK);
                MTEST_ASSERT(widgets.push(lm));
                MTEST_ASSERT(grid->add(lm, 1, 2) == STATUS_OK);

                lm->text_visible()->set(true);
                lm->header_visible()->set(true);
                lm->angle()->set(i*2);
                add_channels(lm, vid, vh, widgets);
            }

            // Create horizontal meters without packing
            for (size_t i=0; i<2; ++i)
            {
                MTEST_ASSERT(id.fmt_ascii("meter-%d", int(vid++)));
                MTEST_ASSERT(lm = new tk::LedMeter(dpy));
                MTEST_ASSERT(init_widget(lm, vh, id.get_ascii()) == STATUS_OK);
                MTEST_ASSERT(widgets.push(lm));
                MTEST_ASSERT(grid->add(lm, 1, 2) == STATUS_OK);

                lm->text_visible()->set(true);
                lm->header_visible()->set(true);
                lm->stereo_groups()->set(false);
                lm->angle()->set(i*2);
                lm->constraints()->set_width(5*12 + lm->border()->get());
                add_channels(lm, vid, vh, widgets);
            }

            // Create vertical meters
            for (size_t i=0; i<2; ++i)
            {
                MTEST_ASSERT(id.fmt_ascii("meter-%d", int(vid++)));
                MTEST_ASSERT(lm = new tk::LedMeter(dpy));
                MTEST_ASSERT(init_widget(lm, vh, id.get_ascii()) == STATUS_OK);
                MTEST_ASSERT(widgets.push(lm));
                MTEST_ASSERT(grid->add(lm) == STATUS_OK);

                lm->text_visible()->set(true);
                lm->header_visible()->set(true);
                lm->angle()->set(i*2 + 1);
                add_channels(lm, vid, vh, widgets);
            }

            // Create vertical meters without packing
            for (size_t i=0; i<2; ++i)
            {
                MTEST_ASSERT(id.fmt_ascii("meter-%d", int(vid++)));
                MTEST_ASSERT(lm = new tk::LedMeter(dpy));
                MTEST_ASSERT(init_widget(lm, vh, id.get_ascii()) == STATUS_OK);
                MTEST_ASSERT(widgets.push(lm));
                MTEST_ASSERT(grid->add(lm) == STATUS_OK);

                lm->text_visible()->set(true);
                lm->header_visible()->set(true);
                lm->stereo_groups()->set(false);
                lm->angle()->set(i*2 + 1);
                add_channels(lm, vid, vh, widgets);
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



