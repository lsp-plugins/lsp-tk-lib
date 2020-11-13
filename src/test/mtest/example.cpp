/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 13 нояб. 2020 г.
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

namespace
{
    // Handle the window close request event
    static lsp::status_t slot_close(lsp::tk::Widget *sender, void *ptr, void *data)
    {
        // Obtain the display and leave the main loop
        sender->display()->quit_main();
        return lsp::STATUS_OK;
    }

    // Handle the key release event
    static lsp::status_t slot_key_up(lsp::tk::Widget *sender, void *ptr, void *data)
    {
        // Obtain the toplevel widget (window)
        lsp::tk::Window *wnd = lsp::tk::widget_cast<lsp::tk::Window>(sender->toplevel());
        lsp::ws::event_t *ev = static_cast<lsp::ws::event_t *>(data);

        if ((wnd != NULL) && (ev != NULL) && (ev->nType == lsp::ws::UIE_KEY_UP))
        {
            ::printf("Key up: %c (0x%x)\n", (char)ev->nCode, int(ev->nCode));

            // Get current scaling
            float scaling = wnd->style()->schema()->scaling()->get();

            // Change the scaling
            switch (ev->nCode)
            {
                case '+':
                case lsp::ws::WSK_KEYPAD_ADD:
                    scaling     = lsp::lsp_max(0.25f, scaling + 0.25f);
                    break;
                case '-':
                case lsp::ws::WSK_KEYPAD_SUBTRACT:
                    scaling     = lsp::lsp_max(0.25f, scaling - 0.25f);
                    break;
                default:
                    break;
            }

            // Update scaling of the window
            wnd->style()->schema()->scaling()->set(scaling);
        }
        return lsp::STATUS_OK;
    }

    static lsp::status_t slot_ok_submitted(lsp::tk::Widget *sender, void *ptr, void *data)
    {
        // Avoid widget destroy in event handlers,
        // query widget for removal at the end of main loop iteration
        lsp::tk::MessageBox *mbox = lsp::tk::widget_ptrcast<lsp::tk::MessageBox>(ptr);
        mbox->display()->queue_destroy(mbox);
        return lsp::STATUS_OK;
    }

    static lsp::status_t slot_btn_submit(lsp::tk::Widget *sender, void *ptr, void *data)
    {
        // Create message box
        lsp::tk::MessageBox *mbox = new lsp::tk::MessageBox(sender->display());
        mbox->init();
        mbox->title()->set("Message");
        mbox->heading()->set_raw("Information");
        mbox->message()->set("Button has been clicked");
        mbox->add("OK", slot_ok_submitted, mbox);
        mbox->button_layout()->set_halign(1.0f);

        mbox->button_constraints()->set_fixed(64, 16);

        mbox->show(sender);
        return lsp::STATUS_OK;
    }

    static int example_main(int argc, const char *argv[])
    {
        // Initialize the tookit
        lsp::tk::init(0, NULL);

        // Create the display
        lsp::tk::Display *dpy = new lsp::tk::Display();

        // Initialize the display
        if (dpy->init(0, NULL) != lsp::STATUS_OK)
            return -1;

        // Create widgets
        lsp::tk::Window *wnd     = new lsp::tk::Window(dpy);
        lsp::tk::Box *box        = new lsp::tk::Box(dpy);
        lsp::tk::Label *label    = new lsp::tk::Label(dpy);
        lsp::tk::Button *btn     = new lsp::tk::Button(dpy);

        // Initialize widgets
        wnd->init();
        box->init();
        label->init();
        btn->init();

        // Initialize widgets
        wnd->title()->set_raw("Example window");
        wnd->role()->set_raw("example-window");
        wnd->bg_color()->set_rgb(0.75f, 1.0f, 1.0f);
        wnd->actions()->set_actions(lsp::ws::WA_MOVE | lsp::ws::WA_RESIZE | lsp::ws::WA_CLOSE);
        wnd->border_style()->set(lsp::ws::BS_DIALOG);
        wnd->constraints()->set(160, 100, 640, 400);
        wnd->size()->set(320, 200);
        wnd->pointer()->set(lsp::ws::MP_TABLE_CELL);
        wnd->layout()->set(0.0f, 0.0f, 1.0f, 1.0f);
        wnd->padding()->set(16);

        box->orientation()->set(lsp::tk::O_VERTICAL);
        box->spacing()->set(4);

        label->font()->set_size(16.0f);
        label->font()->set_bold(true);
        label->text()->set_raw("Example\nLabel");
        label->allocation()->set_vexpand(true);

        btn->text()->set("Button");
        btn->color()->set("#008800");
        btn->text_color()->set("#ffff00");
        btn->constraints()->set(96, 24, 96, -1);

        // Initialize widget structure
        wnd->add(box);
        box->add(label);
        box->add(btn);

        // Initialize event handlers
        wnd->slot(lsp::tk::SLOT_CLOSE)->bind(slot_close, NULL);
        wnd->slot(lsp::tk::SLOT_KEY_UP)->bind(slot_key_up, NULL);
        btn->slot(lsp::tk::SLOT_KEY_UP)->bind(slot_key_up, NULL);
        label->slot(lsp::tk::SLOT_KEY_UP)->bind(slot_key_up, NULL);
        box->slot(lsp::tk::SLOT_KEY_UP)->bind(slot_key_up, NULL);
        btn->slot(lsp::tk::SLOT_SUBMIT)->bind(slot_btn_submit, NULL);

        // Show window at the center of screen
        ssize_t sw = 0, sh = 0;
        lsp::ws::rectangle_t wsz;
        dpy->screen_size(0, &sw, &sh);
        wnd->size()->compute(&wsz, wnd->scaling()->get());
        wnd->position()->set((sw - wsz.nWidth)/2, (sh - wsz.nHeight)/2);
        wnd->visibility()->set(true);

        // Enter the main event loop
        dpy->main();

        // Destroy widgets
        btn->destroy();
        label->destroy();
        box->destroy();
        wnd->destroy();

        // Delete widget objects
        delete btn;
        delete label;
        delete box;
        delete wnd;

        // Destroy display
        dpy->destroy();
        delete dpy;

        return 0;
    }
}

MTEST_BEGIN("tk", example)

    MTEST_MAIN
    {
        example_main(argc, argv);
    }

MTEST_END



