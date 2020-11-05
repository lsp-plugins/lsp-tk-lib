/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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
            LSP_TK_STYLE_DEF_BEGIN(Indicator, Widget)
                prop::Color         sColor;         // Color of the indicator
                prop::Color         sTextColor;     // Color of the text
                prop::Integer       sRows;          // Number of rows
                prop::Integer       sColumns;       // Number of columns
                prop::Integer       sShift;         // Text shift
                prop::Integer       sTextGap;       // Text gap for loop
                prop::Boolean       sLoop;          // Loop flag
                prop::String        sText;          // Actual text to display
            LSP_TK_STYLE_DEF_END
        }

        class Indicator: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                prop::Color         sColor;         // Color of the indicator
                prop::Color         sTextColor;     // Color of the text
                prop::Integer       sRows;          // Number of rows
                prop::Integer       sColumns;       // Number of columns
                prop::Integer       sShift;         // Text shift
                prop::Integer       sTextGap;       // Text gap for loop
                prop::Boolean       sLoop;          // Loop flag
                prop::String        sText;          // Actual text to display

            protected:
                void                draw_digit(ws::ISurface *s, float x, float y, size_t state, const lsp::Color &on, const lsp::Color &off);
                uint8_t             get_char(const LSPString *str, size_t index);

            protected:
                virtual void        size_request(ws::size_limit_t *r);
                virtual void        property_changed(Property *prop);

            public:
                explicit            Indicator(Display *dpy);
                virtual             ~Indicator();

                virtual status_t    init();

            public:
                LSP_TK_PROPERTY(Color,              color,              &sColor)
                LSP_TK_PROPERTY(Color,              text_color,         &sTextColor)
                LSP_TK_PROPERTY(Integer,            rows,               &sRows)
                LSP_TK_PROPERTY(Integer,            columns,            &sColumns)
                LSP_TK_PROPERTY(Integer,            text_shift,         &sShift)
                LSP_TK_PROPERTY(Integer,            text_gap,           &sTextGap)
                LSP_TK_PROPERTY(Boolean,            text_loop,          &sLoop)
                LSP_TK_PROPERTY(String,             text,               &sText)

            public:
                virtual void        draw(ws::ISurface *s);
        };

    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPINDICATOR_H_ */
