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
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(Led, Widget)
                // Bind
                sColor.bind("color", this);
                sLedColor.bind("led.color", this);
                sHoleColor.bind("hole.color", this);
                sBorderColor.bind("border.color", this);
                sLedBorderColor.bind("led.border.color", this);
                sConstraints.bind("size.constraints", this);
                sOn.bind("on", this);
                sHole.bind("hole", this);
                sLed.bind("led", this);
                sRound.bind("round", this);
                sBorderSize.bind("border.size", this);
                sGradient.bind("gradient", this);
                // Configure
                sColor.set("#cccccc");
                sLedColor.set("#00cc00");
                sHoleColor.set("#000000");
                sBorderColor.set("#888888");
                sLedBorderColor.set("#008800");
                sConstraints.set(8, 8, -1, -1);
                sOn.set(false);
                sHole.set(true);
                sRound.set(true);
                sLed.set(8);
                sBorderSize.set(3);
                sGradient.set(true);
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(Led, "Led", "root");
        }

        const w_class_t Led::metadata           = { "Led", &Widget::metadata };

        Led::Led(Display *dpy):
            Widget(dpy),
            sColor(&sProperties),
            sHoleColor(&sProperties),
            sLedColor(&sProperties),
            sBorderColor(&sProperties),
            sLedBorderColor(&sProperties),
            sConstraints(&sProperties),
            sOn(&sProperties),
            sHole(&sProperties),
            sLed(&sProperties),
            sRound(&sProperties),
            sBorderSize(&sProperties),
            sGradient(&sProperties)
        {
            pClass      = &metadata;
        }

        Led::~Led()
        {
            nFlags     |= FINALIZED;
        }

        status_t Led::init()
        {
            status_t res = Widget::init();
            if (res != STATUS_OK)
                return res;

            sColor.bind("color", &sStyle);
            sLedColor.bind("led.color", &sStyle);
            sHoleColor.bind("hole.color", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sLedBorderColor.bind("led.border.color", &sStyle);
            sConstraints.bind("size.constraints", &sStyle);
            sOn.bind("on", &sStyle);
            sHole.bind("hole", &sStyle);
            sLed.bind("led", &sStyle);
            sRound.bind("round", &sStyle);
            sBorderSize.bind("border.size", &sStyle);
            sGradient.bind("gradient", &sStyle);

            return STATUS_OK;
        }

        void Led::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (sColor.is(prop))
                query_draw();
            if (sLedColor.is(prop))
                query_draw();
            if (sHoleColor.is(prop))
                query_draw();
            if (sBorderColor.is(prop))
                query_draw();
            if (sLedBorderColor.is(prop))
                query_draw();
            if (sConstraints.is(prop))
                query_resize();
            if (sHole.is(prop))
                query_resize();
            if (sOn.is(prop))
                query_draw();
            if (sRound.is(prop))
                query_resize();
            if (sBorderSize.is(prop))
                query_resize();
        }

        void Led::size_request(ws::size_limit_t *r)
        {
            ssize_t extra;
            float scaling       = lsp_max(0.0f, sScaling.get());

            sConstraints.compute(r, scaling);

            if (sRound.get())
            {
                r->nMinWidth        = lsp_max(4, r->nMinWidth);
                r->nMinHeight       = lsp_max(4, r->nMinHeight);

                size_t hole         = (sHole.get()) ? lsp_max(1.0f, scaling) : 0;
                size_t border       = (sGradient.get()) ? 0 : lsp_max(0.0f, scaling * sBorderSize.get());
                size_t led          = lsp_max(0.0f, sLed.get() * scaling);
                extra               = (lsp_max(hole, led) + border) * 2;
            }
            else
            {
                float border        = sBorderSize.get() * scaling;
                ssize_t chamfer     = lsp_max(0.0f, border);
                ssize_t hole        = (sHole.get()) ? lsp_max(1, scaling) : 0;
                ssize_t light       = lsp_max(1, scaling * (sLed.get() + 2));
                ssize_t outer       = lsp_max(hole, light);
                extra               = (chamfer + outer) * 2;

                r->nMinWidth        = lsp_max(chamfer * 2, r->nMinWidth);
                r->nMinHeight       = lsp_max(chamfer * 2, r->nMinHeight);
            }

            r->nMinWidth       += extra;
            r->nMinHeight      += extra;
            if (r->nMaxWidth >= 0)
                r->nMaxWidth       += extra;
            if (r->nMaxHeight >= 0)
                r->nMaxHeight      += extra;
        }

        void Led::draw(ws::ISurface *s)
        {
            if (sRound.get())
                draw_round(s);
            else
                draw_rect(s);
        }

        void Led::draw_round(ws::ISurface *s)
        {
            ws::IGradient *g = NULL;
            float scaling       = lsp_max(0.0f, sScaling.get());
            float brightness    = sBrightness.get();
            size_t sz_hole      = (sHole.get()) ? lsp_max(1.0f, scaling) : 0;
            size_t sz_led       = lsp_max(0.0f, sLed.get() * scaling);
            bool gradient       = sGradient.get();
            size_t border       = (gradient) ? 0 : lsp_max(0.0f, scaling * sBorderSize.get());
            size_t extra        = lsp_max(sz_hole, sz_led) + border;
            bool on             = sOn.get();

            // Estimate palette
            lsp::Color bg_color;
            lsp::Color hole(sHoleColor);
            lsp::Color col((on) ? sLedColor : sColor);
            lsp::Color border_color((on) ? sLedBorderColor : sBorderColor);

            get_actual_bg_color(bg_color);
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
                s->fill_circle(cx, cy, r + sz_hole + border, hole);

            if (on)
            {
                // Draw light
                g = s->radial_gradient(cx, cy, r + border, cx, cy, xr);
                g->add_color(0.0, col, 0.5f);
                g->add_color(1.0, col, 1.0f);
                s->fill_circle(cx, cy, xr, g);
                delete g;
            }

            if (gradient)
            {
                if (on)
                {
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
            }
            else
            {
                // Draw border
                s->fill_circle(cx, cy, r + border, border_color);
                // Draw led
                s->fill_circle(cx, cy, r, col);
            }

            s->set_antialiasing(aa);
        }

        void Led::draw_rect(ws::ISurface *s)
        {
            ws::IGradient *g    = NULL;
            float brightness    = sBrightness.get();
            float scaling       = lsp_max(0.0f, sScaling.get());
            float border        = sBorderSize.get() * scaling;
            ssize_t chamfer     = lsp_max(0.0f, border);
            ssize_t hole        = (sHole.get()) ? lsp_max(1, scaling) : 0;
            ssize_t light       = lsp_max(1, scaling * (sLed.get() + 2));
            ssize_t outer       = lsp_max(hole, light);
            bool on             = sOn.get();
            bool gradient       = sGradient.get();

            ws::rectangle_t r;
            r.nLeft             = outer;
            r.nTop              = outer;
            r.nWidth            = sSize.nWidth  - outer * 2;
            r.nHeight           = sSize.nHeight - outer * 2;

            // Prepare palette
            lsp::Color bg_color;
            lsp::Color color((on) ? sLedColor : sColor);
            lsp::Color border_color((on) ? sLedBorderColor : sBorderColor);

            get_actual_bg_color(bg_color);

            color.scale_lightness(brightness);
            border_color.scale_lightness(brightness);

            // Draw background
            bool aa     = s->set_antialiasing(false);
            s->fill_rect(bg_color, 0, 0, sSize.nWidth, sSize.nHeight);

            // Draw hole
            if (hole > 0)
            {
                lsp::Color hcolor(sHoleColor);
                size_t hole         = lsp_max(1, scaling);
                s->fill_rect(hcolor, r.nLeft - hole, r.nTop - hole,
                        r.nWidth + hole*2, r.nHeight + hole * 2);
            }

            // Draw light
            if (on)
            {
                float c_x   = sSize.nWidth  >> 1;
                float c_y   = sSize.nHeight >> 1;
                float h_p   = outer;
                float v_p   = outer;
                ssize_t xe  = sSize.nWidth  - 1;
                ssize_t ye  = sSize.nHeight - 1;

                lsp::Color lc(color);

                // Left
                g   =  s->linear_gradient(h_p, c_y, 0, c_y);
                g->add_color(0.0, lc, 0.5f);
                g->add_color(1.0, color, 1.0f);
                s->fill_triangle(0, 0, c_x, c_y, 0, ye, g);
                delete g;

                // Right
                g   =  s->linear_gradient(xe - h_p, c_y, xe, c_y);
                g->add_color(0.0, lc, 0.5f);
                g->add_color(1.0, color, 1.0f);
                s->fill_triangle(xe, ye, c_x, c_y, xe, 0, g);
                delete g;

                // Top
                g   =  s->linear_gradient(c_x, v_p, c_x, 0);
                g->add_color(0.0, lc, 0.5f);
                g->add_color(1.0, color, 1.0f);
                s->fill_triangle(0, 0, xe, 0, c_x, c_y, g);
                delete g;

                // Bottom
                g   =  s->linear_gradient(c_x, ye - v_p, c_x, ye);
                g->add_color(0.0, lc, 0.5f);
                g->add_color(1.0, color, 1.0f);
                s->fill_triangle(xe, ye, 0, ye, c_x, c_y, g);
                delete g;
            }

            // Draw led
            {
                float delta         = sqrtf(r.nWidth * r.nWidth + r.nHeight * r.nHeight);
                float xb            = color.lightness();

                // Draw chamfer
                if (gradient)
                {
                    for (ssize_t i=0; i<chamfer; ++i)
                    {
                        // Compute color
                        float bright = float(i + 1.0f) / (chamfer + 1);

                        // Create gradient
                        g = s->radial_gradient(
                            r.nLeft + r.nWidth, r.nTop, 0,
                            r.nLeft + r.nWidth, r.nTop, delta
                        );

                        color.lightness(bright);
                        g->add_color(0.0, color.red(), color.green(), color.blue());
                        color.lightness(xb * bright);
                        g->add_color(1.0, color.red(), color.green(), color.blue());
                        s->fill_rect(g, r.nLeft, r.nTop, r.nWidth, r.nHeight);
                        delete g;

                        // Update rect
                        r.nLeft        += 1;
                        r.nTop         += 1;
                        r.nWidth       -= 2;
                        r.nHeight      -= 2;
                    }

                    // Draw face
                    g = s->radial_gradient(
                            r.nLeft + r.nWidth, r.nTop, 0,
                            r.nLeft + r.nWidth, r.nTop, delta
                        );
                    color.lightness(1.0f);
                    g->add_color(0.0, color.red(), color.green(), color.blue());
                    color.lightness(xb);
                    g->add_color(1.0, color.red(), color.green(), color.blue());
                    s->fill_rect(g, r.nLeft, r.nTop, r.nWidth, r.nHeight);
                    delete g;
                }
                else
                {
                    // Draw border
                    s->fill_rect(border_color, r.nLeft, r.nTop, r.nWidth, r.nHeight);
                    r.nLeft        += chamfer;
                    r.nTop         += chamfer;
                    r.nWidth       -= chamfer * 2;
                    r.nHeight      -= chamfer * 2;

                    // Draw face
                    s->fill_rect(color, r.nLeft, r.nTop, r.nWidth, r.nHeight);
                }
            }

            s->set_antialiasing(aa);
        }
    } /* namespace tk */
} /* namespace lsp */
