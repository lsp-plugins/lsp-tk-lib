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

#ifndef LSP_PLUG_IN_TK_WIDGETS_DIALOGS_MESSAGEBOX_H_
#define LSP_PLUG_IN_TK_WIDGETS_DIALOGS_MESSAGEBOX_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
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
                Style                       sBtnStyle;

                prop::WidgetList<Button>    vButtons;
                prop::CollectionListener    sBtnListener;
                prop::SizeConstraints       sBtnConstraints;

            public:
                explicit MessageBox(Display *dpy);
                virtual ~MessageBox();

                virtual status_t                init();
                virtual void                    destroy();

            protected:
                static status_t                 slot_on_button_submit(Widget *sender, void *ptr, void *data);
                static void                     on_add_item(void *obj, Property *prop, void *w);
                static void                     on_remove_item(void *obj, Property *prop, void *w);
                void                            do_destroy();

            public:
                LSP_TK_PROPERTY(Label,                  heading_widget,             &sHeading)
                LSP_TK_PROPERTY(Label,                  message_widget,             &sMessage)
                LSP_TK_PROPERTY(String,                 heading,                    sHeading.text())
                LSP_TK_PROPERTY(String,                 message,                    sMessage.text())
                LSP_TK_PROPERTY(Boolean,                heading_visibiity,          sHeading.visibility())
                LSP_TK_PROPERTY(Boolean,                message_visibiity,          sMessage.visibility())
                LSP_TK_PROPERTY(WidgetList<Button>,     buttons,                    &vButtons)
                LSP_TK_PROPERTY(SizeConstraints,        button_constraints,         &sBtnConstraints)
                LSP_TK_PROPERTY(Integer,                button_spacing,             sBtnBox.spacing())
                LSP_TK_PROPERTY(Padding,                message_padding,            sMessage.padding())

            public:
                virtual status_t                add(const char *text, event_handler_t handler = NULL, void *arg = NULL);
                virtual status_t                add(const LSPString *text, event_handler_t handler = NULL, void *arg = NULL);
                virtual status_t                add(const String *text, event_handler_t handler = NULL, void *arg = NULL);
                virtual status_t                add(Button *btn);
                virtual status_t                madd(Button *btn);
                virtual status_t                add(Widget *widget);
                virtual status_t                remove(Widget *widget);
                virtual status_t                remove_all();
        };

        STYLE_INITIALIZER_DEF(MessageBox, Window);
    }
}

#endif /* LSP_PLUG_IN_TK_WIDGETS_DIALOGS_MESSAGEBOX_H_ */
