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

namespace lsp
{
    namespace tk
    {
        const w_class_t AudioChannel::metadata      = { "AudioChannel", &Widget::metadata };

        AudioChannel::AudioChannel(Display *dpy):
            Widget(dpy),
            vSamples(&sProperties),
            sFadeIn(&sProperties),
            sFadeOut(&sProperties),
            sLineWidth(&sProperties),
            sColor(&sProperties),
            sFadeInColor(&sProperties),
            sFadeOutColor(&sProperties),
            sLineColor(&sProperties),
            sConstraints(&sProperties)
        {
            pClass          = &metadata;
        }

        AudioChannel::~AudioChannel()
        {
        }

        status_t AudioChannel::init()
        {
            status_t res = Widget::init();
            if (res != STATUS_OK)
                return res;

            // Bind properties
            sFadeIn.bind("fade_in.length", &sStyle);
            sFadeOut.bind("fade_out.length", &sStyle);
            sLineWidth.bind("line.width", &sStyle);
            sColor.bind("color", &sStyle);
            sFadeInColor.bind("fade_in.color", &sStyle);
            sFadeOutColor.bind("fade_out.color", &sStyle);
            sLineColor.bind("line.color", &sStyle);
            sConstraints.bind("size.constraints", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sFadeIn.init(sclass, 0);
                sFadeOut.init(sclass, 0);
                sLineWidth.init(sclass, 1);
                sColor.init(sclass, "#00ff00");
                sFadeInColor.init(sclass, "#88ffff00");
                sFadeOutColor.init(sclass, "#88ffff00");
                sLineColor.init(sclass, "#ffffff");
                sConstraints.init(sclass, 128, 32, -1, -1);
            }

            return STATUS_OK;
        }

        void AudioChannel::destroy()
        {
            Widget::destroy();
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
            if (sLineWidth.is(prop))
                query_resize();
            if (sColor.is(prop))
                query_draw();
            if (sFadeInColor.is(prop))
                query_draw();
            if (sFadeOutColor.is(prop))
                query_draw();
            if (sLineColor.is(prop))
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

        void AudioChannel::draw_samples(const ws::rectangle_t *r, ws::ISurface *s, lsp::Color &col, size_t dmask)
        {
            // TODO
        }

        void AudioChannel::draw_fades(const ws::rectangle_t *r, ws::ISurface *s, lsp::Color &fin, lsp::Color &fout, size_t dmask)
        {
            // TODO
        }

        void AudioChannel::draw(ws::ISurface *s)
        {
            float bright        = sBrightness.get();
            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t line_w      = (sLineWidth.get() > 0) ? lsp_max(1.0f, sLineWidth.get() * scaling) : 0.0f;

            lsp::Color col(sColor);
            lsp::Color fin(sFadeInColor);
            lsp::Color fout(sFadeOutColor);

            col.scale_lightness(bright);
            fin.scale_lightness(bright);
            fout.scale_lightness(bright);

            ws::rectangle_t r   = sSize;

            s->clip_begin(&r);
            {
                // Draw top part
                r.nHeight           = (sSize.nHeight - line_w) >> 1;
                draw_samples(&r, s, col, DMASK_RIGHT | DMASK_UP);
                draw_fades(&r, s, fin, fout, DMASK_RIGHT | DMASK_UP);

                // Draw Symmetric bottom part
                r.nTop             += r.nHeight + line_w;
                r.nHeight           = sSize.nTop + sSize.nHeight - r.nTop;
                draw_samples(&r, s, col, DMASK_RIGHT | DMASK_DOWN);
                draw_fades(&r, s, fin, fout, DMASK_RIGHT | DMASK_DOWN);

                // Draw line
                if (line_w > 0)
                {
                    lsp::Color line(sFadeInColor);
                    line.scale_lightness(bright);

                    r.nTop              = sSize.nTop + ((sSize.nHeight - line_w) >> 1);
                    s->line(r.nLeft, r.nTop, r.nLeft + r.nWidth, r.nTop, line_w, line);
                }
            }
            s->clip_end();
        }
    }
}


