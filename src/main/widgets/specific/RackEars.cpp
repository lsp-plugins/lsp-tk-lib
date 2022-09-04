/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 7 окт. 2020 г.
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
            LSP_TK_STYLE_IMPL_BEGIN(RackEars, Widget)
                // Bind
                sFont.bind("font", this);
                sColor.bind("color", this);
                sTextColor.bind("text.color", this);
                sHoleColor.bind("hole.color", this);
                sScrewColor.bind("screw.color", this);
                sAngle.bind("angle", this);
                sButtonPadding.bind("button.padding", this);
                sScrewPadding.bind("screw.padding", this);
                sScrewSize.bind("screw.size", this);
                sTextPadding.bind("text.padding", this);
                // Configure
                sFont.set_size(16.0f);
                sFont.set_bold(true);
                sFont.set_antialiasing(ws::FA_ENABLED);
                sColor.set("#00ccff");
                sScrewColor.set("#444444");
                sTextColor.set("#ffffff");
                sHoleColor.set("#000000");
                sAngle.set(0);
                sButtonPadding.set(2);
                sScrewPadding.set(2);
                sScrewSize.set(20);
                sTextPadding.set(4, 2);
                // Override
                sFont.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(RackEars, "RackEars", "root");
        }

        const w_class_t RackEars::metadata              = { "RackEars", &Widget::metadata };

        RackEars::RackEars(Display *dpy):
            Widget(dpy),
            sFont(&sProperties),
            sText(&sProperties),
            sColor(&sProperties),
            sTextColor(&sProperties),
            sHoleColor(&sProperties),
            sAngle(&sProperties),
            sButtonPadding(&sProperties),
            sScrewPadding(&sProperties),
            sScrewSize(&sProperties),
            sTextPadding(&sProperties)
        {
            nBMask              = 0;
            nXFlags             = 0;

            sButton.nLeft       = 0;
            sButton.nTop        = 0;
            sButton.nWidth      = 0;
            sButton.nHeight     = 0;

            for (size_t i=0; i<2; ++i)
            {
                sScrew[i].nLeft     = 0;
                sScrew[i].nTop      = 0;
                sScrew[i].nWidth    = 0;
                sScrew[i].nHeight   = 0;
            }

            pClass              = &metadata;
        }

        RackEars::~RackEars()
        {
            nFlags     |= FINALIZED;
        }

        status_t RackEars::init()
        {
            status_t res = Widget::init();
            if (res != STATUS_OK)
                return res;

            sFont.bind("font", &sStyle);
            sText.bind(&sStyle, pDisplay->dictionary());
            sColor.bind("color", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sHoleColor.bind("hole.color", &sStyle);
            sScrewColor.bind("screw.color", &sStyle);
            sAngle.bind("angle", &sStyle);
            sButtonPadding.bind("button.padding", &sStyle);
            sScrewPadding.bind("screw.padding", &sStyle);
            sScrewSize.bind("screw.size", &sStyle);
            sTextPadding.bind("text.padding", &sStyle);

            handler_id_t id = sSlots.add(SLOT_SUBMIT, slot_on_submit, self());

            return (id >= 0) ? STATUS_OK : -id;
        }

        void RackEars::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (sFont.is(prop))
                query_resize();
            if (sText.is(prop))
                query_resize();
            if (sColor.is(prop))
                query_draw();
            if (sTextColor.is(prop))
                query_draw();
            if (sHoleColor.is(prop))
                query_draw();
            if (sAngle.is(prop))
                query_resize();
            if (sButtonPadding.is(prop))
                query_resize();
            if (sScrewPadding.is(prop))
                query_resize();
            if (sScrewSize.is(prop))
                query_resize();
            if (sTextPadding.is(prop))
                query_resize();
        }

        void RackEars::estimate_sizes(ws::rectangle_t *screw, ws::rectangle_t *btn)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());
            float fscaling      = lsp_max(0.0f, scaling * sFontScaling.get());
            ssize_t angle       = (sAngle.get() & 0x03);
            ssize_t chamfer     = lsp_max(1.0f, scaling * 3.0f);

            // Screw parameters
            screw->nLeft        = 0;
            screw->nTop         = 0;
            screw->nHeight      = ceilf(sScrewSize.get() * scaling);
            screw->nWidth       = screw->nHeight * 1.5f;

            sScrewPadding.add(screw, scaling);

            // Button parameters
            ws::font_parameters_t fp;
            ws::text_parameters_t tp;
            LSPString text;

            sText.format(&text);
            sFont.get_parameters(pDisplay, fscaling, &fp);
            sFont.get_text_parameters(pDisplay, &tp, fscaling, &text);

            btn->nLeft          = 0;
            btn->nTop           = 0;
            btn->nWidth         = tp.Width;
            btn->nHeight        = fp.Height;
            sTextPadding.add(btn, scaling);

            if (!(angle & 1))
            {
                btn->nHeight        = lsp_max(btn->nHeight, 1.5f * screw->nHeight);
                btn->nWidth         = lsp_max(btn->nWidth, btn->nHeight * M_GOLD_RATIO);
            }

            btn->nWidth        += chamfer * 2;
            btn->nHeight       += chamfer * 2;

            sButtonPadding.add(btn, scaling);
        }

        void RackEars::size_request(ws::size_limit_t *r)
        {
            ws::rectangle_t screw, btn;
            estimate_sizes(&screw, &btn);

            ssize_t angle       = (sAngle.get() & 0x03);

            if (angle & 1)
            {
                // Horizontal orientation
                r->nMinWidth    = screw.nWidth * 2  + btn.nWidth;
                r->nMinHeight   = lsp_max(screw.nHeight, btn.nHeight);

                r->nMaxWidth    = -1;
                r->nMaxHeight   = r->nMinHeight;
            }
            else
            {
                // Vertical orientation
                r->nMinWidth    = lsp_max(screw.nWidth, btn.nWidth);
                r->nMinHeight   = screw.nHeight * 2 + btn.nHeight;

                r->nMaxWidth    = r->nMinWidth;
                r->nMaxHeight   = -1;
            }

            r->nPreWidth    = -1;
            r->nPreHeight   = -1;
        }

        void RackEars::realize(const ws::rectangle_t *r)
        {
            Widget::realize(r);

            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t angle       = (sAngle.get() & 0x03);

            ws::rectangle_t screw[2], btn;
            estimate_sizes(&screw[0], &btn);
            screw[1] = screw[0];

            // Position of screws
            if (angle & 1)
            {
                // Horizontal orientation
                screw[0].nLeft  = sSize.nLeft;
                screw[1].nLeft  = sSize.nLeft + sSize.nWidth - screw[1].nWidth;
                screw[0].nTop   = (angle & 2) ? sSize.nTop + sSize.nHeight - screw[0].nHeight : sSize.nTop;
                screw[1].nTop   = screw[0].nTop;
            }
            else
            {
                // Vertical orientation
                screw[0].nLeft  = (angle & 0x02) ? sSize.nLeft : sSize.nLeft + sSize.nWidth - screw[0].nWidth;
                screw[1].nLeft  = screw[0].nLeft;
                screw[0].nTop   = sSize.nTop;
                screw[1].nTop   = sSize.nTop + sSize.nHeight - screw[1].nHeight;
            }


            // Position of button
            btn.nLeft       = sSize.nLeft + ((sSize.nWidth  - btn.nWidth ) >> 1);
            btn.nTop        = sSize.nTop  + ((sSize.nHeight - btn.nHeight) >> 1);

            // Realize screws and button
            sScrewPadding.enter(&sScrew[0], &screw[0], scaling);
            sScrewPadding.enter(&sScrew[1], &screw[1], scaling);
            sButtonPadding.enter(&sButton, &btn, scaling);
        }

        void RackEars::draw_screw(ws::ISurface *s, const ws::rectangle_t *r, float angle)
        {
            ws::IGradient *gr;
            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t rad         = r->nHeight >> 1;
            float cx            = r->nLeft + (r->nWidth  * 0.5f);
            float cy            = r->nTop  + (r->nHeight * 0.5f);

            // Draw hole
            lsp::Color hole;
            get_actual_bg_color(hole);

            ws::rectangle_t h   = *r;
            ssize_t hole_r      = 0.375f * r->nHeight;
            ssize_t chamfer     = lsp_max(1.0f, lsp_min(scaling * 3.0f, 0.25f * r->nHeight));

            h.nHeight          *= 0.725f;
            h.nTop              = r->nTop + ((r->nHeight - h.nHeight) >> 1);

            float delta         = r->nWidth;

            for (ssize_t i=0; i<chamfer; ++i)
            {
                // Compute color
                float bright = float(chamfer - i) / chamfer;
                hole.lightness(bright);

                gr = s->radial_gradient(r->nLeft, r->nTop + r->nHeight, r->nLeft, r->nTop + r->nHeight, delta);
                gr->add_color(0.0, hole);
                gr->add_color(1.0, 0.5 * hole.red(), 0.5 *  hole.green(), 0.5 * hole.blue());
                s->fill_rect(gr, SURFMASK_ALL_CORNER, hole_r, &h);
                delete gr;

                h.nLeft        += 1;
                h.nTop         += 1;
                h.nWidth       -= 2;
                h.nHeight      -= 2;
                hole_r          = lsp_max(0, hole_r - 1);
            }

            hole.copy(sHoleColor);
            s->fill_rect(hole, SURFMASK_ALL_CORNER, hole_r, &h);

            // Draw screw
            lsp::Color screw(sScrewColor);
            screw.scale_lch_luminance(0.5f);

            gr = s->radial_gradient(cx + (rad * M_RGOLD_RATIO), cy - (rad * M_RGOLD_RATIO), cx, cy, rad);
            gr->add_color(0.0, 1.0, 1.0, 1.0);
            gr->add_color(1.0, screw);
            s->fill_circle(gr, cx, cy, rad);
            delete gr;

            // Draw screw cross
            ssize_t lwidth  = 3.0f * scaling;
            float a_cos     = (rad - lwidth) * cosf(angle), a_sin = (rad - lwidth) * sinf(angle);

            gr = s->radial_gradient(cx - (rad * M_RGOLD_RATIO), cy + (rad * M_RGOLD_RATIO), cx, cy, rad);
            gr->add_color(0.0, 1.0, 1.0, 1.0);
            gr->add_color(1.0, screw);
            ws::surf_line_cap_t cap = s->set_line_cap(ws::SURFLCAP_ROUND);
            s->line(gr, cx + a_cos, cy + a_sin, cx - a_cos, cy - a_sin, lwidth);
            s->line(gr, cx - a_sin, cy + a_cos, cx + a_sin, cy - a_cos, lwidth);
            s->set_line_cap(cap);
            delete gr;
        }

        void RackEars::draw(ws::ISurface *s)
        {
            float scaling   = lsp_max(0.0f, sScaling.get());
            float fscaling  = lsp_max(0.0f, scaling * sFontScaling.get());
            float bright    = sBrightness.get();
            bool aa         = s->set_antialiasing(true);

            ws::rectangle_t screw[2], btn;
            screw[0]        = sScrew[0];
            screw[1]        = sScrew[1];
            btn             = sButton;

            screw[0].nLeft -= sSize.nLeft;
            screw[0].nTop  -= sSize.nTop;
            screw[1].nLeft -= sSize.nLeft;
            screw[1].nTop  -= sSize.nTop;
            btn.nLeft      -= sSize.nLeft;
            btn.nTop       -= sSize.nTop;

            // Draw background
            lsp::Color col;
            get_actual_bg_color(col);
            s->clear(col);

            // Draw screws
            ssize_t xangle  = sAngle.get();

            if (!(xangle & 1))
            {
                float angle  = (xangle & 2) >> 1;

                draw_screw(s, &screw[0], M_PI * (angle + 1) / 8 + M_PI / 16);
                draw_screw(s, &screw[1], M_PI * (angle + 3) / 8 + M_PI / 16);
            }
            else
            {
                draw_screw(s, &screw[0], M_PI * 1.0 / 8.0 + M_PI / 16.0);
                draw_screw(s, &screw[1], M_PI * 3.0 / 8.0 + M_PI / 16.0);
            }

            // Draw button
            lsp::Color logo(sColor);
            lsp::Color font(sTextColor);

            logo.scale_lch_luminance(bright);
            font.scale_lch_luminance(bright);

            float logo_l        = logo.lightness();
            ssize_t chamfer     = lsp_max(1.0f, scaling * 3.0f);

            for (ssize_t i=0; i<=chamfer; ++i)
            {
                float bright = logo_l * (i + 1) / (chamfer + 1);

                ws::IGradient *gr = (nXFlags & XF_DOWN) ?
                        s->radial_gradient(
                            btn.nLeft, btn.nTop + btn.nHeight,
                            btn.nLeft, btn.nTop + btn.nHeight,
                            btn.nWidth) :
                        s->radial_gradient(
                            btn.nLeft + btn.nWidth, btn.nTop,
                            btn.nLeft + btn.nWidth, btn.nTop,
                            btn.nWidth);

                logo.lightness(bright * 1.5f);
                gr->add_color(0.0f, logo);
                logo.lightness(bright);
                gr->add_color(1.0f, logo);
                s->fill_rect(gr, SURFMASK_ALL_CORNER, chamfer - i + 1, btn.nLeft, btn.nTop, btn.nWidth, btn.nHeight);
                delete gr;

                btn.nLeft      += 1;
                btn.nTop       += 1;
                btn.nWidth     -= 2;
                btn.nHeight    -= 2;
            }

            // Draw text
            ws::font_parameters_t fp;
            ws::text_parameters_t tp;
            LSPString text;

            sText.format(&text);
            sFont.get_parameters(pDisplay, fscaling, &fp);
            sFont.get_text_parameters(pDisplay, &tp, fscaling, &text);

            sFont.draw(s, font,
                btn.nLeft + ((btn.nWidth  - tp.Width ) * 0.5f),
                btn.nTop  + ((btn.nHeight - fp.Height) * 0.5f) + fp.Ascent,
                fscaling, &text
            );

            // Restore antialiasing
            s->set_antialiasing(aa);
        }

        status_t RackEars::on_mouse_down(const ws::event_t *e)
        {
            // Handle mouse event
            if (nBMask == 0)
            {
                if (Position::inside(&sButton, e->nLeft, e->nTop))
                {
                    if (e->nCode == ws::MCB_LEFT)
                        nXFlags    |= XF_LBUTTON;
                    else if (e->nCode == ws::MCB_RIGHT)
                        nXFlags    |= XF_RBUTTON;
                }
            }

            nBMask |= 1 << e->nCode;

            return handle_mouse_move(e);
        }

        status_t RackEars::on_mouse_up(const ws::event_t *e)
        {
            size_t mask = nBMask;
            nBMask &= ~(1 << e->nCode);

            if (mask != (1U << e->nCode))
                return handle_mouse_move(e);

            // Last button released, process the vent
            size_t flags    = nXFlags;
            nXFlags         = 0;

            if (Position::inside(&sButton, e->nLeft, e->nTop))
            {
                if ((e->nCode == ws::MCB_LEFT) && (flags & XF_LBUTTON))
                    sSlots.execute(SLOT_SUBMIT, this, NULL);
            }

            if (flags != nXFlags)
                query_draw();

            return STATUS_OK;
        }

        status_t RackEars::on_mouse_move(const ws::event_t *e)
        {
            if (nXFlags == 0)
                return STATUS_OK;

            return handle_mouse_move(e);
        }

        status_t RackEars::handle_mouse_move(const ws::event_t *e)
        {
            if (nXFlags & XF_LBUTTON)
            {
                bool pressed= (nBMask == ws::MCF_LEFT) && (Position::inside(&sButton, e->nLeft, e->nTop));

                size_t old  = nXFlags;
                nXFlags     = lsp_setflag(nXFlags, XF_DOWN, pressed);

                if (old != nXFlags)
                    query_draw();
            }

            return STATUS_OK;
        }

        status_t RackEars::on_submit()
        {
            return STATUS_OK;
        }

        status_t RackEars::slot_on_submit(Widget *sender, void *ptr, void *data)
        {
            RackEars *_this = widget_ptrcast<RackEars>(ptr);
            return (ptr != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }
    }
}
