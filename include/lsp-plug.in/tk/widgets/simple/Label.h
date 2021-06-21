/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 6 июл. 2017 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LABEL_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LABEL_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        // Style definition
        namespace style
        {
            LSP_TK_STYLE_DEF_BEGIN(Label, Widget)
                prop::TextLayout            sTextLayout;    // Text layout
                prop::TextAdjust            sTextAdjust;    // Text adjustment
                prop::Font                  sFont;          // Font parameters
                prop::Color                 sColor;         // Font color
                prop::Color                 sHoverColor;    // Hover color
                prop::Boolean               sHover;         // Hover enable
                prop::String                sText;          // Text to display
                prop::SizeConstraints       sConstraints;   // Size constraints
            LSP_TK_STYLE_DEF_END
        }

        class Label: public Widget
        {
            public:
                static const w_class_t    metadata;

            private:
                Label & operator = (const Label &);
                Label(const Label &);

                friend class MultiLabel;

            protected:
                enum state_t
                {
                    F_MOUSE_IN      = 1 << 0,
                    F_MOUSE_DOWN    = 1 << 1,
                    F_MOUSE_IGN     = 1 << 2,
                };

            protected:
                size_t                      nMFlags;
                size_t                      nState;

                prop::TextLayout            sTextLayout;    // Text layout
                prop::TextAdjust            sTextAdjust;    // Text adjustment
                prop::Font                  sFont;          // Font parameters
                prop::Color                 sColor;         // Font color
                prop::Color                 sHoverColor;    // Hover color
                prop::Boolean               sHover;         // Hover enable
                prop::String                sText;          // Text to display
                prop::SizeConstraints       sConstraints;   // Size constraints
                prop::WidgetPtr<Menu>       sPopup;         // Popup menu

            protected:
                static status_t                 slot_on_submit(Widget *sender, void *ptr, void *data);
                static status_t                 slot_on_before_popup(Widget *sender, void *ptr, void *data);
                static status_t                 slot_on_popup(Widget *sender, void *ptr, void *data);

            protected:
                virtual void                    size_request(ws::size_limit_t *r);
                virtual void                    property_changed(Property *prop);

            public:
                explicit Label(Display *dpy);
                virtual ~Label();

                virtual status_t                init();

            public:
                LSP_TK_PROPERTY(TextLayout,         text_layout,        &sTextLayout)
                LSP_TK_PROPERTY(TextAdjust,         text_adjust,        &sTextAdjust)
                LSP_TK_PROPERTY(Font,               font,               &sFont)
                LSP_TK_PROPERTY(Color,              color,              &sColor)
                LSP_TK_PROPERTY(Color,              hover_color,        &sHoverColor)
                LSP_TK_PROPERTY(Boolean,            hover,              &sHover)
                LSP_TK_PROPERTY(String,             text,               &sText)
                LSP_TK_PROPERTY(SizeConstraints,    constraints,        &sConstraints)
                LSP_TK_PROPERTY(WidgetPtr<Menu>,    popup,              &sPopup)

            public:
                virtual void                    draw(ws::ISurface *s);

                virtual status_t                on_mouse_in(const ws::event_t *e);

                virtual status_t                on_mouse_out(const ws::event_t *e);

                virtual status_t                on_mouse_move(const ws::event_t *e);

                virtual status_t                on_mouse_down(const ws::event_t *e);

                virtual status_t                on_mouse_up(const ws::event_t *e);

                virtual status_t                on_before_popup(Menu *menu);

                virtual status_t                on_popup(Menu *menu);

                virtual status_t                on_submit();
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LABEL_H_ */
