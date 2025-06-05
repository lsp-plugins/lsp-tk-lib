/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 5 июн. 2025 г.
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

MTEST_BEGIN("tk.widgets.specific", audioenvelope)
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

    void init_values(tk::AudioChannel *ac)
    {
        tk::FloatArray *fa = ac->samples();
        MTEST_ASSERT(fa->resize(256) == STATUS_OK);
        float kf = 8.0f * M_PI / fa->size();
        for (size_t i=0; i<fa->size(); ++i)
            fa->set(i, sinf(i * kf));
    }

    void add_samples(tk::AudioSample *as, lltl::parray<tk::Widget> &widgets, lltl::parray<handler_t> &vh, size_t &vid, size_t count)
    {
        LSPString id;
        tk::AudioChannel *ac = NULL;

        static const float layout_x[] = { -1.0f, 1.0f, 0.0f, -1.0f, 1.0f };
        static const float layout_y[] = { -1.0f, -1.0f, 0.0f, 1.0f, 1.0f };

        float hue = 1.0f / count;
        for (size_t i=0; i<count; ++i)
        {
            MTEST_ASSERT(id.fmt_ascii("audiochannel-%d", int(vid++)));
            MTEST_ASSERT(ac = new tk::AudioChannel(as->display()));
            MTEST_ASSERT(init_widget(ac, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(ac));
            MTEST_ASSERT(as->add(ac) == STATUS_OK);

            init_values(ac);
            ac->color()->set_rgba32(0x88ff0000);
            ac->color()->hue(hue * i);
            ac->wave_border_color()->set_rgb24(0xff0000);
            ac->wave_border_color()->hue(hue * i);

            ac->fade_in()->set(ac->samples()->size() * 0.5f * (i + 1) * hue);
            ac->fade_out()->set(ac->samples()->size() * 0.5f * (i + 1) * hue);
            ac->stretch_begin()->set(ac->fade_in()->get());
            ac->stretch_end()->set(ac->fade_in()->get() + 32);
            ac->loop_begin()->set(ac->samples()->size() - ac->fade_out()->get() - 32);
            ac->loop_end()->set(ac->samples()->size() - ac->fade_out()->get());
            ac->play_position()->set(ac->samples()->size() * 0.85f);
        }

        for (size_t i=0; i<5; ++i)
        {
            MTEST_ASSERT(id.fmt_ascii("label%d", int(i)));

            as->label_visibility(i)->set(true);
            as->label_layout(i)->set_align(layout_x[i], layout_y[i]);
            as->label(i)->set(&id);
        }
    }

    void add_samples_with_fades(tk::AudioSample *as, lltl::parray<tk::Widget> &widgets, lltl::parray<handler_t> &vh, size_t &vid, size_t count)
    {
        LSPString id;
        tk::AudioChannel *ac = NULL;

        static const float layout_x[] = { -1.0f, 1.0f, 0.0f, -1.0f, 1.0f };
        static const float layout_y[] = { -1.0f, -1.0f, 0.0f, 1.0f, 1.0f };

        float hue = 1.0f / count;
        for (size_t i=0; i<count; ++i)
        {
            MTEST_ASSERT(id.fmt_ascii("audiochannel-%d", int(vid++)));
            MTEST_ASSERT(ac = new tk::AudioChannel(as->display()));
            MTEST_ASSERT(init_widget(ac, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(ac));
            MTEST_ASSERT(as->add(ac) == STATUS_OK);

            init_values(ac);
            ac->color()->set_rgba32(0x88ff0000);
            ac->color()->hue(hue * i);
            ac->wave_border_color()->set_rgb24(0xff0000);
            ac->wave_border_color()->hue(hue * i);

            ac->fade_in()->set(ac->samples()->size() * 0.25f * i * hue);
            ac->fade_out()->set(ac->samples()->size() * 0.25f * i * hue);
            ac->head_cut()->set(ac->samples()->size() * 0.25f * i * hue);
            ac->tail_cut()->set(ac->samples()->size() * 0.25f * i * hue);
            ac->play_position()->set(ac->samples()->size() * 0.85f);
        }

        for (size_t i=0; i<5; ++i)
        {
            MTEST_ASSERT(id.fmt_ascii("label%d", int(i)));

            as->label_visibility(i)->set(true);
            as->label_layout(i)->set_align(layout_x[i], layout_y[i]);
            as->label(i)->set(&id);
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
        tk::AudioEnvelope *ae = NULL;
        size_t vid = 0;

        // Initialize window
        MTEST_ASSERT(init_widget(wnd, vh, "window") == STATUS_OK);
        MTEST_ASSERT(wnd->title()->set_raw("Test audio sample") == STATUS_OK);
        MTEST_ASSERT(wnd->role()->set_raw("audiosample_test") == STATUS_OK);
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
        grid->rows()->set(2);
        grid->columns()->set(2);
        grid->orientation()->set_horizontal();
        grid->hspacing()->set(2);
        grid->vspacing()->set(2);

        // Create audio envelope widgets
        LSPString id;

        for (size_t i=0; i<4; ++i)
        {
            // Create audio envelope with points enabled
            MTEST_ASSERT(id.fmt_ascii("audioenvelope-%d", int(vid++)));
            MTEST_ASSERT(ae = new tk::AudioEnvelope(dpy));
            MTEST_ASSERT(init_widget(ae, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(ae));
            MTEST_ASSERT(grid->add(ae) == STATUS_OK);

            ae->break_enabled()->set(i & 1);
            ae->hold_enabled()->set(i & 2);
            ae->border_flat()->set(i & 1);
            ae->border_radius()->set((i & 2) ? 0 : 12);
            ae->constraints()->set_min(160, 100);
            ae->ipadding()->set(ae->point_size()->get());
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

