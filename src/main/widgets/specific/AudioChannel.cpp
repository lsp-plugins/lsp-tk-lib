/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 28 сент. 2020 г.
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
#include <lsp-plug.in/common/alloc.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(AudioChannel, Widget)
                // Bind
                sHeadCut.bind("head_cut.length", this);
                sTailCut.bind("tail_cut.length", this);
                sFadeIn.bind("fade_in.length", this);
                sFadeOut.bind("fade_out.length", this);
                sStretchBegin.bind("stretch.begin", this);
                sStretchEnd.bind("stretch.end", this);
                sLoopBegin.bind("loop.begin", this);
                sLoopEnd.bind("loop.end", this);
                sPlayPosition.bind("play.position", this);
                sWaveBorder.bind("wave.border", this);
                sFadeInBorder.bind("fade_in.border", this);
                sFadeOutBorder.bind("fade_out.border", this);
                sStretchBorder.bind("stretch.border", this);
                sLoopBorder.bind("loop.border", this);
                sPlayBorder.bind("play.border", this);
                sLineWidth.bind("line.width", this);
                sColor.bind("color", this);
                sLineColor.bind("line.color", this);
                sWaveBorderColor.bind("wave.border.color", this);
                sHeadCutColor.bind("head_cut.color", this);
                sTailCutColor.bind("tail_cut.color", this);
                sFadeInColor.bind("fade_in.color", this);
                sFadeOutColor.bind("fade_out.color", this);
                sStretchColor.bind("stretch.color", this);
                sLoopColor.bind("loop.color", this);
                sPlayColor.bind("play.color", this);
                sFadeInBorderColor.bind("fade_in.border.color", this);
                sFadeOutBorderColor.bind("fade_out.border.color", this);
                sStretchBorderColor.bind("stretch.border.color", this);
                sLoopBorderColor.bind("loop.border.color", this);
                sConstraints.bind("size.constraints", this);
                // Configure
                sHeadCut.set(0);
                sTailCut.set(0);
                sFadeIn.set(0);
                sFadeOut.set(0);
                sStretchBegin.set(-1);
                sStretchEnd.set(-1);
                sLoopBegin.set(-1);
                sLoopEnd.set(-1);
                sPlayPosition.set(-1);
                sWaveBorder.set(1);
                sFadeInBorder.set(1);
                sFadeOutBorder.set(1);
                sStretchBorder.set(1);
                sLoopBorder.set(1);
                sPlayBorder.set(2);
                sLineWidth.set(1);
                sColor.set("#8800ff00");
                sLineColor.set("#ffffff");
                sWaveBorderColor.set("#00ff00");
                sHeadCutColor.set("#44cccccc");
                sTailCutColor.set("#44cccccc");
                sFadeInColor.set("#88ffff00");
                sFadeOutColor.set("#88ffff00");
                sStretchColor.set("#8800ff00");
                sLoopColor.set("#8800ffff");
                sPlayColor.set("#ffffff");
                sFadeInBorderColor.set("#ffff00");
                sFadeOutBorderColor.set("#ffff00");
                sStretchBorderColor.set("#00ff00");
                sLoopBorderColor.set("#00ffff");
                sConstraints.set(128, 32, -1, -1);
                // Override
                sBgColor.set("#000000");
                // Commit
                sBgColor.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(AudioChannel, "AudioChannel", "root");
        }

        const w_class_t AudioChannel::metadata      = { "AudioChannel", &Widget::metadata };

        AudioChannel::AudioChannel(Display *dpy):
            Widget(dpy),
            vSamples(&sProperties),
            sHeadCut(&sProperties),
            sTailCut(&sProperties),
            sFadeIn(&sProperties),
            sFadeOut(&sProperties),
            sStretchBegin(&sProperties),
            sStretchEnd(&sProperties),
            sLoopBegin(&sProperties),
            sLoopEnd(&sProperties),
            sPlayPosition(&sProperties),
            sWaveBorder(&sProperties),
            sFadeInBorder(&sProperties),
            sFadeOutBorder(&sProperties),
            sStretchBorder(&sProperties),
            sLoopBorder(&sProperties),
            sPlayBorder(&sProperties),
            sLineWidth(&sProperties),
            sColor(&sProperties),
            sLineColor(&sProperties),
            sWaveBorderColor(&sProperties),
            sHeadCutColor(&sProperties),
            sTailCutColor(&sProperties),
            sFadeInColor(&sProperties),
            sFadeOutColor(&sProperties),
            sStretchColor(&sProperties),
            sLoopColor(&sProperties),
            sPlayColor(&sProperties),
            sFadeInBorderColor(&sProperties),
            sFadeOutBorderColor(&sProperties),
            sStretchBorderColor(&sProperties),
            sLoopBorderColor(&sProperties),
            sConstraints(&sProperties)
        {
            pClass          = &metadata;
        }

        AudioChannel::~AudioChannel()
        {
            nFlags     |= FINALIZED;
        }

        status_t AudioChannel::init()
        {
            status_t res = Widget::init();
            if (res != STATUS_OK)
                return res;

            // Bind properties
            sHeadCut.bind("head_cut.length", &sStyle);
            sTailCut.bind("tail_cut.length", &sStyle);
            sFadeIn.bind("fade_in.length", &sStyle);
            sFadeOut.bind("fade_out.length", &sStyle);
            sStretchBegin.bind("stretch.begin", &sStyle);
            sStretchEnd.bind("stretch.end", &sStyle);
            sLoopBegin.bind("loop.begin", &sStyle);
            sLoopEnd.bind("loop.end", &sStyle);
            sPlayPosition.bind("play.position", &sStyle);
            sWaveBorder.bind("wave.border", &sStyle);
            sFadeInBorder.bind("fade_in.border", &sStyle);
            sFadeOutBorder.bind("fade_out.border", &sStyle);
            sStretchBorder.bind("stretch.border", &sStyle);
            sLoopBorder.bind("loop.border", &sStyle);
            sPlayBorder.bind("play.border", &sStyle);
            sLineWidth.bind("line.width", &sStyle);
            sColor.bind("color", &sStyle);
            sLineColor.bind("line.color", &sStyle);
            sWaveBorderColor.bind("wave.border.color", &sStyle);
            sHeadCutColor.bind("head_cut.color", &sStyle);
            sTailCutColor.bind("tail_cut.color", &sStyle);
            sFadeInColor.bind("fade_in.color", &sStyle);
            sFadeOutColor.bind("fade_out.color", &sStyle);
            sStretchColor.bind("stretch.color", &sStyle);
            sLoopColor.bind("loop.color", &sStyle);
            sPlayColor.bind("play.color", &sStyle);
            sFadeInBorderColor.bind("fade_in.border.color", &sStyle);
            sFadeOutBorderColor.bind("fade_out.border.color", &sStyle);
            sStretchBorderColor.bind("stretch.border.color", &sStyle);
            sLoopBorderColor.bind("loop.border.color", &sStyle);
            sConstraints.bind("size.constraints", &sStyle);

            return STATUS_OK;
        }

        void AudioChannel::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (vSamples.is(prop))
                query_draw();
            if (prop->one_of(sHeadCut, sTailCut, sFadeIn, sFadeOut, sStretchBegin, sStretchEnd, sLoopBegin, sLoopEnd, sPlayPosition))
                query_draw();
            if (prop->one_of(sWaveBorder, sFadeInBorder, sFadeOutBorder, sStretchBorder, sLoopBorder, sPlayBorder, sLineWidth))
                query_draw();
            if (prop->one_of(sColor, sLineColor, sWaveBorderColor,
                sHeadCutColor, sTailCutColor, sFadeInColor, sFadeOutColor, sStretchColor, sLoopColor, sPlayColor,
                sFadeInBorderColor, sFadeOutBorderColor, sStretchBorderColor, sLoopBorderColor))
                query_draw();
            if (sConstraints.is(prop))
                query_resize();
        }

        void AudioChannel::size_request(ws::size_limit_t *r)
        {
            Widget::size_request(r);

            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t line_w      = (sLineWidth.get() > 0) ? lsp_max(1.0f, sLineWidth.get() * scaling) : 0.0f;

            r->nMinHeight       = line_w * 8;

            sConstraints.apply(r, scaling);
        }

        void AudioChannel::draw_samples(const ws::rectangle_t *r, ws::ISurface *s, size_t samples, float scaling, float bright)
        {
            // Check limits
            if ((samples <= 0) || (r->nWidth <= 1) || (r->nHeight <= 1))
                return;

            // Init decimation buffer
            ssize_t n_draw      = lsp_min(ssize_t(samples), r->nWidth);
            size_t n_points     = n_draw + 2;
            size_t n_decim      = lsp::align_size(n_points, 16); // 2 additional points at start and end

            // Try to allocate memory
            uint8_t *data       = NULL;
            float *x            = lsp::alloc_aligned<float>(data, n_decim * 2);
            float *y            = &x[n_decim];
            if (x == NULL)
                return;

            // Form the x and y values
            float border        = (sWaveBorder.get() > 0) ? lsp_max(1.0f, sWaveBorder.get() * scaling) : 0.0f;
            float dx            = lsp_max(1.0f, float(r->nWidth) / float(samples));
            float kx            = lsp_max(1.0f, float(samples) / float(r->nWidth));
            float ky            = -0.5f * (r->nHeight - border);
            float sy            = r->nTop + r->nHeight * 0.5f;

            x[0]                = -1.0f;
            y[0]                = sy;
            x[n_points-1]       = r->nWidth;
            y[n_points-1]       = sy;

            for (ssize_t i=1; i <= n_draw; ++i)
            {
                ssize_t xx          = i - 1;
                x[i]                = xx * dx;
                y[i]                = sy + ky * vSamples.get(ssize_t(xx * kx));
            }

            // Draw the poly
            lsp::Color fill(sColor);
            lsp::Color wire(sWaveBorderColor);
            fill.scale_lch_luminance(bright);
            wire.scale_lch_luminance(bright);

            bool aa             = s->set_antialiasing(true);
            s->draw_poly(fill, wire, border, x, y, n_points);
            s->set_antialiasing(aa);

            // Free allocated data
            lsp::free_aligned(data);
        }

        void AudioChannel::draw_fades(const ws::rectangle_t *r, ws::ISurface *s, size_t samples, float scaling, float bright)
        {
            // Check limits
            if ((samples <= 0) || (r->nWidth <= 1) || (r->nHeight <= 1))
                return;

            float x[6], y[6];

            bool aa             = s->set_antialiasing(true);
            lsp_finally { s->set_antialiasing(aa); };

            float fi_border     = (sFadeInBorder.get() > 0)  ? lsp_max(1.0f, sFadeInBorder.get()  * scaling) : 0.0f;
            float fo_border     = (sFadeOutBorder.get() > 0) ? lsp_max(1.0f, sFadeOutBorder.get() * scaling) : 0.0f;

            // Draw fade in
            if ((sFadeIn.get() > 0) || (sHeadCut.get() > 0))
            {
                float xx            = float(sFadeIn.get() * r->nWidth) / float(samples);

                // Form the arrays
                x[0]                = r->nLeft;
                x[1]                = xx;
                x[2]                = x[0];
                x[3]                = x[1];
                x[4]                = x[0];
                x[5]                = x[0];

                y[0]                = r->nTop;
                y[1]                = y[0];
                y[2]                = y[0] + (r->nHeight >> 1);
                y[3]                = y[0] + r->nHeight;
                y[4]                = y[3];
                y[5]                = y[0];

                // Apply head cut if set
                if (sHeadCut.get() > 0)
                {
                    lsp::Color cut(sHeadCutColor);
                    cut.scale_lch_luminance(bright);

                    float dx            = float(sHeadCut.get() * r->nWidth) / float(samples);
                    s->fill_rect(cut, SURFMASK_NONE, 0.0f, r->nLeft, r->nTop, dx, r->nHeight);
                    for (size_t i=0,n=sizeof(x)/sizeof(x[0]); i<n; ++i)
                        x[i]               += dx;
                }

                // Draw fade
                lsp::Color fill(sFadeInColor);
                lsp::Color wire(sFadeInBorderColor);
                fill.scale_lch_luminance(bright);
                wire.scale_lch_luminance(bright);

                s->draw_poly(fill, wire, fi_border, x, y, 6);
            }

            // Draw fade out
            if ((sFadeOut.get() > 0) || (sTailCut.get() > 0))
            {
                float xx            = r->nLeft + r->nWidth - float(sFadeOut.get() * r->nWidth) / float(samples);

                // Form the arrays
                x[0]                = r->nLeft + r->nWidth;
                x[1]                = xx;
                x[2]                = x[0];
                x[3]                = x[1];
                x[4]                = x[0];
                x[5]                = x[0];

                y[0]                = r->nTop;
                y[1]                = y[0];
                y[2]                = y[0] + (r->nHeight >> 1);
                y[3]                = y[0] + r->nHeight;
                y[4]                = y[3];
                y[5]                = y[0];

                // Apply tail cut if set
                if (sTailCut.get() > 0)
                {
                    lsp::Color cut(sTailCutColor);
                    cut.scale_lch_luminance(bright);

                    float dx            = float(sTailCut.get() * r->nWidth) / float(samples);
                    s->fill_rect(cut, SURFMASK_NONE, 0.0f, r->nLeft + r->nWidth - dx, r->nTop, dx, r->nHeight);
                    for (size_t i=0,n=sizeof(x)/sizeof(x[0]); i<n; ++i)
                        x[i]               -= dx;
                }

                // Draw fade
                lsp::Color fill(sFadeOutColor);
                lsp::Color wire(sFadeOutBorderColor);
                fill.scale_lch_luminance(bright);
                wire.scale_lch_luminance(bright);

                s->draw_poly(fill, wire, fo_border, x, y, 6);
            }
        }

        void AudioChannel::draw_range(const ws::rectangle_t *r, ws::ISurface *s, range_t *range, size_t samples, float scaling, float bright)
        {
            // Check limits
            if ((samples <= 0) || (r->nWidth <= 1) || (r->nHeight <= 1))
                return;

            // Check that the value is enabled
            ssize_t begin   = range->begin->get();
            ssize_t end     = range->end->get();
            if ((begin < 0) && (end < 0))
                return;
            if (begin > end)
                return;

            float border        = (range->border->get() > 0) ? lsp_max(1.0f, range->border->get() * scaling) : 0.0f;
            float xb            = r->nLeft + float(begin * r->nWidth) / float(samples);
            float xe            = r->nLeft + float(end * r->nWidth) / float(samples);

            // Draw the range
            lsp::Color fill(*range->color);
            lsp::Color wire(*range->border_color);
            fill.scale_lch_luminance(bright);
            wire.scale_lch_luminance(bright);

            bool aa             = s->set_antialiasing(true);
            lsp_finally { s->set_antialiasing(aa); };

            s->fill_rect(fill, SURFMASK_NONE, 0.0f, r->nLeft + xb, r->nTop, xe - xb, r->nHeight);
            if (border > 0)
            {
                s->line(wire, xb, r->nTop, xb, r->nTop + r->nHeight, border);
                s->line(wire, xe, r->nTop, xe, r->nTop + r->nHeight, border);
            }
        }

        void AudioChannel::draw_play_position(const ws::rectangle_t *r, ws::ISurface *s, size_t samples, float scaling, float bright)
        {
            // Check limits
            if ((samples <= 0) || (r->nWidth <= 1) || (r->nHeight <= 1))
                return;
            ssize_t position    = sPlayPosition.get();
            ssize_t pborder     = sPlayBorder.get();
            if ((position < 0) || (pborder < 0))
                return;

            float x             = float(r->nLeft + (position * r->nWidth) / samples);
            float border        = lsp_max(1.0f, pborder * scaling);

            lsp::Color wire(sPlayColor);
            wire.scale_lch_luminance(bright);

            bool aa             = s->set_antialiasing(true);
            lsp_finally { s->set_antialiasing(aa); };

            s->line(wire, x, r->nTop, x, r->nTop + r->nHeight, border);
        }

        void AudioChannel::draw(ws::ISurface *s)
        {
            float bright        = sBrightness.get();
            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t line_w      = (sLineWidth.get() > 0) ? lsp_max(1.0f, sLineWidth.get() * scaling) : 0.0f;

            ws::rectangle_t r   = sSize;
            r.nLeft             = 0;
            r.nTop              = 0;

            // Clear the surface
            lsp::Color bg;
            get_actual_bg_color(bg);
            bg.scale_lch_luminance(bright);
            s->clear(bg);

            size_t samples      = vSamples.size();

            s->clip_begin(&r);
            {
                // Define ranges
                range_t stretch = {
                    &sStretchBegin,
                    &sStretchEnd,
                    &sStretchBorder,
                    &sStretchColor,
                    &sStretchBorderColor
                };
                range_t loop = {
                    &sLoopBegin,
                    &sLoopEnd,
                    &sLoopBorder,
                    &sLoopColor,
                    &sLoopBorderColor
                };

                // Draw channel
                draw_samples(&r, s, samples, scaling, bright);
                draw_range(&r, s, &stretch, samples, scaling, bright);
                draw_range(&r, s, &loop, samples, scaling, bright);
                draw_fades(&r, s, samples, scaling, bright);

                // Draw line
                if (line_w > 0)
                {
                    lsp::Color line(sLineColor);
                    line.scale_lch_luminance(bright);

                    float sy            = r.nHeight * 0.5f;
                    bool aa             = s->set_antialiasing(false);
                    s->line(line, r.nLeft, sy, r.nLeft + r.nWidth, sy, line_w);
                    s->set_antialiasing(aa);
                }

                // Draw play position
                draw_play_position(&r, s, samples, scaling, bright);
            }
            s->clip_end();
        }
    } /* namespace tk */
} /* namespace lsp */


