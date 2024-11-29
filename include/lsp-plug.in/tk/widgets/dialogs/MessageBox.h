/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_DIALOGS_MESSAGEBOX_H_
#define LSP_PLUG_IN_TK_WIDGETS_DIALOGS_MESSAGEBOX_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_DEF_BEGIN(MessageBox, Window)
            LSP_TK_STYLE_DEF_END
        }

        /**
         * Message box
         */
        class MessageBox: public Window
        {
            public:
                static const w_class_t metadata;

            protected:
                Label                       sHeading;
                Label                       sMessage;
                Box                         sVBox;
                Align                       sBtnAlign;
                Box                         sBtnBox;

                // Inner properties
                prop::WidgetList<Button>    vButtons;
                prop::CollectionListener    sBtnListener;

                // Foreign properties
                prop::Integer               sVSpacing;
                prop::Boolean               sHeadingVisibility;
                prop::Boolean               sMessageVisibility;
                prop::Padding               sMessagePadding;
                prop::SizeConstraints       sBtnConstraints;
                prop::Integer               sBtnSpacing;
                prop::Layout                sBtnLayout;

                // Overrided styles
                Style                      *pVBox;
                Style                      *pHeadingStyle;
                Style                      *pMessageStyle;
                Style                      *pBtnAlign;
                Style                      *pBtnBox;
                Style                      *pBtnStyle;

            public:
                explicit MessageBox(Display *dpy);
                MessageBox(const MessageBox &) = delete;
                MessageBox(MessageBox &&) = delete;
                virtual ~MessageBox() override;
                MessageBox & operator = (const MessageBox &) = delete;
                MessageBox & operator = (MessageBox &&) = delete;

                virtual status_t                init() override;
                virtual void                    destroy() override;

            protected:
                static status_t                 slot_on_button_submit(Widget *sender, void *ptr, void *data);
                static status_t                 slot_on_dialog_escape(Widget *sender, void *ptr, void *data);
                static status_t                 slot_on_dialog_return(Widget *sender, void *ptr, void *data);

            protected:
                static void                     on_add_item(void *obj, Property *prop, void *w);
                static void                     on_remove_item(void *obj, Property *prop, void *w);
                void                            do_destroy();

            public:
                LSP_TK_PROPERTY(Label,                  heading_widget,             &sHeading)
                LSP_TK_PROPERTY(String,                 heading,                    sHeading.text())
                LSP_TK_PROPERTY(Boolean,                heading_visibiity,          &sHeadingVisibility)

                LSP_TK_PROPERTY(Label,                  message_widget,             &sMessage)
                LSP_TK_PROPERTY(String,                 message,                    sMessage.text())
                LSP_TK_PROPERTY(Boolean,                message_visibiity,          &sMessageVisibility)
                LSP_TK_PROPERTY(Padding,                message_padding,            &sMessagePadding)

                LSP_TK_PROPERTY(WidgetList<Button>,     buttons,                    &vButtons)
                LSP_TK_PROPERTY(SizeConstraints,        button_constraints,         &sBtnConstraints)
                LSP_TK_PROPERTY(Integer,                button_spacing,             &sBtnSpacing)
                LSP_TK_PROPERTY(Layout,                 button_layout,              &sBtnLayout)

            public: // tk::Widget
                virtual status_t                add(Widget *widget) override;
                virtual status_t                remove(Widget *widget) override;
                virtual status_t                remove_all() override;

            public:
                virtual status_t                add(const char *text, event_handler_t handler = NULL, void *arg = NULL);
                virtual status_t                add(const LSPString *text, event_handler_t handler = NULL, void *arg = NULL);
                virtual status_t                add(const String *text, event_handler_t handler = NULL, void *arg = NULL);
                virtual status_t                add(Button *btn);
                virtual status_t                madd(Button *btn);
        };
    } /* namespace lsp */
} /* namespace tk */

#endif /* LSP_PLUG_IN_TK_WIDGETS_DIALOGS_MESSAGEBOX_H_ */
