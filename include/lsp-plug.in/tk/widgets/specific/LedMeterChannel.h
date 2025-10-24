/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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
            typedef struct LedMeterChannelColors
            {
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

                void listener(tk::prop::Listener *listener);
                bool property_changed(Property *prop);
            } LedMeterChannelColors;

            enum LedMeterChannelColorState
            {
                LEDMETERCH_NORMAL       = 0,
                LEDMETERCH_INACTIVE     = 1 << 0,

                LEDMETERCH_TOTAL        = 1 << 1
            };

            LSP_TK_STYLE_DEF_BEGIN(LedMeterChannel, Widget)
                LedMeterChannelColors   vColors[LEDMETERCH_TOTAL];

                prop::RangeFloat        sValue;
                prop::Float             sPeak;
                prop::Float             sHeaderValue;
                prop::Float             sBalance;
                prop::String            sText;
                prop::String            sHeader;
                prop::String            sEstText;
                prop::String            sEstHeader;
                prop::Boolean           sPeakVisible;
                prop::Boolean           sBalanceVisible;
                prop::Boolean           sTextVisible;
                prop::Boolean           sHeaderVisible;
                prop::Boolean           sReversive;
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
                enum lmc_flags_t
                {
                    LMC_0           = style::LEDMETERCH_NORMAL,
                    LMC_1           = style::LEDMETERCH_INACTIVE,
                    LMC_TOTAL       = style::LEDMETERCH_TOTAL
                };

            protected:
                style::LedMeterChannelColors    vColors[LMC_TOTAL];

                prop::RangeFloat        sValue;
                prop::Float             sPeak;
                prop::Float             sHeaderValue;
                prop::Float             sBalance;
                prop::String            sText;
                prop::String            sHeader;
                prop::String            sEstText;
                prop::String            sEstHeader;
                prop::Boolean           sPeakVisible;
                prop::Boolean           sBalanceVisible;
                prop::Boolean           sTextVisible;
                prop::Boolean           sHeaderVisible;
                prop::Boolean           sReversive;
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
                void                            draw_meter(ws::ISurface *s, ssize_t angle, float scaling, float bright);
                void                            draw_label(ws::ISurface *s, const Font *f, float fscaling, float bright);
                void                            draw_header(ws::ISurface *s, const Font *f, float fscaling, float bright);
                const lsp::Color               *get_color(float value, const ColorRanges *ranges, const Color *dfl);
                style::LedMeterChannelColors   *select_colors();

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
                LSP_TK_PROPERTY(Color,              color,                      &vColors[LMC_0].sColor)
                LSP_TK_PROPERTY(Color,              value_color,                &vColors[LMC_0].sValueColor)
                LSP_TK_PROPERTY(ColorRanges,        value_ranges,               &vColors[LMC_0].sValueRanges)
                LSP_TK_PROPERTY(Color,              peak_color,                 &vColors[LMC_0].sPeakColor)
                LSP_TK_PROPERTY(ColorRanges,        peak_ranges,                &vColors[LMC_0].sPeakRanges)
                LSP_TK_PROPERTY(Color,              text_color,                 &vColors[LMC_0].sTextColor)
                LSP_TK_PROPERTY(Color,              header_color,               &vColors[LMC_0].sHeaderColor)
                LSP_TK_PROPERTY(ColorRanges,        text_ranges,                &vColors[LMC_0].sTextRanges)
                LSP_TK_PROPERTY(ColorRanges,        header_ranges,              &vColors[LMC_0].sHeaderRanges)
                LSP_TK_PROPERTY(Color,              balance_color,              &vColors[LMC_0].sBalanceColor)

                LSP_TK_PROPERTY(Color,              inactive_color,             &vColors[LMC_1].sColor)
                LSP_TK_PROPERTY(Color,              inactive_value_color,       &vColors[LMC_1].sValueColor)
                LSP_TK_PROPERTY(ColorRanges,        inactive_value_ranges,      &vColors[LMC_1].sValueRanges)
                LSP_TK_PROPERTY(Color,              inactive_peak_color,        &vColors[LMC_1].sPeakColor)
                LSP_TK_PROPERTY(ColorRanges,        inactive_peak_ranges,       &vColors[LMC_1].sPeakRanges)
                LSP_TK_PROPERTY(Color,              inactive_text_color,        &vColors[LMC_1].sTextColor)
                LSP_TK_PROPERTY(Color,              inactive_header_color,      &vColors[LMC_1].sHeaderColor)
                LSP_TK_PROPERTY(ColorRanges,        inactive_text_ranges,       &vColors[LMC_1].sTextRanges)
                LSP_TK_PROPERTY(ColorRanges,        inactive_header_ranges,     &vColors[LMC_1].sHeaderRanges)
                LSP_TK_PROPERTY(Color,              inactive_balance_color,     &vColors[LMC_1].sBalanceColor)

                LSP_TK_PROPERTY(RangeFloat,         value,              &sValue)
                LSP_TK_PROPERTY(Float,              peak,               &sPeak)
                LSP_TK_PROPERTY(Float,              header_value,       &sHeaderValue)
                LSP_TK_PROPERTY(Float,              balance,            &sBalance)
                LSP_TK_PROPERTY(String,             text,               &sText)
                LSP_TK_PROPERTY(String,             header,             &sHeader)
                LSP_TK_PROPERTY(String,             estimation_text,    &sEstText)
                LSP_TK_PROPERTY(String,             estimation_header,  &sEstHeader)
                LSP_TK_PROPERTY(Boolean,            peak_visible,       &sPeakVisible)
                LSP_TK_PROPERTY(Boolean,            balance_visible,    &sBalanceVisible)
                LSP_TK_PROPERTY(Boolean,            text_visible,       &sTextVisible)
                LSP_TK_PROPERTY(Boolean,            header_visible,     &sHeaderVisible)
                LSP_TK_PROPERTY(Boolean,            reversive,          &sReversive)
                LSP_TK_PROPERTY(Integer,            min_segments,       &sMinSegments)
                LSP_TK_PROPERTY(SizeConstraints,    constraints,        &sConstraints)
                LSP_TK_PROPERTY(Font,               font,               &sFont)
                LSP_TK_PROPERTY(Integer,            border,             &sBorder)
                LSP_TK_PROPERTY(Integer,            angle,              &sAngle)
                LSP_TK_PROPERTY(Pointer,            header_pointer,     &sHeaderPointer)

            public:
                virtual void                draw(ws::ISurface *s, bool force) override;
                virtual status_t            on_mouse_pointer(pointer_event_t *e) override;
        };
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_METERCHANNEL_H_ */
