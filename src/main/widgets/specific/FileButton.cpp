/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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

namespace lsp
{
    namespace tk
    {
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
            sColor(&sProperties),
            sInvColor(&sProperties),
            sLineColor(&sProperties),
            sInvLineColor(&sProperties),
            sTextColor(&sProperties),
            sInvTextColor(&sProperties)
        {
            nBMask              = 0;
            nXFlags             = 0;

            sButton.nLeft       = 0;
            sButton.nTop        = 0;
            sButton.nWidth      = 0;
            sButton.nHeight     = 0;

            pClass              = &metadata;
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

            sValue.bind("value", &sStyle);
            sText.bind(&sStyle, pDisplay->dictionary());
            sTextList.bind(&sStyle, pDisplay->dictionary());
            sFont.bind("font", &sStyle);
            sTextLayout.bind("text.layout", &sStyle);
            sTextPadding.bind("text.padding", &sStyle);
            sConstraints.bind("size.constraints", &sStyle);
            sColor.bind("color", &sStyle);
            sInvColor.bind("inv.color", &sStyle);
            sLineColor.bind("line.color", &sStyle);
            sInvLineColor.bind("line.inv.color", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sInvTextColor.bind("text.inv.color", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sValue.init(sclass, 0.0f, 0.0f, 1.0f);
                sFont.init(sclass, 10.0f);
                sTextLayout.init(sclass, 0.0f, 0.0f);
                sTextPadding.init(sclass, 2, 2, 2, 2);
                sConstraints.init(sclass);
                sColor.init(sclass, "#cccccc");
                sInvColor.init(sclass, "#00cc00");
                sLineColor.init(sclass, "#000000");
                sInvLineColor.init(sclass, "#000000");
                sTextColor.init(sclass, "#cccccc");
                sInvTextColor.init(sclass, "#00cc00");
            }

            // Additional slots
            handler_id_t id = 0;
            id = sSlots.add(SLOT_CHANGE, slot_on_submit, self());

            return (id >= 0) ? STATUS_OK : -id;
        }

        void FileButton::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (sValue.is(prop))
                query_draw();
            if (sText.is(prop))
                query_resize();
            if (sTextList.is(prop))
                query_resize();
            if (sFont.is(prop))
                query_resize();
            if (sTextLayout.is(prop))
                query_resize();
            if (sTextPadding.is(prop))
                query_resize();
            if (sConstraints.is(prop))
                query_resize();
            if (sColor.is(prop))
                query_draw();
            if (sInvColor.is(prop))
                query_draw();
            if (sLineColor.is(prop))
                query_draw();
            if (sInvLineColor.is(prop))
                query_draw();
            if (sTextColor.is(prop))
                query_draw();
            if (sInvTextColor.is(prop))
                query_draw();
        }

        void FileButton::size_request(ws::size_limit_t *r)
        {
            float scaling           = lsp_max(0.0f, sScaling.get());

            // Estimate minmum size required for the text field
            LSPString s;
            ws::font_parameters_t fp;
            ws::text_parameters_t tp;
            ws::rectangle_t xr;

            xr.nLeft        = 0;
            xr.nTop         = 0;
            xr.nWidth       = 0;
            xr.nHeight      = 0;

            sFont.get_parameters(pDisplay, scaling, &fp);

            for (size_t i=0,n=sTextList.size(); i<n; ++i)
            {
                String *si      = sTextList.get(i);
                si->format(&s);
                sFont.get_multitext_parameters(pDisplay, &tp, scaling, &s);

                xr.nWidth       = lsp_max(xr.nWidth,  tp.Width );
                xr.nHeight      = lsp_max(xr.nHeight, tp.Height);
            }

            sText.format(&s);
            sFont.get_multitext_parameters(pDisplay, &tp, scaling, &s);

            xr.nWidth       = lsp_max(xr.nWidth,  tp.Width );
            xr.nHeight      = lsp_max(xr.nHeight, tp.Height);
            sTextPadding.add(&xr, scaling);

            // Estimate the overall button size relative to the text area size
            xr.nWidth       = ceilf((xr.nWidth  * 8.0f) / 7.0f);
            xr.nHeight      = ceilf((xr.nHeight * 8.0f) / 3.5f);
            xr.nWidth       = lsp_max(xr.nWidth, xr.nHeight);
            xr.nHeight      = xr.nWidth;

            // Apply chamfer size
            size_t max_chamfer  = lsp_max(1.0f, scaling * 4.0f);
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
            lsp::Color bg(sBgColor);
            s->clear(bg);

            ws::rectangle_t clip    = sButton;
            clip.nLeft             -= sSize.nLeft;
            clip.nTop              -= sSize.nTop;
            clip.nWidth             = v * sButton.nWidth;
            if (clip.nWidth > 0)
            {
                lsp::Color col(sInvColor);
                lsp::Color text(sInvTextColor);
                lsp::Color line(sInvLineColor);
                col.scale_lightness(bright);
                text.scale_lightness(bright);
                line.scale_lightness(bright);

                s->clip_begin(&clip);
                    draw_button(s, col, text, line);
                s->clip_end();
            }

            clip.nLeft             += clip.nWidth;
            clip.nWidth             = sButton.nWidth - clip.nWidth;
            if (clip.nWidth > 0)
            {
                lsp::Color col(sColor);
                lsp::Color text(sTextColor);
                lsp::Color line(sLineColor);
                col.scale_lightness(bright);
                text.scale_lightness(bright);
                line.scale_lightness(bright);

                s->clip_begin(&clip);
                    draw_button(s, col, text, line);
                s->clip_end();
            }
        }

