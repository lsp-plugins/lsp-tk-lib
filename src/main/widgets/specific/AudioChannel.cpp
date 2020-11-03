/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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
        STYLE_INITIALIZER_BEGIN(AudioChannel, Widget);

            // Bind properties
            prop::Integer::init("fade_in.length", style, 0);
            prop::Integer::init("fade_out.length", style, 0);
            prop::Integer::init("wave.border", style, 1);
            prop::Integer::init("fade_in.border", style, 1);
            prop::Integer::init("fade_out.border", style, 1);
            prop::Integer::init("line.width", style, 1);
            prop::Color::init("color", style, "#8800ff00");
            prop::Color::init("line.color", style, "#ffffff");
            prop::Color::init("wave.border.color", style, "#00ff00");
            prop::Color::init("fade_in.color", style, "#88ffff00");
            prop::Color::init("fade_out.color", style, "#88ffff00");
            prop::Color::init("fade_in.border.color", style, "#ffff00");
            prop::Color::init("fade_out.border.color", style, "#ffff00");
            prop::SizeConstraints::init("size.constraints", style, 128, 32, -1, -1);
            // Overrides
            prop::Color::override("bg.color", style, "#000000");

        STYLE_INITIALIZER_END(AudioChannel, "AudioChannel");
        LSP_BUILTIN_STYLE(AudioChannel);

        const w_class_t AudioChannel::metadata      = { "AudioChannel", &Widget::metadata };

        AudioChannel::AudioChannel(Display *dpy):
            Widget(dpy),
            vSamples(&sProperties),
            sFadeIn(&sProperties),
            sFadeOut(&sProperties),
            sWaveBorder(&sProperties),
            sFadeInBorder(&sProperties),
            sFadeOutBorder(&sProperties),
            sLineWidth(&sProperties),
            sColor(&sProperties),
            sLineColor(&sProperties),
            sWaveBorderColor(&sProperties),
            sFadeInColor(&sProperties),
            sFadeOutColor(&sProperties),
            sFadeInBorderColor(&sProperties),
            sFadeOutBorderColor(&sProperties),
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
            sFadeIn.bind("fade_in.length", &sStyle);
            sFadeOut.bind("fade_out.length", &sStyle);
            sWaveBorder.bind("wave.border", &sStyle);
            sFadeInBorder.bind("fade_in.border", &sStyle);
            sFadeOutBorder.bind("fade_out.border", &sStyle);
            sLineWidth.bind("line.width", &sStyle);
            sColor.bind("color", &sStyle);
            sLineColor.bind("line.color", &sStyle);
            sWaveBorderColor.bind("wave.border.color", &sStyle);
            sFadeInColor.bind("fade_in.color", &sStyle);
            sFadeOutColor.bind("fade_out.color", &sStyle);
            sFadeInBorderColor.bind("fade_in.border.color", &sStyle);
            sFadeOutBorderColor.bind("fade_out.border.color", &sStyle);
            sConstraints.bind("size.constraints", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sFadeIn.init(sclass, 0);
                sFadeOut.init(sclass, 0);
                sWaveBorder.init(sclass, 1);
                sFadeInBorder.init(sclass, 1);
                sFadeOutBorder.init(sclass, 1);
                sLineWidth.init(sclass, 1);
                sColor.init(sclass, "#8800ff00");
                sLineColor.init(sclass, "#ffffff");
                sWaveBorderColor.init(sclass, "#00ff00");
                sFadeInColor.init(sclass, "#88ffff00");
                sFadeOutColor.init(sclass, "#88ffff00");
                sFadeInBorderColor.init(sclass, "#ffff00");
                sFadeOutBorderColor.init(sclass, "#ffff00");
                sConstraints.init(sclass, 128, 32, -1, -1);

                sBgColor.override(sclass, "#000000");
            }

            return STATUS_OK;
        }

        void AudioChannel::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (vSamples.is(prop))
                query_draw();

            if (sFadeIn.is(prop))
                query_draw();
            if (sFadeOut.is(prop))
                query_draw();
            if (sWaveBorder.is(prop))
                query_draw();
            if (sFadeInBorder.is(prop))
                query_draw();
            if (sFadeOutBorder.is(prop))
                query_draw();
            if (sLineWidth.is(prop))
                query_resize();
            if (sColor.is(prop))
                query_draw();
            if (sLineColor.is(prop))
                query_draw();
            if (sWaveBorderColor.is(prop))
                query_draw();
            if (sFadeInColor.is(prop))
                query_draw();
            if (sFadeOutColor.is(prop))
                query_draw();
            if (sFadeInBorderColor.is(prop))
                query_draw();
            if (sFadeOutBorderColor.is(prop))
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
            fill.scale_lightness(bright);
            wire.scale_lightness(bright);

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

            // Draw fade in
            if (sFadeIn.get() > 0)
            {
                float border        = (sFadeInBorder.get() > 0) ? lsp_max(1.0f, sFadeInBorder.get() * scaling) : 0.0f;
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

                lsp::Color fill(sFadeInColor);
                lsp::Color wire(sFadeInBorderColor);
                fill.scale_lightness(bright);
                wire.scale_lightness(bright);

                s->draw_poly(fill, wire, border, x, y, 6);
            }

            // Draw fade out
            if (sFadeOut.get() > 0)
            {
                float border        = (sFadeOutBorder.get() > 0) ? lsp_max(1.0f, sFadeOutBorder.get() * scaling) : 0.0f;
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

                lsp::Color fill(sFadeOutColor);
                lsp::Color wire(sFadeOutBorderColor);
                fill.scale_lightness(bright);
                wire.scale_lightness(bright);

                s->draw_poly(fill, wire, border, x, y, 6);
            }

            s->set_antialiasing(aa);
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
            lsp::Color bg(sBgColor);
            bg.scale_lightness(bright);
            s->clear(bg);

            size_t samples      = vSamples.size();

            s->clip_begin(&r);
            {
                // Draw channel
                draw_samples(&r, s, samples, scaling, bright);
                draw_fades(&r, s, samples, scaling, bright);

                // Draw line
                if (line_w > 0)
                {
                    lsp::Color line(sLineColor);
                    line.scale_lightness(bright);

                    float sy            = r.nHeight * 0.5f;
                    bool aa             = s->set_antialiasing(false);
                    s->line(r.nLeft, sy, r.nLeft + r.nWidth, sy, line_w, line);
                    s->set_antialiasing(aa);
                }
            }
            s->clip_end();
        }
    }
}


