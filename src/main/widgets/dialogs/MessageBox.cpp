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
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        STYLE_INITIALIZER_BEGIN(MessageBox, Window);

        STYLE_INITIALIZER_END(MessageBox, "MessageBox");
        LSP_BUILTIN_STYLE(MessageBox);

        const w_class_t MessageBox::metadata        = { "MessageBox", &Window::metadata };

        MessageBox::MessageBox(Display *dpy):
            Window(dpy),
            sHeading(dpy),
            sMessage(dpy),
            sVBox(dpy),
            sBtnAlign(dpy),
            sBtnBox(dpy),
            sBtnStyle(dpy->schema()),
            vButtons(&sProperties, &sBtnListener)
        {
            pClass          = &metadata;
        }

        MessageBox::~MessageBox()
        {
            nFlags     |= FINALIZED;
            do_destroy();
        }

        void MessageBox::do_destroy()
        {
            vButtons.clear();

            sStyle.destroy();
            sBtnAlign.destroy();
            sBtnBox.destroy();
            sVBox.destroy();
            sMessage.destroy();
            sHeading.destroy();
        }

        void MessageBox::destroy()
        {
            nFlags     |= FINALIZED;
            Window::destroy();
            do_destroy();
        }

        status_t MessageBox::init()
        {
            status_t res    = Window::init();
            if (res != STATUS_OK)
                return res;

            // Init listener
            sBtnListener.bind_all(this, on_add_item, on_remove_item);

            // Init button style
            if (res == STATUS_OK)
                res = sBtnStyle.init();
            sBtnConstraints.bind("size.constraints", &sBtnStyle);

            // Override parameters
            Style *sclass = style_class();
            if (sclass != NULL)
            {
                // Overrides
                sSizeConstraints.override(sclass, 0, 0, 0, 0);
            }

            // Initialize widgets
            if (res == STATUS_OK)
                res = sHeading.init();
            if (res == STATUS_OK)
            {
                sHeading.font()->set_size(16);
                sHeading.font()->set_bold(true);
                sHeading.allocation()->set_fill(true);
                sHeading.text_layout()->set(-1.0f, 0.0f);
            }
            if (res == STATUS_OK)
                res = sMessage.init();
            if (res == STATUS_OK)
            {
                sMessage.allocation()->set_fill(true);
                sMessage.allocation()->set_expand(true);
                sMessage.text_layout()->set(-1.0f, 0.0f);
                sMessage.padding()->set_bottom(8);
            }

            // Initialize containers
            if (res == STATUS_OK)
            {
                if ((res = sVBox.init()) == STATUS_OK)
                {
                    sVBox.orientation()->set_vertical();
                    sVBox.spacing()->set(8);
                }
            }
            if (res == STATUS_OK)
            {
                if ((res = sBtnBox.init()) == STATUS_OK)
                {
                    sBtnBox.orientation()->set_horizontal();
                    sBtnBox.spacing()->set(8);
                }
            }
            if (res == STATUS_OK)
            {
                if ((res = sBtnAlign.init()) == STATUS_OK)
                    sBtnAlign.layout()->set(0.0f, 0.0f);
            }

            // Initialize structure
            if (res == STATUS_OK)
                res = sBtnAlign.add(&sBtnBox);
            if (res == STATUS_OK)
                res = sVBox.add(&sHeading);
            if (res == STATUS_OK)
                res = sVBox.add(&sMessage);
            if (res == STATUS_OK)
                res = sVBox.add(&sBtnAlign);

            // Add child
            if (res == STATUS_OK)
                res = Window::add(&sVBox);

            if (res == STATUS_OK)
            {
                padding()->set(16);
                border_style()->set(ws::BS_DIALOG);
                actions()->set(ws::WA_DIALOG);
                layout()->set_scale(1.0f);
            }

            return res;
        }

        status_t MessageBox::slot_on_button_submit(Widget *sender, void *ptr, void *data)
        {
            // Hide message box
            MessageBox *mbox = widget_ptrcast<MessageBox>(ptr);
            if (mbox != NULL)
                mbox->visibility()->set(false);

            return STATUS_OK;
        }

        void MessageBox::on_add_item(void *obj, Property *prop, void *w)
        {
            MessageBox *_this = widget_ptrcast<MessageBox>(obj);
            if (_this == NULL)
                return;

            Button *btn = widget_ptrcast<Button>(w);
            if (btn == NULL)
                return;

            btn->style()->add_parent(&_this->sBtnStyle);

            ssize_t index = _this->vButtons.index_of(btn);
            if (index >= 0)
            {
                btn->slot(SLOT_SUBMIT)->bind(slot_on_button_submit, _this->self());
                _this->sBtnBox.items()->insert(btn, index);
            }
        }

        void MessageBox::on_remove_item(void *obj, Property *prop, void *w)
        {
            MessageBox *_this = widget_ptrcast<MessageBox>(obj);
            if (_this == NULL)
                return;

            Button *btn = widget_ptrcast<Button>(w);
            if (btn == NULL)
                return;

            btn->style()->remove_parent(&_this->sStyle);

            // Remove button from list
            btn->slot(SLOT_SUBMIT)->unbind(slot_on_button_submit, _this->self());
            _this->sBtnBox.remove(btn);
        }

        status_t MessageBox::add(const char *text, event_handler_t handler, void *arg)
        {
            LSPString tmp;
            if (!tmp.set_utf8(text))
                return STATUS_NO_MEM;
            return add(&tmp, handler, arg);
        }

        status_t MessageBox::add(const LSPString *text, event_handler_t handler, void *arg)
        {
            Button *btn = new Button(pDisplay);
            if (btn == NULL)
                return STATUS_NO_MEM;

            status_t res = btn->init();
            if (res == STATUS_OK)
                res = btn->text()->set(text);
            if ((res == STATUS_OK) && (handler != NULL))
                btn->slot(SLOT_SUBMIT)->bind(handler, arg);
            if (res == STATUS_OK)
                res = vButtons.madd(btn);

            if (res != STATUS_OK)
            {
                btn->destroy();
                delete btn;
            }

            return res;
        }

        status_t MessageBox::add(const String *text, event_handler_t handler, void *arg)
        {
            Button *btn = new Button(pDisplay);
            if (btn == NULL)
                return STATUS_NO_MEM;

            status_t res = btn->init();
            if (res == STATUS_OK)
                res = btn->text()->set(text);
            if ((res == STATUS_OK) && (handler != NULL))
                btn->slot(SLOT_SUBMIT)->bind(handler, arg);
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

