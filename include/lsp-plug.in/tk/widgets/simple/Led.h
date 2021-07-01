/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 10 июл. 2017 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LED_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LED_H_

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
            LSP_TK_STYLE_DEF_BEGIN(Led, Widget)
                prop::Color             sColor;
                prop::Color             sHoleColor;
                prop::Color             sLedColor;
                prop::Color             sBorderColor;
                prop::Color             sLedBorderColor;
                prop::SizeConstraints   sConstraints;
                prop::Boolean           sOn;
                prop::Boolean           sHole;
                prop::Integer           sLed;
                prop::Boolean           sRound;
                prop::Integer           sBorderSize;
                prop::Boolean           sGradient;
            LSP_TK_STYLE_DEF_END
        }

        class Led: public Widget
        {
            public:
                static const w_class_t    metadata;

            private:
                Led & operator = (const Led &);
                Led(const Led &);

            protected:
                prop::Color             sColor;
                prop::Color             sHoleColor;
                prop::Color             sLedColor;
                prop::Color             sBorderColor;
                prop::Color             sLedBorderColor;
                prop::SizeConstraints   sConstraints;
                prop::Boolean           sOn;
                prop::Boolean           sHole;
                prop::Integer           sLed;
                prop::Boolean           sRound;
                prop::Integer           sBorderSize;
                prop::Boolean           sGradient;

            protected:
                void                            draw_round(ws::ISurface *s);
                void                            draw_rect(ws::ISurface *s);

            protected:
                virtual void                    size_request(ws::size_limit_t *r);
                virtual void                    property_changed(Property *prop);

            public:
                explicit Led(Display *dpy);
                virtual ~Led();

                virtual status_t                init();

            public:
                LSP_TK_PROPERTY(Color,              color,              &sColor)
                LSP_TK_PROPERTY(Color,              hole_color,         &sHoleColor)
                LSP_TK_PROPERTY(Color,              led_color,          &sLedColor)
                LSP_TK_PROPERTY(Color,              border_color,       &sBorderColor)
                LSP_TK_PROPERTY(Color,              led_border_color,   &sLedBorderColor)
                LSP_TK_PROPERTY(SizeConstraints,    constraints,        &sConstraints)
                LSP_TK_PROPERTY(Boolean,            on,                 &sOn)
                LSP_TK_PROPERTY(Boolean,            hole,               &sHole)
                LSP_TK_PROPERTY(Integer,            led,                &sLed)
                LSP_TK_PROPERTY(Boolean,            round,              &sRound)
                LSP_TK_PROPERTY(Integer,            border_size,        &sBorderSize)
                LSP_TK_PROPERTY(Boolean,            gradient,           &sGradient)

            public:
                virtual void                    draw(ws::ISurface *s);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LED_H_ */
