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
        namespace style
        {
            //-----------------------------------------------------------------
            // MessageBox style
            LSP_TK_STYLE_IMPL_BEGIN(MessageBox, Window)
                // Override
                sPadding.set(16);
                sBorderStyle.set(ws::BS_DIALOG);
                sActions.set_actions(ws::WA_DIALOG);
                sLayout.set_scale(1.0f);
                sConstraints.set_all(0);
                // Commit
                sPadding.override();
                sBorderStyle.override();
                sActions.override();
                sLayout.override();
                sConstraints.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(MessageBox, "MessageBox");

            //-----------------------------------------------------------------
            // MessageBox::VBox style
            LSP_TK_STYLE_DEF_BEGIN(MessageBox__VBox, Box)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(MessageBox__VBox, Box)
                // Override
                sSpacing.set(8);
                // Commit
                sSpacing.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(MessageBox__VBox, "MessageBox::VBox");

            //-----------------------------------------------------------------
            // MessageBox::Heading style
            LSP_TK_STYLE_DEF_BEGIN(MessageBox__Heading, Label)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(MessageBox__Heading, Label)
                // Override
                sFont.set_size(16.0f);
                sFont.set_bold(true);
                sAllocation.set_fill(true);
                sTextLayout.set(-1.0f, 0.0f);
                // Commit
                sFont.override();
                sAllocation.override();
                sTextLayout.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(MessageBox__Heading, "MessageBox::Heading");

            //-----------------------------------------------------------------
            // MessageBox::Message style
            LSP_TK_STYLE_DEF_BEGIN(MessageBox__Message, Label)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(MessageBox__Message, Label)
                // Override
                sAllocation.set(true, true);
                sTextLayout.set(-1.0f, 0.0f);
                sPadding.set_bottom(8);
                // Commit
                sAllocation.override();
                sTextLayout.override();
                sPadding.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(MessageBox__Message, "MessageBox::Message");

            //-----------------------------------------------------------------
            // MessageBox::ButtonAlign style
            LSP_TK_STYLE_DEF_BEGIN(MessageBox__ButtonAlign, Align)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(MessageBox__ButtonAlign, Align)
                // Override
                sLayout.set(0.0f, 0.0f);
                // Commit
                sLayout.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(MessageBox__ButtonAlign, "MessageBox::ButtonAlign");

            //-----------------------------------------------------------------
            // MessageBox::ButtonBox style
            LSP_TK_STYLE_DEF_BEGIN(MessageBox__ButtonBox, Box)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(MessageBox__ButtonBox, Box)
                // Override
                sSpacing.set(8);
                // Commit
                sSpacing.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(MessageBox__ButtonBox, "MessageBox::ButtonBox");

            //-----------------------------------------------------------------
            // MessageBox::Button style
            LSP_TK_STYLE_DEF_BEGIN(MessageBox__Button, Button)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(MessageBox__Button, Button)
                // Override
                sConstraints.set(96, 0, 0, 0);
                // Commit
                sConstraints.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(MessageBox__Button, "MessageBox::Button");
        }

        const w_class_t MessageBox::metadata        = { "MessageBox", &Window::metadata };

        MessageBox::MessageBox(Display *dpy):
            Window(dpy),
            sHeading(dpy),
            sMessage(dpy),
            sVBox(dpy),
            sBtnAlign(dpy),
            sBtnBox(dpy),
            vButtons(&sProperties, &sBtnListener)
        {
            pVBox           = NULL;
            pHeadingStyle   = NULL;
            pMessageStyle   = NULL;
            pBtnAlign       = NULL;
            pBtnBox         = NULL;
            pBtnStyle       = NULL;

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

            // Init Styles for nested widgets
            pVBox           = pDisplay->schema()->get("MessageBox::VBox");
            if (pVBox == NULL)
                return STATUS_BAD_STATE;
            pHeadingStyle   = pDisplay->schema()->get("MessageBox::Heading");
            if (pHeadingStyle == NULL)
                return STATUS_BAD_STATE;
            pMessageStyle   = pDisplay->schema()->get("MessageBox::Message");
            if (pMessageStyle == NULL)
                return STATUS_BAD_STATE;
            pBtnAlign       = pDisplay->schema()->get("MessageBox::ButtonAlign");
            if (pBtnAlign == NULL)
                return STATUS_BAD_STATE;
            pBtnBox         = pDisplay->schema()->get("MessageBox::ButtonBox");
            if (pBtnBox == NULL)
                return STATUS_BAD_STATE;
            pBtnStyle       = pDisplay->schema()->get("MessageBox::Button");
            if (pBtnStyle == NULL)
                return STATUS_BAD_STATE;

            // Bind
            sVSpacing.bind("spacing", pVBox);
            sHeadingVisibility.bind("visible", pHeadingStyle);
            sMessageVisibility.bind("visible", pMessageStyle);
            sMessagePadding.bind("padding", pMessageStyle);
            sBtnLayout.bind("layout", pBtnAlign);
            sBtnSpacing.bind("spacing", pBtnBox);
            sBtnConstraints.bind("size.constraints", pBtnStyle);

            // Initialize widgets
            LSP_STATUS_ASSERT(sHeading.init());
            LSP_STATUS_ASSERT(sHeading.style()->inject_parent(pHeadingStyle));
            LSP_STATUS_ASSERT(sMessage.init());
            LSP_STATUS_ASSERT(sMessage.style()->inject_parent(pMessageStyle));

            // Initialize containers
            LSP_STATUS_ASSERT(sVBox.init());
            LSP_STATUS_ASSERT(sVBox.style()->inject_parent(pVBox));
            sVBox.orientation()->set_vertical();

            LSP_STATUS_ASSERT(sBtnAlign.init());
            LSP_STATUS_ASSERT(sBtnAlign.style()->inject_parent(pBtnAlign));

            LSP_STATUS_ASSERT(sBtnBox.init());
            LSP_STATUS_ASSERT(sBtnBox.style()->inject_parent(pBtnBox));
            sBtnBox.orientation()->set_horizontal();

            // Initialize structure
            LSP_STATUS_ASSERT(sBtnAlign.add(&sBtnBox));
            LSP_STATUS_ASSERT(sVBox.add(&sHeading));
            LSP_STATUS_ASSERT(sVBox.add(&sMessage));
            LSP_STATUS_ASSERT(sVBox.add(&sBtnAlign));

            // Add child
            LSP_STATUS_ASSERT(Window::add(&sVBox));

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

            btn->style()->inject_parent(_this->pBtnStyle);

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

            btn->style()->remove_parent(_this->pBtnStyle);

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

