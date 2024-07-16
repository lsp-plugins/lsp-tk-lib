/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 22 сент. 2020 г.
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

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/stdlib/math.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(LedMeterChannel, Widget)
                // Bind
                sValue.bind("value", this);
                sPeak.bind("peak", this);
                sHeaderValue.bind("header.value", this);
                sBalance.bind("balance", this);
                sColor.bind("color", this);
                sValueColor.bind("value.color", this);
                sValueRanges.bind("value.ranges", this);
                sPeakColor.bind("peak.color", this);
                sPeakRanges.bind("peak.ranges", this);
                sTextColor.bind("text.color", this);
                sHeaderColor.bind("header.color", this);
                sTextRanges.bind("text.ranges", this);
                sHeaderRanges.bind("header.ranges", this);
                sBalanceColor.bind("balance.color", this);
                sPeakVisible.bind("peak.visible", this);
                sBalanceVisible.bind("balance.visible", this);
                sTextVisible.bind("text.visible", this);
                sHeaderVisible.bind("header.visible", this);
                sReversive.bind("reversive", this);
                sActive.bind("active", this);
                sMinSegments.bind("segments.min", this);
                sConstraints.bind("constraints", this);
                sFont.bind("font", this);
                sBorder.bind("border", this);
                sAngle.bind("angle", this);
                // Configure
                sValue.set_all(0.0f, 0.0f, 1.0f);
                sPeak.set(0.0f);
                sHeaderValue.set(0.0f);
                sBalance.set(0.5f);
                sColor.set("#000000");
                sValueColor.set("#00ff00");
                sValueRanges.set_all("");
                sPeakColor.set("#ff0000");
                sPeakRanges.set_all("");
                sBalanceColor.set("#ffff00");
                sTextColor.set("#00ff00");
                sHeaderColor.set("#00ff00");
                sTextRanges.set_all("");
                sHeaderRanges.set_all("");
                sBalanceColor.set("#ffff00");
                sPeakVisible.set(false);
                sBalanceVisible.set(false);
                sTextVisible.set(false);
                sHeaderVisible.set(false);
                sReversive.set(false);
                sActive.set(true);
                sMinSegments.set(12);
                sConstraints.set(20, -1, 20, -1);
                sFont.set_size(9.0f);
                sBorder.set(2);
                sAngle.set(0);
                // Override
                sFont.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(LedMeterChannel, "LedMeterChannel", "root");
        }

        const w_class_t LedMeterChannel::metadata           = { "LedMeterChannel", &Widget::metadata };

        LedMeterChannel::LedMeterChannel(Display *dpy):
            Widget(dpy),
            sValue(&sProperties),
            sPeak(&sProperties),
            sHeaderValue(&sProperties),
            sBalance(&sProperties),
            sColor(&sProperties),
            sValueColor(&sProperties),
            sValueRanges(&sProperties),
            sPeakColor(&sProperties),
            sPeakRanges(&sProperties),
            sTextColor(&sProperties),
            sHeaderColor(&sProperties),
            sTextRanges(&sProperties),
            sHeaderRanges(&sProperties),
            sBalanceColor(&sProperties),
            sText(&sProperties),
            sHeader(&sProperties),
            sEstText(&sProperties),
            sEstHeader(&sProperties),
            sPeakVisible(&sProperties),
            sBalanceVisible(&sProperties),
            sTextVisible(&sProperties),
            sReversive(&sProperties),
            sActive(&sProperties),
            sMinSegments(&sProperties),
            sConstraints(&sProperties),
            sFont(&sProperties),
            sBorder(&sProperties),
            sAngle(&sProperties)
        {
            sAMeter.nLeft   = 0;
            sAMeter.nTop    = 0;
            sAMeter.nWidth  = 0;
            sAMeter.nHeight = 0;

            sAText.nLeft    = 0;
            sAText.nTop     = 0;
            sAText.nWidth   = 0;
            sAText.nHeight  = 0;

            sAAll.nLeft     = 0;
            sAAll.nTop      = 0;
            sAAll.nWidth    = 0;
            sAAll.nHeight   = 0;

            pClass          = &metadata;
        }

        LedMeterChannel::~LedMeterChannel()
        {
            nFlags     |= FINALIZED;
        }

        status_t LedMeterChannel::init()
        {
            status_t res = Widget::init();
            if (res != STATUS_OK)
                return res;

            sValue.bind("value", &sStyle);
            sPeak.bind("peak", &sStyle);
            sHeaderValue.bind("header.value", &sStyle);
            sBalance.bind("balance", &sStyle);
            sColor.bind("color", &sStyle);
            sValueColor.bind("value.color", &sStyle);
            sValueRanges.bind("value.ranges", &sStyle);
            sPeakColor.bind("peak.color", &sStyle);
            sPeakRanges.bind("peak.ranges", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sHeaderColor.bind("header.color", &sStyle);
            sTextRanges.bind("text.ranges", &sStyle);
            sHeaderRanges.bind("header.ranges", &sStyle);
            sBalanceColor.bind("balance.color", &sStyle);
            sText.bind(&sStyle, pDisplay->dictionary());
            sHeader.bind(&sStyle, pDisplay->dictionary());
            sEstText.bind(&sStyle, pDisplay->dictionary());
            sEstHeader.bind(&sStyle, pDisplay->dictionary());
            sPeakVisible.bind("peak.visible", &sStyle);
            sBalanceVisible.bind("balance.visible", &sStyle);
            sTextVisible.bind("text.visible", &sStyle);
            sHeaderVisible.bind("header.visible", &sStyle);
            sReversive.bind("reversive", &sStyle);
            sActive.bind("active", &sStyle);
            sMinSegments.bind("segments.min", &sStyle);
            sConstraints.bind("constraints", &sStyle);
            sFont.bind("font", &sStyle);
            sBorder.bind("border", &sStyle);
            sAngle.bind("angle", &sStyle);

            // Disable automatic limit apply
            sValue.set_auto_limit(false);

            sEstText.set_raw("+99.9");
            sEstHeader.set_raw("+99.9");

            return STATUS_OK;
        }

        void LedMeterChannel::property_changed(Property *prop)
        {
            Widget::property_changed(prop);
            if (sActive.get())
            {
                if (sValue.is(prop))
                    query_draw();
                if ((sHeaderValue.is(prop)) && (sHeaderVisible.get()))
                    query_draw();
                if (sPeak.is(prop) && (sPeakVisible.get()))
                    query_draw();
                if (sPeakVisible.is(prop))
                    query_draw();
            }

            if (sBalance.is(prop) && (sBalanceVisible.get()))
                query_draw();
            if (sColor.is(prop))
                query_draw();
            if (sValueColor.is(prop))
                query_draw();
            if (sValueRanges.is(prop))
                query_draw();
            if (sPeakColor.is(prop) && (sPeakVisible.get()))
                query_draw();
            if (sPeakRanges.is(prop) && (sPeakVisible.get()))
                query_draw();
            if (sBalanceColor.is(prop) && (sBalanceVisible.get()))
                query_draw();
            if ((sTextVisible.get()) && (prop->one_of(sText, sEstText, sTextColor, sTextRanges)))
                query_draw();
            if ((sHeaderVisible.get()) && (prop->one_of(sHeader, sEstHeader, sHeaderColor, sHeaderRanges)))
                query_draw();
            if (prop->one_of(sTextVisible, sHeaderVisible))
                query_draw();
            if (sBalanceVisible.is(prop))
                query_draw();
            if (sReversive.is(prop))
                query_draw();
            if (sActive.is(prop))
                query_draw();
            if (sMinSegments.is(prop))
                query_resize();
            if (sConstraints.is(prop))
                query_resize();
            if (sFont.is(prop) && (sTextVisible.get() || sHeaderVisible.get()))
                query_resize();
            if (sBorder.is(prop))
                query_resize();
            if (sAngle.is(prop))
                query_resize();
        }

        void LedMeterChannel::size_request(ws::size_limit_t *r)
        {
            float scaling   = lsp_max(0.0f, sScaling.get());
            float fscaling  = lsp_max(0.0f, scaling * sFontScaling.get());
            float seg_size  = 4.0f * scaling;
            ssize_t border  = (sBorder.get() > 0) ? lsp_max(1.0f, sBorder.get() * scaling) : 0;
            ssize_t angle   = sAngle.get();
            bool has_text   = sTextVisible.get();
            bool has_header = sHeaderVisible.get();

            ws::text_parameters_t tp_text, tp_header;
            ws::font_parameters_t fp;

            LSPString text;
            if (has_text)
            {
                sEstText.format(&text);
                sFont.get_parameters(pDisplay, fscaling, &fp);
                sFont.get_text_parameters(pDisplay, &tp_text, fscaling, &text);
                tp_text.Height      = lsp_max(tp_text.Height, fp.Height);
            }

            if (has_header)
            {
                sEstHeader.format(&text);
                sFont.get_parameters(pDisplay, fscaling, &fp);
                sFont.get_text_parameters(pDisplay, &tp_header, fscaling, &text);
                tp_header.Height    = lsp_max(tp_header.Height, fp.Height);
            }

            // Check for odd/even angle
            if (angle & 1)
            {
                // Vertical
                r->nMinWidth        = ceilf(seg_size);
                r->nMinHeight       = ceilf(seg_size * lsp_min(0, sMinSegments.get()));

                if (has_text)
                {
                    r->nMinHeight      += border + tp_text.Height;
                    r->nMinWidth        = lsp_max(r->nMinWidth, tp_text.Width);
                }
                if (has_header)
                {
                    r->nMinHeight      += border + tp_header.Height;
                    r->nMinWidth        = lsp_max(r->nMinWidth, tp_header.Width);
                }
            }
            else
            {
                // Horizontal
                r->nMinWidth        = ceilf(seg_size * lsp_min(0, sMinSegments.get()));
                r->nMinHeight       = ceilf(seg_size);

                if (has_text)
                {
                    r->nMinWidth       += border + tp_text.Width;
                    r->nMinHeight       = lsp_max(r->nMinHeight, tp_text.Height);
                }
                if (has_header)
                {
                    r->nMinWidth       += border + tp_header.Width;
                    r->nMinHeight       = lsp_max(r->nMinHeight, tp_header.Height);
                }
            }

            r->nMinWidth       += border * 2;
            r->nMinHeight      += border * 2;
            r->nMaxWidth        = -1;
            r->nMaxHeight       = -1;
            r->nPreWidth        = -1;
            r->nPreHeight       = -1;

            // Apply size constraints
            if (angle & 1)
                sConstraints.apply(r, scaling);  // Apply non-transposed size constraints
            else
                sConstraints.tapply(r, scaling); // Apply transposed size constraints
        }

        void LedMeterChannel::realize(const ws::rectangle_t *r)
        {
            Widget::realize(r);

            float scaling       = lsp_max(0.0f, sScaling.get());
            float fscaling      = lsp_max(0.0f, scaling * sFontScaling.get());
            float seg_size      = 4.0f * scaling;
            ssize_t border      = (sBorder.get() > 0) ? lsp_max(1.0f, sBorder.get() * scaling) : 0;
            ssize_t angle       = sAngle.get();
            bool has_text       = sTextVisible.get();
            bool has_header     = sTextVisible.get();

            ws::text_parameters_t tp_text, tp_header;
            ws::font_parameters_t fp;
            ws::rectangle_t xr;

            sAAll.nLeft         = 0;
            sAAll.nTop          = 0;
            sAAll.nWidth        = r->nWidth;
            sAAll.nHeight       = r->nHeight;

            xr.nLeft            = border;
            xr.nTop             = border;
            xr.nWidth           = r->nWidth  - border*2;
            xr.nHeight          = r->nHeight - border*2;

            sAText.nLeft        = 0;
            sAText.nTop         = 0;
            sAText.nWidth       = 0;
            sAText.nHeight      = 0;

            sAHeader.nLeft      = 0;
            sAHeader.nTop       = 0;
            sAHeader.nWidth     = 0;
            sAHeader.nHeight    = 0;

            sAMeter.nLeft       = 0;
            sAMeter.nTop        = 0;
            sAMeter.nWidth      = 0;
            sAMeter.nHeight     = 0;

            // Compute the amount of space used for text
            ssize_t led_size    = (angle & 1) ? xr.nHeight : xr.nWidth;

            LSPString text;
            if (has_text)
            {
                sEstText.format(&text);
                sFont.get_parameters(pDisplay, fscaling, &fp);
                sFont.get_text_parameters(pDisplay, &tp_text, fscaling, &text);

                if (angle & 1) // Vertical
                {
                    sAText.nLeft        = xr.nLeft;
                    sAText.nWidth       = xr.nWidth;
                    sAText.nHeight      = lsp_max(tp_text.Height, fp.Height);
                    led_size           -= border + sAText.nHeight;
                }
                else // Horizontal
                {
                    sAText.nTop         = xr.nTop;
                    sAText.nWidth       = tp_text.Width;
                    sAText.nHeight      = xr.nHeight;
                    led_size           -= border + sAText.nWidth;
                }
            }

            if (has_header)
            {
                sEstHeader.format(&text);
                sFont.get_parameters(pDisplay, fscaling, &fp);
                sFont.get_text_parameters(pDisplay, &tp_header, fscaling, &text);

                if (angle & 1) // Vertical
                {
                    sAHeader.nLeft      = xr.nLeft;
                    sAHeader.nWidth     = xr.nWidth;
                    sAHeader.nHeight    = lsp_max(tp_header.Height, fp.Height);
                    led_size           -= border + sAHeader.nHeight;
                }
                else // Horizontal
                {
                    sAHeader.nTop       = xr.nTop;
                    sAHeader.nWidth     = tp_header.Width;
                    sAHeader.nHeight    = xr.nHeight;
                    led_size           -= border + sAHeader.nWidth;
                }
            }

            // Compute overall areas
            ssize_t segments    = led_size / seg_size;
            ssize_t gap         = led_size - ceilf(segments * seg_size);
            led_size            = led_size - gap;

            switch (size_t(angle & 3))
            {
                case 1: // Bottom to top
                {
                    xr.nTop        += (gap >> 1);

                    sAAll.nTop     += (gap >> 1);
                    sAAll.nHeight  -= gap;

                    sAHeader.nTop   = xr.nTop;

                    sAMeter.nLeft   = xr.nLeft;
                    sAMeter.nTop    = sAHeader.nTop;
                    sAMeter.nWidth  = xr.nWidth;
                    sAMeter.nHeight = led_size;

                    if (has_header)
                        sAMeter.nTop   += sAHeader.nHeight + border;

                    sAText.nTop     = sAMeter.nTop + sAMeter.nHeight + border;

                    break;
                }

                case 3: // Top to bottom
                {
                    xr.nTop        += (gap >> 1);

                    sAAll.nTop     += (gap >> 1);
                    sAAll.nHeight  -= gap;

                    sAText.nTop     = xr.nTop;

                    sAMeter.nLeft   = xr.nLeft;
                    sAMeter.nTop    = sAText.nTop;
                    sAMeter.nWidth  = xr.nWidth;
                    sAMeter.nHeight = led_size;

                    if (has_text)
                        sAMeter.nTop   += sAText.nHeight + border;

                    sAHeader.nTop   = sAMeter.nTop + sAMeter.nHeight + border;

                    break;
                }

                case 2: // Right to left
                {
                    xr.nLeft       += (gap >> 1);

                    sAAll.nLeft    += (gap >> 1);
                    sAAll.nWidth   -= gap;

                    sAHeader.nLeft  = xr.nLeft;

                    sAMeter.nLeft   = sAHeader.nLeft;
                    sAMeter.nTop    = xr.nTop;
                    sAMeter.nWidth  = led_size;
                    sAMeter.nHeight = xr.nHeight;
                    if (has_header)
                        sAMeter.nLeft  += sAHeader.nWidth + border;

                    sAText.nLeft    = sAMeter.nLeft + sAMeter.nWidth + border;

                    break;
                }

                case 0: // Left to right
                default:
                {
                    xr.nLeft       += (gap >> 1);

                    sAAll.nLeft    += (gap >> 1);
                    sAAll.nWidth   -= gap;

                    sAText.nLeft    = xr.nLeft;

                    sAMeter.nLeft   = sAText.nLeft;
                    sAMeter.nTop    = xr.nTop;
                    sAMeter.nWidth  = led_size;
                    sAMeter.nHeight = xr.nHeight;
                    if (has_text)
                        sAMeter.nLeft  += sAText.nWidth + border;

                    sAHeader.nLeft  = sAMeter.nLeft + sAMeter.nWidth + border;

                    break;
                }
            }
        }

        bool LedMeterChannel::is_text(ssize_t x, ssize_t y) const
        {
            x -= sSize.nLeft;
            y -= sSize.nTop;

            return Position::inside(&sAText, x, y);
        }

        bool LedMeterChannel::is_header(ssize_t x, ssize_t y) const
        {
            x -= sSize.nLeft;
            y -= sSize.nTop;

            return Position::inside(&sAHeader, x, y);
        }

        void LedMeterChannel::draw_meter(ws::ISurface *s, ssize_t angle, float scaling, float bright)
        {
            float seg_size      = 4.0f * scaling;
            float range         = sValue.range();
            ssize_t segments    = (angle & 1) ? (sAMeter.nHeight / seg_size) : (sAMeter.nWidth / seg_size);
            float step          = range / lsp_max(1, segments - 1);
            lsp::Color fc, bc;
            const lsp::Color *lc;

            float bx            = ((angle & 3) == 2) ? sAMeter.nLeft + sAMeter.nWidth  - seg_size : sAMeter.nLeft;
            float by            = ((angle & 3) == 1) ? sAMeter.nTop  + sAMeter.nHeight - seg_size : sAMeter.nTop;
            float bw            = (angle & 1) ? sAMeter.nWidth : seg_size;
            float bh            = (angle & 1) ? seg_size : sAMeter.nHeight;

            float fx            = bx + scaling;
            float fy            = by + scaling;
            float fw            = lsp_max(0.0f, bw - scaling * 2.0f);
            float fh            = lsp_max(0.0f, bh - scaling * 2.0f);

            float dx            = ((angle & 1)) ? 0.0f : ((angle & 2) ? -seg_size : seg_size);
            float dy            = ((angle & 1)) ? ((angle & 2) ? seg_size : -seg_size) : 0.0f;

            bool has_balance    = sBalanceVisible.get();
            bool active         = sActive.get();
            bool has_peak       = (active) && (sPeakVisible.get());
            bool reversive      = sReversive.get();
            float balance       = sBalance.get();
            float peak          = sPeak.get();
            float value         = sValue.get();

            float first         = sValue.min();
            float vmin          = first - 0.5f * step;

            bool aa             = s->set_antialiasing(true);
            lsp_finally { s->set_antialiasing(aa); };

            s->clip_begin(&sAMeter);
            {
                lsp_finally { s->clip_end(); };
                for (ssize_t i=0; i<segments; ++i)
                {
                    float vmax          = first + step * (i + 0.5f);

                    // Estimate the segment color (special values for peak and balance
                    if ((has_balance) && (vmin <= balance) && (balance < vmax))
                        lc                  = sBalanceColor.color();
                    else if ((has_peak) && (vmin <= peak) && (peak < vmax))
                        lc                  = get_color(peak,  &sPeakRanges, &sPeakColor);
                    else
                        lc                  = get_color(vmin, &sValueRanges, &sValueColor);

                    // Now determine if we need to darken the color
                    bool matched = false;

                    if (active)
                    {
                        if (has_balance)
                        {
                            matched     = (balance < value) ?
                                ((vmax > balance) && (vmin <= value))
                                : ((vmax > value) && (vmin <= balance));

                            if ((vmin <= balance) && (balance < vmax))
                                matched     = !reversive;
                            else if ((!matched) && (has_peak))
                                matched     = (peak >= vmin) && (peak < vmax);
                        }
                        else
                        {
                            matched     = (vmin < value);
                            if ((!matched) && (has_peak))
                                matched     = (peak > vmin) && (peak <= vmax);
                        }

                        matched    ^= reversive;
                    }

                    // Compute color of the segment
                    fc.copy(lc);
                    bc.copy(lc);
                    fc.scale_lch_luminance(bright);
                    bc.scale_lch_luminance(bright);

                    if (matched)
                        bc.alpha(0.5f);
                    else
                    {
                        bc.alpha(0.95f);
                        fc.alpha(0.9f);
                    }

                    // Draw the bar
                    s->fill_rect(bc, SURFMASK_NONE, 0.0f, bx, by, bw, bh);
                    s->fill_rect(fc, SURFMASK_NONE, 0.0f, fx, fy, fw, fh);

                    // Update coordinates and value
                    vmin                = vmax;
                    bx                 += dx;
                    by                 += dy;
                    fx                 += dx;
                    fy                 += dy;
                }
            }
        }

        const lsp::Color *LedMeterChannel::get_color(float value, const ColorRanges *ranges, const Color *dfl)
        {
            for (size_t i=0, n=ranges->size(); i<n; ++i)
            {
                const ColorRange *r = ranges->get(i);
                if (r->matches(value))
                    return r->color();
            }

            return dfl->color();
        }

        void LedMeterChannel::draw_label(ws::ISurface *s, const Font *f, float fscaling, float bright)
        {
            if (!sActive.get())
                return;

            ws::font_parameters_t   fp;
            ws::text_parameters_t   tp;

            LSPString text;
            sText.format(&text);
            sFont.get_parameters(s, fscaling, &fp);
            sFont.get_text_parameters(s, &tp, fscaling, &text);

            ssize_t fx  = sAText.nLeft + ((sAText.nWidth  - tp.Width ) * 0.5f) + tp.XBearing;
            ssize_t fy  = sAText.nTop  + ((sAText.nHeight - fp.Height) * 0.5f) + fp.Ascent;
            float value = (sPeakVisible.get()) ? sValue.limit(sPeak.get()) : sValue.get();

            const lsp::Color *col   = get_color(value, &sTextRanges, &sTextColor);
            lsp::Color xcol(*col);
            xcol.scale_lch_luminance(bright);

            s->clip_begin(&sAText);
                sFont.draw(s, xcol, fx, fy, fscaling, &text);
            s->clip_end();
        }

        void LedMeterChannel::draw_header(ws::ISurface *s, const Font *f, float fscaling, float bright)
        {
            if (!sActive.get())
                return;

            ws::font_parameters_t   fp;
            ws::text_parameters_t   tp;

            LSPString text;
            sHeader.format(&text);
            sFont.get_parameters(s, fscaling, &fp);
            sFont.get_text_parameters(s, &tp, fscaling, &text);

            ssize_t fx  = sAHeader.nLeft + ((sAHeader.nWidth  - tp.Width ) * 0.5f) + tp.XBearing;
            ssize_t fy  = sAHeader.nTop  + ((sAHeader.nHeight - fp.Height) * 0.5f) + fp.Ascent;

            float value             = sHeaderValue.get();
            const lsp::Color *col   = get_color(value, &sHeaderRanges, &sHeaderColor);
            lsp::Color xcol(*col);
            xcol.scale_lch_luminance(bright);

            s->clip_begin(&sAHeader);
                sFont.draw(s, xcol, fx, fy, fscaling, &text);
            s->clip_end();
        }

        void LedMeterChannel::draw(ws::ISurface *s)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());
            float fscaling      = lsp_max(0.0f, scaling * sFontScaling.get());
            float bright        = sBrightness.get();

            lsp::Color col;
            get_actual_bg_color(col);
            s->clear(col);
            col.copy(sColor);
            s->fill_rect(col, SURFMASK_NONE, 0.0f, &sAAll);

            draw_meter(s, sAngle.get(), scaling, bright);

            if (sTextVisible.get())
                draw_label(s, &sFont, fscaling, bright);
            if (sHeaderVisible.get())
                draw_header(s, &sFont, fscaling, bright);
        }
    } /* namespace tk */
} /* namespace lsp */

