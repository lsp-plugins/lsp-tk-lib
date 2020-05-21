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

//        Grid::cell_t *Grid::alloc_cell()
//        {
//            size_t n_cols   = vCols.size();
//            if (n_cols <= 0)
//                return NULL;
//            size_t n_rows   = vRows.size();
//            if (n_rows <= 0)
//                return NULL;
//
//            // Move iterator
//            while (true)
//            {
//                // Get cell
//                cell_t *cell = vCells.get(nCurrRow*n_cols + nCurrCol);
//                if (cell == NULL)
//                    return NULL;
//
//                // Check that cell is not tagged as ignore
//                if ((cell->pWidget != NULL) || (cell->nRows <= 0))
//                    cell = NULL;
//
//                // Iterate to new row and column
//                if (bVertical)
//                {
//                    if ((++nCurrRow) >= n_rows)
//                    {
//                        nCurrRow = 0;
//                        if ((++nCurrCol) >= n_cols)
//                            return cell;
//                    }
//                }
//                else
//                {
//                    if ((++nCurrCol) >= n_cols)
//                    {
//                        nCurrCol = 0;
//                        if ((++nCurrRow) >= n_rows)
//                            return cell;
//                    }
//                }
//
//                // Check that cell is valid
//                if (cell != NULL)
//                    return cell;
//            }
//        }
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
            cell->nRows     = xmax - w->nLeft;
            cell->nCols     = ymax - w->nTop;
            cell->nFlags    = 0;

            // Fill table with the cell
            for (size_t y=w->nTop; y < ymax; ++y)
            {
                cell_t **c = a->vTable.upget(y * a->nRows + w->nLeft);
                for (size_t x=0; x < xmax; ++x, ++c)
                    *c      = cell;
            }

            return true;
        }

        bool Grid::cleanup_row(alloc_t *a, size_t row)
        {
            cell_t **vc = a->vTable.pget(row * a->nCols);

            // Step 1: Scan for NULLs
            bool remove = true;
            for (size_t x=0; x < a->nCols; ++x)
                if (vc[x] != NULL)
                {
                    remove      = false;
                    break;
                }

            // Step 2: Scan for filling spans
            if ((!remove) && ((row + 1) < a->nRows))
            {
                remove      = true;
                cell_t **xc = &vc[a->nCols]; // Pointer to next row
                for (size_t x=0; x < a->nCols; ++x)
                    if (vc[x] != xc[x])
                    {
                        remove      = false;
                        break;
                    }
            }

            // Check that we can remove row
            if (!remove)
                return false;

            // Eliminate the row
            for (size_t x=0; x < a->nCols; ++x)
                if (vc[x] != NULL)
                    --vc[x]->nRows;

            a->vTable.remove_n(row * a->nCols, a->nCols);
            return true;
        }

        bool Grid::cleanup_column(alloc_t *a, size_t col)
        {
            cell_t **vc = a->vTable.pget(col);
            bool remove = true;

            // Step 1: Scan for NULLs
            bool remove = true;
            for (size_t y=0, off=0; y < a->nRows; ++y, off += a->nCols)
                if (vc[off] != NULL)
                {
                    remove      = false;
                    break;
                }

            // Step 2: Scan for filling spans
            if ((!remove) && ((col + 1) < a->nCols))
            {
                remove      = true;
                for (size_t y=0, off=9; y < a->nRows; ++y, off += a->nCols)
                    if (vc[off] != vc[off+1])
                    {
                        remove      = false;
                        break;
                    }
            }

            // Eliminate the column
            for (size_t y=0, off=0; y < a->nRows; ++y, off += a->nCols-1)
            {
                cell_t *c = a->vTable.get(off);
                if (c != NULL)
                    --c->nCols;
                a->vTable.remove(off);
            }

            return true;
        }

        status_t Grid::allocate_cells(alloc_t *a)
        {
            // Check size of grid
            size_t rows     = lsp_min(0, sRows.get());
            size_t cols     = lsp_min(0, sColumns.get());
            size_t items    = rows * cols;
            if (items < 1)
                return STATUS_OK;

            // Allocate necessary number of cells and clear
            cell_t **vcells = a->vTable.add_n(items);
            if (!vcells)
                return STATUS_NO_MEM;
            for (size_t i=0, n=items; i<n; ++i)
                vcells[i]       = NULL;
            a->nRows        = rows;
            a->nCols        = cols;

            // FIRST STEP: scan for directly attached cells and distribute
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                widget_t *w = vItems.uget(i);
                if ((w->nLeft < 0) || (w->nTop < 0))
                    continue;

                attach_cell(a, w, w->nLeft, w->nTop);
            }

            // SECOND STEP: distribute rest widgets between unallocated cells
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

            // THIRD STEP: Remove unallocated rows and columns
            for (size_t y=0; y < a->nRows; )
            {
                // Try to cleanup row
                if (!cleanup_row(a, y))
                    ++y;
            }
            for (size_t x=0; x < a->nCols; )
            {
                if (!cleanup_column(a, x))
                    ++x;
            }

            // FOURTH STEP: fill gaps with empty horizontal cells
            vcells = a->vTable.array();
            for (size_t y=0; (y < a->nRows) && (i < n); ++y)
            {
                cell_t *prev = NULL;
                for (size_t x=0; (x < a->nCols) && (i < n); ++x, ++vcells)
                {
                    if (*vcells == NULL)
                    {
                        if (prev == NULL)
                        {
                            // Allocate cell
                            if ((prev = a->vCells.add()) == NULL)
                                return STATUS_NO_MEM;

                            prev->pWidget   = NULL;
                            prev->nRows     = 1;
                            prev->nCols     = 1;
                            prev->nFlags    = 0;
                        }
                        else
                            ++prev->nCols;

                        *vcells     = prev;
                    }
                    else
                        prev    = NULL;
                }
            }

            return STATUS_OK;
        }
    
    } /* namespace tk */
} /* namespace lsp */
