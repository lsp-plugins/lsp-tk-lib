/*
 * Grid.cpp
 *
 *  Created on: 20 июн. 2017 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t Grid::metadata = { "Grid", &WidgetContainer::metadata };
        
        Grid::Grid(Display *dpy):
            WidgetContainer(dpy),
            sRows(&sProperties),
            sColumns(&sProperties),
            sOrientation(&sProperties),
            sHSpacing(&sProperties),
            sVSpacing(&sProperties)
        {
            pClass          = &metadata;
            sAlloc.nRows    = 0;
            sAlloc.nCols    = 0;
        }
        
        Grid::~Grid()
        {
            do_destroy();
        }


        void Grid::do_destroy()
        {
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                // Get widget
                widget_t *w = vItems.uget(i);
                if (w->pWidget == NULL)
                    continue;

                unlink_widget(w->pWidget);
                w->pWidget = NULL;
            }

            vItems.flush();

            sAlloc.vCells.flush();
            sAlloc.vTable.flush();
        }

        void Grid::destroy()
        {
            do_destroy();
            WidgetContainer::destroy();
        }

        status_t Grid::init()
        {
            status_t res = WidgetContainer::init();
            if (res == STATUS_OK)
            {
                sRows.bind("rows", &sStyle);
                sColumns.bind("columns", &sStyle);
                sHSpacing.bind("hspacing", &sStyle);
                sVSpacing.bind("vspacing", &sStyle);
                sOrientation.bind("orientation", &sStyle);

                Style *sclass = style_class();
                if (sclass != NULL)
                {
                    sStyle.add_parent(sclass);
                    sRows.init(sclass, 1);
                    sColumns.init(sclass, 1);
                    sHSpacing.init(sclass, 0);
                    sVSpacing.init(sclass, 0);
                    sOrientation.init(sclass, O_HORIZONTAL);
                }

                // Override settings for hfill and vfill
                sAllocation.set(true, false);
            }

            return STATUS_OK;
        }

        void Grid::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);
            if (sRows.is(prop))
                query_resize();
            if (sColumns.is(prop))
                query_resize();
            if (sHSpacing.is(prop))
                query_resize();
            if (sVSpacing.is(prop))
                query_resize();
            if (sOrientation.is(prop))
                query_resize();
        }

        bool Grid::hidden_widget(const widget_t *w)
        {
            if (w == NULL)
                return true;
            if (w->pWidget == NULL)
                return true;
            return !w->pWidget->visibility()->get();
        }

        Widget *Grid::find_widget(ssize_t x, ssize_t y)
        {
            for (size_t i=0, n=sAlloc.vCells.size(); i<n; ++i)
            {
                cell_t *w = sAlloc.vCells.uget(i);
                if (w->pWidget == NULL)
                    continue;
                if (w->pWidget->inside(x, y))
                    return w->pWidget;
            }

            return NULL;
        }

//        void Grid::split_size(header_t *h, size_t items, size_t size)
//        {
//            // Increase the size of each row
//            size_t delta    = size / items;
//            if (delta > 0)
//            {
//                for (size_t k=0; k < items; ++k)
//                    h[k].nSize += delta;
//
//                size       -= delta * items;
//            }
//
//            // Increment the size of other cells
//            for (size_t k=0; size > 0; k = (k+1) % items, size--)
//                h[k].nSize ++;
//        }
//
//        void Grid::distribute_size(cstorage<header_t> &vh, size_t idx, size_t items, size_t rq_size)
//        {
//            ssize_t size    = estimate_size(vh, idx, items, NULL);
//            ssize_t left    = rq_size - size;
//
//            if (left <= 0)
//                return;
//
//            size_t expanded = 0;
//            for (size_t k=0; k<items; ++k)
//            {
//                header_t *h     = vh.at(idx + k);
//                if (h->bExpand)
//                    expanded ++;
//            }
//
//            // Distribute size between expanded first
//            if (expanded > 0)
//            {
//                ssize_t total = 0;
//                for (size_t k=0; (k<items); ++k)
//                {
//                    header_t *h     = vh.at(idx + k);
//                    if (!h->bExpand)
//                        continue;
//                    size_t delta    = (h->nSize * left) / size;
//                    h->nSize       += delta;
//                    total          += delta;
//                }
//                left           -= total;
//
//                // Add equal size to each element
//                if (left > 0)
//                {
//                    size_t delta    = left / expanded;
//                    if (delta > 0)
//                    {
//                        for (size_t k=0; k < items; ++k)
//                        {
//                            header_t *h     = vh.at(idx + k);
//                            if (!h->bExpand)
//                                continue;
//                            h->nSize   += delta;
//                            left       -= delta;
//                        }
//                    }
//                }
//
//                // Distribute the non-distributed size
//                if (left > 0)
//                {
//                    for (size_t k=0; left > 0; k = (k+1) % items)
//                    {
//                        header_t *h     = vh.at(idx + k);
//                        if (!h->bExpand)
//                            continue;
//                        h->nSize ++;
//                        left --;
//                    }
//                }
//            }
//            else
//            {
//                if (size > 0)
//                {
//                    // Distribute left size proportionally
//                    ssize_t total = 0;
//                    for (size_t k=0; (k<items); ++k)
//                    {
//                        header_t *h     = vh.at(idx + k);
//                        size_t delta    = (h->nSize * left) / size;
//                        h->nSize       += delta;
//                        total          += delta;
//                    }
//                    left           -= total;
//                }
//
//                // Add equal size to each element
//                if (left > 0)
//                {
//                    size_t delta    = left / items;
//                    if (delta > 0)
//                    {
//                        for (size_t k=0; k < items; ++k)
//                        {
//                            header_t *h     = vh.at(idx + k);
//                            h->nSize       += delta;
//                        }
//                        left       -= items * delta;
//                    }
//                }
//
//                // Distribute the non-distributed size
//                if (left > 0)
//                {
//                    for (size_t k=0; left > 0; k = (k+1) % items)
//                    {
//                        header_t *h     = vh.at(idx + k);
//                        h->nSize ++;
//                        left --;
//                    }
//                }
//            }
//        }
//
//        size_t Grid::estimate_size(cstorage<header_t> &vh, size_t idx, size_t items, size_t *spacing)
//        {
//            size_t size = 0, last = 0;
//
//            for (size_t i=0, k=idx; i < items; ++i, ++k)
//            {
//                header_t *h = vh.at(k);
//                size       += h->nSize + last;
//                last        = h->nSpacing;
//            }
//
//            if (spacing != NULL)
//                *spacing = last;
//
//            return size;
//        }
//
//        void Grid::assign_coords(header_t *h, size_t items, size_t start)
//        {
//            for (size_t i=0; i<items; ++i)
//            {
//                h[i].nOffset    = start;
//                start          += h[i].nSize + h[i].nSpacing;
//            }
//        }

        void Grid::render(ws::ISurface *s, bool force)
        {
//            size_t items = vCells.size();

            // Check dirty flag
            if (nFlags & REDRAW_SURFACE)
                force = true;
/*
            // Estimate palette
            Color bg_color;

            // Render nested widgets
            size_t visible = 0;
            for (size_t i=0; i<items; ++i)
            {
                cell_t *w = vCells.at(i);
                if (w->nRows <= 0)
                    continue;
                if (hidden_widget(w))
                {
                    if (w->pWidget != NULL)
                        bg_color.copy(w->pWidget->bg_color()->color());
                    else
                        bg_color.copy(sBgColor);
                    s->fill_rect(w->a.nLeft, w->a.nTop, w->a.nWidth, w->a.nHeight, bg_color);
                    continue;
                }

                visible ++;
                if ((force) || (w->pWidget->redraw_pending()))
                {
//                    lsp_trace("render child=%p (%s), force=%d, pending=%d",
//                            w->pWidget, w->pWidget->get_class()->name,
//                            int(force), int(w->pWidget->redraw_pending()));
                    if (force)
                    {
                        bg_color.copy(w->pWidget->bg_color()->color());
                        s->fill_frame(
                            w->a.nLeft, w->a.nTop, w->a.nWidth, w->a.nHeight,
                            w->s.nLeft, w->s.nTop, w->s.nWidth, w->s.nHeight,
                            bg_color
                        );
//                        s->wire_rect(w->a.nLeft, w->a.nTop, w->a.nWidth, w->a.nHeight, 1, red);
                    }

    //                lsp_trace("Rendering this=%p, tgt=%p, force=%d", this, w->pWidget, int(force));
                    w->pWidget->render(s, force);
                    w->pWidget->commit_redraw();
                }
            }

            // Draw background if needed
            if ((!visible) && (force))
            {
                bg_color.copy(sBgColor);
                s->fill_rect(sSize.nLeft, sSize.nTop, sSize.nWidth, sSize.nHeight, bg_color);
            }

//            s->wire_rect(sSize.nLeft, sSize.nTop, sSize.nWidth, sSize.nHeight, 1, red);
 */
        }

