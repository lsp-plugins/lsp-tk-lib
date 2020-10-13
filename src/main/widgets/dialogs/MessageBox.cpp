/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 13 окт. 2020 г.
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

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t MessageBox::metadata        = { "MessageBox", &MessageBox::metadata };

        MessageBox::MessageBox(Display *dpy):
            Window(dpy),
            sHeading(dpy),
            sMessage(dpy),
            sVBox(dpy),
            sBtnBox(dpy),
            vButtons(&sProperties, &sIListener)
        {
            pClass          = &metadata;
        }

        MessageBox::~MessageBox()
        {
            do_destroy();
        }

        void MessageBox::do_destroy()
        {
            sBtnBox.destroy();
            sVBox.destroy();
            sMessage.destroy();
            sHeading.destroy();
        }

        void MessageBox::destroy()
        {
            Window::destroy();
            do_destroy();
        }

        status_t MessageBox::init()
        {
            status_t res    = Window::init();
            if (res != STATUS_OK)
                return res;

            return STATUS_OK;
        }

        status_t MessageBox::slot_on_button_submit(Widget *sender, void *ptr, void *data)
        {
            // Hide message box
            MessageBox *mbox = widget_ptrcast<MessageBox>(ptr);
            if (mbox != NULL)
                mbox->visibility()->set(false);

            return STATUS_OK;
        }

        void MessageBox::on_add_item(void *obj, Property *prop, Widget *w)
        {
            MessageBox *_this = widget_ptrcast<MessageBox>(obj);
            if (_this == NULL)
                return;

            ssize_t index = _this->vButtons.index_of(w);
            if (index >= 0)
                _this->sBtnBox.add(w); // TODO: replace with code below
//                _this->sBtnBox.items()->insert(index, w);
        }

        void MessageBox::on_remove_item(void *obj, Property *prop, Widget *w)
        {
            MessageBox *_this = widget_ptrcast<MessageBox>(obj);
            if (_this == NULL)
                return;

            // Remove button from list
            _this->sBtnBox.remove(w);
        }

        status_t MessageBox::add(const char *text, event_handler_t *handler, void *arg)
        {
            LSPString tmp;
            if (!tmp.set_utf8(text))
                return STATUS_NO_MEM;
            return add(&tmp, handler, arg);
        }

        status_t MessageBox::add(const LSPString *text, event_handler_t *handler, void *arg)
        {
            Button *btn = new Button(pDisplay);
            if (btn == NULL)
                return STATUS_NO_MEM;

            status_t res = btn->init();
            if (res == STATUS_OK)
                res = btn->text()->set_raw(text);
            if ((res == STATUS_OK) && (handler != NULL))
                btn->slot(SLOT_SUBMIT)->bind(*handler, arg);
            if (res == STATUS_OK)
                res = vButtons.madd(btn);

            if (res != STATUS_OK)
            {
                btn->destroy();
                delete btn;
            }

            return res;
        }

        status_t MessageBox::add(const String *text, event_handler_t *handler, void *arg)
        {
            Button *btn = new Button(pDisplay);
            if (btn == NULL)
                return STATUS_NO_MEM;

            status_t res = btn->init();
            if (res == STATUS_OK)
                res = btn->text()->set(text);
            if ((res == STATUS_OK) && (handler != NULL))
                btn->slot(SLOT_SUBMIT)->bind(*handler, arg);
            if (res == STATUS_OK)
                res = vButtons.madd(btn);

            if (res != STATUS_OK)
            {
                btn->destroy();
                delete btn;
            }

            return res;
        }

        status_t MessageBox::add(Button *btn)
        {
            return (btn != NULL) ? vButtons.add(btn) : STATUS_INVALID_VALUE;
        }

        status_t MessageBox::madd(Button *btn)
        {
            return (btn != NULL) ? vButtons.madd(btn) : STATUS_INVALID_VALUE;
        }

        status_t MessageBox::add(Widget *widget)
        {
            Button *item  = widget_cast<Button>(widget);
            return (item != NULL) ? vButtons.add(item) : STATUS_BAD_TYPE;
        }

        status_t MessageBox::remove(Widget *widget)
        {
            Button *item  = widget_cast<Button>(widget);
            return (item != NULL) ? vButtons.premove(item) : STATUS_BAD_TYPE;
        }

        status_t MessageBox::remove_all()
        {
            vButtons.clear();
            return STATUS_OK;
        }
    }
}

