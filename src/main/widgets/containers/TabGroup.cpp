/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 3 дек. 2024 г.
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

#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/tk/tk.h>

#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(TabGroup, TabControl)
            LSP_TK_STYLE_IMPL_END

            LSP_TK_BUILTIN_STYLE(TabGroup, "TabGroup", "TabControl");
        } /* namepsace style */

        //-----------------------------------------------------------------------------
        // TabGroup implementation
        const w_class_t TabGroup::metadata        = { "TabGroup", &WidgetContainer::metadata };

        TabGroup::TabGroup(Display *dpy):
            WidgetContainer(dpy),
            sBorderColor(&sProperties),
            sHeadingColor(&sProperties),
            sHeadingSpacingColor(&sProperties),
            sHeadingGapColor(&sProperties),
            sBorderSize(&sProperties),
            sBorderRadius(&sProperties),
            sTabSpacing(&sProperties),
            sHeadingSpacing(&sProperties),
            sHeadingGap(&sProperties),
            sHeadingGapBrightness(&sProperties),
            sEmbedding(&sProperties),
            sHeading(&sProperties),
            sSizeConstraints(&sProperties),
            sTabJoint(&sProperties),
            sHeadingFill(&sProperties),
            sHeadingSpacingFill(&sProperties),
            sTabPointer(&sProperties),
            vItems(&sProperties, &sIListener),
            sSelected(&sProperties),
            vWidgets(&sProperties, &sIListener),
            sActive(&sProperties)
        {
            sArea.nLeft         = 0;
            sArea.nTop          = 0;
            sArea.nWidth        = 0;
            sArea.nHeight       = 0;

            sBounds.nLeft       = 0;
            sBounds.nTop        = 0;
            sBounds.nWidth      = 0;
            sBounds.nHeight     = 0;

            sTabArea.nLeft      = 0;
            sTabArea.nTop       = 0;
            sTabArea.nWidth     = 0;
            sTabArea.nHeight    = 0;

            for (size_t i=0; i<2; ++i)
            {
                sHead[i].nLeft      = 0;
                sHead[i].nTop       = 0;
                sHead[i].nWidth     = 0;
                sHead[i].nHeight    = 0;
            }

            sHeadSpacing.nLeft  = 0;
            sHeadSpacing.nTop   = 0;
            sHeadSpacing.nWidth = 0;
            sHeadSpacing.nHeight= 0;

            sHeadGap.nLeft      = 0;
            sHeadGap.nTop       = 0;
            sHeadGap.nWidth     = 0;
            sHeadGap.nHeight    = 0;

            nTabShift           = 0;
            nMBState            = 0;
            pEventTab           = NULL;

            pClass              = &metadata;
        }

        TabGroup::~TabGroup()
        {
            nFlags     |= FINALIZED;
            do_destroy();
        }

        status_t TabGroup::init()
        {
            // Initialize widgets
            status_t result = WidgetContainer::init();
            if (result != STATUS_OK)
                return result;

            sIListener.bind_all(this, on_add_widget, on_remove_widget);

            // Configure Properties
            sBorderColor.bind("border.color", &sStyle);
            sHeadingColor.bind("heading.color", &sStyle);
            sHeadingSpacingColor.bind("heading.spacing.color", &sStyle);
            sHeadingGapColor.bind("heading.gap.color", &sStyle);
            sBorderSize.bind("border.size", &sStyle);
            sBorderRadius.bind("border.radius", &sStyle);
            sTabSpacing.bind("tab.spacing", &sStyle);
            sHeadingSpacing.bind("heading.spacing", &sStyle);
            sHeadingGap.bind("heading.gap", &sStyle);
            sHeadingGapBrightness.bind("heading.gap.brightness", &sStyle);
            sEmbedding.bind("embed", &sStyle);
            sHeading.bind("heading", &sStyle);
            sSizeConstraints.bind("size.constraints", &sStyle);
            sTabJoint.bind("tab.joint", &sStyle);
            sHeadingFill.bind("heading.fill", &sStyle);
            sHeadingSpacingFill.bind("heading.spacing.fill", &sStyle);
            sTabPointer.bind("tab.pointer", &sStyle);

            // Bind slots
            handler_id_t id;
            id = sSlots.add(SLOT_CHANGE, slot_on_change, self());
            if (id >= 0) id = sSlots.add(SLOT_SUBMIT, slot_on_change, self());
            if (id < 0)
                return -id;

            return STATUS_OK;
        }

        void TabGroup::destroy()
        {
            nFlags     |= FINALIZED;
            do_destroy();
            WidgetContainer::destroy();
        }

        void TabGroup::do_destroy()
        {
            vItems.flush();
        }

        void TabGroup::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);

            // Self properties
            if (prop->one_of(sBorderColor, sHeadingColor, sHeadingSpacingColor, sHeadingGapColor, sHeadingGapBrightness))
                query_draw();
            if (prop->one_of(sBorderSize, sBorderRadius, sTabSpacing, sHeadingSpacing, sHeadingGap))
                query_resize();
            if (prop->one_of(sEmbedding, sHeading, sSizeConstraints))
                query_resize();
            if (prop->one_of(sTabJoint, sHeadingFill, sHeadingSpacingFill))
                query_draw();
            if (prop->one_of(vWidgets, vItems, sActive, sSelected))
                query_resize();
        }

        void TabGroup::allocate_tabs(size_t *max_tab_border, ws::rectangle_t *area, lltl::darray<tab_t> *tabs)
        {
            float scaling           = lsp_max(0.0f, sScaling.get());
            float fscaling          = lsp_max(0.0f, scaling * sFontScaling.get());
            size_t tab_spacing      = lsp_max(0.0f, sTabSpacing.get() * scaling);
            bool top_align          = sHeading.valign() <= 0.0f;

            area->nLeft             = 0;
            area->nTop              = 0;
            area->nWidth            = 0;
            area->nHeight           = 0;

            LSPString caption;

            size_t x                = 0;
            ssize_t max_h           = 0;
            size_t max_b            = 0;

            // Step 1: allocate tabs
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                tk::TabItem *w          = vItems.get(i);
                if ((w == NULL) || (!w->is_visible_child_of(this)))
                    continue;

                // Create new record
                tab_t *tab              = tabs->append();
                if (tab == NULL)
                    return;

                // Obtain font properties and padding
                padding_t padding;
                ws::text_parameters_t tp;
                size_t tab_border       = (w->border_size()->get() > 0) ? lsp_max(1.0f, w->border_size()->get() * scaling) : 0;
                size_t tab_radius       = (w->border_radius()->get() > 0) ? lsp_max(1.0f, w->border_radius()->get() * scaling) : 0;
                size_t border_rgap      = lsp_max(0.0f, tab_radius * M_SQRT1_2);
                w->text()->format(&caption);
                w->text_adjust()->apply(&caption);
                w->font()->get_multitext_parameters(pDisplay, &tp, fscaling, &caption);
                w->text_padding()->compute(&padding, scaling);

                // Write parameters
                tab->widget             = w;
                tab->bounds.nLeft       = x;
                tab->bounds.nTop        = 0;
                tab->text.nWidth        = tp.Width;
                tab->text.nHeight       = tp.Height;
                tab->bounds.nWidth      = 2 * border_rgap + tab->text.nWidth + padding.nLeft + padding.nRight + tab_border * 2;
                tab->bounds.nHeight     = border_rgap + tab->text.nHeight + padding.nTop + padding.nBottom + tab_border * 2;
                tab->text.nLeft         = tab->bounds.nLeft + border_rgap + tab_border + padding.nLeft;
                tab->text.nTop          = tab->bounds.nTop + padding.nTop + tab_border + ((top_align) ? border_rgap : 0);
                tab->border             = tab_border;

                // Update coordinates of the next tab
                max_h                   = lsp_max(max_h, tab->bounds.nHeight);
                max_b                   = lsp_max(max_b, tab->border);
                x                      += tab->bounds.nWidth + tab_spacing;
            }

            area->nHeight           = max_h;

            // Step 2: make tabs of the same height
            for (size_t i=0, n=tabs->size(); i<n; ++i)
            {
                tab_t *tab              = tabs->uget(i);
                ssize_t dy              = max_h - tab->bounds.nHeight;
                tab->bounds.nHeight    += dy;
                tab->text.nHeight      += dy;
                if (!top_align)
                    tab->text.nTop         -= dy;
                area->nWidth            = tab->bounds.nLeft + tab->bounds.nWidth;
            }

            *max_tab_border         = max_b;
        }

        void TabGroup::size_request(ws::size_limit_t *r)
        {
            ws::rectangle_t tab_area, w_area;
            size_t max_tab_border;
            lltl::darray<tab_t> tabs;

            // Allocate tab header
            allocate_tabs(&max_tab_border, &tab_area, &tabs);

            // Compute padding
            float scaling           = lsp_max(0.0f, sScaling.get());
            ssize_t border          = (sBorderSize.get() > 0) ? lsp_max(1.0f, sBorderSize.get() * scaling) : 0;
            ssize_t radius          = lsp_max(0.0f, sBorderRadius.get() * scaling);
            ssize_t xborder         = lsp_max(0.0f, (radius-border) * M_SQRT1_2);
            ssize_t hd_spacing      = lsp_max(sHeadingSpacing.get(), -ssize_t(max_tab_border)) * scaling;
            ssize_t hd_gap          = (sHeadingGap.get() > 0) ? lsp_max(1.0f, sHeadingGap.get() * scaling) : 0;

            tab_area.nWidth        += radius;
            tab_area.nHeight       += hd_spacing;

            padding_t padding;
            padding.nLeft           = (sEmbedding.left())   ? border : xborder;
            padding.nRight          = (sEmbedding.right())  ? border : xborder;
            padding.nTop            = (sEmbedding.top())    ? border : xborder;
            padding.nBottom         = (sEmbedding.bottom()) ? border : xborder;

            w_area.nWidth           = radius * 2;
            w_area.nHeight          = radius * 2;

            // Estimate the size of the area for the widget
            tk::Widget *w           = current_widget();
            if (w != NULL)
            {
                w->get_padded_size_limits(r);
                if (r->nMinWidth > 0)
                    w_area.nWidth       = lsp_max(w_area.nWidth,  ssize_t(r->nMinWidth + padding.nLeft + padding.nRight));
                if (r->nMinHeight > 0)
                    w_area.nHeight      = lsp_max(w_area.nHeight, ssize_t(r->nMinHeight+ padding.nTop + padding.nBottom));
            }

            // Write the actual estimated values
            r->nMinWidth            = lsp_max(tab_area.nWidth, w_area.nWidth);
            r->nMinHeight           = tab_area.nHeight + w_area.nHeight + hd_gap;
            r->nMaxWidth            = -1;
            r->nMaxHeight           = -1;
            r->nPreWidth            = -1;
            r->nPreHeight           = -1;

            // Apply size constraints
            sSizeConstraints.apply(r, scaling);
        }

        void TabGroup::realize(const ws::rectangle_t *r)
        {
            WidgetContainer::realize(r);

            // Compute text and widget area
            lltl::darray<tab_t> tabs;
            size_t max_tab_border;
            allocate_tabs(&max_tab_border, &sTabArea, &tabs);

            // Compute parameters
            float scaling           = lsp_max(0.0f, sScaling.get());
            ssize_t border          = (sBorderSize.get() > 0) ? lsp_max(1.0f, sBorderSize.get() * scaling) : 0;
            ssize_t radius          = lsp_max(0.0f, sBorderRadius.get() * scaling);
            ssize_t xborder         = lsp_max(0.0f, (radius-border) * M_SQRT1_2);
            size_t tab_spacing      = lsp_max(0.0f, sTabSpacing.get() * scaling);
            ssize_t hd_gap          = (sHeadingGap.get() > 0) ? lsp_max(1.0f, sHeadingGap.get() * scaling) : 0;
            nTabShift               = lsp_max(sHeadingSpacing.get(), -ssize_t(max_tab_border)) * scaling;
            ssize_t heading_shift   = lsp_min(nTabShift, 0);
            bool top_align          = sHeading.valign() <= 0.0f;

            // Apply horizontal offset to tabs
            sBounds.nLeft           = sSize.nLeft;
            sBounds.nWidth          = sSize.nWidth;
            sHeadSpacing.nLeft      = sSize.nLeft;
            sHeadSpacing.nWidth     = sSize.nWidth;
            sHeadSpacing.nHeight    = lsp_max(0, nTabShift);
            sHeadGap.nLeft          = sSize.nLeft;
            sHeadGap.nWidth         = sSize.nWidth;
            sHeadGap.nHeight        = hd_gap;
            sTabArea.nLeft          = sSize.nLeft + lsp_limit(sHeading.halign() + 1.0f, 0.0f, 2.0f) * (sSize.nWidth - sTabArea.nWidth) * 0.5f;

            if (top_align)
            {
                sTabArea.nTop           = sSize.nTop;
                sHeadSpacing.nTop       = sTabArea.nTop + sTabArea.nHeight + heading_shift;
                sHeadGap.nTop           = sHeadSpacing.nTop + sHeadSpacing.nHeight;
                sBounds.nTop            = sHeadGap.nTop + sHeadGap.nHeight;
                sBounds.nHeight         = sSize.nTop + sSize.nHeight - sBounds.nTop;
            }
            else
            {
                sTabArea.nTop           = sSize.nTop + sSize.nHeight - sTabArea.nHeight;
                sHeadSpacing.nTop       = sTabArea.nTop - sHeadSpacing.nHeight - heading_shift;
                sHeadGap.nTop           = sHeadSpacing.nTop - sHeadGap.nHeight;
                sBounds.nTop            = sSize.nTop;
                sBounds.nHeight         = sHeadGap.nTop - sBounds.nTop;
            }

            // Compute heading
            sHead[0].nLeft          = sSize.nLeft;
            sHead[0].nTop           = sTabArea.nTop;
            sHead[0].nHeight        = sTabArea.nHeight + heading_shift;
            sHead[0].nWidth         = sTabArea.nLeft - sSize.nLeft - tab_spacing;

            sHead[1].nLeft          = sTabArea.nLeft + sTabArea.nWidth + tab_spacing;
            sHead[1].nTop           = sTabArea.nTop;
            sHead[1].nHeight        = sTabArea.nHeight + heading_shift;
            sHead[1].nWidth         = sSize.nLeft + sSize.nWidth - sHead[1].nLeft;

            // Update tab elements
            for (size_t i=0, n=tabs.size(); i<n; ++i)
            {
                tab_t *tab              = tabs.uget(i);
                if (tab != NULL)
                {
                    tab->bounds.nLeft  += sTabArea.nLeft;
                    tab->bounds.nTop   += sTabArea.nTop;
                    tab->text.nLeft    += sTabArea.nLeft;
                    tab->text.nTop     += sTabArea.nTop;
                }
            }

            // Compute padding
            padding_t padding;
            padding.nLeft           = (sEmbedding.left())   ? border : xborder;
            padding.nRight          = (sEmbedding.right())  ? border : xborder;
            padding.nTop            = (sEmbedding.top())    ? border : xborder;
            padding.nBottom         = (sEmbedding.bottom()) ? border : xborder;

            // Realize child widget
            tk::Widget *w           = current_widget();
            if (w != NULL)
            {
                Padding::enter(&sArea, &sBounds, &padding);
                if (w->is_visible_child_of(this))
                    w->realize_widget(&sArea);
            }

            // Commit allocation parameters
            vVisible.swap(&tabs);
        }

        void TabGroup::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            if (nFlags & REDRAW_SURFACE)
                force = true;

            // Compute parameters
            ws::rectangle_t xr;
            lsp::Color color;

            float scaling           = lsp_max(0.0f, sScaling.get());
            ssize_t border          = (sBorderSize.get() > 0) ? lsp_max(1.0f, sBorderSize.get() * scaling) : 0;
            ssize_t radius          = lsp_max(0.0f, sBorderRadius.get() * scaling);
            ssize_t xborder         = lsp_max(0.0f, (radius-border) * M_SQRT1_2);
            float bright            = lsp_max(0.0f, sBrightness.get());
            bool top_align          = sHeading.valign() <= 0.0f;
            bool bg                 = false;

            // Set anti-aliasing
            bool aa                 = s->set_antialiasing(false);
            lsp_finally { s->set_antialiasing(aa); };

            // Draw background if child is invisible or not present
            tk::Widget *w           = current_widget();
            if ((w != NULL) && (w->is_visible_child_of(this)))
            {
                w->get_rectangle(&xr);

                // Draw the nested widget
                if ((force) || (w->redraw_pending()))
                {
                    if (Size::intersection(&xr, &sArea))
                        w->render(s, &xr, force);
                    w->commit_redraw();
                }

                if (force)
                {
                    // Render the child background
                    if (Size::overlap(area, &sSize))
                    {
                        s->clip_begin(area);
                        lsp_finally { s->clip_end(); };

                        w->get_actual_bg_color(color);
                        s->fill_frame(color, SURFMASK_NONE, 0.0f, &sSize, &xr);
                    }
                }
            }
            else
            {
                s->clip_begin(area);
                lsp_finally { s->clip_end(); };

                get_child_bg_color(color);
                s->fill_rect(color, SURFMASK_NONE, 0.0f, &sSize);
                bg   = true;
            }

            // Render frame
            if (!force)
                return;

            ssize_t ir, xg;

            // Compute the rounding mask
            size_t surfmask = SURFMASK_ALL_CORNER;
            if (sTabArea.nLeft < (sSize.nLeft + xborder))
                surfmask &= (top_align) ? ~SURFMASK_LT_CORNER : ~SURFMASK_LB_CORNER;
            if ((sTabArea.nLeft + sTabArea.nWidth) > (sSize.nLeft + sSize.nWidth - xborder))
                surfmask &= (top_align) ? ~SURFMASK_RT_CORNER : ~SURFMASK_RB_CORNER;

            // Draw the frame
            if (Size::overlap(area, &sBounds))
            {
                s->clip_begin(area);
                lsp_finally { s->clip_end(); };

                if (!bg)
                {
                    get_actual_bg_color(color);

                    xr          = sBounds;
                    xg          = border * 2;
                    xr.nLeft   += border;
                    xr.nTop    += border;
                    xr.nWidth  -= xg;
                    xr.nHeight -= xg;

                    ir          = lsp_max(0, radius - border);
                    s->fill_frame(color, surfmask, ir, &sBounds, &xr);
                }

                // Draw frame
                color.copy(sBorderColor);
                color.scale_lch_luminance(bright);

                s->set_antialiasing(true);
                s->wire_rect(color, surfmask, radius, &sBounds, border);
            }

            // Draw tabs
            tk::TabItem *ct         = current_tab();
            if (Size::overlap(area, &sTabArea))
            {
                for (size_t i=0, n=vVisible.size(); i<n; ++i)
                {
                    tab_t *tab              = vVisible.uget(i);
                    tk::TabItem *w          = tab->widget;
                    draw_tab(s, tab, (w == ct), (w == pEventTab), area);
                }
            }

            // Draw head spacing
            if ((sHeadingSpacingFill.get()) && (Size::overlap(area, &sHeadSpacing)))
            {
                s->clip_begin(area);
                lsp_finally { s->clip_end(); };

                color.copy(sHeadingSpacingColor);
                color.scale_lch_luminance(bright);
                s->set_antialiasing(false);
                s->fill_rect(color, SURFMASK_NO_CORNER, radius, &sHeadSpacing);
            }

            // Draw head gap
            if ((sHeadGap.nHeight > 0) && (Size::overlap(area, &sHeadGap)))
            {
                float bright2 = sHeadingGapBrightness.get();
                color.copy(sHeadingGapColor);
                color.scale_lch_luminance(bright * bright2);

                s->clip_begin(area);
                lsp_finally { s->clip_end(); };
                s->set_antialiasing(false);
                s->fill_rect(color, SURFMASK_NO_CORNER, radius, &sHeadGap);
            }

            // Fill space near tabs
            if (sHeadingFill.get())
            {
                s->clip_begin(area);
                lsp_finally { s->clip_end(); };

                color.copy(sHeadingColor);
                color.scale_lch_luminance(bright);
                s->set_antialiasing(false);
                for (size_t i=0; i<2; ++i)
                    if (sHead[i].nWidth > 0)
                        s->fill_rect(color, SURFMASK_NO_CORNER, radius, &sHead[i]);
            }

            // Draw tab text
            if (Size::overlap(area, &sTabArea))
            {
                for (size_t i=0, n=vVisible.size(); i<n; ++i)
                {
                    tab_t *tab              = vVisible.uget(i);
                    tk::TabItem *w          = tab->widget;
                    draw_tab_text(s, tab, (w == ct), (w == pEventTab), area);
                }
            }
        }

        void TabGroup::draw_tab(ws::ISurface *s, const tab_t *tab, bool selected, bool hover, const ws::rectangle_t *area)
        {
            // Compute parameters
            tk::TabItem *w          = tab->widget;
            lsp::Color color;
            ws::rectangle_t clip, r;

            float bright            = lsp_max(0.0f, sBrightness.get());
            float scaling           = lsp_max(0.0f, sScaling.get());
            ssize_t border          = (sBorderSize.get() > 0) ? lsp_max(1.0f, sBorderSize.get() * scaling) : 0;
            size_t tab_radius       = (w->border_radius()->get() > 0) ? lsp_max(1.0f, w->border_radius()->get() * scaling) : 0;
            bool top_align          = sHeading.valign() <= 0.0f;
            size_t mask             = (top_align) ? SURFMASK_T_CORNER : SURFMASK_B_CORNER;
            const style::TabItemColors *colors = tab->widget->select_colors(selected, hover);

            const bool aa           = s->set_antialiasing(true);
            lsp_finally { s->set_antialiasing(aa); };

            // Draw tab header
            r                   = tab->bounds;
            if (nTabShift < 0)
            {
                r.nHeight          += nTabShift;
                if (!top_align)
                    r.nTop             -= nTabShift;
            }

            if (Size::intersection(&clip, &r, area))
            {
                s->clip_begin(&clip);
                lsp_finally { s->clip_end(); };

                if (r.nHeight > 0)
                {
                    // Draw the tab background
                    color.copy(colors->sColor);
                    color.scale_lch_luminance(bright);
                    s->fill_rect(color, mask, tab_radius, &tab->bounds);

                    // Draw the tab border
                    color.copy(colors->sBorderColor);
                    color.scale_lch_luminance(bright);
                    s->wire_rect(color, mask, tab_radius, &tab->bounds, tab->border);
                }
            }

            // For selected tab: join it with the body
            if ((selected) && (nTabShift < 0) &&
                (sTabJoint.get()) && (Size::overlap(area, &sBounds)))
            {
                s->clip_begin(area);
                lsp_finally { s->clip_end(); };

                // Erase the border
                color.copy(colors->sColor);
                color.scale_lch_luminance(bright);
                if (top_align)
                    s->fill_rect(color, SURFMASK_NO_CORNER, 0,
                        tab->bounds.nLeft + tab->border, sBounds.nTop,
                        tab->bounds.nWidth - tab->border * 2, border);
                else
                    s->fill_rect(color, SURFMASK_NO_CORNER, 0,
                        tab->bounds.nLeft + tab->border, sBounds.nTop + sBounds.nHeight - border,
                        tab->bounds.nWidth - tab->border * 2, border);
            }
        }

        void TabGroup::draw_tab_text(ws::ISurface *s, const tab_t *tab, bool selected, bool hover, const ws::rectangle_t *area)
        {
            // Compute parameters
            tk::TabItem *w          = tab->widget;
            lsp::Color color;
            ws::rectangle_t clip, r;
            ws::font_parameters_t fp;
            ws::text_parameters_t tp;

            float scaling           = lsp_max(0.0f, sScaling.get());
            float fscaling          = lsp_max(0.0f, scaling * sFontScaling.get());
            const style::TabItemColors *colors = tab->widget->select_colors(selected, hover);

            // Draw tab text
            if (Size::intersection(&clip, &tab->text, area))
            {
                s->clip_begin(&clip);
                lsp_finally { s->clip_end(); };

                // Form the text string
                LSPString text;
                w->text()->format(&text);
                w->text_adjust()->apply(&text);

                // Estimate text sizes
                w->font()->get_parameters(s, fscaling, &fp);
                w->font()->get_multitext_parameters(s, &tp, fscaling, &text);

                // Estimate drawing area
                tp.Height       = lsp_max(tp.Height, fp.Height);
                if (tp.Width <= tab->text.nWidth)
                {
                    r.nLeft         = tab->text.nLeft;
                    r.nWidth        = tab->text.nWidth;
                }
                else
                {
                    r.nLeft         = tab->text.nLeft - 0.5f * (tp.Width - tab->text.nWidth);
                    r.nWidth        = ceil(tp.Width);
                }

                if (tp.Height <= tab->text.nHeight)
                {
                    r.nTop          = tab->text.nTop;
                    r.nHeight       = tab->text.nHeight;
                }
                else
                {
                    r.nTop          = tab->text.nTop - 0.5f * (tp.Height - tab->text.nHeight);
                    r.nHeight       = ceil(tp.Height);
                }

                // Initialize palette
                color.copy(colors->sTextColor);
                color.scale_lch_luminance(sBrightness.get());

                // Draw background
                float halign    = lsp_limit(w->text_layout()->halign() + 1.0f, 0.0f, 2.0f);
                float valign    = lsp_limit(w->text_layout()->valign() + 1.0f, 0.0f, 2.0f);
                float dy        = (r.nHeight - tp.Height) * 0.5f;
                ssize_t y       = r.nTop + dy * valign - fp.Descent;

                // Estimate text size
                ssize_t last = 0, curr = 0, tail = 0, len = text.length();

                s->clip_begin(&tab->text);
                lsp_finally { s->clip_end(); };

                while (curr < len)
                {
                    // Get next line indexes
                    curr    = text.index_of(last, '\n');
                    if (curr < 0)
                    {
                        curr        = len;
                        tail        = len;
                    }
                    else
                    {
                        tail        = curr;
                        if ((tail > last) && (text.at(tail-1) == '\r'))
                            --tail;
                    }

                    // Calculate text location
                    w->font()->get_text_parameters(s, &tp, fscaling, &text, last, tail);
                    float dx    = (r.nWidth - tp.Width) * 0.5f;
                    ssize_t x   = r.nLeft   + dx * halign - tp.XBearing;
                    y          += fp.Height;

                    w->font()->draw(s, color, x, y, fscaling, &text, last, tail);
                    last    = curr + 1;
                }
            }
        }

        const lsp::Color *TabGroup::select_color(tab_mode_t mode, const tk::Color *normal, const tk::Color * selected, const tk::Color *hover)
        {
            switch (mode)
            {
                case TM_SELECTED:   return selected->color();
                case TM_HOVER:      return hover->color();
                case TM_NORMAL:
                default:
                    break;
            }
            return normal->color();
        }

        status_t TabGroup::add(Widget *child)
        {
            return vWidgets.add(child);
        }

        status_t TabGroup::remove(Widget *child)
        {
            return vWidgets.premove(child);
        }

        status_t TabGroup::remove_all()
        {
            vWidgets.clear();
            return STATUS_OK;
        }

        status_t TabGroup::add_item(TabItem *child)
        {
            return vItems.add(child);
        }

        status_t TabGroup::remove_item(TabItem *child)
        {
            TabItem *item = widget_cast<TabItem>(child);
            if (item == NULL)
                return STATUS_BAD_TYPE;

            return vItems.premove(item);
        }

        status_t TabGroup::remove_all_items()
        {
            vItems.clear();
            return STATUS_OK;
        }

        status_t TabGroup::on_change()
        {
            return STATUS_OK;
        }

        status_t TabGroup::on_submit()
        {
            return STATUS_OK;
        }

        tk::Widget *TabGroup::current_widget()
        {
            tk::Widget *active  = sActive.get();
            if ((active != NULL) && (vWidgets.contains(active)))
                return active;

            TabItem *it     = sSelected.get();
            ssize_t index   = ((it != NULL) && (it->visibility()->get())) ? vItems.index_of(it) : 0;
            return vWidgets.get(index);
        }

        tk::TabItem *TabGroup::current_tab()
        {
            tk::TabItem *it = sSelected.get();
            if ((it != NULL) && (vItems.contains(it)) && (it->is_visible_child_of(this)))
                return it;

            // Find first visible tab
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                it = vItems.get(i);
                if ((it != NULL) && (it->is_visible_child_of(this)))
                {
                    sSelected.set(it);
                    return it;
                }
            }

            return NULL;
        }

        tk::TabItem *TabGroup::find_tab(ssize_t x, ssize_t y)
        {
            // First simple test to match cursor the specific area
            if (!Position::inside(&sTabArea, x, y))
                return NULL;

            // Now check that we fit the tab
            float scaling           = lsp_max(0.0f, sScaling.get());
            bool top_align          = sHeading.valign() <= 0.0f;
            size_t mask             = (top_align) ? SURFMASK_T_CORNER : SURFMASK_B_CORNER;

            // Check that mouse pointer is inside of the tab control
            for (size_t i=0, n=vVisible.size(); i<n; ++i)
            {
                tab_t *tab              = vVisible.uget(i);
                tk::TabItem *w      = tab->widget;
                size_t tab_radius       = (w->border_radius()->get() > 0) ? lsp_max(1.0f, w->border_radius()->get() * scaling) : 0;

                if (Position::rminside(&tab->bounds, x, y, mask, tab_radius))
                    return w;
            }

            return NULL;
        }

        bool TabGroup::scroll_item(ssize_t increment)
        {
            tk::TabItem *sel    = current_tab();
            ssize_t count           = vItems.size();
            if (increment == 0)
                return false;
            increment      %= count;

            // Do the loop until we find another one tab to select
            for (ssize_t i=vItems.index_of(sel); i<count; )
            {
                i  += increment;
                if (i < 0)
                    i += count;
                else if (i >= count)
                    i -= count;

                tk::TabItem *it     = vItems.get(i);
                if ((it != NULL) && (it->is_visible_child_of(this)))
                {
                    if (sel == it)
                        return false;

                    sSelected.set(it);
                    return true;
                }
            }

            return false;
        }

        status_t TabGroup::on_mouse_down(const ws::event_t *e)
        {
            if (nMBState == 0)
            {
                pEventTab   = find_tab(e->nLeft, e->nTop);
                if (pEventTab != NULL)
                    query_resize();
            }
            nMBState       |= 1 << e->nCode;

            return STATUS_OK;
        }

        Widget *TabGroup::find_widget(ssize_t x, ssize_t y)
        {
            Widget *widget  = current_widget();
            if (widget == NULL)
                return NULL;
            if ((widget->is_visible_child_of(this)) && (widget->inside(x, y)))
                return widget;

            return NULL;
        }

        status_t TabGroup::on_mouse_up(const ws::event_t *e)
        {
            size_t mask     = 1 << e->nCode;
            size_t prev     = nMBState;
            nMBState       &= (~mask);

            if (prev == mask)
            {
                if ((e->nCode == ws::MCB_LEFT) && (pEventTab != NULL))
                {
                    tk::TabItem *found = find_tab(e->nLeft, e->nTop);
                    if ((found == pEventTab) && (sSelected.get() != found))
                    {
                        sSelected.set(found);
                        sSlots.execute(SLOT_SUBMIT, this, NULL);
                    }
                }
            }

            if (nMBState == 0)
            {
                if (pEventTab != NULL)
                    query_resize();
            }

            return STATUS_OK;
        }

        status_t TabGroup::on_mouse_pointer(pointer_event_t *e)
        {
            tk::TabItem *tab = find_tab(e->nLeft, e->nTop);
            if (tab != NULL)
                e->enPointer        = sTabPointer.get();

            return STATUS_OK;
        }

        status_t TabGroup::on_mouse_move(const ws::event_t *e)
        {
            if (nMBState != 0)
                return STATUS_OK;

            tk::TabItem *tab = find_tab(e->nLeft, e->nTop);
            if (pEventTab != tab)
            {
                pEventTab   = tab;
                query_resize();
            }

            return STATUS_OK;
        }

        status_t TabGroup::on_mouse_out(const ws::event_t *e)
        {
            if (nMBState != 0)
                return STATUS_OK;

            if (pEventTab != NULL)
            {
                pEventTab   = NULL;
                query_resize();
            }
            return STATUS_OK;
        }

        status_t TabGroup::on_mouse_scroll(const ws::event_t *e)
        {
            if (nMBState != 0)
                return STATUS_OK;

            tk::Widget *found = find_tab(e->nLeft, e->nTop);
            if (found != NULL)
            {
                if (e->nCode == ws::MCD_UP)
                {
                    if (scroll_item(-1))
                        sSlots.execute(SLOT_SUBMIT, this, NULL);
                }
                else if (e->nCode == ws::MCD_DOWN)
                {
                    if (scroll_item(1))
                        sSlots.execute(SLOT_SUBMIT, this, NULL);
                }
            }

            return STATUS_OK;
        }

        status_t TabGroup::on_key_down(const ws::event_t *e)
        {
            switch (e->nCode)
            {
                case ws::WSK_LEFT:
                case ws::WSK_KEYPAD_LEFT:
                    if (scroll_item(-1))
                        sSlots.execute(SLOT_SUBMIT, this, NULL);
                    break;

                case ws::WSK_RIGHT:
                case ws::WSK_KEYPAD_RIGHT:
                    if (scroll_item(1))
                        sSlots.execute(SLOT_SUBMIT, this, NULL);
                    break;

                default:
                    break;
            }

            return STATUS_OK;
        }

        void TabGroup::on_add_widget(void *obj, Property *prop, void *w)
        {
            Widget *item = widget_ptrcast<Widget>(w);
            if (item == NULL)
                return;

            TabGroup *self = widget_ptrcast<TabGroup>(obj);
            if (self == NULL)
                return;

            item->set_parent(self);

            self->vVisible.clear();
            self->query_resize();
        }

        void TabGroup::on_remove_widget(void *obj, Property *prop, void *w)
        {
            Widget *item = widget_ptrcast<Widget>(w);
            if (item == NULL)
                return;

            TabGroup *self = widget_ptrcast<TabGroup>(obj);
            if (self == NULL)
                return;

            // Reset active widget if present
            if (self->sSelected.get() == item)
                self->sSelected.set(NULL);
            if (self->sActive.get() == item)
                self->sActive.set(NULL);
            if (self->pEventTab == item)
                self->pEventTab       = NULL;

            self->vVisible.flush();
            self->unlink_widget(item);
            self->query_resize();
        }

        status_t TabGroup::slot_on_change(Widget *sender, void *ptr, void *data)
        {
            TabGroup *_this = widget_ptrcast<TabGroup>(ptr);
            return (_this != NULL) ? _this->on_change() : STATUS_BAD_ARGUMENTS;
        }

        status_t TabGroup::slot_on_submit(Widget *sender, void *ptr, void *data)
        {
            TabGroup *_this = widget_ptrcast<TabGroup>(ptr);
            return (_this != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }

    } /* namespace tk */
} /* namespace lsp */