//
//        status_t Grid::tag_cell(cell_t *c, bool main)
//        {
//            ssize_t idx = vCells.indexof(c);
//            if (idx < 0)
//                return STATUS_NOT_FOUND;
//
//            size_t row          = idx / vCols.size();
//            size_t col          = idx % vCols.size();
//            ssize_t max_rows    = vRows.size() - row;
//            ssize_t max_cols    = vCols.size() - col;
//
//            if (c->nRows > max_rows)
//                c->nRows        = max_rows;
//            if (c->nCols > max_cols)
//                c->nCols        = max_cols;
//            ssize_t rc          = (main) ? 1 : -1;
//
//            for (ssize_t j=0; j<c->nRows; ++j)
//            {
//                for (ssize_t i=0; i<c->nCols; ++i)
//                {
//                    cell_t *x   = vCells.get(idx + i);
//                    if ((x != NULL) && (x != c))
//                    {
//                        x->nRows    = rc;
//                        x->nCols    = rc;
//                    }
//                }
//                idx    += vCols.size();
//            }
//
//            return STATUS_OK;
//        }

        status_t Grid::add(Widget *widget)
        {
            return attach(-1, -1, widget, 1, 1);
        }

        status_t Grid::attach(size_t left, size_t top, Widget *widget)
        {
            return attach(left, top, widget, 1, 1);
        }

        status_t Grid::add(Widget *widget, size_t rows, size_t cols)
        {
            return attach(-1, -1, widget, rows, cols);
        }

        status_t Grid::attach(size_t left, size_t top, Widget *widget, size_t rows, size_t cols)
        {
            // Add widget
            if (widget == NULL)
                return STATUS_BAD_ARGUMENTS;

            // Check that widget already exists
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                widget_t *cell      = vItems.uget(i);
                if (cell->pWidget == widget)
                    return STATUS_ALREADY_EXISTS;
            }

            // Allocate item
            widget_t *item = vItems.add();
            if (item == NULL)
                return STATUS_NO_MEM;

            // Complete item
            item->pWidget   = widget;
            item->nLeft     = ssize_t(left);
            item->nTop      = ssize_t(top);
            item->nRows     = rows;
            item->nCols     = cols;

            if (widget != NULL)
                widget->set_parent(this);

            query_resize();
            return STATUS_OK;
        }

        status_t Grid::remove(Widget *widget)
        {
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                widget_t *cell    = vItems.uget(i);
                if (cell->pWidget == widget)
                {
                    if (!vItems.remove(i))
                        return STATUS_NO_MEM;

                    sAlloc.vCells.clear();
                    sAlloc.vTable.clear();

                    unlink_widget(widget);
                    return STATUS_OK;
                }
            }

            return STATUS_NOT_FOUND;
        }

        void Grid::realize(const ws::rectangle_t *r)
        {
            /*
            cell_t *w;
            header_t *h, *v;
            size_t hs = 0, vs = 0;
            realize_t alloc;

            size_t n_rows   = vRows.size();
            size_t n_cols   = vCols.size();
            
            // Distribute size between cells
            distribute_size(vRows, 0, n_rows, r->nHeight);
            distribute_size(vCols, 0, n_cols, r->nWidth);

            assign_coords(vRows.get_array(), n_rows, r->nTop);
            assign_coords(vCols.get_array(), n_cols, r->nLeft);

            // Now we are ready to realize cells
            w           = vCells.get_array();
            for (size_t i=0; i<n_rows; ++i)
            {
                h               = vRows.at(i);

                for (size_t j=0; j<n_cols; ++j, ++w)
                {
                    v               = vCols.at(j);

                    if ((w->nRows <= 0) || (w->nCols <= 0))
                        continue;

                    w->a.nLeft      = v->nOffset;
                    w->a.nTop       = h->nOffset;
                    w->a.nWidth     = estimate_size(vCols, j, w->nCols, &hs);
                    w->a.nHeight    = estimate_size(vRows, i, w->nRows, &vs);

                    alloc           = w->a;
                    if ((j + w->nCols) < n_cols)
                        w->a.nWidth    += hs;
                    if ((i + w->nRows) < n_rows)
                        w->a.nHeight   += vs;

                    if (hidden_widget(w))
                        continue;

                    w->s            = alloc; // Copy cell parameters to cell size attributes
                    w->s.nWidth    -= w->p.nLeft + w->p.nRight;
                    w->s.nHeight   -= w->p.nTop  + w->p.nBottom;

                    // Do not fill horizontally
                    if (!w->pWidget->hfill())
                    {
                        ssize_t nw      = (w->r.nMinWidth >= 0) ? w->r.nMinWidth : 0;
                        w->s.nLeft     += (w->s.nWidth - nw) >> 1;
                        w->s.nWidth     = nw;
                    }
                    else
                    {
                        if ((w->r.nMaxWidth >= 0) && (w->s.nWidth > w->r.nMaxWidth))
                        {
                            w->s.nLeft     +=   (w->s.nWidth - w->r.nMaxWidth) >> 1;
                            w->s.nWidth     =   w->r.nMaxWidth;
                        }
                    }

                    // Do not fill vertically
                    if (!w->pWidget->vfill())
                    {
                        ssize_t nh      = (w->r.nMinHeight >= 0) ? w->r.nMinHeight : 0;
                        w->s.nTop      += (w->s.nHeight - nh) >> 1;
                        w->s.nHeight    = nh;
                    }
                    else
                    {
                        if ((w->r.nMaxHeight >= 0) && (w->s.nHeight > w->r.nMaxHeight))
                        {
                            w->s.nTop      +=   (w->s.nHeight - w->r.nMaxHeight) >> 1;
                            w->s.nHeight    =   w->r.nMaxHeight;
                        }
                    }

                    w->s.nLeft     += w->p.nLeft;
                    w->s.nTop      += w->p.nTop;

                    // Issue realize
                    w->pWidget->realize(&w->s);
                    w->pWidget->query_draw();
                }
            }

            // Save geometry
            LSPWidgetContainer::realize(r);*/
        }

        void Grid::size_request(ws::size_limit_t *r)
        {
            /*
            header_t *h, *v;
            cell_t *w;

            r->nMinWidth    = 0;
            r->nMinHeight   = 0;
            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;

            if (vCells.size() <= 0)
                return;

            size_t n_rows   = vRows.size();
            size_t n_cols   = vCols.size();

//            #ifdef LSP_TRACE
//            for (size_t i=0; i<n_rows; ++i)
//                for (size_t j=0; j<n_cols; ++j)
//                {
//                    cell_t *c = vCells.get(i*n_cols + j);
//                    lsp_trace("c[%d,%d] = %p { rows=%d, cols=%d, widget=%p }",
//                            int(i), int(j), c, int(c->nRows), int(c->nCols), c->pWidget);
//                }
//            #endif

            // Perform size request for all visible child widgets
            size_t n_cells = vCells.size();
            for (size_t i=0; i<n_cells; ++i)
            {
                // Get widget
                w           = vCells.get(i);
                if ((w->nRows <= 0) || (hidden_widget(w)))
                    continue;

                // Perform size request
                w->r.nMinWidth      = -1;
                w->r.nMinHeight     = -1;
                w->r.nMaxWidth      = -1;
                w->r.nMaxHeight     = -1;
                if (w->pWidget == NULL)
                    continue;

                w->pWidget->size_request(&w->r);
                w->pWidget->padding()->get(&w->p);
            }

            // Estimate minimum size of each row by using 1xM (single) cells
            for (size_t i=0; i<n_rows; ++i)
            {
                h           = vRows.get(i);
                h->nSize    = 0;
                h->nSpacing = 0;
                h->nOffset  = 0;
                h->bExpand  = false;
            }
            for (size_t i=0; i<n_cols; ++i)
            {
                h           = vCols.get(i);
                h->nSize    = 0;
                h->nSpacing = 0;
                h->nOffset  = 0;
                h->bExpand  = false;
            }

            // Estimate minimum row/column size for 1xN and Mx1 cells
            w           = vCells.get_array();
            for (size_t i=0; i<n_rows; ++i)
            {
                h          = vRows.get(i);
                for (size_t j=0; j<n_cols; ++j, ++w)
                {
                    v           = vCols.get(j);
                    if (hidden_widget(w))
                        continue;

                    if (w->nRows == 1)
                    {
                        ssize_t space   = w->p.nTop + w->p.nBottom;
                        if (w->r.nMinHeight >= 0)
                            space          += w->r.nMinHeight;
                        if (h->nSize < space)
                            h->nSize        = space;
                        h->nSpacing     = nVSpacing;
                    }
                    if (w->nCols == 1)
                    {
                        ssize_t space   = w->p.nLeft + w->p.nRight;
                        if (w->r.nMinWidth >= 0)
                            space          += w->r.nMinWidth;
                        if (v->nSize < space)
                            v->nSize    = space;
                        v->nSpacing     = nHSpacing;
                    }
                }
            }

            // Estimate minimum row/column size for N x M cells
            w           = vCells.get(0);
            for (size_t i=0; i<n_rows; ++i)
            {
                h          = vRows.get(i);
                for (size_t j=0; j<n_cols; ++j, ++w)
                {
                    v           = vCols.get(j);
                    if (hidden_widget(w))
                        continue;

                    if (w->nRows > 1)
                    {
                        ssize_t space   = w->p.nTop + w->p.nBottom;
                        if (w->r.nMinHeight >= 0)
                            space          += space;
                        distribute_size(vRows, i, w->nRows, space);
                    }
                    if (w->nCols > 1)
                    {
                        ssize_t space   = w->p.nLeft + w->p.nRight;
                        if (w->r.nMinWidth >= 0)
                            space          += w->r.nMinWidth;
                        distribute_size(vCols, j, w->nCols, space);
                    }
                }
            }

            // Mark some rows/cols as expanded
            for (size_t i=0, n=vCells.size(); i<n; ++i)
            {
                w           = vCells.get(i);
                if (hidden_widget(w))
                    continue;
                if (!w->pWidget->expand())
                    continue;
                size_t row = i / n_cols;
                size_t col = i % n_cols;
                for (ssize_t i=0; i<w->nRows; ++i)
                    vRows.get(row + i)->bExpand = true;
                for (ssize_t i=0; i<w->nCols; ++i)
                    vCols.get(col + i)->bExpand = true;
            }

            // Calculate the size of table
            r->nMinHeight  += estimate_size(vRows, 0, n_rows, NULL);
            r->nMinWidth   += estimate_size(vCols, 0, n_cols, NULL);
            for (size_t i=0; i<n_rows; ++i)
            {
                h   = vRows.at(i);
                h->nMinSize   = h->nSize;
            }
            for (size_t i=0; i<n_cols; ++i)
            {
                h   = vCols.at(i);
                h->nMinSize   = h->nSize;
            }
//            lsp_trace("MinWidth = %d, MinHeight=%d", int(r->nMinWidth), int(r->nMinHeight));
 */
        }

        bool Grid::attach_cell(alloc_t *a, widget_t *w, size_t left, size_t top)
        {
            // Check that cell does not go outside the
            if ((size_t(w->nLeft) >= a->nCols) || (size_t(w->nTop) >= a->nRows))
                return false;

            size_t xmax = lsp_min(w->nLeft + w->nCols, a->nCols);
            size_t ymax = lsp_min(w->nTop  + w->nRows, a->nRows);

            // Check that we can allocate cells
            for (size_t y=w->nTop; y < ymax; ++y)
            {
                cell_t **c = a->vTable.upget(y * a->nRows + w->nLeft);
                for (size_t x=0; x < xmax; ++x, ++c)
                    if (*c != NULL)
                        return false;
            }

            // Allocate cell
            cell_t *cell = a->vCells.add();
            if (cell == NULL)
                return false;

            cell->pWidget   = w->pWidget;
            cell->nLeft     = left;
            cell->nTop      = top;
            cell->nRows     = xmax - w->nLeft;
            cell->nCols     = ymax - w->nTop;

            // Fill table with the cell
            for (size_t y=w->nTop; y < ymax; ++y)
            {
                cell_t **c = a->vTable.upget(y * a->nRows + w->nLeft);
                for (size_t x=0; x < xmax; ++x, ++c)
                    *c      = cell;
            }

            return true;
        }

        status_t Grid::attach_cells(alloc_t *a)
        {
            // Check size of grid
            a->nRows        = lsp_min(0, sRows.get());
            a->nCols        = lsp_min(0, sColumns.get());
            size_t items    = a->nRows * a->nCols;
            if (items < 1)
                return STATUS_OK;

            // Allocate necessary number of cells and clear
            cell_t **vcells = a->vTable.add_n(items);
            if (!vcells)
                return STATUS_NO_MEM;
            for (size_t i=0, n=items; i<n; ++i)
                vcells[i]       = NULL;

            // Scan for directly attach()'ed cells and distribute
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                widget_t *w = vItems.uget(i);
                if ((w->nLeft < 0) || (w->nTop < 0))
                    continue;

                attach_cell(a, w, w->nLeft, w->nTop);
            }

            // Distribute add()'ed widgets between unallocated cells depending on orientation
            size_t i = 0, n = vItems.size();

            if (sOrientation.horizontal())
            {
                for (size_t x=0; (x < a->nCols) && (i < n); ++x)
                    for (size_t y=0; (y < a->nRows) && (i < n); ++y)
                        for (; i < n; ++i)
                        {
                            widget_t *w = vItems.uget(i);
                            if ((w->nLeft >= 0) || (w->nTop >= 0))
                                continue;
                            if (attach_cell(a, w, x, y))
                                break;
                        }
            }
            else
            {
                for (size_t y=0; (y < a->nRows) && (i < n); ++y)
                    for (size_t x=0; (x < a->nCols) && (i < n); ++x)
                        for (; i < n; ++i)
                        {
                            widget_t *w = vItems.uget(i);
                            if ((w->nLeft >= 0) || (w->nTop >= 0))
                                continue;
                            if (attach_cell(a, w, x, y))
                                break;
                        }
            }

            return STATUS_OK;
        }

        bool Grid::is_null_row(alloc_t *a, size_t row)
        {
            size_t off = row * a->nCols;
            for (size_t x=0; x < a->nCols; ++x, ++off)
            {
                if (a->vTable.uget(off) != NULL)
                    return false;
            }

            return true;
        }

        bool Grid::is_null_col(alloc_t *a, size_t col)
        {
            size_t off = col;
            for (size_t y=0; y < a->nRows; ++y, off += a->nCols)
            {
                if (a->vTable.uget(off) != NULL)
                    return false;
            }

            return true;
        }

        bool Grid::row_equals(alloc_t *a, size_t r1, size_t r2)
        {
            if ((r1 >= a->nRows) || (r2 >= a->nRows))
                return false;

            r1 *= a->nCols;
            r2 *= a->nCols;
            for (size_t i=0; i<a->nCols; ++i, ++r1, ++r2)
            {
                cell_t *x1 = a->vTable.uget(r1);
                cell_t *x2 = a->vTable.uget(r2);
                if (x1 != x2)
                    return false;
            }

            return true;
        }

        bool Grid::col_equals(alloc_t *a, size_t c1, size_t c2)
        {
            if ((c1 >= a->nCols) || (c2 >= a->nCols))
                return false;

            for (size_t i=0; i<a->nRows; ++i, c1 += a->nCols, c2 += a->nCols)
            {
                cell_t *x1 = a->vTable.uget(c1);
                cell_t *x2 = a->vTable.uget(c2);
                if (x1 != x2)
                    return false;
            }

            return true;
        }

        void Grid::remove_row(alloc_t *a, size_t id)
        {
            // Decrement number of rows used by column
            for (size_t i=0, off=id*a->nCols; i<a->nCols; ++i, ++off)
            {
                cell_t *c       = a->vTable.uget(off);
                if (c != NULL)
                    --c->nRows;
            }

            // Remove row in table and decrement overall number of rows
            a->vTable.remove_n(id * a->nCols, a->nCols);
            a->vRows.remove(id);
            --a->nRows;
        }

        void Grid::remove_col(alloc_t *a, size_t id)
        {
            // Eliminate the column and decrement number of cells used by column
            for (size_t i=0, off=id; i < a->nRows; ++i, off += a->nCols-1)
            {
                cell_t *c = a->vTable.get(off);
                if (c != NULL)
                    --c->nCols;
                a->vTable.remove(off);
            }

            // Remove column in table and decrement overall number of columns
            a->vCols.remove(id);
            --a->nCols;
        }

        status_t Grid::create_row_col_descriptors(alloc_t *a)
        {
            header_t *h;

            // Allocate row and column descriptors
            if (!a->vRows.add_n(a->nRows))
                return STATUS_NO_MEM;
            if (!a->vCols.add_n(a->nCols))
                return STATUS_NO_MEM;

            // Get scaling and spacings
            float scaling   = lsp_min(0.0f, sScaling.get());
            size_t hspacing = lsp_min(0, scaling * sHSpacing.get());
            size_t vspacing = lsp_min(0, scaling * sVSpacing.get());

            // Initialize row and column descriptors
            for (size_t i=0; i<a->nRows; ++i)
            {
                h               = a->vRows.uget(i);
                h->nSize        = 0;
                h->nWeight      = 1;
                h->nSpacing     = hspacing;
                h->nFlags       = 0;
            }
            for (size_t i=0; i<a->nCols; ++i)
            {
                h               = a->vCols.uget(i);
                h->nSize        = 0;
                h->nWeight      = 1;
                h->nSpacing     = vspacing;
                h->nFlags       = 0;
            }

            // Remove empty rows and columns
            for (size_t y=0; y < a->nRows; )
            {
                if (is_null_row(a, y))
                    remove_row(a, y);
                else if (row_equals(a, y, y+1))
                {
                    remove_row(a, y+1);
                    h               = a->vRows.uget(y);
                    ++h->nWeight;   // Increment weight of current row
                }
                else
                    ++y;
            }
            for (size_t x=0; x < a->nCols; )
            {
                if (is_null_col(a, x))
                    remove_col(a, x);
                else if (col_equals(a, x, x+1))
                {
                    remove_col(a, x);
                    h               = a->vCols.uget(x);
                    ++h->nWeight;   // Increment weight of current column
                }
                else
                    ++x;
            }

            // Replace empty cells with stubs
            for (size_t y=0, off=0; (y < a->nRows); ++y)
            {
                cell_t *prev = NULL;
                for (size_t x=0; (x < a->nCols); ++x, ++off)
                {
                    cell_t **p = a->vTable.upget(off);
                    if (*p == NULL)
                    {
                        if (prev == NULL)
                        {
                            // Allocate cell
                            if ((prev = a->vCells.add()) == NULL)
                                return STATUS_NO_MEM;

                            prev->pWidget   = NULL;
                            prev->nLeft     = x;
                            prev->nTop      = y;
                            prev->nRows     = 1;
                            prev->nCols     = 0;
                        }

                        ++prev->nCols;
                        *p      = prev;
                    }
                    else
                        prev    = NULL;
                }
            }

            // Mark last row and last column as non-spacing
            h   = a->vRows.get(a->nRows - 1);
            h->nSpacing     = 0;
            h   = a->vCols.get(a->nCols - 1);
            h->nSpacing     = 0;

            // Initialize padding
            for (size_t i=0, n=a->vCells.size(); i<n; ++i)
            {
                cell_t *c       = a->vCells.uget(i);

                // Horizontal
                if ((c->pWidget != NULL) && (c->pWidget->allocation()->hexpand()))
                    for (ssize_t j=0; j<c->nCols; ++j)
                    {
                        h               = a->vCols.uget(c->nLeft + j);
                        h->nFlags      |= F_EXPAND;
                    }

                // Vertical
                if ((c->pWidget != NULL) && (c->pWidget->allocation()->vexpand()))
                    for (ssize_t j=0; j<c->nRows; ++j)
                    {
                        h               = a->vRows.uget(c->nTop + j);
                        h->nFlags      |= F_EXPAND;
                    }
            }

            return STATUS_OK;
        }

        size_t Grid::estimate_size(lltl::darray<header_t> *hdr, size_t first, size_t count)
        {
            size_t res  = 0;
            for (size_t i=0; i<count; ++i)
            {
                header_t *h = hdr->uget(first);
                res += h->nSize;
                if ((++first) < count)
                    res    += h->nSpacing;
            }
            return res;
        }

        void Grid::distribute_size(lltl::darray<header_t> *vh, size_t first, size_t count, size_t size)
        {
            // Estimate number of expanded items and overall weight
            size_t expanded = 0, weight = 0, width = 0;
            for (size_t k=0; k<count; ++k)
            {
                header_t *h     = vh->uget(first + k);
                weight         += h->nSize * h->nWeight;
                width          += h->nSize;
                if ((k+1) < count)
                {
                    weight         += h->nSpacing;
                    width          += h->nSpacing;
                }
                if (h->nFlags & F_EXPAND)
                    expanded ++;
            }

            if (size <= width)
                return;
            size_t left = size - width;

            // Form list of items for size distribution
            lltl::parray<header_t> vl;
            for (size_t k=0; k<count; ++k)
            {
                header_t *h     = vh->uget(first + k);
                if (expanded > 0)
                {
                    if (h->nFlags & F_EXPAND)
                        vl.add(h);
                }
                else
                    vl.add(h);
            }
            count   = vl.size();

            // Distribute size between selected items
            if (weight > 0)
            {
                // Distribute left size proportionally
                ssize_t total = 0;
                for (size_t k=0; k<count; ++k)
                {
                    header_t *h     = vl.uget(k);
                    size_t hw       = h->nSize * h->nWeight;
                    if ((k + 1) < count)
                        hw             += h->nSpacing;
                    size_t delta    = (hw * left) / weight;
                    h->nSize       += delta;
                    total          += delta;
                }
                left           -= total;
            }

            // Add equal size to each element
            if (left > 0)
            {
                size_t delta    = left / count;
                if (delta > 0)
                {
                    for (size_t k=0; k<count; ++k)
                    {
                        header_t *h     = vl.uget(k);
                        h->nSize       += delta;
                        left           -= delta;
                    }
                }
            }

            // Distribute the non-distributed size
            for (size_t k=0; left > 0; k = (k+1) % count)
            {
                header_t *h     = vl.uget(k);
                h->nSize ++;
                left --;
            }
        }

        status_t Grid::estimate_sizes(alloc_t *a)
        {
            ws::size_limit_t sr;
            header_t *h;
            header_t *vrows = a->vRows.array();
            header_t *vcols = a->vCols.array();

            // Estimate minimum row/column size for 1xN and Mx1 cells
            for (size_t i=0, n=a->vCells.size(); i<n; ++i)
            {
                cell_t *w       = a->vCells.uget(i);
                if ((w->pWidget == NULL) || (w->nRows != 1) || (w->nCols != 1))
                    continue;

                // Get size limits of the widget
                w->pWidget->get_size_limits(&sr);

                if (w->nRows == 1)
                {
                    h               = &vrows[i];
                    h->nSize        = lsp_max(h->nSize, sr.nMinHeight);
                }
                if (w->nCols == 1)
                {
                    h               = &vcols[i];
                    h->nSize        = lsp_max(h->nSize, sr.nMinWidth);
                }
            }

            // Estimate minimum row/column size for N x M cells
            for (size_t i=0, n=a->vCells.size(); i<n; ++i)
            {
                cell_t *w       = a->vCells.uget(i);
                if (w->pWidget == NULL)
                    continue;
                if ((w->nRows <= 1) && (w->nCols <= 1))
                    continue;

                // Get size limits of the widget
                w->pWidget->get_size_limits(&sr);

                if ((w->nRows > 1) && (sr.nMinHeight > 0))
                {
                    if (sr.nMinHeight > 0)
                        distribute_size(&a->vRows, w->nTop,  w->nRows, sr.nMinHeight);
                }
                if ((w->nCols > 1) && (sr.nMinWidth > 0))
                {
                    if (sr.nMinWidth > 0)
                        distribute_size(&a->vCols, w->nLeft, w->nCols, sr.nMinWidth);
                }
            }

            return STATUS_OK;
        }

        status_t Grid::allocate_cells(alloc_t *a)
        {
            // Attach cells
            status_t res    = attach_cells(a);
            if ((res == STATUS_OK) || (a->nRows < 1) || (a->nCols < 1))
                return res;

            // Estimate row and column parameters
            if ((res = create_row_col_descriptors(a)) != STATUS_OK)
                return res;

            // Estimate cell sizes
            if ((res = estimate_sizes(a)) != STATUS_OK)
                return res;

            return STATUS_OK;
        }
    
    } /* namespace tk */
} /* namespace lsp */
