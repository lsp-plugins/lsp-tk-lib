/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 23 окт. 2020 г.
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
#include <lsp-plug.in/resource/DirLoader.h>

MTEST_BEGIN("tk.widgets.dialogs", filedialog)
    typedef struct handler_t
    {
        test_type_t    *test;
        char           *label;
        tk::FileDialog *dlg;
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

    static status_t slot_ok_submitted(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("DIALOG OK SUBMITTED: %s\n", h->label);

        LSPString path;
        h->dlg->selected_file()->format(&path);
        h->test->printf(" path: %s\n", path.get_utf8());

        sender->display()->queue_destroy(h->dlg);
        h->dlg      = NULL;
        return STATUS_OK;
    }

    static status_t slot_cancel_submitted(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("DIALOG CANCEL SUBMITTED: %s\n", h->label);

        sender->display()->queue_destroy(h->dlg);
        h->dlg      = NULL;
        return STATUS_OK;
    }

    static void init_dialog(tk::FileDialog *dlg, handler_t *h)
    {
        dlg->init();
        dlg->slots()->bind(tk::SLOT_SUBMIT, slot_ok_submitted, h);
        dlg->slots()->bind(tk::SLOT_CANCEL, slot_cancel_submitted, h);

        tk::FileMask *m;
        m = dlg->filter()->add();
        m->pattern()->set("*.txt");
        m->title()->set_raw("Text files");
        m->extensions()->set_raw(".txt");

        m = dlg->filter()->add();
        m->pattern()->set("*.c|*.cc|*.cpp|*.h|*.hpp");
        m->title()->set_raw("C/C++ files");
        m->extensions()->set_raw(".c:.cc:.cpp:.h:.hpp");

        m = dlg->filter()->add();
        m->pattern()->set("*");
        m->title()->set_raw("All files");
        m->extensions()->set_raw("");

        h->dlg      = dlg;
    }

    static status_t slot_dialog_open(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("BUTTON SUBMITTED: %s\n", h->label);

        tk::FileDialog *dlg = new tk::FileDialog(sender->display());

        init_dialog(dlg, h);
        dlg->mode()->set_open_file();
        dlg->use_confirm()->set(true);
        dlg->confirm_message()->set("messages.file.confirm_open");
        dlg->title()->set_raw("Load from file dialog");
        dlg->show(sender);

        return STATUS_OK;
    }

    static status_t slot_dialog_save(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("BUTTON SUBMITTED: %s\n", h->label);

        tk::FileDialog *dlg = new tk::FileDialog(sender->display());
        init_dialog(dlg, h);
        dlg->use_confirm()->set(true);
        dlg->confirm_message()->set("messages.file.confirm_save");
        dlg->mode()->set_save_file();
        dlg->title()->set_raw("Save to file dialog");
        dlg->show(sender);

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
        h->dlg      = NULL;

        tk::handler_id_t hid;
        hid = w->slots()->bind(tk::SLOT_MOUSE_IN, slot_mouse_in, h);
        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_DOWN, slot_mouse_down, h);
        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_MOVE, slot_mouse_move, h);
        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_UP, slot_mouse_up, h);
        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_CLICK, slot_mouse_click, h);
        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_DBL_CLICK, slot_mouse_dbl_click, h);
        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_TRI_CLICK, slot_mouse_tri_click, h);
        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_OUT, slot_mouse_out, h);
        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_KEY_UP, slot_key_up, h);

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
        // Initialize resources
        const char *res_path = resources();
        printf("Resource path: %s\n", res_path);
        resource::DirLoader loader;
        resource::Environment env;
        loader.set_enforce(true);
        MTEST_ASSERT(loader.set_path(res_path) == STATUS_OK);
        MTEST_ASSERT(env.set(LSP_TK_ENV_DICT_PATH, "i18n") == STATUS_OK);
        MTEST_ASSERT(env.set(LSP_TK_ENV_LANG, "en_US") == STATUS_OK);
