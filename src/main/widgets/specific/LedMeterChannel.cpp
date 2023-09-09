/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
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
                sBalance.bind("balance", this);
                sColor.bind("color", this);
                sValueColor.bind("value.color", this);
                sValueRanges.bind("value.ranges", this);
                sPeakColor.bind("peak.color", this);
                sPeakRanges.bind("peak.ranges", this);
                sTextColor.bind("text.color", this);
                sTextRanges.bind("text.ranges", this);
                sBalanceColor.bind("balance.color", this);
                sPeakVisible.bind("peak.visible", this);
                sBalanceVisible.bind("balance.visible", this);
                sTextVisible.bind("text.visible", this);
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
                sBalance.set(0.5f);
                sColor.set("#000000");
                sValueColor.set("#00ff00");
                sValueRanges.set_all("");
                sPeakColor.set("#ff0000");
                sPeakRanges.set_all("");
                sBalanceColor.set("#ffff00");
                sTextColor.set("#00ff00");
                sTextRanges.set_all("");
                sBalanceColor.set("#ffff00");
                sPeakVisible.set(false);
                sBalanceVisible.set(false);
                sTextVisible.set(false);
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
            sBalance(&sProperties),
            sColor(&sProperties),
            sValueColor(&sProperties),
            sValueRanges(&sProperties),
            sPeakColor(&sProperties),
            sPeakRanges(&sProperties),
            sTextColor(&sProperties),
            sTextRanges(&sProperties),
            sBalanceColor(&sProperties),
            sText(&sProperties),
            sEstText(&sProperties),
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
            sBalance.bind("balance", &sStyle);
            sColor.bind("color", &sStyle);
            sValueColor.bind("value.color", &sStyle);
            sValueRanges.bind("value.ranges", &sStyle);
            sPeakColor.bind("peak.color", &sStyle);
            sPeakRanges.bind("peak.ranges", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sTextRanges.bind("text.ranges", &sStyle);
            sBalanceColor.bind("balance.color", &sStyle);
            sText.bind(&sStyle, pDisplay->dictionary());
            sEstText.bind(&sStyle, pDisplay->dictionary());
            sPeakVisible.bind("peak.visible", &sStyle);
            sBalanceVisible.bind("balance.visible", &sStyle);
            sTextVisible.bind("text.visible", &sStyle);
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

            return STATUS_OK;
        }

        void LedMeterChannel::property_changed(Property *prop)
        {
            Widget::property_changed(prop);
            if (sActive.get())
            {
                if (sValue.is(prop))
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
            if (sText.is(prop) && (sTextVisible.get()))
                query_draw();
            if (sEstText.is(prop) && (sTextVisible.get()))
                query_resize();
            if (sBalanceVisible.is(prop))
                query_draw();
            if (sTextVisible.is(prop))
                query_draw();
            if (sReversive.is(prop))
                query_draw();
            if (sActive.is(prop))
                query_draw();
            if (sMinSegments.is(prop))
                query_resize();
            if (sConstraints.is(prop))
                query_resize();
            if (sFont.is(prop) && (sTextVisible.get()))
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

            ws::text_parameters_t tp;
            ws::font_parameters_t fp;

            if (has_text)
            {
                LSPString text;
                sEstText.format(&text);
                sFont.get_parameters(pDisplay, fscaling, &fp);
                sFont.get_text_parameters(pDisplay, &tp, fscaling, &text);
                tp.Height           = lsp_max(tp.Height, fp.Height);
            }

            if (angle & 1)
            {
                // Vertical
                r->nMinWidth        = ceilf(seg_size);
                r->nMinHeight       = ceilf(seg_size * lsp_min(0, sMinSegments.get()));

                if (has_text)
                {
                    r->nMinHeight      += border + tp.Height;
                    r->nMinWidth        = lsp_max(r->nMinWidth, tp.Width);
                }
            }
            else
            {
                // Horizontal
                r->nMinWidth        = ceilf(seg_size * lsp_min(0, sMinSegments.get()));
                r->nMinHeight       = ceilf(seg_size);

                if (has_text)
                {
                    r->nMinWidth       += border + tp.Width;
                    r->nMinHeight       = lsp_max(r->nMinHeight, tp.Height);
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

            ws::text_parameters_t tp;
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

            sAMeter.nLeft       = 0;
            sAMeter.nTop        = 0;
            sAMeter.nWidth      = 0;
            sAMeter.nHeight     = 0;

            // Compute the amount of space used for text
            ssize_t led_size    = (angle & 1) ? xr.nHeight : xr.nWidth;

            if (has_text)
            {
                LSPString text;
                sEstText.format(&text);
                sFont.get_parameters(pDisplay, fscaling, &fp);
                sFont.get_text_parameters(pDisplay, &tp, fscaling, &text);

                if (angle & 1) // Vertical
                {
                    sAText.nLeft    = xr.nLeft;
                    sAText.nWidth   = xr.nWidth;
                    sAText.nHeight  = lsp_max(tp.Height, fp.Height);
                    led_size       -= border + sAText.nHeight;
                }
                else // Horizontal
                {
                    sAText.nTop     = xr.nTop;
                    sAText.nWidth   = tp.Width;
                    sAText.nHeight  = xr.nHeight;
                    led_size       -= border + sAText.nWidth;
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

                    sAMeter.nLeft   = xr.nLeft;
                    sAMeter.nTop    = xr.nTop;
                    sAMeter.nWidth  = xr.nWidth;
                    sAMeter.nHeight = led_size;

                    sAText.nTop     = xr.nTop + sAMeter.nHeight + border;

                    break;
                }

                case 3: // Top to bottom
                {
                    xr.nTop        += (gap >> 1);

                    sAAll.nTop     += (gap >> 1);
                    sAAll.nHeight  -= gap;

                    sAMeter.nLeft   = xr.nLeft;
                    sAMeter.nTop    = xr.nTop + ((has_text) ? sAText.nHeight + border : 0);
                    sAMeter.nWidth  = xr.nWidth;
                    sAMeter.nHeight = led_size;

                    sAText.nTop     = xr.nTop;

                    break;
                }

                case 2: // Right to left
                {
                    xr.nLeft       += (gap >> 1);

                    sAAll.nLeft    += (gap >> 1);
                    sAAll.nWidth   -= gap;

                    sAMeter.nLeft   = xr.nLeft;
                    sAMeter.nTop    = xr.nTop;
                    sAMeter.nWidth  = led_size;
                    sAMeter.nHeight = xr.nHeight;

                    sAText.nLeft    = xr.nLeft + sAMeter.nWidth + border;

                    break;
                }

                case 0: // Left to right
                default:
                {
                    xr.nLeft       += (gap >> 1);

                    sAAll.nLeft    += (gap >> 1);
                    sAAll.nWidth   -= gap;

                    sAMeter.nLeft   = xr.nLeft + ((has_text) ? sAText.nWidth + border : 0);
                    sAMeter.nTop    = xr.nTop;
                    sAMeter.nWidth  = led_size;
                    sAMeter.nHeight = xr.nHeight;

                    sAText.nLeft    = xr.nLeft;

                    break;
                }
            }
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
        }
    } /* namespace tk */
} /* namespace lsp */

