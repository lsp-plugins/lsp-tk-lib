/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 26 июн. 2020 г.
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
#include <lsp-plug.in/common/debug.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(Group, Align)
                // Bind
                sFont.bind("font", this);
                sTextAdjust.bind("text.adjust", this);
                sColor.bind("color", this);
                sIBGColor.bind("ibg.color", this);
                sTextColor.bind("text.color", this);
                sShowText.bind("text.show", this);
                sBorder.bind("border.size", this);
                sTextPadding.bind("text.padding", this);
                sRadius.bind("border.radius", this);
                sTextRadius.bind("text.radius", this);
                sEmbedding.bind("embed", this);
                sIPadding.bind("ipadding", this);
                sHeading.bind("heading", this);
                sIBGInherit.bind("ibg.inherit", this);
                sIBGBrightness.bind("ibg.brightness", this);
                // Configure
                sFont.set_size(12.0f);
                sTextAdjust.set(TA_NONE);
                sColor.set("#000000");
                sTextColor.set("#ffffff");
                sShowText.set(true);
                sBorder.set(2);
                sTextPadding.set(2);
                sRadius.set(10);
                sTextRadius.set(10);
                sEmbedding.set(false);
                sIPadding.set_all(0);
                sHeading.set(-1.0f, 0.0f);
                sIBGInherit.set(true);
                sIBGBrightness.set(1.0f);
                // Override
                sLayout.set(0.0f, 0.0f, 1.0f, 1.0f);
                // Commit
                sLayout.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(Group, "Group", "root");
        }

        const w_class_t Group::metadata         = { "Group", &Align::metadata };

        Group::Group(Display *dpy):
            Align(dpy),
            sFont(&sProperties),
            sTextAdjust(&sProperties),
            sColor(&sProperties),
            sIBGColor(&sProperties),
            sTextColor(&sProperties),
            sText(&sProperties),
            sShowText(&sProperties),
            sBorder(&sProperties),
            sTextPadding(&sProperties),
            sRadius(&sProperties),
            sTextRadius(&sProperties),
            sEmbedding(&sProperties),
            sIPadding(&sProperties),
            sHeading(&sProperties),
            sIBGInherit(&sProperties),
            sIBGBrightness(&sProperties)
        {
            pWidget             = NULL;

            sLabel.nLeft        = 0;
            sLabel.nTop         = 0;
            sLabel.nWidth       = 0;
            sLabel.nHeight      = 0;

            sArea.nLeft         = 0;
            sArea.nTop          = 0;
            sArea.nWidth        = 0;
            sArea.nHeight       = 0;

            pClass              = &metadata;
        }

        Group::~Group()
        {
            nFlags     |= FINALIZED;
        }

        status_t Group::init()
        {
            status_t result = Align::init();
            if (result != STATUS_OK)
                return result;

            sFont.bind("font", &sStyle);
            sTextAdjust.bind("text.adjust", &sStyle);
            sColor.bind("color", &sStyle);
            sIBGColor.bind("ibg.color", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sText.bind(&sStyle, pDisplay->dictionary());
            sShowText.bind("text.show", &sStyle);
            sBorder.bind("border.size", &sStyle);
            sTextPadding.bind("text.padding", &sStyle);
            sRadius.bind("border.radius", &sStyle);
            sTextRadius.bind("text.radius", &sStyle);
            sEmbedding.bind("embed", &sStyle);
            sIPadding.bind("ipadding", &sStyle);
            sHeading.bind("heading", &sStyle);
            sIBGInherit.bind("ibg.inherit", &sStyle);
            sIBGBrightness.bind("ibg.brightness", &sStyle);

            return STATUS_OK;
        }

        void Group::property_changed(Property *prop)
        {
            Align::property_changed(prop);
            if (sFont.is(prop))
                query_resize();
            if (sTextAdjust.is(prop))
                query_resize();
            if (sColor.is(prop))
                query_draw();
            if (sIBGColor.is(prop))
                query_draw(REDRAW_CHILD | REDRAW_SURFACE);
            if (sTextColor.is(prop))
                query_draw();
            if (sText.is(prop))
                query_resize();
            if (sShowText.is(prop))
                query_resize();
            if (sBorder.is(prop))
                query_resize();
            if (sTextPadding.is(prop))
                query_resize();
            if (sRadius.is(prop))
                query_resize();
            if (sTextRadius.is(prop))
                query_resize();
            if (sEmbedding.is(prop))
                query_resize();
            if (sIPadding.is(prop))
                query_resize();
            if (sHeading.is(prop))
                query_resize();
            if (sIBGInherit.is(prop))
                query_draw(REDRAW_CHILD | REDRAW_SURFACE);
            if (sIBGBrightness.is(prop))
                query_draw(REDRAW_CHILD | REDRAW_SURFACE);
        }

        void Group::allocate(alloc_t *alloc)
        {
            float scaling   = lsp_max(0.0f, sScaling.get());
            float fscaling  = lsp_max(0.0f, scaling * sFontScaling.get());
            ssize_t border  = (sBorder.get() > 0) ? lsp_max(1.0f, sBorder.get() * scaling) : 0;
            ssize_t radius  = lsp_max(0.0f, sRadius.get() * scaling);

            // Text allocation
            ws::rectangle_t xr;
            xr.nLeft        = 0;
            xr.nTop         = 0;

            if (sShowText.get())
            {
                LSPString s;
                ws::text_parameters_t tp;
                ws::font_parameters_t fp;

                ssize_t tradius     = lsp_max(0.0f, sTextRadius.get() * scaling);
                sText.format(&s);
                sTextAdjust.apply(&s);

                sFont.get_parameters(pDisplay, fscaling, &fp);
                sFont.get_text_parameters(pDisplay, &tp, fscaling, &s);
                xr.nWidth           = tp.Width + tradius;
                xr.nHeight          = lsp_max(fp.Height, tp.Height);
                sTextPadding.add(&xr, scaling);
                alloc->text         = xr;

                xr.nWidth          += radius * 1.5f;
            }
            else
            {
                xr.nWidth           = 0;
                xr.nHeight          = 0;

                alloc->text.nLeft   = 0;
                alloc->text.nTop    = 0;
                alloc->text.nWidth  = 0;
                alloc->text.nHeight = 0;
            }
            alloc->rtext        = xr;

            // Compute padding
            ssize_t xborder = lsp_max(0.0f, (radius-border) * M_SQRT1_2);
            padding_t pad;

            pad.nLeft       = (sEmbedding.left())   ? border : xborder;
            pad.nRight      = (sEmbedding.right())  ? border : xborder;
            pad.nTop        = (sEmbedding.top())    ? border : xborder;
            pad.nBottom     = (sEmbedding.bottom()) ? border : xborder;
            if (!sEmbedding.top())
                pad.nTop        = lsp_max(xr.nHeight, ssize_t(pad.nTop));

            sIPadding.add(&pad, scaling);
            alloc->pad      = pad;

            pad.nLeft       = lsp_max(pad.nLeft,   size_t(radius));
            pad.nRight      = lsp_max(pad.nRight,  size_t(radius));
            pad.nTop        = lsp_max(pad.nTop,    size_t(radius));
            pad.nBottom     = lsp_max(pad.nBottom, size_t(radius));

            sIPadding.add(&pad, scaling);
            alloc->xpad     = pad;
        }

        void Group::size_request(ws::size_limit_t *r)
        {
            float scaling   = lsp_max(0.0f, sScaling.get());
            alloc_t alloc;

            allocate(&alloc);

            ssize_t hpad       = alloc.pad.nLeft + alloc.pad.nRight;
            ssize_t vpad       = alloc.pad.nTop  + alloc.pad.nBottom;

            if (pWidget == NULL)
            {
                r->nMinWidth        = 0;
                r->nMinHeight       = 0;
                r->nMaxWidth        = -1;
                r->nMaxHeight       = -1;
                r->nPreWidth        = -1;
                r->nPreHeight       = -1;
            }
            else
            {
                pWidget->get_padded_size_limits(r);
                r->nMinWidth        = (r->nMinWidth  >= 0) ? r->nMinWidth  + hpad : hpad;
                r->nMinHeight       = (r->nMinHeight >= 0) ? r->nMinHeight + vpad : vpad;
                r->nMaxWidth        = -1;
                r->nMaxHeight       = -1;
                r->nPreWidth        = -1;
                r->nPreHeight       = -1;
            }

            r->nMinWidth        = lsp_max(alloc.rtext.nWidth, r->nMinWidth);
            r->nMinHeight       = lsp_max(alloc.rtext.nHeight, r->nMinHeight);

            hpad                = alloc.xpad.nLeft + alloc.xpad.nRight;
            vpad                = alloc.xpad.nTop  + alloc.xpad.nBottom;
            r->nMinWidth        = lsp_max(r->nMinWidth,  hpad);
            r->nMinHeight       = lsp_max(r->nMinHeight, vpad);

            // Apply size constraints
            sConstraints.apply(r, scaling);
        }

        void Group::realize(const ws::rectangle_t *r)
        {
            WidgetContainer::realize(r);

            // Compute text and widget area
            alloc_t alloc;
            allocate(&alloc);

            sHeading.happly(&sLabel, &alloc.text, r->nWidth);
            sLabel.nLeft   += r->nLeft;
            sLabel.nTop    += r->nTop;

            Padding::enter(&sArea, r, &alloc.pad);

            // Realize child widget
            if ((pWidget != NULL) && (pWidget->visibility()->get()))
            {
                ws::rectangle_t xr;
                ws::size_limit_t sr;

                pWidget->get_padded_size_limits(&sr);
                sLayout.apply(&xr, &sArea, &sr);
                pWidget->padding()->enter(&xr, pWidget->scaling()->get());
                pWidget->realize_widget(&xr);
            }
        }

        void Group::get_child_bg_color(lsp::Color *color) const
        {
            float ibg_bright = sIBGBrightness.get();
            if (sIBGInherit.get())
            {
                get_actual_bg_color(color, ibg_bright);
                return;
            }

            color->copy(sIBGColor);
            color->scale_lch_luminance(ibg_bright);
        }

        void Group::get_child_bg_color(lsp::Color &color) const
        {
            return get_child_bg_color(&color);
        }

        void Group::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            if (nFlags & REDRAW_SURFACE)
                force = true;

            // Clear the space
            lsp::Color color;

            ws::rectangle_t xr;
            float scaling   = lsp_max(0.0f, sScaling.get());
            float fscaling  = lsp_max(0.0f, scaling * sFontScaling.get());
            float bright    = lsp_max(0.0f, sBrightness.get());
            ssize_t border  = (sBorder.get() > 0) ? lsp_max(1.0f, sBorder.get() * scaling) : 0;
            ssize_t radius  = lsp_max(0.0f, sRadius.get() * scaling);
            bool bg         = false;

            bool aa         = s->set_antialiasing(false);

            // Draw background if child is invisible or not present
            if ((pWidget != NULL) && (pWidget->visibility()->get()))
            {
                pWidget->get_rectangle(&xr);

                // Draw the nested widget
                if ((force) || (pWidget->redraw_pending()))
                {
                    if (Size::intersection(&xr, &sSize))
                        pWidget->render(s, &xr, force);
                    pWidget->commit_redraw();
                }

                if (force)
                {
                    // Render the child background
                    if (Size::overlap(area, &sSize))
                    {
                        s->clip_begin(area);
                        {
                            pWidget->get_actual_bg_color(color);
                            s->fill_frame(color, &sSize, &xr);
                        }
                        s->clip_end();
                    }
                }
            }
            else
            {
                s->clip_begin(area);
                {
                    get_child_bg_color(color);
                    s->fill_rect(color, SURFMASK_NONE, 0.0f, &sSize);
                    bg   = true;
                }
                s->clip_end();
            }

            // Render frame
            if (force)
            {
                ssize_t ir, xg;

                s->clip_begin(area);

                if (Size::overlap(area, &sSize))
                {
                    if (!bg)
                    {
                        get_actual_bg_color(color);

                        xr          = sSize;
                        xg          = border * 2;
                        xr.nLeft   += border;
                        xr.nTop    += border;
                        xr.nWidth  -= xg;
                        xr.nHeight -= xg;

                        ssize_t ir  = lsp_max(0, radius - border);
                        s->fill_round_frame(color, ir, SURFMASK_ALL_CORNER ^ SURFMASK_LT_CORNER, &sSize, &xr);
                    }

                    // Draw frame
                    color.copy(sColor);
                    color.scale_lch_luminance(bright);

                    s->set_antialiasing(true);
                    s->wire_rect(color, SURFMASK_ALL_CORNER ^ SURFMASK_LT_CORNER, radius, &sSize, border);
                }

                // Draw text
                if ((sShowText.get()) && (Size::overlap(area, &sLabel)))
                {
                    ir          = lsp_max(0.0f, sTextRadius.get() * scaling);

                    size_t mask = 0;
                    if (sHeading.align() > -1.0f)
                        mask       |= SURFMASK_LB_CORNER;
                    if (sHeading.align() < 1.0f)
                        mask       |= SURFMASK_RB_CORNER;

                    // Draw text background
                    color.copy(sColor);
                    color.scale_lch_luminance(bright);

                    s->set_antialiasing(true);
                    s->fill_rect(color, mask, ir, &sLabel);

                    // Draw text
                    LSPString text;
                    ws::text_parameters_t tp;
                    ws::font_parameters_t fp;
                    color.copy(sTextColor);
                    color.scale_lch_luminance(bright);

                    sText.format(&text);
                    sTextAdjust.apply(&text);

                    sFont.get_parameters(s, fscaling, &fp);
                    sFont.get_text_parameters(s, &tp, fscaling, &text);
                    ws::rectangle_t tloc;
                    sTextPadding.enter(&tloc, &sLabel, scaling);
                    tloc.nLeft -= tp.XBearing;
                    tloc.nTop  += fp.Ascent;

                    sFont.draw(s, color, tloc.nLeft, tloc.nTop, fscaling, &text);
                }

                s->clip_end();
            }

            s->set_antialiasing(aa);
        }

    } /* namespace tk */
} /* namespace lsp */


