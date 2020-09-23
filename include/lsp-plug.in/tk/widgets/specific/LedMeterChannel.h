/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 21 сент. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_METERCHANNEL_H_
#define LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_METERCHANNEL_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class LedMeterChannel: public Widget
        {
            public:
                static const w_class_t    metadata;

            private:
                LedMeterChannel & operator = (const LedMeterChannel &);

                friend class LedMeter;

            protected:
                prop::RangeFloat        sValue;
                prop::Float             sPeak;
                prop::Float             sBalance;
                prop::Color             sColor;
                prop::Color             sValueColor;
                prop::ColorRanges       sValueRanges;
                prop::Color             sPeakColor;
                prop::ColorRanges       sPeakRanges;
                prop::Color             sTextColor;
                prop::ColorRanges       sTextRanges;
                prop::Color             sBalanceColor;
                prop::String            sText;
                prop::String            sEstText;
                prop::Boolean           sPeakVisible;
                prop::Boolean           sBalanceVisible;
                prop::Boolean           sTextVisible;
                prop::Boolean           sReversive;
                prop::Boolean           sActive;
                prop::Integer           sMinSegments;
                prop::SizeConstraints   sConstraints;
                prop::Font              sFont;
                prop::Integer           sBorder;
                prop::Integer           sAngle;

                ws::rectangle_t         sAAll;              // All drawing area
                ws::rectangle_t         sAMeter;            // Meter drawing area
                ws::rectangle_t         sAText;             // Text drawing area

            protected:
                void                        draw_meter(ws::ISurface *s, const ws::rectangle_t *r, ssize_t angle, float scaling);
                void                        draw_label(ws::ISurface *s, const ws::rectangle_t *r, const Font *f, float scaling);
                const lsp::Color           *get_color(float value, const ColorRanges *ranges, const Color *dfl);

            public:
                explicit LedMeterChannel(Display *dpy);
                virtual ~LedMeterChannel();

                virtual status_t            init();
                virtual void                destroy();

            protected:
                virtual void                property_changed(Property *prop);
                virtual void                size_request(ws::size_limit_t *r);
                virtual void                realize(const ws::rectangle_t *r);

            public:
                LSP_TK_PROPERTY(RangeFloat,         value,              &sValue)
                LSP_TK_PROPERTY(Float,              peak,               &sPeak)
                LSP_TK_PROPERTY(Float,              balance,            &sBalance)
                LSP_TK_PROPERTY(Color,              color,              &sColor)
                LSP_TK_PROPERTY(Color,              value_color,        &sValueColor)
                LSP_TK_PROPERTY(ColorRanges,        value_ranges,       &sValueRanges)
                LSP_TK_PROPERTY(Color,              peak_color,         &sPeakColor)
                LSP_TK_PROPERTY(ColorRanges,        peak_ranges,        &sPeakRanges)
                LSP_TK_PROPERTY(Color,              text_color,         &sTextColor)
                LSP_TK_PROPERTY(ColorRanges,        text_ranges,        &sTextRanges)
                LSP_TK_PROPERTY(Color,              balance_color,      &sBalanceColor)
                LSP_TK_PROPERTY(String,             text,               &sText)
                LSP_TK_PROPERTY(String,             estimation_text,    &sEstText)
                LSP_TK_PROPERTY(Boolean,            peak_visible,       &sPeakVisible)
                LSP_TK_PROPERTY(Boolean,            balance_visible,    &sBalanceVisible)
                LSP_TK_PROPERTY(Boolean,            text_visible,       &sTextVisible)
                LSP_TK_PROPERTY(Boolean,            reversive,          &sReversive)
                LSP_TK_PROPERTY(Boolean,            active,             &sActive)
                LSP_TK_PROPERTY(Integer,            min_segments,       &sMinSegments)
                LSP_TK_PROPERTY(SizeConstraints,    constraints,        &sConstraints)
                LSP_TK_PROPERTY(Font,               font,               &sFont)
                LSP_TK_PROPERTY(Integer,            border,             &sBorder)
                LSP_TK_PROPERTY(Integer,            angle,              &sAngle)

            public:
                virtual void                draw(ws::ISurface *s);
        };
    }
}

#endif /* LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_METERCHANNEL_H_ */
