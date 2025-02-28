/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 7 сент. 2020 г.
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
#include <lsp-plug.in/tk/helpers/draw.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(FileButton, Widget)
                // Bind
                style::FileButtonColors *c = &vColors[style::FILEBUTTON_NORMAL];
                c->sColor.bind("color", this);
                c->sInvColor.bind("inv.color", this);
                c->sBorderColor.bind("border.color", this);
                c->sInvBorderColor.bind("border.inv.color", this);
                c->sLineColor.bind("line.color", this);
                c->sInvLineColor.bind("line.inv.color", this);
                c->sTextColor.bind("text.color", this);
                c->sInvTextColor.bind("text.inv.color", this);

                c = &vColors[style::FILEBUTTON_INACTIVE];
                c->sColor.bind("inactive.color", this);
                c->sInvColor.bind("inactive.inv.color", this);
                c->sBorderColor.bind("inactive.border.color", this);
                c->sInvBorderColor.bind("inactive.border.inv.color", this);
                c->sLineColor.bind("inactive.line.color", this);
                c->sInvLineColor.bind("inactive.line.inv.color", this);
                c->sTextColor.bind("inactive.text.color", this);
                c->sInvTextColor.bind("inactive.text.inv.color", this);

                sValue.bind("value", this);
                sFont.bind("font", this);
                sTextLayout.bind("text.layout", this);
                sTextPadding.bind("text.padding", this);
                sConstraints.bind("size.constraints", this);
                sGradient.bind("gradient", this);
                sActive.bind("active", this);
                sBorderSize.bind("border.size", this);
                sBorderPressedSize.bind("border.pressed.size", this);

                // Configure
                c = &vColors[style::FILEBUTTON_NORMAL];
                c->sColor.set("#cccccc");
                c->sInvColor.set("#00cc00");
                c->sBorderColor.set("#000000");
                c->sInvBorderColor.set("#ffffff");
                c->sLineColor.set("#000000");
                c->sInvLineColor.set("#000000");
                c->sTextColor.set("#cccccc");
                c->sInvTextColor.set("#00cc00");

                c = &vColors[style::FILEBUTTON_INACTIVE];
                c->sColor.set("#cccccc");
                c->sInvColor.set("#888888");
                c->sBorderColor.set("#000000");
                c->sInvBorderColor.set("#ffffff");
                c->sLineColor.set("#000000");
                c->sInvLineColor.set("#000000");
                c->sTextColor.set("#cccccc");
                c->sInvTextColor.set("#444444");

                sValue.set_all(0.0f, 0.0f, 1.0f);
                sFont.set_size(10.0f);
                sTextLayout.set(0.0f, 0.0f);
                sTextPadding.set(2, 2, 2, 2);
                sConstraints.set_all(-1);
                sGradient.set(true);
                sActive.set(true);
                sBorderSize.set(4);
                sBorderPressedSize.set(3);

                // Override
                sFont.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileButton, "FileButton", "root");

            void FileButtonColors::listener(tk::prop::Listener *listener)
            {
                sColor.listener(listener);
                sInvColor.listener(listener);
                sBorderColor.listener(listener);
                sInvBorderColor.listener(listener);
                sLineColor.listener(listener);
                sInvLineColor.listener(listener);
                sTextColor.listener(listener);
                sInvTextColor.listener(listener);
            }

            bool FileButtonColors::property_changed(Property *prop)
            {
                return prop->one_of(
                    sColor, sInvColor, sBorderColor, sInvBorderColor,
                    sLineColor, sInvLineColor, sTextColor, sInvTextColor);
            }
        }

        #define NPOINTS 9
        static const float xx[NPOINTS] = { 0.5f, 7.0f, 8.0f, 8.0f, 7.5f, 0.5f, 0.0f, 0.0f, 0.5f };
        static const float yy[NPOINTS] = { 0.0f, 0.0f, 1.0f, 7.5f, 8.0f, 8.0f, 7.5f, 0.5f, 0.0f };

        const w_class_t FileButton::metadata            = { "FileButton", &Widget::metadata };

        FileButton::FileButton(Display *dpy):
            Widget(dpy),
            sValue(&sProperties),
            sText(&sProperties),
            sTextList(&sProperties),
            sFont(&sProperties),
            sTextLayout(&sProperties),
            sTextPadding(&sProperties),
            sConstraints(&sProperties),
            sGradient(&sProperties),
            sActive(&sProperties),
            sBorderSize(&sProperties),
            sBorderPressedSize(&sProperties)
        {
            pClass              = &metadata;

            for (size_t i=0; i<style::FILEBUTTON_TOTAL; ++i)
                vColors[i].listener(&sProperties);

            nBMask              = 0;
            nXFlags             = 0;

            sButton.nLeft       = 0;
            sButton.nTop        = 0;
            sButton.nWidth      = 0;
            sButton.nHeight     = 0;
        }

        FileButton::~FileButton()
        {
            nFlags     |= FINALIZED;
        }

        status_t FileButton::init()
        {
            status_t result = Widget::init();
            if (result != STATUS_OK)
                return result;

            style::FileButtonColors *c = &vColors[style::FILEBUTTON_NORMAL];
            c->sColor.bind("color", &sStyle);
            c->sInvColor.bind("inv.color", &sStyle);
            c->sBorderColor.bind("border.color", &sStyle);
            c->sInvBorderColor.bind("border.inv.color", &sStyle);
            c->sLineColor.bind("line.color", &sStyle);
            c->sInvLineColor.bind("line.inv.color", &sStyle);
            c->sTextColor.bind("text.color", &sStyle);
            c->sInvTextColor.bind("text.inv.color", &sStyle);

            c = &vColors[style::FILEBUTTON_INACTIVE];
            c->sColor.bind("inactive.color", &sStyle);
            c->sInvColor.bind("inactive.inv.color", &sStyle);
            c->sBorderColor.bind("inactive.border.color", &sStyle);
            c->sInvBorderColor.bind("inactive.border.inv.color", &sStyle);
            c->sLineColor.bind("inactive.line.color", &sStyle);
            c->sInvLineColor.bind("inactive.line.inv.color", &sStyle);
            c->sTextColor.bind("inactive.text.color", &sStyle);
            c->sInvTextColor.bind("inactive.text.inv.color", &sStyle);

            sValue.bind("value", &sStyle);
            sText.bind(&sStyle, pDisplay->dictionary());
            sTextList.bind(&sStyle, pDisplay->dictionary());
            sFont.bind("font", &sStyle);
            sTextLayout.bind("text.layout", &sStyle);
            sTextPadding.bind("text.padding", &sStyle);
            sConstraints.bind("size.constraints", &sStyle);
            sGradient.bind("gradient", &sStyle);
            sActive.bind("active", &sStyle);
            sBorderSize.bind("border.size", &sStyle);
            sBorderPressedSize.bind("border.pressed.size", &sStyle);

            // Additional slots
            handler_id_t id = 0;
            id = sSlots.add(SLOT_SUBMIT, slot_on_submit, self());

            return (id >= 0) ? STATUS_OK : -id;
        }

        style::FileButtonColors *FileButton::select_colors()
        {
            size_t flags = (sActive.get()) ? style::FILEBUTTON_NORMAL : style::FILEBUTTON_INACTIVE;
            return &vColors[flags];
        }

        void FileButton::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            // Self properties
            style::FileButtonColors *colors = select_colors();
            if (colors->property_changed(prop))
                query_draw();

            if (sActive.is(prop))
                query_draw();

            if (prop->one_of(sValue, sGradient))
                query_draw();

            if (prop->one_of(sText, sTextList, sFont, sTextLayout,
                sTextPadding, sConstraints, sBorderSize, sBorderPressedSize))
                query_resize();
        }

        void FileButton::size_request(ws::size_limit_t *r)
        {
            float scaling   = lsp_max(0.0f, sScaling.get());
            float fscaling  = lsp_max(0.0f, scaling * sFontScaling.get());

            // Estimate minmum size required for the text field
            LSPString s;
            ws::font_parameters_t fp;
            ws::text_parameters_t tp;
            ws::rectangle_t xr;

            xr.nLeft        = 0;
            xr.nTop         = 0;
            xr.nWidth       = 0;
            xr.nHeight      = 0;

            sFont.get_parameters(pDisplay, fscaling, &fp);

            for (size_t i=0,n=sTextList.size(); i<n; ++i)
            {
                String *si      = sTextList.get(i);
                si->format(&s);
                sFont.get_multitext_parameters(pDisplay, &tp, fscaling, &s);

                xr.nWidth       = lsp_max(xr.nWidth,  tp.Width );
                xr.nHeight      = lsp_max(xr.nHeight, tp.Height);
            }

            sText.format(&s);
            sFont.get_multitext_parameters(pDisplay, &tp, fscaling, &s);

            xr.nWidth       = lsp_max(xr.nWidth,  tp.Width );
            xr.nHeight      = lsp_max(xr.nHeight, tp.Height);
            sTextPadding.add(&xr, scaling);

            // Estimate the overall button size relative to the text area size
            xr.nWidth       = ceilf((xr.nWidth  * 8.0f) / 7.0f);
            xr.nHeight      = ceilf((xr.nHeight * 8.0f) / 3.5f);
            xr.nWidth       = lsp_max(xr.nWidth, xr.nHeight);
            xr.nHeight      = xr.nWidth;

            // Apply chamfer size
            size_t border_norm  = lsp_max(0.0f, scaling * sBorderSize.get());
            size_t border_down  = lsp_max(0.0f, scaling * sBorderPressedSize.get());
            size_t max_chamfer  = lsp_max(border_norm, border_down);
            xr.nWidth          += max_chamfer << 1;
            xr.nHeight         += max_chamfer << 1;

            // Form the output constraints
            r->nMinWidth        = xr.nWidth;
            r->nMinHeight       = xr.nHeight;
            r->nMaxWidth        = -1;
            r->nMaxHeight       = -1;
            r->nPreWidth        = xr.nWidth;
            r->nPreHeight       = xr.nHeight;

            // Apply size constraints
            sConstraints.apply(r, scaling);
        }

        void FileButton::realize(const ws::rectangle_t *r)
        {
            // Realize
            Widget::realize(r);

            // Button is always of the squared form
            sButton.nWidth          = lsp_min(r->nWidth, r->nHeight);
            sButton.nHeight         = sButton.nWidth;
            sButton.nLeft           = r->nLeft + ((r->nWidth  - sButton.nWidth ) >> 1);
            sButton.nTop            = r->nTop  + ((r->nHeight - sButton.nHeight) >> 1);
        }

        void FileButton::draw(ws::ISurface *s)
        {
            float v                 = sValue.get_normalized();
            float bright            = sBrightness.get();
            lsp::Color bg;
            const style::FileButtonColors *colors = select_colors();

            get_actual_bg_color(bg);
            s->clear(bg);

            ws::rectangle_t clip    = sButton;
            clip.nLeft             -= sSize.nLeft;
            clip.nTop              -= sSize.nTop;
            clip.nWidth             = v * sButton.nWidth;
            if (clip.nWidth > 0)
            {
                lsp::Color col(colors->sInvColor);
                lsp::Color text(colors->sInvTextColor);
                lsp::Color line(colors->sInvLineColor);
                lsp::Color border(colors->sBorderColor);
                col.scale_lch_luminance(bright);
                text.scale_lch_luminance(bright);
                line.scale_lch_luminance(bright);
                border.scale_lch_luminance(bright);

                s->clip_begin(&clip);
                    draw_button(s, col, text, line, border);
                s->clip_end();
            }

            clip.nLeft             += clip.nWidth;
            clip.nWidth             = sButton.nWidth - clip.nWidth;
            if (clip.nWidth > 0)
            {
                lsp::Color col(colors->sColor);
                lsp::Color text(colors->sTextColor);
                lsp::Color line(colors->sLineColor);
                lsp::Color border(colors->sInvBorderColor);
                col.scale_lch_luminance(bright);
                text.scale_lch_luminance(bright);
                line.scale_lch_luminance(bright);
                border.scale_lch_luminance(bright);

                s->clip_begin(&clip);
                    draw_button(s, col, text, line, border);
                s->clip_end();
            }
        }

        void FileButton::init_points(float *xa, float *ya, const ws::rectangle_t &b)
        {
            float k     = b.nWidth * 0.125f;
            for (size_t j=0; j<NPOINTS; ++j)
            {
                xa[j] = b.nLeft + xx[j] * k;
                ya[j] = b.nTop  + yy[j] * k;
            }
        }

        void FileButton::draw_button(ws::ISurface *s, lsp::Color &col, lsp::Color &text, lsp::Color &line, lsp::Color &border)
        {
            float xa[NPOINTS], ya[NPOINTS];

            float scaling           = lsp_max(0.0f, sScaling.get());
            float fscaling          = lsp_max(0.0f, scaling * sFontScaling.get());
            size_t border_norm      = lsp_max(0.0f, scaling * sBorderSize.get());
            size_t border_down      = lsp_max(0.0f, scaling * sBorderPressedSize.get());
            float line_width        = lsp_max(1.0f, scaling);

            ws::IGradient *gr = NULL;

            // Determine button parameters
            float b_rad  = sButton.nWidth;
            ssize_t b_rr = (nXFlags & FB_DOWN) ? border_down : border_norm;    // Button rounding radius

            // Change size if pressed
            ws::rectangle_t b = sButton;
            b.nLeft            -= sSize.nLeft;
            b.nTop             -= sSize.nTop;

            bool gradient   = sGradient.get();
            bool aa         = s->set_antialiasing(true);
            lsp_finally { s->set_antialiasing(aa); };

            if (gradient)
            {
                for (ssize_t i=0; i < b_rr; ++i)
                {
                    float z = i + 1.0f;
                    float bright = sqrtf(z * z) / b_rr;

                    lsp::Color dcol(col);
                    dcol.scale_hsl_lightness(bright);

                    if (nXFlags & FB_DOWN)
                        gr = s->radial_gradient(b.nLeft, b.nHeight, b.nLeft, b.nHeight, b_rad * 3.0f);
                    else
                        gr = s->radial_gradient(b.nWidth, b.nTop, b.nWidth, b.nTop, b_rad * 3.0f);

                    gr->set_start(dcol);
                    dcol.darken(0.9f);
                    gr->set_stop(dcol);

                    init_points(xa, ya, b);
                    s->fill_poly(gr, xa, ya, NPOINTS);
                    delete gr; // Delete gradient!

                    b.nLeft        += 1;
                    b.nTop         += 1;
                    b.nWidth       -= 2;
                    b.nHeight      -= 2;
                }
            }
            else
            {
                init_points(xa, ya, b);
                s->fill_poly(border, xa, ya, NPOINTS);

                b.nLeft        += b_rr;
                b.nTop         += b_rr;
                b.nWidth       -= b_rr * 2;
                b.nHeight      -= b_rr * 2;

                init_points(xa, ya, b);
                s->fill_poly(col, xa, ya, NPOINTS);
            }

            // Clear canvas
            float k     = b.nWidth * 0.125f;

            s->wire_rect(line, SURFMASK_NONE, 0.0f, b.nLeft + k + 0.5f, b.nTop + 0.5f, 5.5*k, 3.5*k - 0.5f, line_width);
            s->fill_rect(line, SURFMASK_NONE, 0.0f, b.nLeft + k*2.5f, b.nTop, 4.0*k, 3.5*k);
            s->fill_rect(col, SURFMASK_NONE, 0.0f, b.nLeft + 4.5*k, b.nTop + 0.5*k, k, 2.5*k);
            s->fill_rect(line, SURFMASK_NONE, 0.0f, b.nLeft + 0.5*k, b.nTop + 4.0*k, 7.0*k, 3.5*k);
            for (size_t i=0; i<NPOINTS; ++i)
            {
                xa[i] = b.nLeft + xx[i] * k;
                ya[i] = b.nTop  + yy[i] * k;
            }
            s->wire_poly(line, line_width, xa, ya, NPOINTS);

            // Output text
            b.nLeft    += 0.5*k;
            b.nTop     += 4.0*k;
            b.nWidth    = 7.0*k;
            b.nHeight   = 3.5*k;
            sTextPadding.enter(&b, scaling);

            LSPString stext;
            ws::font_parameters_t fp;
            ws::text_parameters_t tp;
            sFont.get_parameters(s, fscaling, &fp);
            sText.format(&stext);
            sFont.get_multitext_parameters(s, &tp, fscaling, &stext);

            draw_multiline_text(
                s, &sFont, &b, text,
                &fp, &tp,
                sTextLayout.halign(), sTextLayout.valign(),
                fscaling, &stext);
        }

        status_t FileButton::on_mouse_down(const ws::event_t *e)
        {
            if (nBMask == 0)
            {
                if (Position::inside(&sButton, e->nLeft, e->nTop))
                {
                    if (e->nCode == ws::MCB_LEFT)
                        nXFlags    |= FB_LBUTTON;
                    else if (e->nCode == ws::MCB_RIGHT)
                        nXFlags    |= FB_RBUTTON;
                }
            }

            nBMask |= size_t(1) << e->nCode;

            return handle_mouse_move(e);
        }

        status_t FileButton::on_mouse_up(const ws::event_t *e)
        {
            size_t mask = nBMask;
            nBMask &= ~(size_t(1) << e->nCode);

            if (mask != (size_t(1) << e->nCode))
                return handle_mouse_move(e);

            // Last button released
            size_t flags = nXFlags;
            nXFlags      = 0;

            if (Position::inside(&sButton, e->nLeft, e->nTop))
            {
                if ((e->nCode == ws::MCB_LEFT) && (flags & FB_LBUTTON))
                    sSlots.execute(SLOT_SUBMIT, this, NULL);
                else if ((e->nCode == ws::MCB_RIGHT) && (flags & FB_RBUTTON))
                {
                    Menu *popup = sPopup.get();
                    if (popup != NULL)
                    {
                        ws::rectangle_t sr;
                        Window *wnd = widget_cast<Window>(this->toplevel());
                        wnd->get_screen_rectangle(&sr);
                        sr.nLeft       += e->nLeft;
                        sr.nTop        += e->nTop;
                        popup->show(this, sr.nLeft, sr.nTop);
                    }
                }
            }

            if (flags != nXFlags)
                query_draw();

            return STATUS_OK;
        }

        status_t FileButton::on_mouse_move(const ws::event_t *e)
        {
            if (nXFlags == 0)
                return STATUS_OK;

            return handle_mouse_move(e);
        }

        status_t FileButton::handle_mouse_move(const ws::event_t *e)
        {
            if (nXFlags & FB_LBUTTON)
            {
                bool pressed= (nBMask == ws::MCF_LEFT) && (Position::inside(&sButton, e->nLeft, e->nTop));

                size_t old  = nXFlags;
                nXFlags     = lsp_setflag(nXFlags, FB_DOWN, pressed);

                if (old != nXFlags)
                    query_draw();
            }

            return STATUS_OK;
        }

        status_t FileButton::slot_on_submit(Widget *sender, void *ptr, void *data)
        {
            FileButton *_this = widget_ptrcast<FileButton>(ptr);
            return (_this != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }

        status_t FileButton::on_submit()
        {
            return STATUS_OK;
        }

    } /* namespace tk */
} /* namespace lsp */


