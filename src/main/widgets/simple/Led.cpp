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

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t Led::metadata           = { "Led", &Widget::metadata };

        Led::Led(Display *dpy):
            Widget(dpy),
            sColor(&sProperties),
            sHoleColor(&sProperties),
            sLightColor(&sProperties),
            sSizeRange(&sProperties),
            sOn(&sProperties),
            sHole(&sProperties),
            sLed(&sProperties)
        {
            pClass      = &metadata;
        }

        Led::~Led()
        {
        }

        status_t Led::init()
        {
            status_t res = Widget::init();
            if (res != STATUS_OK)
                return res;

            sColor.bind("color", &sStyle);
            sLightColor.bind("led.color", &sStyle);
            sHoleColor.bind("hole.color", &sStyle);
            sSizeRange.bind("size", &sStyle);
            sOn.bind("on", &sStyle);
            sHole.bind("hole", &sStyle);
            sLed.bind("led", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sColor.init(sclass, "#cccccc");
                sLightColor.init(sclass, "#00cc00");
                sHoleColor.init(sclass, "#000000");
                sSizeRange.init(sclass, 8, -1);
                sOn.init(sclass, false);
                sHole.init(sclass, true);
                sLed.init(sclass, 8);
            }

            return STATUS_OK;
        }

        void Led::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (sColor.is(prop))
                query_draw();
            if (sLightColor.is(prop))
                query_draw();
            if (sHoleColor.is(prop))
                query_draw();
            if (sSizeRange.is(prop))
                query_resize();
            if (sHole.is(prop))
                query_resize();
            if (sOn.is(prop))
                query_draw();
        }

        void Led::size_request(ws::size_limit_t *r)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());
            sSizeRange.compute(r, scaling);
            r->nMinWidth        = lsp_max(4, r->nMinWidth);
            r->nMinHeight       = lsp_max(4, r->nMinHeight);

            size_t hole         = (sHole.get()) ? lsp_max(1.0f, scaling) : 0;
            size_t led          = lsp_max(0.0f, sLed.get() * scaling);
            size_t extra        = lsp_max(hole, led);

            r->nMinWidth       += extra * 2;
            r->nMinHeight      += extra * 2;
            if (r->nMaxWidth >= 0)
                r->nMaxWidth       += extra * 2;
            if (r->nMaxHeight >= 0)
                r->nMaxHeight      += extra * 2;
            r->nPreWidth        = -1;
            r->nPreHeight       = -1;
        }

        void Led::draw(ws::ISurface *s)
        {
            ws::IGradient *g = NULL;
            float scaling       = lsp_max(0.0f, sScaling.get());
            float brightness    = sBrightness.get();
            size_t sz_hole      = (sHole.get()) ? lsp_max(1.0f, scaling) : 0;
            size_t sz_led       = lsp_max(0.0f, sLed.get() * scaling);
            size_t extra        = lsp_max(sz_hole, sz_led);
            bool on             = sOn.get();

            // Estimate palette
            lsp::Color bg_color(sBgColor);
            lsp::Color hole(sHoleColor);
            lsp::Color col((on) ? sLightColor : sColor);

            col.scale_lightness(brightness);

            // Draw background
            s->fill_rect(bg_color, 0, 0, sSize.nWidth, sSize.nHeight);

            // Move to center of the led
            ssize_t xr          = lsp_min(sSize.nWidth, sSize.nHeight) >> 1;
            ssize_t r           = xr - extra;
            ssize_t cx          = (sSize.nWidth >> 1);
            ssize_t cy          = (sSize.nHeight >> 1);

            // Draw hole
            bool aa = s->set_antialiasing(true);
            if (sHole.get())
                s->fill_circle(cx, cy, r + sz_hole, hole);

            if (on)
            {
                // Draw light
                g = s->radial_gradient(cx, cy, r, cx, cy, xr);
                g->add_color(0.0, col, 0.5f);
                g->add_color(1.0, col, 1.0f);
                s->fill_circle(cx, cy, xr, g);
                delete g;

                // Draw led spot
                lsp::Color c_light(col);
                c_light.lightness(c_light.lightness() * 1.5);

                g = s->radial_gradient(cx, cy, r * 0.25, cx, cy, r);
                g->add_color(0.0f, c_light);
                g->add_color(1.0f, col);
                s->fill_circle(cx, cy, r, g);
                delete g;

                // Add blink
                g = s->radial_gradient(cx + (r * 0.25f), cy - (r * 0.25f), r * 0.125f, cx, cy, r);
                g->add_color(0.0, 1.0, 1.0, 1.0, 0.0f);
                g->add_color(1.0, 1.0, 1.0, 1.0, 1.0f);
                s->fill_circle(cx, cy, r, g);
                delete g;
            }
            else
            {
                lsp::Color c(col);
                c.scale_lightness(0.4f);

                // Draw led glass
                g = s->radial_gradient(cx, cy, r * 0.25f, cx, cy, r);
                g->add_color(0.0, col);
                g->add_color(1.0, c);
                s->fill_circle(cx, cy, r, g);
                delete g;

                // Add blink
                g = s->radial_gradient(cx + (r * 0.25f), cy - (r * 0.25f), r * 0.125f, cx, cy, r);
                g->add_color(0.0, 1.0, 1.0, 1.0, 0.5);
                g->add_color(1.0, 1.0, 1.0, 1.0, 1.0);
                s->fill_circle(cx, cy, r, g);
                delete g;
            }

            s->set_antialiasing(aa);
        }
    } /* namespace tk */
} /* namespace lsp */
