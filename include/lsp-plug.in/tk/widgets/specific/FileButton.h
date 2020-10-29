/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 7 сент. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_FILEBUTTON_H_
#define LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_FILEBUTTON_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * File button: widget for saving/loading files and showing the progress
         */
        class FileButton: public Widget
        {
            public:
                static const w_class_t    metadata;

            private:
                FileButton & operator = (const FileButton &);

            protected:
                enum flags_t
                {
                    FB_LBUTTON      = 1 << 0,
                    FB_RBUTTON      = 1 << 1,
                    FB_DOWN         = 1 << 2
                };

            protected:
                prop::RangeFloat        sValue;             // The actual progress value
                prop::String            sText;              // Text to display
                prop::StringList        sTextList;          // Possible text values used for size estimation
                prop::Font              sFont;              // Font parameters
                prop::TextLayout        sTextLayout;        // Text layout
                prop::Padding           sTextPadding;       // Text padding
                prop::SizeConstraints   sConstraints;       // Size constraints
                prop::Color             sColor;             // Color
                prop::Color             sInvColor;          // Progress color
                prop::Color             sLineColor;         // Color of lines
                prop::Color             sInvLineColor;      // Inverse color of lines
                prop::Color             sTextColor;         // Text color
                prop::Color             sInvTextColor;      // Progress color for text
                prop::WidgetPtr<Menu>   sPopup;             // Popup Menu

                size_t                  nBMask;             // Mouse button state
                size_t                  nXFlags;            // Button flags
                ws::rectangle_t         sButton;            // Area for button

            protected:
                static status_t     slot_on_submit(Widget *sender, void *ptr, void *data);

            protected:
                virtual void        size_request(ws::size_limit_t *r);
                virtual void        realize(const ws::rectangle_t *r);
                virtual void        property_changed(Property *prop);

                void                draw_button(ws::ISurface *s, lsp::Color &col, lsp::Color &text, lsp::Color & line);
                status_t            handle_mouse_move(const ws::event_t *ev);

            public:
                explicit FileButton(Display *dpy);
                virtual ~FileButton();

                virtual status_t            init();

            public:
                LSP_TK_PROPERTY(RangeFloat,             value,              &sValue);
                LSP_TK_PROPERTY(String,                 text,               &sText);
                LSP_TK_PROPERTY(StringList,             text_list,          &sTextList);
                LSP_TK_PROPERTY(Font,                   font,               &sFont);
                LSP_TK_PROPERTY(TextLayout,             text_layout,        &sTextLayout);
                LSP_TK_PROPERTY(Padding,                text_padding,       &sTextPadding);
                LSP_TK_PROPERTY(SizeConstraints,        constraints,        &sConstraints);
                LSP_TK_PROPERTY(Color,                  color,              &sColor);
                LSP_TK_PROPERTY(Color,                  inv_color,          &sInvColor);
                LSP_TK_PROPERTY(Color,                  line_color,         &sLineColor);
                LSP_TK_PROPERTY(Color,                  inv_line_color,     &sInvLineColor);
                LSP_TK_PROPERTY(Color,                  text_color,         &sTextColor);
                LSP_TK_PROPERTY(Color,                  inv_text_color,     &sInvTextColor);
                LSP_TK_PROPERTY(WidgetPtr<Menu>,        popup,              &sPopup);

            public:
                virtual void        draw(ws::ISurface *s);

                virtual status_t    on_mouse_down(const ws::event_t *e);

                virtual status_t    on_mouse_up(const ws::event_t *e);

                virtual status_t    on_mouse_move(const ws::event_t *e);

                virtual status_t    on_submit();
        };
    }
}

#endif /* LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_FILEBUTTON_H_ */
