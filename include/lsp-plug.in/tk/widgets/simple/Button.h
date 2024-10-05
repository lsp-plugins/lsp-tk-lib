/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 21 июн. 2017 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_BUTTON_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_BUTTON_H_

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
            LSP_TK_STYLE_DEF_BEGIN(Button, Widget)
                prop::Color             sColor;
                prop::Color             sTextColor;
                prop::Color             sBorderColor;
                prop::Color             sDownColor;
                prop::Color             sTextDownColor;
                prop::Color             sBorderDownColor;
                prop::Color             sHoverColor;
                prop::Color             sTextHoverColor;
                prop::Color             sBorderHoverColor;
                prop::Color             sDownHoverColor;
                prop::Color             sTextDownHoverColor;
                prop::Color             sBorderDownHoverColor;
                prop::Color             sHoleColor;

                prop::Font              sFont;
                prop::String            sText;
                prop::TextAdjust        sTextAdjust;
                prop::SizeConstraints   sConstraints;
                prop::TextLayout        sTextLayout;
                prop::ButtonMode        sMode;
                prop::Boolean           sDown;
                prop::Boolean           sDownColors;
                prop::Integer           sLed;
                prop::Integer           sBorderSize;
                prop::Integer           sBorderPressedSize;
                prop::Integer           sBorderDownSize;
                prop::Boolean           sEditable;
                prop::Boolean           sHole;
                prop::Boolean           sFlat;
                prop::Boolean           sTextClip;
                prop::Padding           sTextPadding;
                prop::Boolean           sHover;
                prop::Boolean           sGradient;
                prop::Position          sTextShift;
                prop::Position          sTextDownShift;
                prop::Position          sTextPressedShift;
            LSP_TK_STYLE_DEF_END
        }

        /**
         * Button control widget
         */
        class Button: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum state_t
                {
                    S_PRESSED   = (1 << 0),
                    S_TOGGLED   = (1 << 1),
                    S_OUT       = (1 << 2),
                    S_LED       = (1 << 3),
                    S_TRIGGER   = (1 << 4),
                    S_TOGGLE    = (1 << 5),
                    S_DOWN      = (1 << 6),
                    S_EDITABLE  = (1 << 7),
                    S_HOLE      = (1 << 8),
                    S_FLAT      = (1 << 9),
                    S_HOVER     = (1 << 10)
                };

                typedef struct estimation_t
                {
                    LSPString text;
                    float scaling;
                    float fscaling;
                    ssize_t min_width;
                    ssize_t min_height;
                    ws::font_parameters_t fp;
                    ws::text_parameters_t tp;
                } estimation_t;

            protected:
                size_t                  nState;
                size_t                  nBMask;
                size_t                  nChanges;
                ws::rectangle_t         sButton;

                prop::Color             sColor;
                prop::Color             sTextColor;
                prop::Color             sBorderColor;
                prop::Color             sDownColor;
                prop::Color             sTextDownColor;
                prop::Color             sBorderDownColor;
                prop::Color             sHoverColor;
                prop::Color             sTextHoverColor;
                prop::Color             sBorderHoverColor;
                prop::Color             sDownHoverColor;
                prop::Color             sTextDownHoverColor;
                prop::Color             sBorderDownHoverColor;
                prop::Color             sHoleColor;

                prop::Font              sFont;
                prop::String            sText;
                prop::TextAdjust        sTextAdjust;
                prop::SizeConstraints   sConstraints;
                prop::TextLayout        sTextLayout;
                prop::ButtonMode        sMode;
                prop::Boolean           sDown;
                prop::Boolean           sDownColors;
                prop::Integer           sLed;
                prop::Integer           sBorderSize;
                prop::Integer           sBorderPressedSize;
                prop::Integer           sBorderDownSize;
                prop::Boolean           sEditable;
                prop::Boolean           sHole;
                prop::Boolean           sFlat;
                prop::Boolean           sTextClip;
                prop::Padding           sTextPadding;
                prop::Boolean           sHover;
                prop::Boolean           sGradient;
                prop::Position          sTextShift;
                prop::Position          sTextDownShift;
                prop::Position          sTextPressedShift;

                lltl::parray<prop::String>  vEstimations;   // Estimation string

            protected:
                void                update_mode(button_mode_t mode);
                void                estimate_string_size(estimation_t *e, tk::String *s);

                static status_t     slot_on_change(Widget *sender, void *ptr, void *data);
                static status_t     slot_on_submit(Widget *sender, void *ptr, void *data);
                static ws::IGradient   *create_gradient(ws::ISurface *s, ws::rectangle_t &r, size_t pressed, float radius);


                prop::Color        &select4(prop::Color &dfl, prop::Color &hover, prop::Color &down, prop::Color &hover_down);
                prop::Color        &select_color();
                prop::Color        &select_text_color();
                prop::Color        &select_border_color();

            protected:
                virtual void        size_request(ws::size_limit_t *r) override;
                virtual void        realize(const ws::rectangle_t *r) override;
                virtual void        property_changed(Property *prop) override;

            public:
                explicit Button(Display *dpy);
                Button(const Button &) = delete;
                Button(Button &&) = delete;
                virtual ~Button() override;

                Button & operator = (const Button &) = delete;
                Button & operator = (Button &&) = delete;

                virtual status_t                init() override;

            public:
                void                            clear_text_estimations();
                tk::String                     *add_text_estimation();

            public:
                LSP_TK_PROPERTY(Color,              color,              &sColor)
                LSP_TK_PROPERTY(Color,              text_color,         &sTextColor)
                LSP_TK_PROPERTY(Color,              border_color,       &sBorderColor)
                LSP_TK_PROPERTY(Color,              down_color,         &sDownColor)
                LSP_TK_PROPERTY(Color,              text_down_color,    &sTextDownColor)
                LSP_TK_PROPERTY(Color,              border_down_color,  &sBorderDownColor)
                LSP_TK_PROPERTY(Color,              hover_color,        &sHoverColor)
                LSP_TK_PROPERTY(Color,              text_hover_color,   &sTextHoverColor)
                LSP_TK_PROPERTY(Color,              border_hover_color, &sBorderHoverColor)
                LSP_TK_PROPERTY(Color,              down_hover_color,        &sDownHoverColor)
                LSP_TK_PROPERTY(Color,              text_down_hover_color,   &sTextDownHoverColor)
                LSP_TK_PROPERTY(Color,              border_down_hover_color, &sBorderDownHoverColor)
                LSP_TK_PROPERTY(Color,              hole_color,         &sHoleColor)

                LSP_TK_PROPERTY(Font,               font,               &sFont)
                LSP_TK_PROPERTY(String,             text,               &sText)
                LSP_TK_PROPERTY(TextAdjust,         text_adjust,        &sTextAdjust)
                LSP_TK_PROPERTY(SizeConstraints,    constraints,        &sConstraints)
                LSP_TK_PROPERTY(ButtonMode,         mode,               &sMode)
                LSP_TK_PROPERTY(Boolean,            down,               &sDown)
                LSP_TK_PROPERTY(Boolean,            down_colors,        &sDownColors)
                LSP_TK_PROPERTY(Integer,            led,                &sLed)
                LSP_TK_PROPERTY(Integer,            border_size,        &sBorderSize)
                LSP_TK_PROPERTY(Integer,            border_pressed_size,&sBorderPressedSize)
                LSP_TK_PROPERTY(Integer,            border_down_size,   &sBorderDownSize)
                LSP_TK_PROPERTY(Boolean,            editable,           &sEditable)
                LSP_TK_PROPERTY(Boolean,            hole,               &sHole)
                LSP_TK_PROPERTY(Boolean,            flat,               &sFlat)
                LSP_TK_PROPERTY(TextLayout,         text_layout,        &sTextLayout)
                LSP_TK_PROPERTY(Boolean,            text_clip,          &sTextClip)
                LSP_TK_PROPERTY(Padding,            text_padding,       &sTextPadding)
                LSP_TK_PROPERTY(Boolean,            hover,              &sHover)
                LSP_TK_PROPERTY(Boolean,            gradient,           &sGradient)

            public:
                virtual void        draw(ws::ISurface *s) override;
                virtual status_t    on_mouse_down(const ws::event_t *e) override;
                virtual status_t    on_mouse_up(const ws::event_t *e) override;
                virtual status_t    on_mouse_move(const ws::event_t *e) override;
                virtual status_t    on_mouse_out(const ws::event_t *e) override;

            public:
                virtual status_t    on_change();
                virtual status_t    on_submit();
        };

    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_BUTTON_H_ */