//        MTEST_ASSERT(env.set(LSP_TK_ENV_SCHEMA_PATH, "schema/lsp.xml") == STATUS_OK);
        MTEST_ASSERT(env.set(LSP_TK_ENV_SCHEMA_PATH, "schema/lsp_v2.xml") == STATUS_OK);
        MTEST_ASSERT(env.set(LSP_TK_ENV_CONFIG, "lsp-tk-lib") == STATUS_OK);

        // Create display
        tk::display_settings_t dpy_settings;
        dpy_settings.resources      = &loader;
        dpy_settings.environment    = &env;
        tk::Display *dpy = new tk::Display(&dpy_settings);
        MTEST_ASSERT(dpy != NULL);

        MTEST_ASSERT(dpy->init(0, NULL) == STATUS_OK);

        lltl::parray<tk::Widget> widgets;
        tk::Widget *w = NULL;
        tk::Window *wnd = new tk::Window(dpy);
        tk::Grid *grid = NULL;
        tk::Button *btn = NULL;

        // Initialize window
        MTEST_ASSERT(init_widget(wnd, vh, "window") == STATUS_OK);
        MTEST_ASSERT(wnd->title()->set_raw("Test file dialog") == STATUS_OK);
        MTEST_ASSERT(wnd->role()->set_raw("filedialog_test") == STATUS_OK);
        wnd->bg_color()->set_rgb(0, 0.75, 1.0);
        wnd->actions()->set_actions(ws::WA_MOVE | ws::WA_RESIZE | ws::WA_CLOSE);
        wnd->border_style()->set(ws::BS_DIALOG);
        wnd->border_color()->set_rgb(1.0f, 1.0f, 0.0f);
        wnd->constraints()->set(160, 100, 640, 400);
        wnd->size()->set(320, 200);
        wnd->slot(tk::SLOT_CLOSE)->bind(slot_close, this);
        wnd->slot(tk::SLOT_KEY_UP)->bind(slot_key_up, this);
        wnd->pointer()->set(ws::MP_TABLE_CELL);
        wnd->layout()->set(-0.5, 0.5, 0.5, 0.5);
        wnd->position()->set(200, 200);
        MTEST_ASSERT(widgets.push(wnd));

        // Create Grid
        MTEST_ASSERT(grid = new tk::Grid(dpy));
        MTEST_ASSERT(init_widget(grid, vh, "grid") == STATUS_OK);
        MTEST_ASSERT(widgets.push(grid));
        MTEST_ASSERT(wnd->add(grid) == STATUS_OK);
        grid->bg_color()->set_rgb(1.0f, 1.0f, 1.0f);
        grid->padding()->set(8);
        grid->rows()->set(2);
        grid->columns()->set(1);
        grid->orientation()->set_horizontal();
        grid->hspacing()->set(2);
        grid->vspacing()->set(2);

        {
            // Create alignment and child widget
            LSPString id;
            size_t col = 0;
            int wid = 0;

            // Create open dialog button
            MTEST_ASSERT(id.fmt_ascii("button-%d", wid++));
            MTEST_ASSERT(btn = new tk::Button(dpy));
            MTEST_ASSERT(init_widget(btn, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(btn));
            MTEST_ASSERT(grid->add(btn) == STATUS_OK);

            btn->color()->set_rgb24(next_color(col));
            btn->constraints()->set_fixed(64, 32);
            btn->text()->set("actions.dlg.open");
            btn->slots()->bind(tk::SLOT_SUBMIT, slot_dialog_open, vh.last());

            // Create save dialog button
            MTEST_ASSERT(id.fmt_ascii("button-%d", wid++));
            MTEST_ASSERT(btn = new tk::Button(dpy));
            MTEST_ASSERT(init_widget(btn, vh, id.get_ascii()) == STATUS_OK);
            MTEST_ASSERT(widgets.push(btn));
            MTEST_ASSERT(grid->add(btn) == STATUS_OK);

            btn->color()->set_rgb24(next_color(col));
            btn->constraints()->set_fixed(64, 32);
            btn->text()->set("actions.dlg.save");
            btn->slots()->bind(tk::SLOT_SUBMIT, slot_dialog_save, vh.last());
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



