/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 7 июл. 2017 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_INDICATOR_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_INDICATOR_H_

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
            typedef struct IndicatorColors
            {
                prop::Color         sColor;         // Color of the indicator
                prop::Color         sTextColor;     // Color of the text

                void listener(tk::prop::Listener *listener);
                bool property_changed(Property *prop);
            } IndicatorColors;

            enum IndicatorColorState
            {
                INDICATOR_NORMAL        = 0,
                INDICATOR_INACTIVE      = 1 << 0,

                INDICATOR_TOTAL         = 1 << 1
            };

            LSP_TK_STYLE_DEF_BEGIN(Indicator, Widget)
                IndicatorColors     vColors[INDICATOR_TOTAL];

                prop::Integer       sRows;          // Number of rows
                prop::Integer       sColumns;       // Number of columns
                prop::Integer       sShift;         // Text shift
                prop::Integer       sTextGap;       // Text gap for loop
                prop::Boolean       sLoop;          // Loop flag
                prop::Boolean       sDarkText;      // Enables drawing of the dark text/dark segments
                prop::String        sText;          // Actual text to display
                prop::IndicatorType sType;          // Indicator type
                prop::Boolean       sActive;        // Active/inactive state
                prop::Font          sFont;          // Font properties
                prop::Integer       sSpacing;       // Spacing between digits
                prop::Padding       sIPadding;      // Internal padding
            LSP_TK_STYLE_DEF_END
        }

        class Indicator: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum ind_flags_t
                {
                    IND_0       = style::INDICATOR_NORMAL,
                    IND_1       = style::INDICATOR_INACTIVE,
                    IND_TOTAL   = style::INDICATOR_TOTAL
                };

            protected:
                style::IndicatorColors  vColors[IND_TOTAL];

                prop::Integer           sRows;          // Number of rows
                prop::Integer           sColumns;       // Number of columns
                prop::Integer           sShift;         // Text shift
                prop::Integer           sTextGap;       // Text gap for loop
                prop::Boolean           sLoop;          // Loop flag
                prop::Boolean           sDarkText;      // Enables drawing of the dark text/dark segments
                prop::String            sText;          // Actual text to display
                prop::IndicatorType     sType;          // Indicator type
                prop::Boolean           sActive;        // Active/inactive state
                prop::Font              sFont;          // Font properties
                prop::Integer           sSpacing;       // Spacing between digits
                prop::Padding           sIPadding;      // Internal padding

                ssize_t                 nDWidth;        // Width of the digit
                ssize_t                 nDHeight;       // Height of the digit

            protected:
                void                    draw_digit(ws::ISurface *s, float x, float y, size_t state, const lsp::Color &on, const lsp::Color &off);
                void                    draw_pixel(ws::ISurface *s, float x, float y, char ch, const lsp::Color &on, const lsp::Color &off);
                void                    draw_simple(ws::ISurface *s, float x, float y, char ch, const lsp::Color &on, const ws::font_parameters_t *fp);
                uint8_t                 get_char(const LSPString *str, size_t index);
                void                    calc_digit_size(ssize_t *w, ssize_t *h);
                style::IndicatorColors *select_colors();

            protected:
                virtual void            size_request(ws::size_limit_t *r) override;
                virtual void            property_changed(Property *prop) override;
                virtual void            realize(const ws::rectangle_t *r) override;

            public:
                explicit Indicator(Display *dpy);
                Indicator(const Indicator &) = delete;
                Indicator(Indicator &&) = delete;
                virtual ~Indicator() override;
                Indicator & operator = (const Indicator &) = delete;
                Indicator & operator = (Indicator &&) = delete;

                virtual status_t        init() override;

            public:
                LSP_TK_PROPERTY(Color,              color,                  &vColors[IND_0].sColor)
                LSP_TK_PROPERTY(Color,              text_color,             &vColors[IND_0].sTextColor)
                LSP_TK_PROPERTY(Color,              inactive_color,         &vColors[IND_1].sColor)
                LSP_TK_PROPERTY(Color,              inactive_text_color,    &vColors[IND_1].sTextColor)

                LSP_TK_PROPERTY(Integer,            rows,                   &sRows)
                LSP_TK_PROPERTY(Integer,            columns,                &sColumns)
                LSP_TK_PROPERTY(Integer,            text_shift,             &sShift)
                LSP_TK_PROPERTY(Integer,            text_gap,               &sTextGap)
                LSP_TK_PROPERTY(Boolean,            text_loop,              &sLoop)
                LSP_TK_PROPERTY(Boolean,            dark_text,              &sDarkText)
                LSP_TK_PROPERTY(String,             text,                   &sText)
                LSP_TK_PROPERTY(IndicatorType,      type,                   &sType)
                LSP_TK_PROPERTY(Boolean,            active,                 &sActive)
                LSP_TK_PROPERTY(Font,               font,                   &sFont)
                LSP_TK_PROPERTY(Integer,            spacing,                &sSpacing)
                LSP_TK_PROPERTY(Padding,            ipadding,               &sIPadding)

            public:
                virtual void        draw(ws::ISurface *s) override;
        };

    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPINDICATOR_H_ */
