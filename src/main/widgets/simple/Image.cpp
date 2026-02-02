/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 31 янв. 2026 г.
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
            LSP_TK_STYLE_IMPL_BEGIN(Image, Widget)
                // Bind
                ImageColors *c = &vColors[IMAGE_NORMAL];
                c->sColor.bind("color", this);
                c->sBorderColor.bind("border.color", this);

                c = &vColors[IMAGE_INACTIVE];
                c->sColor.bind("inactive.color", this);
                c->sBorderColor.bind("inactive.border.color", this);

                c = &vColors[IMAGE_NORMAL | IMAGE_HOVER];
                c->sColor.bind("hover.color", this);
                c->sBorderColor.bind("border.hover.color", this);

                c = &vColors[IMAGE_INACTIVE | IMAGE_HOVER];
                c->sColor.bind("inactive.hover.color", this);
                c->sBorderColor.bind("inactive.border.hover.color", this);

                sBitmap.bind("bitmap", this);
                sConstraints.bind("size.constraints", this);
                sLayout.bind("layout", this);
                sFitting.bind("fitting", this);
                sBorderWidth.bind("border.width", this);
                sTransparency.bind("transparency", this);
                sProportion.bind("proportion", this);

                // Configure
                c = &vColors[IMAGE_NORMAL];
                c->sColor.set_rgb24(0xcccccc);
                c->sBorderColor.set_rgb24(0x000000);

                c = &vColors[IMAGE_INACTIVE];
                c->sColor.set_rgb24(0xcccccc);
                c->sBorderColor.set_rgb24(0x000000);

                c = &vColors[IMAGE_NORMAL | IMAGE_HOVER];
                c->sColor.set_rgb24(0xcccccc);
                c->sBorderColor.set_rgb24(0x000000);

                c = &vColors[IMAGE_INACTIVE | IMAGE_HOVER];
                c->sColor.set_rgb24(0xcccccc);
                c->sBorderColor.set_rgb24(0x000000);

                sConstraints.set(10, 10, -1, -1);
                sLayout.set(-1.0f, -1.0f, 1.0f, 1.0f);
                sFitting.set(FIT_STRETCH);
                sBorderWidth.set(0);
                sTransparency.set(0.0f);
                sProportion.set(-1.0f);

            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(Image, "Image", "root");

            void ImageColors::listener(tk::prop::Listener *listener)
            {
                sColor.listener(listener);
                sBorderColor.listener(listener);
            }

            bool ImageColors::property_changed(Property *prop) const
            {
                return prop->one_of(sColor, sBorderColor);
            }
        } /* namespace style */

        const w_class_t Image::metadata     = { "Image", &Widget::metadata };

        Image::Image(Display *dpy):
            Widget(dpy),
            sBitmap(&sProperties),
            sConstraints(&sProperties),
            sLayout(&sProperties),
            sFitting(&sProperties),
            sBorderWidth(&sProperties),
            sTransparency(&sProperties),
            sProportion(&sProperties)
        {
            sArea.nLeft     = -1;
            sArea.nTop      = -1;
            sArea.nWidth    = -1;
            sArea.nHeight   = -1;

            sBorder.nLeft   = -1;
            sBorder.nTop    = -1;
            sBorder.nWidth  = -1;
            sBorder.nHeight = -1;

            nMFlags         = 0;
            nState          = 0;

            pClass          = &metadata;
        }

        Image::~Image()
        {
            nFlags     |= FINALIZED;
        }

        status_t Image::init()
        {
            status_t res = Widget::init();
            if (res != STATUS_OK)
                return res;

            style::ImageColors *c = &vColors[style::IMAGE_NORMAL];
            c->sColor.bind("color", &sStyle);
            c->sBorderColor.bind("border.color", &sStyle);

            c = &vColors[style::IMAGE_INACTIVE];
            c->sColor.bind("inactive.color", &sStyle);
            c->sBorderColor.bind("inactive.border.color", &sStyle);

            c = &vColors[style::IMAGE_NORMAL | style::IMAGE_HOVER];
            c->sColor.bind("hover.color", &sStyle);
            c->sBorderColor.bind("border.hover.color", &sStyle);

            c = &vColors[style::IMAGE_INACTIVE | style::IMAGE_HOVER];
            c->sColor.bind("inactive.hover.color", &sStyle);
            c->sBorderColor.bind("inactive.border.hover.color", &sStyle);

            sBitmap.bind("bitmap", &sStyle);
            sConstraints.bind("size.constraints", &sStyle);
            sLayout.bind("layout", &sStyle);
            sFitting.bind("fitting", &sStyle);
            sBorderWidth.bind("border.width", &sStyle);
            sTransparency.bind("transparency", &sStyle);
            sProportion.bind("proportion", &sStyle);

            handler_id_t id = sSlots.add(SLOT_SUBMIT, slot_on_submit, self());
            if (id < 0)
                return -id;

            return STATUS_OK;
        }

        void Image::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            style::ImageColors * const colors = select_colors();
            if (colors->property_changed(prop))
                query_draw();

            if (prop->one_of(sBitmap, sLayout, sFitting, sTransparency))
                query_draw();

            if (prop->one_of(sConstraints, sBorderWidth, sFitting, sProportion))
                query_resize();
        }

        void Image::size_request(ws::size_limit_t *r)
        {
            const float scaling         = lsp_max(0.0f, sScaling.get());
            const size_t border_size    = lsp_max(sBorderWidth.get() * scaling, 0);
            const fitting_t fit         = sFitting.get();

            if (fit == FIT_FULL)
            {
                ws::rectangle_t wr;
                wr.nLeft                    = 0;
                wr.nTop                     = 0;
                wr.nWidth                   = floorf(scaling * sBitmap.width());
                wr.nHeight                  = floorf(scaling * sBitmap.height());

                sConstraints.merge(r, &wr, scaling);
            }
            else
                sConstraints.compute(r, scaling);

            SizeConstraints::add(r, border_size * 2, border_size * 2);
        }

        void Image::realize(const ws::rectangle_t *r)
        {
            Widget::realize(r);

            const float scaling         = lsp_max(0.0f, sScaling.get());
            const size_t border_size    = lsp_max(sBorderWidth.get() * scaling, 0);
            const float proportion      = sProportion.get();
            const fitting_t fitting     = sFitting.get();

            // Add image border
            sArea.nLeft                 = sSize.nLeft + border_size;
            sArea.nTop                  = sSize.nTop + border_size;
            sArea.nWidth                = sSize.nWidth - border_size * 2;
            sArea.nHeight               = sSize.nHeight - border_size * 2;

            if ((fitting != FIT_FULL) && (proportion > 1e-6f))
            {
                const float xprop           = float(sArea.nWidth) / float (sArea.nHeight);

                if (xprop > proportion)         // Width is larger than desired
                    sArea.nWidth            = sArea.nHeight * proportion;
                else if (xprop < proportion)    // Width is smaller than desired
                    sArea.nHeight           = sArea.nWidth / proportion;

                // Align area
                sArea.nLeft                 = sSize.nLeft + (sSize.nWidth  - sArea.nWidth ) / 2;
                sArea.nTop                  = sSize.nTop  + (sSize.nHeight - sArea.nHeight) / 2;

                sBorder.nLeft               = sArea.nLeft - border_size;
                sBorder.nTop                = sArea.nTop - border_size;
                sBorder.nWidth              = sArea.nWidth + border_size * 2;
                sBorder.nHeight             = sArea.nHeight + border_size * 2;
            }
            else if (fitting == FIT_FULL)
            {
                sArea.nWidth                = floorf(scaling * sBitmap.width());
                sArea.nHeight               = floorf(scaling * sBitmap.height());

                // Align area
                sArea.nLeft                 = sSize.nLeft + (sSize.nWidth  - sArea.nWidth ) / 2;
                sArea.nTop                  = sSize.nTop  + (sSize.nHeight - sArea.nHeight) / 2;

                sBorder.nLeft               = sArea.nLeft - border_size;
                sBorder.nTop                = sArea.nTop - border_size;
                sBorder.nWidth              = sArea.nWidth + border_size * 2;
                sBorder.nHeight             = sArea.nHeight + border_size * 2;
            }
            else
                sBorder                 = sSize;

        }

        void Image::draw(ws::ISurface *s, bool force)
        {
            const float scaling         = lsp_max(0.0f, sScaling.get());
            const float bright          = select_brightness();
            const fitting_t fitting     = sFitting.get();
            const float hscale          = sLayout.hscale() * scaling;
            const float vscale          = sLayout.vscale() * scaling;
            const float halign          = lsp_limit(sLayout.halign(), -1.0f, 1.0f);
            const float valign          = lsp_limit(sLayout.valign(), -1.0f, 1.0f);
            const size_t border_size    = lsp_max(sBorderWidth.get() * scaling, 0);

            lsp::Color c;
            get_actual_bg_color(c);
            s->clear(c);

            style::ImageColors * const colors = select_colors();
            c.copy(colors->sColor);
            c.scale_lch_luminance(bright);

            ws::rectangle_t a_in = sArea, a_out = sBorder;

            a_in.nLeft     -= sSize.nLeft;
            a_in.nTop      -= sSize.nTop;
            a_out.nLeft    -= sSize.nLeft;
            a_out.nTop     -= sSize.nTop;

            if (!sBitmap.is_empty())
            {
                // Compute position of image
                ws::rectangle_t img;
                img.nLeft       = a_in.nLeft;
                img.nTop        = a_in.nTop;

                switch (fitting)
                {
                    case FIT_WIDTH:
                    {
                        const float k   = float(a_in.nWidth) / float(sBitmap.width());
                        img.nWidth      = a_in.nWidth;
                        img.nHeight     = floorf(sBitmap.height() * k);
                        break;
                    }
                    case FIT_HEIGHT:
                    {
                        const float k   = float(a_in.nHeight) / float(sBitmap.height());
                        img.nWidth      = floorf(sBitmap.width() * k);
                        img.nHeight     = a_in.nHeight;
                        break;
                    }
                    case FIT_COVER:
                    {
                        const float k   = lsp_max(float(a_in.nWidth) / float(sBitmap.width()), float(a_in.nHeight) / float(sBitmap.height()));
                        img.nWidth      = floorf(sBitmap.width()  * k);
                        img.nHeight     = floorf(sBitmap.height() * k);
                        break;
                    }
                    case FIT_CONTAIN:
                    {
                        const float k   = lsp_min(float(a_in.nWidth) / float(sBitmap.width()), float(a_in.nHeight) / float(sBitmap.height()));
                        img.nWidth      = floorf(sBitmap.width()  * k);
                        img.nHeight     = floorf(sBitmap.height() * k);
                        break;
                    }
                    case FIT_HSTRETCH:
                        img.nWidth      = a_in.nWidth;
                        img.nHeight     = sBitmap.height();
                        break;

                    case FIT_VSTRETCH:
                        img.nWidth      = sBitmap.width();
                        img.nHeight     = a_in.nHeight;
                        break;

                    case FIT_STRETCH:
                        img.nWidth      = a_in.nWidth;
                        img.nHeight     = a_in.nHeight;
                        break;

                    case FIT_FULL:
                    case FIT_NONE:
                    default:
                        img.nWidth      = sBitmap.width();
                        img.nHeight     = sBitmap.height();
                        break;
                }

                // Apply layout
                img.nWidth     *= hscale;
                img.nHeight    *= vscale;
                img.nLeft      += floorf((a_in.nWidth -  img.nWidth ) * (halign + 1.0f) * 0.5f);
                img.nTop       += floorf((a_in.nHeight - img.nHeight) * (valign + 1.0f) * 0.5f);

                // Draw clipped image
                s->fill_rect(c, SURFMASK_NO_CORNER, 0.0f, &a_in);
                s->clip_begin(&a_in);
                lsp_finally { s->clip_end(); };
                s->draw_raw(
                    sBitmap.data(), sBitmap.width(), sBitmap.height(), sBitmap.stride(),
                    img.nLeft, img.nTop,
                    float(img.nWidth) / float(sBitmap.width()),
                    float(img.nHeight) / float(sBitmap.height()),
                    sTransparency.get());
            }
            else
                s->fill_rect(c, SURFMASK_NO_CORNER, 0.0f, &a_in);


            // Draw border
            if (border_size > 0)
            {
                c.copy(colors->sBorderColor.color());
                c.scale_lch_luminance(bright);
                s->fill_frame(c, SURFMASK_NO_CORNER, 0.0f, &a_out, &a_in);
            }
        }

        style::ImageColors *Image::select_colors()
        {
            size_t flags = (sActive.get()) ? style::IMAGE_NORMAL : style::IMAGE_INACTIVE;
            flags = lsp_setflag(flags, style::IMAGE_HOVER, nState & F_MOUSE_OVER);

            return &vColors[flags];
        }

        bool Image::check_mouse_over(ssize_t x, ssize_t y)
        {
            return Position::inside(&sBorder, x, y);
        }

        status_t Image::on_mouse_in(const ws::event_t *e)
        {
            LSP_STATUS_ASSERT(Widget::on_mouse_in(e));

            const size_t old_flags = nState;
            nState      = lsp_setflag(nState, F_MOUSE_OVER, check_mouse_over(e->nLeft, e->nTop));
            if (old_flags != nState)
                query_draw();
            return STATUS_OK;
        }

        status_t Image::on_mouse_out(const ws::event_t *e)
        {
            LSP_STATUS_ASSERT(Widget::on_mouse_out(e));

            const size_t old_flags = nState;
            nState      = lsp_setflag(nState, F_MOUSE_OVER, false);
            if (old_flags != nState)
                query_draw();
            return STATUS_OK;
        }

        status_t Image::on_mouse_move(const ws::event_t *e)
        {
            LSP_STATUS_ASSERT(Widget::on_mouse_move(e));

            const size_t old_flags = nState;
            nState      = lsp_setflag(nState, F_MOUSE_OVER, check_mouse_over(e->nLeft, e->nTop));
            if (old_flags != nState)
                query_draw();

            return STATUS_OK;
        }

        status_t Image::on_mouse_down(const ws::event_t *e)
        {
            LSP_STATUS_ASSERT(Widget::on_mouse_down(e));

            const size_t flags = nState;
            const bool over = check_mouse_over(e->nLeft, e->nTop);

            if (nMFlags == 0)
            {
                if ((e->nCode == ws::MCB_LEFT) && (over))
                    nState     |= F_MOUSE_DOWN;
                else
                    nState     |= F_MOUSE_IGN;
            }

            nState      = lsp_setflag(nState, F_MOUSE_OVER, over);
            nMFlags |= size_t(1) << e->nCode;

            if (flags != nState)
                query_draw();

            return STATUS_OK;
        }

        status_t Image::on_mouse_up(const ws::event_t *e)
        {
            LSP_STATUS_ASSERT(Widget::on_mouse_up(e));

            const size_t flags = nMFlags;
            const size_t state = nState;
            nMFlags &= ~ (size_t(1) << e->nCode);
            if (nMFlags == 0)
                nState      = 0;

            const bool over = check_mouse_over(e->nLeft, e->nTop);
            nState      = lsp_setflag(nState, F_MOUSE_OVER, over);

            if (flags != nState)
                query_draw();

            // Trigger submit action
            if ((over) && (nMFlags == 0))
            {
                if ((state & F_MOUSE_DOWN) && (flags == (size_t(1) << ws::MCB_LEFT)) && (e->nCode == ws::MCB_LEFT))
                    sSlots.execute(SLOT_SUBMIT, this);
            }

            return STATUS_OK;
        }

        ws::mouse_pointer_t Image::current_pointer(ssize_t x, ssize_t y)
        {
            const bool over = check_mouse_over(x, y);
            Widget * const parent = this->parent();
            if ((parent != NULL) && (!over))
                return parent->current_pointer(x, y);

            return Widget::current_pointer(x, y);
        }

        status_t Image::on_submit()
        {
            return STATUS_OK;
        }

        status_t Image::slot_on_submit(Widget *sender, void *ptr, void *data)
        {
            Image *self = widget_ptrcast<Image>(ptr);
            return (self != NULL) ? self->on_submit() : STATUS_BAD_ARGUMENTS;
        }
    } /* namespace tk */
} /* namespace lsp */
