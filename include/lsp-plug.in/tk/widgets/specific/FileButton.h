/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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
        // Style definition
        namespace style
        {
            typedef struct FileButtonColors
            {
                prop::Color             sColor;             // Color
                prop::Color             sInvColor;          // Progress color
                prop::Color             sBorderColor;       // Border Color
                prop::Color             sInvBorderColor;    // Inverse color of border
                prop::Color             sLineColor;         // Color of lines
                prop::Color             sInvLineColor;      // Inverse color of lines
                prop::Color             sTextColor;         // Text color
                prop::Color             sInvTextColor;      // Progress color for text

                void listener(tk::prop::Listener *listener);
                bool property_changed(Property *prop);
            } FileButtonColors;

            enum FileButtonColorState
            {
                FILEBUTTON_NORMAL       = 0,
                FILEBUTTON_INACTIVE     = 1 << 0,

                FILEBUTTON_TOTAL        = 1 << 1
            };

            LSP_TK_STYLE_DEF_BEGIN(FileButton, Widget)
                FileButtonColors        vColors[FILEBUTTON_TOTAL];

                prop::RangeFloat        sValue;             // The actual progress value
                prop::String            sText;              // Text to display
                prop::StringList        sTextList;          // Possible text values used for size estimation
                prop::Font              sFont;              // Font parameters
                prop::TextLayout        sTextLayout;        // Text layout
                prop::Padding           sTextPadding;       // Text padding
                prop::SizeConstraints   sConstraints;       // Size constraints
                prop::Boolean           sGradient;          // Use gradient when drawing
                prop::Boolean           sActive;            // Active state
                prop::Integer           sBorderSize;        // Border size
                prop::Integer           sBorderPressedSize; // Border size when pressed
            LSP_TK_STYLE_DEF_END
        }

        /**
         * File button: widget for saving/loading files and showing the progress
         */
        class FileButton: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum flags_t
                {
                    FB_LBUTTON      = 1 << 0,
                    FB_RBUTTON      = 1 << 1,
                    FB_DOWN         = 1 << 2
                };

                enum btn_flags_t
                {
                    FBTN_0          = style::FILEBUTTON_NORMAL,
                    FBTN_1          = style::FILEBUTTON_INACTIVE,
                    FBTN_TOTAL      = style::FILEBUTTON_TOTAL
                };

            protected:
                style::FileButtonColors vColors[style::FILEBUTTON_TOTAL];
                prop::RangeFloat        sValue;             // The actual progress value
                prop::String            sText;              // Text to display
                prop::StringList        sTextList;          // Possible text values used for size estimation
                prop::Font              sFont;              // Font parameters
                prop::TextLayout        sTextLayout;        // Text layout
                prop::Padding           sTextPadding;       // Text padding
                prop::SizeConstraints   sConstraints;       // Size constraints
                prop::Boolean           sGradient;          // Use gradient when drawing
                prop::Boolean           sActive;            // Active state
                prop::Integer           sBorderSize;        // Border size
                prop::Integer           sBorderPressedSize; // Border size when pressed
                prop::WidgetPtr<Menu>   sPopup;             // Popup Menu

                size_t                  nBMask;             // Mouse button state
                size_t                  nXFlags;            // Button flags
                ws::rectangle_t         sButton;            // Area for button

            protected:
                static status_t     slot_on_submit(Widget *sender, void *ptr, void *data);
                static void         init_points(float *xa, float *ya, const ws::rectangle_t &b);

            protected:
                virtual void        size_request(ws::size_limit_t *r) override;
                virtual void        realize(const ws::rectangle_t *r) override;
                virtual void        property_changed(Property *prop) override;

            protected:
                void                draw_button(ws::ISurface *s, lsp::Color &col, lsp::Color &text, lsp::Color &line, lsp::Color &border);
                status_t            handle_mouse_move(const ws::event_t *ev);
                style::FileButtonColors *select_colors();

            public:
                explicit FileButton(Display *dpy);
                FileButton(const FileButton &) = delete;
                FileButton(FileButton &&) = delete;
                virtual ~FileButton() override;
                FileButton & operator = (const FileButton &) = delete;
                FileButton & operator = (FileButton &&) = delete;

                virtual status_t            init() override;

            public:
                LSP_TK_PROPERTY(Color,                  color,                      &vColors[FBTN_0].sColor);
                LSP_TK_PROPERTY(Color,                  inv_color,                  &vColors[FBTN_0].sInvColor);
                LSP_TK_PROPERTY(Color,                  border_color,               &vColors[FBTN_0].sBorderColor);
                LSP_TK_PROPERTY(Color,                  inv_border_color,           &vColors[FBTN_0].sInvBorderColor);
                LSP_TK_PROPERTY(Color,                  line_color,                 &vColors[FBTN_0].sLineColor);
                LSP_TK_PROPERTY(Color,                  inv_line_color,             &vColors[FBTN_0].sInvLineColor);
                LSP_TK_PROPERTY(Color,                  text_color,                 &vColors[FBTN_0].sTextColor);
                LSP_TK_PROPERTY(Color,                  inv_text_color,             &vColors[FBTN_0].sInvTextColor);
                LSP_TK_PROPERTY(Color,                  inactive_color,             &vColors[FBTN_1].sColor);
                LSP_TK_PROPERTY(Color,                  inactive_inv_color,         &vColors[FBTN_1].sInvColor);
                LSP_TK_PROPERTY(Color,                  inactive_border_color,      &vColors[FBTN_1].sBorderColor);
                LSP_TK_PROPERTY(Color,                  inactive_inv_border_color,  &vColors[FBTN_1].sInvBorderColor);
                LSP_TK_PROPERTY(Color,                  inactive_line_color,        &vColors[FBTN_1].sLineColor);
                LSP_TK_PROPERTY(Color,                  inactive_inv_line_color,    &vColors[FBTN_1].sInvLineColor);
                LSP_TK_PROPERTY(Color,                  inactive_text_color,        &vColors[FBTN_1].sTextColor);
                LSP_TK_PROPERTY(Color,                  inactive_inv_text_color,    &vColors[FBTN_1].sInvTextColor);

                LSP_TK_PROPERTY(RangeFloat,             value,                      &sValue);
                LSP_TK_PROPERTY(String,                 text,                       &sText);
                LSP_TK_PROPERTY(StringList,             text_list,                  &sTextList);
                LSP_TK_PROPERTY(Font,                   font,                       &sFont);
                LSP_TK_PROPERTY(TextLayout,             text_layout,                &sTextLayout);
                LSP_TK_PROPERTY(Padding,                text_padding,               &sTextPadding);
                LSP_TK_PROPERTY(SizeConstraints,        constraints,                &sConstraints);
                LSP_TK_PROPERTY(Boolean,                gradient,                   &sGradient)
                LSP_TK_PROPERTY(Boolean,                active,                     &sActive)
                LSP_TK_PROPERTY(Integer,                border_size,                &sBorderSize)
                LSP_TK_PROPERTY(Integer,                border_pressed_size,        &sBorderPressedSize)
                LSP_TK_PROPERTY(WidgetPtr<Menu>,        popup,                      &sPopup);

            public:
                virtual void        draw(ws::ISurface *s, bool force) override;
                virtual status_t    on_mouse_down(const ws::event_t *e) override;
                virtual status_t    on_mouse_up(const ws::event_t *e) override;
                virtual status_t    on_mouse_move(const ws::event_t *e) override;

            public:
                virtual status_t    on_submit();
        };

    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_FILEBUTTON_H_ */
