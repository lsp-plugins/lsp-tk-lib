/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 5 мая 2020 г.
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

MTEST_BEGIN("tk.sys", win_actions)

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

    static status_t slot_state(tk::Widget *sender, void *ptr, void *data)
    {
        tk::Window *wnd = tk::widget_cast<tk::Window>(sender);
        test_type_t *_this = static_cast<test_type_t *>(ptr);

        _this->printf("Window state is now: %d\n", int(wnd->state()->get()));
        return STATUS_OK;
    }

    static status_t slot_minimize(tk::Widget *sender, void *ptr, void *data)
    {
        tk::Window *wnd = static_cast<tk::Window *>(ptr);
        wnd->state()->set_minimized();
        return STATUS_OK;
    }

    static status_t slot_maximize(tk::Widget *sender, void *ptr, void *data)
    {
        tk::Window *wnd = static_cast<tk::Window *>(ptr);
        wnd->state()->set_maximized();
        return STATUS_OK;
    }

    static status_t slot_restore(tk::Widget *sender, void *ptr, void *data)
    {
        tk::Window *wnd = static_cast<tk::Window *>(ptr);
        wnd->state()->set_normal();
        return STATUS_OK;
    }

    MTEST_MAIN
    {
        tk::Display *dpy = new tk::Display();
        MTEST_ASSERT(dpy != NULL);
        lsp_finally { delete dpy; };

        MTEST_ASSERT(dpy->init(0, NULL) == STATUS_OK);

        // Create widgets
        tk::Window *wnd = new tk::Window(dpy);
        lsp_finally {
            wnd->destroy();
            delete wnd;
        };
        tk::Box *box = new tk::Box(dpy);
        lsp_finally {
            box->destroy();
            delete box;
        };
        tk::Button *minimize = new tk::Button(dpy);
        lsp_finally {
            minimize->destroy();
            delete minimize;
        };
        tk::Button *maximize = new tk::Button(dpy);
        lsp_finally {
            maximize->destroy();
            delete maximize;
        };
        tk::Button *restore = new tk::Button(dpy);
        lsp_finally {
            restore->destroy();
            delete restore;
        };


        // Initialize window
        MTEST_ASSERT(wnd->init() == STATUS_OK);
        MTEST_ASSERT(wnd->title()->set_raw("Test window") == STATUS_OK);
        MTEST_ASSERT(wnd->role()->set_raw("test") == STATUS_OK);
        wnd->bg_color()->set_rgb(0, 0.75, 1.0);
        wnd->actions()->set_actions(ws::WA_MOVE | ws::WA_RESIZE | ws::WA_CLOSE | ws::WA_MINIMIZE | ws::WA_MAXIMIZE);
        wnd->border_style()->set(ws::BS_SINGLE);
        wnd->constraints()->set(160, 100, 640, 400);
        wnd->size()->set(320, 200);
        wnd->slot(tk::SLOT_CLOSE)->bind(slot_close, this);
        wnd->slot(tk::SLOT_KEY_UP)->bind(slot_key_up, this);
        wnd->slot(tk::SLOT_STATE)->bind(slot_state, this);
        wnd->layout()->set(-0.5, 0.5, 0.5, 0.5);

        // Initialize void widget
        MTEST_ASSERT(box->init() == STATUS_OK);
        box->orientation()->set_horizontal();
        MTEST_ASSERT(wnd->add(box) == STATUS_OK);

        MTEST_ASSERT(minimize->init() == STATUS_OK);
        minimize->text()->set_raw("Minimize");
        minimize->slot(tk::SLOT_SUBMIT)->bind(slot_minimize, wnd);
        MTEST_ASSERT(box->add(minimize) == STATUS_OK);

        MTEST_ASSERT(maximize->init() == STATUS_OK);
        maximize->text()->set_raw("Maximize");
        maximize->slot(tk::SLOT_SUBMIT)->bind(slot_maximize, wnd);
        MTEST_ASSERT(box->add(maximize) == STATUS_OK);

        MTEST_ASSERT(restore->init() == STATUS_OK);
        restore->text()->set_raw("Restore");
        restore->slot(tk::SLOT_SUBMIT)->bind(slot_restore, wnd);
        MTEST_ASSERT(box->add(restore) == STATUS_OK);

        // Show window
        wnd->visibility()->set(true);

        MTEST_ASSERT(dpy->main() == STATUS_OK);
    }

MTEST_END