        void FileButton::draw_button(ws::ISurface *s, lsp::Color &col, lsp::Color &text, lsp::Color & line)
        {
            float xa[NPOINTS], ya[NPOINTS];

            float scaling           = lsp_max(0.0f, sScaling.get());
            size_t max_chamfer      = lsp_max(1.0f, scaling * 4.0f);
            size_t min_chamfer      = lsp_max(1.0f, scaling * 3.0f);
            float line_width        = lsp_max(1.0f, scaling);

            ws::IGradient *gr = NULL;

            // Determine button parameters
            float b_rad  = sButton.nWidth;
            ssize_t b_rr = (nXFlags & FB_DOWN) ? min_chamfer : max_chamfer;    // Button rounding radius

            // Change size if pressed
            ws::rectangle_t b = sButton;
            b.nLeft            -= sSize.nLeft;
            b.nTop             -= sSize.nTop;

            float aa    = s->set_antialiasing(true);

            for (ssize_t i=0; (i++)<b_rr; )
            {
                float bright = sqrtf(i * i) / b_rr;

                if (nXFlags & FB_DOWN)
                    gr = s->radial_gradient(b.nLeft, b.nHeight, b_rad * 0.25f, b.nLeft, b.nHeight, b_rad * 3.0f);
                else
                    gr = s->radial_gradient(b.nWidth, b.nTop, b_rad * 0.25f, b.nWidth, b.nTop, b_rad * 3.0f);

                lsp::Color dcol(col);
                dcol.scale_lightness(bright);
                gr->add_color(0.0f, dcol);
                dcol.darken(0.9f);
                gr->add_color(1.0f, dcol);

                float k     = b.nWidth * 0.125f;
                for (size_t j=0; j<NPOINTS; ++j)
                {
                    xa[j] = b.nLeft + xx[j] * k;
                    ya[j] = b.nTop  + yy[j] * k;
                }

                s->fill_poly(gr, xa, ya, NPOINTS);
                delete gr; // Delete gradient!

                b.nLeft        += 1;
                b.nTop         += 1;
                b.nWidth       -= 2;
                b.nHeight      -= 2;
            }

            // Clear canvas
            float k     = b.nWidth * 0.125f;

            s->wire_rect(line, b.nLeft + k + 0.5f, b.nTop + 0.5f, 5.5*k, 3.5*k - 0.5f, line_width);
            s->fill_rect(line, b.nLeft + k*2.5f, b.nTop, 4.0*k, 3.5*k);
            s->fill_rect(col, b.nLeft + 4.5*k, b.nTop + 0.5*k, k, 2.5*k);
            s->fill_rect(line, b.nLeft + 0.5*k, b.nTop + 4.0*k, 7.0*k, 3.5*k);
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
            sFont.get_parameters(s, scaling, &fp);
            sText.format(&stext);
            sFont.get_multitext_parameters(s, &tp, scaling, &stext);

            float halign    = lsp_limit(sTextLayout.halign() + 1.0f, 0.0f, 2.0f);
            float valign    = lsp_limit(sTextLayout.valign() + 1.0f, 0.0f, 2.0f);
            float dy        = (b.nHeight - tp.Height) * 0.5f;
            ssize_t y       = b.nTop + dy * valign - fp.Descent;

            // Estimate text size
            ssize_t last = 0, curr = 0, tail = 0, len = stext.length();

            while (curr < len)
            {
                // Get next line indexes
                curr    = stext.index_of(last, '\n');
                if (curr < 0)
                {
                    curr        = len;
                    tail        = len;
                }
                else
                {
                    tail        = curr;
                    if ((tail > last) && (stext.at(tail-1) == '\r'))
                        --tail;
                }

                // Calculate text location
                sFont.get_text_parameters(s, &tp, scaling, &stext, last, tail);
                float dx    = (b.nWidth - tp.Width) * 0.5f;
                ssize_t x   = b.nLeft   + dx * halign - tp.XBearing;
                y          += fp.Height;

                sFont.draw(s, text, x, y, scaling, &stext, last, tail);
                last    = curr + 1;
            }

            // Restore antialiasing
            s->set_antialiasing(aa);
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

            nBMask |= 1 << e->nCode;

            return handle_mouse_move(e);
        }

        status_t FileButton::on_mouse_up(const ws::event_t *e)
        {
            size_t mask = nBMask;
            nBMask &= ~(1 << e->nCode);

            if (mask != (1U << e->nCode))
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
    }
}


