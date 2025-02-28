/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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
            typedef struct LedColors
            {
                prop::Color             sColor;
                prop::Color             sLedColor;
                prop::Color             sBorderColor;
                prop::Color             sLedBorderColor;

                void listener(tk::prop::Listener *listener);
                bool property_changed(Property *prop);
            } LedColors;

            enum LedColorState
            {
                LED_NORMAL          = 0,
                LED_INACTIVE        = 1 << 0,

                LED_TOTAL           = 1 << 1
            };

            LSP_TK_STYLE_DEF_BEGIN(Led, Widget)
                LedColors               vColors[LED_TOTAL];
                prop::Color             sHoleColor;
                prop::SizeConstraints   sConstraints;
                prop::Boolean           sOn;
                prop::Boolean           sHole;
                prop::Integer           sLed;
                prop::Boolean           sRound;
                prop::Integer           sBorderSize;
                prop::Boolean           sGradient;
                prop::Boolean           sActive;
            LSP_TK_STYLE_DEF_END
        }

        class Led: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum led_flags_t
                {
                    LED_0       = style::LED_NORMAL,
                    LED_1       = style::LED_INACTIVE,
                    LED_TOTAL   = style::LED_TOTAL
                };

            protected:
                style::LedColors        vColors[LED_TOTAL];
                prop::Color             sHoleColor;
                prop::SizeConstraints   sConstraints;
                prop::Boolean           sOn;
                prop::Boolean           sHole;
                prop::Integer           sLed;
                prop::Boolean           sRound;
                prop::Integer           sBorderSize;
                prop::Boolean           sGradient;
                prop::Boolean           sActive;

            protected:
                void                            draw_round(ws::ISurface *s);
                void                            draw_rect(ws::ISurface *s);
                style::LedColors               *select_colors();

            protected:
                virtual void                    size_request(ws::size_limit_t *r) override;
                virtual void                    property_changed(Property *prop) override;

            public:
                explicit Led(Display *dpy);
                Led(const Led &) = delete;
                Led(Led &&) = delete;
                virtual ~Led();
                Led & operator = (const Led &) = delete;
                Led & operator = (Led &&) = delete;

                virtual status_t                init() override;

            public:
                LSP_TK_PROPERTY(Color,              color,                      &vColors[LED_0].sColor)
                LSP_TK_PROPERTY(Color,              led_color,                  &vColors[LED_0].sLedColor)
                LSP_TK_PROPERTY(Color,              border_color,               &vColors[LED_0].sBorderColor)
                LSP_TK_PROPERTY(Color,              led_border_color,           &vColors[LED_0].sLedBorderColor)
                LSP_TK_PROPERTY(Color,              inactive_color,             &vColors[LED_1].sColor)
                LSP_TK_PROPERTY(Color,              inactive_led_color,         &vColors[LED_1].sLedColor)
                LSP_TK_PROPERTY(Color,              inactive_border_color,      &vColors[LED_1].sBorderColor)
                LSP_TK_PROPERTY(Color,              inactive_led_border_color,  &vColors[LED_1].sLedBorderColor)
                LSP_TK_PROPERTY(Color,              hole_color,                 &sHoleColor)

                LSP_TK_PROPERTY(SizeConstraints,    constraints,                &sConstraints)
                LSP_TK_PROPERTY(Boolean,            on,                         &sOn)
                LSP_TK_PROPERTY(Boolean,            hole,                       &sHole)
                LSP_TK_PROPERTY(Integer,            led,                        &sLed)
                LSP_TK_PROPERTY(Boolean,            round,                      &sRound)
                LSP_TK_PROPERTY(Integer,            border_size,                &sBorderSize)
                LSP_TK_PROPERTY(Boolean,            gradient,                   &sGradient)
                LSP_TK_PROPERTY(Boolean,            active,                     &sActive)

            public:
                virtual void                    draw(ws::ISurface *s) override;
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LED_H_ */
