/*
 * Box.cpp
 *
 *  Created on: 20 июн. 2017 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t Box::metadata   =   { "Box", &WidgetContainer::metadata     };

        Box::Box(Display *dpy):
            WidgetContainer(dpy),
            sSpacing(&sProperties),
            sHomogeneous(&sProperties),
            sOrientation(&sProperties)
        {
            pClass          = &metadata;
        }
        
        Box::~Box()
        {
            do_destroy();
        }

        status_t Box::init()
        {
            status_t res = WidgetContainer::init();
            if (res == STATUS_OK)
            {
                sSpacing.bind("spacing", &sStyle);
                sHomogeneous.bind("homogeneous", &sStyle);
                sOrientation.bind("orientation", &sStyle);

                Style *sclass = style_class();
                if (sclass != NULL)
                {
                    sSpacing.init(sclass, 0);
                    sHomogeneous.init(sclass, false);
                    sOrientation.init(sclass, O_HORIZONTAL);
                }

                // Override settings for hfill and vfill
                sAllocation.set(true, false);
            }

            return res;
        }

        void Box::destroy()
        {
            do_destroy();
            WidgetContainer::destroy();
        }

        void Box::do_destroy()
        {
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                // Get widget
                cell_t *w = vItems.uget(i);
                if (w->pWidget == NULL)
                    continue;

                unlink_widget(w->pWidget);
                w->pWidget = NULL;
            }

            vItems.flush();
        }

        bool Box::hidden_widget(const cell_t *w)
        {
            if ((w == NULL) || (w->pWidget == NULL))
                return true;
            return !w->pWidget->visibility()->get();
        }

        status_t Box::visible_items(lltl::parray<cell_t> *out, lltl::darray<cell_t> *list)
        {
            // Estimate number of visible items
            for (size_t i=0, n=list->size(); i<n; ++i)
            {
                // Get widget
                cell_t *w = list->uget(i);
                if (w->pWidget == NULL)
                    continue;
                if (!w->pWidget->visibility()->get())
                    continue;

                // Add visible widget to list
                if (!out->add(w))
                    return STATUS_NO_MEM;
            }

            return STATUS_OK;
        }

        Widget *Box::find_widget(ssize_t x, ssize_t y)
        {
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                cell_t *w = vItems.uget(i);
                if (hidden_widget(w))
                    continue;
                if (w->pWidget->inside(x, y))
                    return w->pWidget;
            }

            return NULL;
        }

        void Box::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (sSpacing.is(prop))
                query_resize();
            if (sHomogeneous.is(prop))
                query_resize();
            if (sOrientation.is(prop))
                query_resize();
        }

        void Box::render(ws::ISurface *s, bool force)
        {
            // Check dirty flag
            if (nFlags & REDRAW_SURFACE)
                force = true;

            // Estimate palette
            lsp::Color bg_color(sBgColor);

            // Render child widgets
            lltl::parray<cell_t>    visible;
            status_t res    = visible_items(&visible, &vItems);
            if (res != STATUS_OK)
            {
                s->fill_rect(sSize.nLeft, sSize.nTop, sSize.nWidth, sSize.nHeight, bg_color);
                return;
            }

            // Draw background if needed
            if ((visible.is_empty()) && (force))
            {
                s->fill_rect(sSize.nLeft, sSize.nTop, sSize.nWidth, sSize.nHeight, bg_color);
                return;
            }

            // Compute spacing size
            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t spacing     = scaling * sSpacing.get();

            // Draw items
            bool horizontal     = sOrientation.horizontal();

            for (size_t i=0, n=visible.size(); i<n; ++i)
            {
                cell_t *wc = visible.uget(i);
                Widget *w = wc->pWidget;

                if ((!force) && (!w->redraw_pending()))
                    continue;

                // Fill unused space with background
                if (force)
                {
                    bg_color.copy(w->bg_color()->color());
                    s->fill_frame(
                        wc->a.nLeft, wc->a.nTop, wc->a.nWidth, wc->a.nHeight,
                        wc->s.nLeft, wc->s.nTop, wc->s.nWidth, wc->s.nHeight,
                        bg_color
                    );

                    // Draw spacing
                    if (((i + 1) < n) && (spacing > 0))
                    {
                        bg_color.copy(sBgColor);
                        if (horizontal)
                            s->fill_rect(wc->a.nLeft + wc->a.nWidth, wc->a.nTop, spacing, wc->a.nHeight, bg_color);
                        else
                            s->fill_rect(wc->a.nLeft, wc->a.nTop + wc->a.nHeight, wc->a.nWidth, spacing, bg_color);
                    }
                }

                // Render widget
                w->render(s, force);
                w->commit_redraw();
            }
        }

        status_t Box::add(Widget *widget)
        {
            if (widget == NULL)
                return STATUS_BAD_ARGUMENTS;

            // Check that widget already exists
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                cell_t *cell        = vItems.uget(i);
                if (cell->pWidget == widget)
                    return STATUS_ALREADY_EXISTS;
            }

            cell_t *cell = vItems.append();
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
            cell->pWidget       = widget;

            widget->set_parent(this);

            if (widget->visibility()->get())
                query_resize();
            return STATUS_SUCCESS;
        }

        status_t Box::remove(Widget *child)
        {
            size_t n            = vItems.size();
            for (size_t i=0; i<n; ++i)
            {
                cell_t *cell        = vItems.uget(i);
                if (cell->pWidget == child)
                {
                    if (!vItems.remove(i))
                        return STATUS_UNKNOWN_ERR;
                    query_resize();
                    child->set_parent(NULL);
                    return STATUS_OK;
                }
            }

            return STATUS_NOT_FOUND;
        }

        status_t Box::remove_all()
        {
            if (vItems.size() <= 0)
                return STATUS_OK;

            lltl::darray<cell_t> tmp;
            vItems.swap(&tmp);

            for (size_t i=0, n=tmp.size(); i<n; ++i)
            {
                cell_t *cell        = tmp.uget(i);
                if (cell->pWidget != NULL)
                    cell->pWidget->set_parent(NULL);
            }

            tmp.flush();
            query_resize();
            return STATUS_OK;
        }

        status_t Box::allocate_homogeneous(const ws::rectangle_t *r, lltl::parray<cell_t> &visible)
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

        status_t Box::allocate_proportional(const ws::rectangle_t *r, lltl::parray<cell_t> &visible)
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

            for (size_t i=0, n=visible.size(); i<n; ++i)
            {
                // Get widget
                cell_t *w =          visible.uget(i);

                // Request size limit and padding of the widget
                w->pWidget->get_padded_size_limits(&sr);

                if (horizontal)
                {
                    w->a.nWidth         = lsp_max(0, sr.nMinWidth);     // Add minimum width to allocation
                    w->a.nHeight        = r->nHeight;                   // All allocations have same height for horizontal box
                    n_left             -= w->a.nWidth;

                    // Estimate number of expanded widgets and their allocated space
                    if (w->pWidget->allocation()->hexpand())
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
                    if (w->pWidget->allocation()->vexpand())
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

                if (expand.is_empty())
                {
                    // Split unused space between all visible widgets
                    for (size_t i=0, n=visible.size(); i<n; ++i)
                    {
                        // Get widget
                        cell_t *w = visible.uget(i);

                        if (horizontal)
                        {
                            ssize_t delta   = (w->a.nWidth * n_left) / n_size;
                            w->a.nWidth    += delta;
                            total          += delta;
                        }
                        else // vertical
                        {
                            ssize_t delta   = (w->a.nHeight * n_left) / n_size;
                            w->a.nHeight   += delta;
                            total          += delta;
                        }
                    }
                }
                else if (n_expand == 0) // All widgets have minimum width 0
                {
                    // Split unused space between all expanded widgets
                    ssize_t delta   = n_left / expand.size();
                    for (size_t i=0, n=expand.size(); i<n; ++i)
                    {
                        // Get widget
                        cell_t *w       = expand.uget(i);

                        if (horizontal)
                        {
                            w->a.nWidth    += delta;
                            total          += delta;
                        }
                        else // vertical
                        {
                            w->a.nHeight   += delta;
                            total          += delta;
                        }
                    }
                }
                else
                {
                    // Split unused space between all expanded widgets
                    // Distribute space proportionally to the current area size
                    for (size_t i=0, n=expand.size(); i<n; ++i)
                    {
                        // Get widget
                        cell_t *w       = expand.uget(i);

                        if (horizontal)
                        {
                            ssize_t delta   = (w->a.nWidth * n_left) / n_expand;
                            w->a.nWidth    += delta;
                            total          += delta;
                        }
                        else // vertical
                        {
                            ssize_t delta   = (w->a.nHeight * n_left) / n_expand;
                            w->a.nHeight   += delta;
                            total          += delta;
                        }
                    }
                } // n_expand

                n_left     -= total;
            }

            // FOURTH PASS: utilize still unallocated pixels between all visible widgets
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

        void Box::allocate_widget_space(const ws::rectangle_t *r, lltl::parray<cell_t> &visible, ssize_t spacing)
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

        void Box::realize_children(lltl::parray<cell_t> &visible)
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
                lsp_trace("realize child=%p, id=%d, parameters = {%d, %d, %d, %d}",
                        w->pWidget, int(i), int(w->s.nLeft), int(w->s.nTop), int(w->s.nWidth), int(w->s.nHeight));
                w->pWidget->realize_widget(&w->s);
            }
        }

        void Box::realize(const ws::rectangle_t *r)
        {
            lsp_trace("this=%p, size={%d, %d, %d, %d}",
                    this, int(r->nLeft), int(r->nTop), int(r->nWidth), int(r->nHeight)
                );

            // Make a copy of current widget list
            lltl::darray<cell_t>    items;
            if (items.add(&vItems))
            {
                lltl::parray<cell_t>    visible;
                status_t res    = visible_items(&visible, &items);

                // Realize child widgets
                if ((res == STATUS_OK) && (visible.size() > 0))
                {
                    res = (sHomogeneous.get()) ?
                        allocate_homogeneous(r, visible) :
                        allocate_proportional(r, visible);
                }

                // Update state of all widgets
                if (res == STATUS_OK)
                {
                    realize_children(visible);
                    vItems.swap(&items);
                }
            }

            // Call parent method to realize
            WidgetContainer::realize(r);
        }

        void Box::size_request(ws::size_limit_t *r)
        {
            // Default size request
            r->nMinWidth    = -1;
            r->nMinHeight   = -1;
            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;

            // Create copy of current state
            lltl::darray<cell_t>    items;
            if (!items.add(&vItems))
                return;

            // Obtain list of visible items
            lltl::parray<cell_t>    visible;
            status_t res    = visible_items(&visible, &items);
            if ((res != STATUS_OK) || (visible.is_empty()))
                return;

            // Estimate parameters
            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t spacing     = scaling * sSpacing.get();

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
                lsp_trace("size_request id=%d, parameters = {%d, %d, %d, %d}",
                    int(i), int(sr.nMinWidth), int(sr.nMinHeight), int(sr.nMaxWidth), int(sr.nMaxHeight));

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
                    r->nMinWidth        = (m_width + spacing) * visible.size() - spacing;
                else
                    r->nMinWidth        = e_width + spacing * (visible.size() - 1);
                r->nMinHeight       = m_height;
            }
            else // vertical
            {
                r->nMinWidth        = m_width;
                if (sHomogeneous.get())
                    r->nMinHeight       = (m_height + spacing) * visible.size() - spacing;
                else
                    r->nMinHeight       = e_height + spacing * (visible.size() - 1);
            }

            lsp_trace("this=%p, w={%d, %d}, h={%d, %d}", this, int(r->nMinWidth), int(r->nMaxWidth), int(r->nMinHeight), int(r->nMaxHeight));
        }
    
    } /* namespace tk */
} /* namespace lsp */
