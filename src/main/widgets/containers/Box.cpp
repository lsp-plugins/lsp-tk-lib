/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 20 июн. 2017 г.
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
#include <lsp-plug.in/common/debug.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(Box, WidgetContainer)
                // Bind
                sSpacing.bind("spacing", this);
                sBorder.bind("border.size", this);
                sHomogeneous.bind("homogeneous", this);
                sOrientation.bind("orientation", this);
                sConstraints.bind("size.constraints", this);
                sBorderColor.bind("border.color", this);
                sSolid.bind("solid", this);
                // Configure
                sSpacing.set(0);
                sBorder.set(0);
                sHomogeneous.set(false);
                sOrientation.set(O_HORIZONTAL);
                sConstraints.set_all(-1);
                sBorderColor.set("#000000");
                sSolid.set(false);
                // Override
                sAllocation.set(true, false);
                // Commit
                sAllocation.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(Box, "Box", "root");
        }

        const w_class_t Box::metadata   =   { "Box", &WidgetContainer::metadata     };

        Box::Box(Display *dpy):
            WidgetContainer(dpy),
            vItems(&sProperties, &sIListener),
            sSpacing(&sProperties),
            sBorder(&sProperties),
            sHomogeneous(&sProperties),
            sOrientation(&sProperties),
            sConstraints(&sProperties),
            sBorderColor(&sProperties),
            sSolid(&sProperties)
        {
            nMFlags         = 0;
            nState          = 0;

            pClass          = &metadata;
        }
        
        Box::~Box()
        {
            nFlags     |= FINALIZED;
            do_destroy();
        }

        status_t Box::init()
        {
            status_t res = WidgetContainer::init();
            if (res != STATUS_OK)
                return res;

            // Init listener
            sIListener.bind_all(this, on_add_item, on_remove_item);

            // Init properties
            sSpacing.bind("spacing", &sStyle);
            sBorder.bind("border.size", &sStyle);
            sHomogeneous.bind("homogeneous", &sStyle);
            sOrientation.bind("orientation", &sStyle);
            sConstraints.bind("size.constraints", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sSolid.bind("solid", &sStyle);

            handler_id_t id = sSlots.add(SLOT_SUBMIT, slot_on_submit, self());
            if (id < 0)
                return -id;

            return res;
        }

        void Box::destroy()
        {
            nFlags     |= FINALIZED;
            do_destroy();
            WidgetContainer::destroy();
        }

        void Box::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (vItems.is(prop))
                query_resize();
            if (sSpacing.is(prop))
                query_resize();
            if (sBorder.is(prop))
                query_resize();
            if (sHomogeneous.is(prop))
                query_resize();
            if (sOrientation.is(prop))
                query_resize();
            if (sConstraints.is(prop))
                query_resize();
            if (sBorderColor.is(prop))
                query_resize();
        }

        void Box::on_add_item(void *obj, Property *prop, void *w)
        {
            Widget *widget = widget_ptrcast<Widget>(w);
            if (widget == NULL)
                return;

            Box *_this = widget_ptrcast<Box>(obj);
            if (_this == NULL)
                return;

            widget->set_parent(_this);
            _this->query_resize();
        }

        void Box::on_remove_item(void *obj, Property *prop, void *w)
        {
            Widget *widget = widget_ptrcast<Widget>(w);
            if (widget == NULL)
                return;

            Box *_this = widget_ptrcast<Box>(obj);
            if (_this == NULL)
                return;

            _this->unlink_widget(widget);
            _this->query_resize();
        }

        void Box::do_destroy()
        {
            // Clear cached values
            vVisible.flush();

            // Unlink children
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                // Get widget
                Widget *w = vItems.get(i);
                if (w != NULL)
                    unlink_widget(w);
            }

            // Free list of children
            vItems.flush();
        }

        status_t Box::visible_items(lltl::darray<cell_t> *out)
        {
            // Estimate number of visible items
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                // Get widget
                Widget *w = vItems.get(i);
                if ((w == NULL) || (!w->visibility()->get()))
                    continue;

                // Add cell
                cell_t *cell = out->add();
                if (cell == NULL)
                    return STATUS_NO_MEM;

                cell->a.nLeft       = 0;
                cell->a.nTop        = 0;
                cell->a.nWidth      = 0;
                cell->a.nHeight     = 0;
                cell->s.nLeft       = 0;
                cell->s.nTop        = 0;
                cell->s.nWidth      = 0;
                cell->s.nHeight     = 0;
                cell->pWidget       = w;
            }

            return STATUS_OK;
        }

        Widget *Box::find_widget(ssize_t x, ssize_t y)
        {
            // Do not search for any widget
            if (sSolid.get())
                return NULL;

            for (size_t i=0, n=vVisible.size(); i<n; ++i)
            {
                cell_t *w = vVisible.uget(i);
                Widget *pw = w->pWidget;

                if ((pw == NULL) || (!pw->is_visible_child_of(this)))
                    continue;
                if (!pw->visibility()->get())
                    continue;
                if (pw->inside(x, y))
                    return pw;
            }

            return NULL;
        }

        void Box::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            // Check dirty flag
            if (nFlags & REDRAW_SURFACE)
                force = true;

            // Estimate palette
            ws::rectangle_t xr;
            lsp::Color bg_color, border_color;
            float scaling   = lsp_max(0.0f, sScaling.get());
            float bright    = sBrightness.get();
            size_t border   = (sBorder.get() > 0) ? lsp_max(1.0f, sBorder.get() * scaling) : 0;
            get_actual_bg_color(bg_color);

            // Draw background if needed
            if ((vVisible.is_empty()) && (force))
            {
                s->clip_begin(area);
                s->fill_rect(bg_color, SURFMASK_NONE, 0.0f, &sSize);
                if (border > 0)
                {
                    border_color.copy(sBorderColor);
                    border_color.scale_lch_luminance(bright);
                    Rectangle::enter_border(&xr, &sSize, border);
                    s->fill_frame(border_color, SURFMASK_NONE, 0.0f, &sSize, &xr);
                }

                s->clip_end();
                return;
            }

            // Compute spacing size
            ssize_t spacing     = scaling * sSpacing.get();

            // Draw items
            bool horizontal     = sOrientation.horizontal();

            for (size_t i=0, n=vVisible.size(); i<n; ++i)
            {
                cell_t *wc = vVisible.uget(i);
                Widget *w = wc->pWidget;

                // Render the child widget
                if ((force) || (w->redraw_pending()))
                {
                    if (Size::intersection(&xr, area, &wc->s))
                        w->render(s, &xr, force);
                    w->commit_redraw();
                }

                // Fill unused space with background
                if (force)
                {
                    s->clip_begin(area);
                    {
                        w->get_actual_bg_color(bg_color);
                        if (Size::overlap(area, &wc->a))
                            s->fill_frame(bg_color, SURFMASK_NONE, 0.0f, &wc->a, &wc->s);

                        // Draw spacing
                        if (((i + 1) < n) && (spacing > 0))
                        {
                            get_actual_bg_color(bg_color);
                            if (horizontal)
                            {
                                xr.nLeft    = wc->a.nLeft + wc->a.nWidth;
                                xr.nTop     = wc->a.nTop;
                                xr.nWidth   = spacing;
                                xr.nHeight  = wc->a.nHeight;
                            }
                            else
                            {
                                xr.nLeft    = wc->a.nLeft;
                                xr.nTop     = wc->a.nTop + wc->a.nHeight;
                                xr.nWidth   = wc->a.nWidth;
                                xr.nHeight  = spacing;
                            }

                            if (Size::overlap(area, &xr))
                                s->fill_rect(bg_color, SURFMASK_NONE, 0.0f, &xr);
                        }

                        // Draw border
                        if (border > 0)
                        {
                            border_color.copy(sBorderColor);
                            border_color.scale_lch_luminance(bright);
                            Rectangle::enter_border(&xr, &sSize, border);
                            s->fill_frame(border_color, SURFMASK_NONE, 0.0f, &sSize, &xr);
                        }
                    }
                    s->clip_end();
                }
            }
        }

        status_t Box::add(Widget *widget)
        {
            if (widget == NULL)
                return STATUS_BAD_ARGUMENTS;
            return vItems.add(widget);
        }

        status_t Box::remove(Widget *child)
        {
            return vItems.premove(child);
        }

        status_t Box::remove_all()
        {
            vItems.clear();
            return STATUS_OK;
        }

        status_t Box::allocate_homogeneous(const ws::rectangle_t *r, lltl::darray<cell_t> &visible)
        {
            // Get the final area to perform allocation
            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t spacing     = scaling * sSpacing.get();
            bool horizontal     = sOrientation.horizontal();

            ssize_t n_left      = (horizontal) ? r->nWidth : r->nHeight;
            n_left             -= spacing * (visible.size() - 1);
            size_t m_size       = n_left / visible.size();

            // FIRST PASS: Initialize widgets allocations same width/size
            for (size_t i=0, n=visible.size(); i<n; ++i)
            {
                // Get widget
                cell_t *w =          visible.uget(i);

                // Allocate space
                if (horizontal)
                {
                    w->a.nWidth     = m_size;
                    w->a.nHeight    = r->nHeight;
                }
                else // vertical
                {
                    w->a.nWidth     = r->nWidth;
                    w->a.nHeight    = m_size;
                }
                n_left         -= m_size;
            }

            // SECOND PASS: Split unused space between widgets
            while (n_left > 0)
            {
                for (size_t i=0, n=visible.size(); i<n; ++i)
                {
                    // Get widget
                    cell_t *w = visible.uget(i);
                    if (horizontal)
                        w->a.nWidth     ++;
                    else // vertical
                        w->a.nHeight    ++;

                    if ((n_left--) <= 0)
                        break;
                }
            }

            // Now we have n_left=0, now need to generate proper Left and Top coordinates of widget
            allocate_widget_space(r, visible, spacing);

            return STATUS_OK;
        }

        status_t Box::allocate_proportional(const ws::rectangle_t *r, lltl::darray<cell_t> &visible)
        {
            // Get the final area to perform allocation
            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t spacing     = scaling * sSpacing.get();
            bool horizontal     = sOrientation.horizontal();

            ssize_t n_left      = (horizontal) ? r->nWidth : r->nHeight;
            n_left             -= spacing * (visible.size() - 1);
            size_t n_size       = n_left;

            // FIRST PASS: Initialize widgets with their minimum widths
            // Estimate number of expanded widgets and space used by them
            ws::size_limit_t sr;
            lltl::parray<cell_t>    expand;
            size_t n_expand     = 0;
            size_t num_reduce   = 0;
            size_t n_reduced    = 0;

            for (size_t i=0, n=visible.size(); i<n; ++i)
            {
                // Get widget
                cell_t *w               = visible.uget(i);
                tk::Allocation *alloc   = w->pWidget->allocation();

                // Request size limit and padding of the widget
                w->pWidget->get_padded_size_limits(&sr);

                if (horizontal)
                {
                    w->a.nWidth         = lsp_max(0, sr.nMinWidth);     // Add minimum width to allocation
                    w->a.nHeight        = r->nHeight;                   // All allocations have same height for horizontal box
                    n_left             -= w->a.nWidth;

                    // Estimate number of expanded widgets and their allocated space
                    if (alloc->hreduce())
                    {
                        n_reduced      += w->a.nWidth;
                        ++num_reduce;
                    }
                    else if (alloc->hexpand())
                    {
                        n_expand           += w->a.nWidth;
                        if (!expand.add(w))
                            return STATUS_NO_MEM;
                    }
                }
                else // vertical
                {
                    w->a.nHeight        = lsp_max(0, sr.nMinHeight);    // Add minimum height to allocation
                    w->a.nWidth         = r->nWidth;                    // All allocation have same width for vertical box
                    n_left             -= w->a.nHeight;

                    // Estimate number of expanded widgets and their allocated space
                    if (alloc->vreduce())
                    {
                        n_reduced          += w->a.nHeight;
                        ++num_reduce;
                    }
                    else if (alloc->vexpand())
                    {
                        n_expand           += w->a.nHeight;
                        if (!expand.add(w))
                            return STATUS_NO_MEM;
                    }
                }
            }

            // SECOND PASS: Split unused space between widgets
            if (n_left > 0)
            {
                ssize_t total = 0;

                if (num_reduce >= visible.size())
                {
                    // All widgets are marked as 'reduced', just distribute unused space between all
                    if (horizontal)
                    {
                        for (size_t i=0, n=visible.size(); i<n; ++i)
                        {
                            cell_t *w = visible.uget(i);

                            ssize_t delta   = (w->a.nWidth * n_left) / n_size;
                            w->a.nWidth    += delta;
                            total          += delta;
                        }
                    }
                    else
                    {
                        for (size_t i=0, n=visible.size(); i<n; ++i)
                        {
                            cell_t *w = visible.uget(i);
                            if (w->pWidget->allocation()->vreduce())
                                continue;

                            ssize_t delta   = (w->a.nHeight * n_left) / n_size;
                            w->a.nHeight   += delta;
                            total          += delta;
                        }
                    }
                }
                else if (expand.is_empty())
                {
                    // Distribute unused space between all visible widgets (excluding reduced)
                    n_size     -= n_reduced; // Exclude reduced widgets from overall size

                    if (horizontal)
                    {
                        for (size_t i=0, n=visible.size(); i<n; ++i)
                        {
                            cell_t *w = visible.uget(i);
                            if (w->pWidget->allocation()->hreduce())
                                continue;

                            ssize_t delta   = (w->a.nWidth * n_left) / n_size;
                            w->a.nWidth    += delta;
                            total          += delta;
                        }
                    }
                    else
                    {
                        for (size_t i=0, n=visible.size(); i<n; ++i)
                        {
                            cell_t *w = visible.uget(i);
                            if (w->pWidget->allocation()->vreduce())
                                continue;

                            ssize_t delta   = (w->a.nHeight * n_left) / n_size;
                            w->a.nHeight   += delta;
                            total          += delta;
                        }
                    }
                }
                else if (n_expand == 0) // Expand flag is set but all widgets have minimum width 0
                {
                    // Distribute unused space between all expanded widgets
                    ssize_t delta   = n_left / expand.size();

                    if (horizontal)
                    {
                        for (size_t i=0, n=expand.size(); i<n; ++i)
                        {
                            cell_t *w       = expand.uget(i);
                            w->a.nWidth    += delta;
                            total          += delta;
                        }
                    }
                    else
                    {
                        for (size_t i=0, n=expand.size(); i<n; ++i)
                        {
                            cell_t *w       = expand.uget(i);
                            w->a.nHeight   += delta;
                            total          += delta;
                        }
                    }
                }
                else
                {
                    // Distribute space proportionally to the current area size of all expanded widgets
                    if (horizontal)
                    {
                        for (size_t i=0, n=expand.size(); i<n; ++i)
                        {
                            cell_t *w       = expand.uget(i);
                            ssize_t delta   = (w->a.nWidth * n_left) / n_expand;
                            w->a.nWidth    += delta;
                            total          += delta;
                        }
                    }
                    else
                    {
                        for (size_t i=0, n=expand.size(); i<n; ++i)
                        {
                            cell_t *w       = expand.uget(i);
                            ssize_t delta   = (w->a.nHeight * n_left) / n_expand;
                            w->a.nHeight   += delta;
                            total          += delta;
                        }
                    }
                } // n_expand

                n_left     -= total;
            }

            // FOURTH PASS: utilize still unallocated pixels between all visible widgets
            if (num_reduce >= visible.size())
            {
                if (horizontal)
                {
                    for (size_t i=0, n=visible.size(); n_left > 0; --n_left)
                    {
                        cell_t *w = visible.uget(i);
                        w->a.nWidth     ++;
                        if ((++i) >= n)
                            i   = 0;
                    }
                }
                else
                {
                    for (size_t i=0, n=visible.size(); n_left > 0; --n_left)
                    {
                        cell_t *w = visible.uget(i);
                        w->a.nHeight    ++;
                        if ((++i) >= n)
                            i   = 0;
                    }
                }
            }
            else
            {
                // Skip all 'reduced' widgets and distribute the rest amount of space between other widgets
                if (horizontal)
                {
                    for (size_t i=0, n=visible.size(); n_left > 0; )
                    {
                        cell_t *w = visible.uget(i);
                        if (!w->pWidget->allocation()->hreduce())
                        {
                            w->a.nWidth     ++;
                            --n_left;
                        }
                        if ((++i) >= n)
                            i   = 0;
                    }
                }
                else
                {
                    for (size_t i=0, n=visible.size(); n_left > 0; )
                    {
                        cell_t *w = visible.uget(i);
                        if (!w->pWidget->allocation()->vreduce())
                        {
                            w->a.nHeight    ++;
                            --n_left;
                        }
                        if ((++i) >= n)
                            i   = 0;
                    }
                }
            }

            // Now we have n_left=0, now need to generate proper Left and Top coordinates of widget
            allocate_widget_space(r, visible, spacing);

            return STATUS_OK;
        }

        void Box::allocate_widget_space(const ws::rectangle_t *r, lltl::darray<cell_t> &visible, ssize_t spacing)
        {
            bool horizontal     = sOrientation.horizontal();
            ssize_t l = r->nLeft, t = r->nTop; // Left-Top corner

            // Now completely apply geometry to each allocation
            for (size_t i=0, n=visible.size(); i<n; ++i)
            {
                // Get widget
                cell_t *w       = visible.uget(i);

                // Initial coordinates
                w->a.nLeft      = l;
                w->a.nTop       = t;
                w->s            = w->a;

                // Compute coordinates of next cell
                if (horizontal)
                    l   += ((i + 1) < n) ? (spacing + w->a.nWidth)  : w->a.nWidth ;
                else // vertical
                    t   += ((i + 1) < n) ? (spacing + w->a.nHeight) : w->a.nHeight;
            }
        }

        void Box::realize_children(lltl::darray<cell_t> &visible)
        {
            ws::size_limit_t sr;
            ws::rectangle_t r;

            for (size_t i=0, n=visible.size(); i<n; ++i)
            {
                // Get widget
                cell_t *w       = visible.uget(i);

                // Allocated widget area may be too large, restrict it with size constraints
                w->pWidget->get_padded_size_limits(&sr);
                SizeConstraints::apply(&r, &w->s, &sr);

                // Estimate the real widget allocation size
                ssize_t xw      = (w->pWidget->allocation()->hfill()) ? r.nWidth    : lsp_max(0, sr.nMinWidth);
                ssize_t xh      = (w->pWidget->allocation()->vfill()) ? r.nHeight   : lsp_max(0, sr.nMinHeight);

                // Update location of the widget
                w->s.nLeft     += lsp_max(0, w->s.nWidth  - xw) >> 1;
                w->s.nTop      += lsp_max(0, w->s.nHeight - xh) >> 1;
                w->s.nWidth     = xw;
                w->s.nHeight    = xh;
                w->pWidget->padding()->enter(&w->s, w->pWidget->scaling()->get());

                // Realize the widget
//                lsp_trace("realize child=%p, id=%d, parameters = {%d, %d, %d, %d}",
//                        w->pWidget, int(i), int(w->s.nLeft), int(w->s.nTop), int(w->s.nWidth), int(w->s.nHeight));
                w->pWidget->realize_widget(&w->s);
            }
        }

        void Box::realize(const ws::rectangle_t *r)
        {
            // Flush previously visible widgets
            vVisible.flush();

            // Call parent method to realize
            WidgetContainer::realize(r);

            // Add border
            ws::rectangle_t xr;
            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t border      = (sBorder.get() > 0) ? lsp_max(1, sBorder.get() * scaling) : 0;
            Rectangle::enter_border(&xr, r, border);

            // Create list of visible items
            lltl::darray<cell_t>    visible;
            status_t res    = visible_items(&visible);
            if (res != STATUS_OK)
                return;

            // Allocate space for child widgets
            if (visible.size() > 0)
            {
                res = (sHomogeneous.get()) ?
                    allocate_homogeneous(&xr, visible) :
                    allocate_proportional(&xr, visible);
            }

            // Update list of visible items
            if (res == STATUS_OK)
            {
                realize_children(visible);
                vVisible.swap(&visible);
            }
        }

        void Box::size_request(ws::size_limit_t *r)
        {
            // Default size request
            r->nMinWidth    = -1;
            r->nMinHeight   = -1;
            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;
            r->nPreWidth    = -1;
            r->nPreHeight   = -1;

            // Obtain list of visible items
            lltl::darray<cell_t>    visible;
            status_t res    = visible_items(&visible);
            if ((res != STATUS_OK) || (visible.is_empty()))
                return;

            // Estimate parameters
            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t spacing     = scaling * sSpacing.get();
            ssize_t border      = (sBorder.get() > 0) ? lsp_max(1, sBorder.get() * scaling) : 0;

            // Estimated width and height, maximum width and height
            ws::size_limit_t sr;
            ssize_t e_width = 0, e_height = 0;
            ssize_t m_width = 0, m_height = 0;

            // Estimate self size
            for (size_t i=0, n=visible.size(); i<n; ++i)
            {
                // Get widget
                cell_t *w = visible.uget(i);

                w->pWidget->get_padded_size_limits(&sr);

                // Analyze widget class
                ssize_t x_width     = lsp_max(0, sr.nMinWidth);
                ssize_t x_height    = lsp_max(0, sr.nMinHeight);
                m_width             = lsp_max(m_width,  x_width );
                m_height            = lsp_max(m_height, x_height);
                e_width            += x_width;
                e_height           += x_height;
            }

            // Compute the final size
            if (sOrientation.horizontal())
            {
                if (sHomogeneous.get())
                    r->nMinWidth        = (m_width + spacing) * visible.size() - spacing + border * 2;
                else
                    r->nMinWidth        = e_width + spacing * (visible.size() - 1) + border * 2;
                r->nMinHeight       = m_height + border * 2;
            }
            else // vertical
            {
                r->nMinWidth        = m_width + border * 2;
                if (sHomogeneous.get())
                    r->nMinHeight       = (m_height + spacing) * visible.size() - spacing + border * 2;
                else
                    r->nMinHeight       = e_height + spacing * (visible.size() - 1) + border * 2;
            }

            // Apply size constraints
            sConstraints.apply(r, scaling);
        }

        status_t Box::on_mouse_in(const ws::event_t *e)
        {
            WidgetContainer::on_mouse_in(e);

            if (sSolid.get())
            {
                size_t flags = nState;
                nState |= F_MOUSE_IN;
                if (flags != nState)
                    query_draw();

                for (size_t i=0, n=vItems.size(); i<n; ++i)
                {
                    Widget *w = vItems.get(i);
                    if ((w == NULL) || (!w->visibility()->get()))
                        continue;

                    w->handle_event(e);
                }
            }

            return STATUS_OK;
        }


        status_t Box::on_mouse_out(const ws::event_t *e)
        {
            Widget::on_mouse_out(e);

            if (sSolid.get())
            {
                size_t flags = nState;
                nState &= ~F_MOUSE_IN;
                if (flags != nState)
                    query_draw();

                for (size_t i=0, n=vItems.size(); i<n; ++i)
                {
                    Widget *w = vItems.get(i);
                    if ((w == NULL) || (!w->visibility()->get()))
                        continue;

                    w->handle_event(e);
                }
            }

            return STATUS_OK;
        }

        status_t Box::on_mouse_move(const ws::event_t *e)
        {
            if (!sSolid.get())
                return STATUS_OK;

            size_t flags = nState;
            nState = lsp_setflag(nState, F_MOUSE_IN, inside(e->nLeft, e->nTop));
            if (flags != nState)
                query_draw();
            return STATUS_OK;
        }

        status_t Box::on_mouse_down(const ws::event_t *e)
        {
            if (!sSolid.get())
                return STATUS_OK;

            size_t flags = nState;

            if (nMFlags == 0)
            {
                if (e->nCode == ws::MCB_LEFT)
                    nState |= F_MOUSE_DOWN;
                else
                    nState |= F_MOUSE_IGN;
            }

            nMFlags |= size_t(1) << e->nCode;
            nState = lsp_setflag(nState, F_MOUSE_IN, inside(e->nLeft, e->nTop));

            if (flags != nState)
                query_draw();
            return STATUS_OK;
        }

        status_t Box::on_mouse_up(const ws::event_t *e)
        {
            if (!sSolid.get())
                return STATUS_OK;

            size_t flags = nMFlags;
            nMFlags &= ~ (size_t(1) << e->nCode);
            if (nMFlags == 0)
                nState      = 0;

            bool xinside = inside(e->nLeft, e->nTop);
            nState = lsp_setflag(nState, F_MOUSE_IN, xinside);
            if (flags != nState)
                query_draw();

            // Trigger submit action
            if (xinside)
            {
                if ((flags == (size_t(1) << ws::MCB_LEFT)) && (e->nCode == ws::MCB_LEFT))
                    sSlots.execute(SLOT_SUBMIT, this);
            }

            return STATUS_OK;
        }

        status_t Box::on_submit()
        {
            return STATUS_OK;
        }

        status_t Box::slot_on_submit(Widget *sender, void *ptr, void *data)
        {
            Box *_this = widget_ptrcast<Box>(ptr);
            return (_this != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }
    } /* namespace tk */
} /* namespace lsp */
