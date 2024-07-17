/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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
        // Style definition
        namespace style
        {
            LSP_TK_STYLE_DEF_BEGIN(LedMeterChannel, Widget)
                prop::RangeFloat        sValue;
                prop::Float             sPeak;
                prop::Float             sHeaderValue;
                prop::Float             sBalance;
                prop::Color             sColor;
                prop::Color             sValueColor;
                prop::ColorRanges       sValueRanges;
                prop::Color             sPeakColor;
                prop::ColorRanges       sPeakRanges;
                prop::Color             sTextColor;
                prop::Color             sHeaderColor;
                prop::ColorRanges       sTextRanges;
                prop::ColorRanges       sHeaderRanges;
                prop::Color             sBalanceColor;
                prop::String            sText;
                prop::String            sHeader;
                prop::String            sEstText;
                prop::String            sEstHeader;
                prop::Boolean           sPeakVisible;
                prop::Boolean           sBalanceVisible;
                prop::Boolean           sTextVisible;
                prop::Boolean           sHeaderVisible;
                prop::Boolean           sReversive;
                prop::Boolean           sActive;
                prop::Integer           sMinSegments;
                prop::SizeConstraints   sConstraints;
                prop::Font              sFont;
                prop::Integer           sBorder;
                prop::Integer           sAngle;
                prop::Pointer           sHeaderPointer;
            LSP_TK_STYLE_DEF_END
        }

        class LedMeterChannel: public Widget
        {
            public:
                static const w_class_t    metadata;

            private:
                friend class LedMeter;

            protected:
                prop::RangeFloat        sValue;
                prop::Float             sPeak;
                prop::Float             sHeaderValue;
                prop::Float             sBalance;
                prop::Color             sColor;
                prop::Color             sValueColor;
                prop::ColorRanges       sValueRanges;
                prop::Color             sPeakColor;
                prop::ColorRanges       sPeakRanges;
                prop::Color             sTextColor;
                prop::Color             sHeaderColor;
                prop::ColorRanges       sTextRanges;
                prop::ColorRanges       sHeaderRanges;
                prop::Color             sBalanceColor;
                prop::String            sText;
                prop::String            sHeader;
                prop::String            sEstText;
                prop::String            sEstHeader;
                prop::Boolean           sPeakVisible;
                prop::Boolean           sBalanceVisible;
                prop::Boolean           sTextVisible;
                prop::Boolean           sHeaderVisible;
                prop::Boolean           sReversive;
                prop::Boolean           sActive;
                prop::Integer           sMinSegments;
                prop::SizeConstraints   sConstraints;
                prop::Font              sFont;
                prop::Integer           sBorder;
                prop::Integer           sAngle;
                prop::Pointer           sHeaderPointer;

                ws::rectangle_t         sAAll;              // All drawing area
                ws::rectangle_t         sAMeter;            // Meter drawing area
                ws::rectangle_t         sAText;             // Text drawing area
                ws::rectangle_t         sAHeader;           // Header drawing area

            protected:
                void                        draw_meter(ws::ISurface *s, ssize_t angle, float scaling, float bright);
                void                        draw_label(ws::ISurface *s, const Font *f, float fscaling, float bright);
                void                        draw_header(ws::ISurface *s, const Font *f, float fscaling, float bright);
                const lsp::Color           *get_color(float value, const ColorRanges *ranges, const Color *dfl);

            public:
                explicit LedMeterChannel(Display *dpy);
                LedMeterChannel(const LedMeterChannel &) = delete;
                LedMeterChannel(LedMeterChannel &&) = delete;
                virtual ~LedMeterChannel() override;

                LedMeterChannel & operator = (const LedMeterChannel &) = delete;
                LedMeterChannel & operator = (LedMeterChannel &&) = delete;

                virtual status_t            init() override;

            public:
                bool                        is_text(ssize_t x, ssize_t y) const;
                bool                        is_header(ssize_t x, ssize_t y) const;

            protected:
                virtual void                property_changed(Property *prop) override;
                virtual void                size_request(ws::size_limit_t *r) override;
                virtual void                realize(const ws::rectangle_t *r) override;

            public:
                LSP_TK_PROPERTY(RangeFloat,         value,              &sValue)
                LSP_TK_PROPERTY(Float,              peak,               &sPeak)
                LSP_TK_PROPERTY(Float,              header_value,       &sHeaderValue)
                LSP_TK_PROPERTY(Float,              balance,            &sBalance)
                LSP_TK_PROPERTY(Color,              color,              &sColor)
                LSP_TK_PROPERTY(Color,              value_color,        &sValueColor)
                LSP_TK_PROPERTY(ColorRanges,        value_ranges,       &sValueRanges)
                LSP_TK_PROPERTY(Color,              peak_color,         &sPeakColor)
                LSP_TK_PROPERTY(ColorRanges,        peak_ranges,        &sPeakRanges)
                LSP_TK_PROPERTY(Color,              text_color,         &sTextColor)
                LSP_TK_PROPERTY(Color,              header_color,       &sHeaderColor)
                LSP_TK_PROPERTY(ColorRanges,        text_ranges,        &sTextRanges)
                LSP_TK_PROPERTY(ColorRanges,        header_ranges,      &sHeaderRanges)
                LSP_TK_PROPERTY(Color,              balance_color,      &sBalanceColor)
                LSP_TK_PROPERTY(String,             text,               &sText)
                LSP_TK_PROPERTY(String,             header,             &sHeader)
                LSP_TK_PROPERTY(String,             estimation_text,    &sEstText)
                LSP_TK_PROPERTY(String,             estimation_header,  &sEstHeader)
                LSP_TK_PROPERTY(Boolean,            peak_visible,       &sPeakVisible)
                LSP_TK_PROPERTY(Boolean,            balance_visible,    &sBalanceVisible)
                LSP_TK_PROPERTY(Boolean,            text_visible,       &sTextVisible)
                LSP_TK_PROPERTY(Boolean,            header_visible,     &sHeaderVisible)
                LSP_TK_PROPERTY(Boolean,            reversive,          &sReversive)
                LSP_TK_PROPERTY(Boolean,            active,             &sActive)
                LSP_TK_PROPERTY(Integer,            min_segments,       &sMinSegments)
                LSP_TK_PROPERTY(SizeConstraints,    constraints,        &sConstraints)
                LSP_TK_PROPERTY(Font,               font,               &sFont)
                LSP_TK_PROPERTY(Integer,            border,             &sBorder)
                LSP_TK_PROPERTY(Integer,            angle,              &sAngle)
                LSP_TK_PROPERTY(Pointer,            header_pointer,     &sHeaderPointer)

            public:
                virtual void                draw(ws::ISurface *s) override;
                virtual status_t            on_mouse_pointer(pointer_event_t *e) override;
        };
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_METERCHANNEL_H_ */
