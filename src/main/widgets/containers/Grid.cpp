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
            LSP_TK_STYLE_IMPL_BEGIN(Grid, WidgetContainer)
                // Bind
                sRows.bind("rows", this);
                sColumns.bind("columns", this);
                sHSpacing.bind("hspacing", this);
                sVSpacing.bind("vspacing", this);
                sOrientation.bind("orientation", this);
                sConstraints.bind("size.constraints", this);
                // Configure
                sRows.set(1);
                sColumns.set(1);
                sHSpacing.set(0);
                sVSpacing.set(0);
                sOrientation.set(O_HORIZONTAL);
                // Override
                sAllocation.set(true, true, false, false);
                // Commit
                sAllocation.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(Grid, "Grid", "root");
        }

        const w_class_t Grid::metadata = { "Grid", &WidgetContainer::metadata };
        
        Grid::Grid(Display *dpy):
            WidgetContainer(dpy),
            sRows(&sProperties),
            sColumns(&sProperties),
            sHSpacing(&sProperties),
            sVSpacing(&sProperties),
            sOrientation(&sProperties)
        {
            pClass          = &metadata;
            sAlloc.nRows    = 0;
            sAlloc.nCols    = 0;
        }
        
        Grid::~Grid()
        {
            nFlags     |= FINALIZED;
            do_destroy();
        }


        void Grid::do_destroy()
        {
            // Cleanup cached data
            free_cells(&sAlloc);

            // Unlink children
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                // Get widget
                widget_t *w = vItems.uget(i);
                if (w->pWidget == NULL)
                    continue;

                unlink_widget(w->pWidget);
                w->pWidget = NULL;
            }

            // Free list of children
            vItems.flush();
        }

        void Grid::destroy()
        {
            nFlags     |= FINALIZED;
            do_destroy();
            WidgetContainer::destroy();
        }

        Grid::cell_t *Grid::alloc_cell(lltl::parray<cell_t> *list)
        {
            cell_t *cell = static_cast<cell_t *>(malloc(sizeof(cell_t)));
            if (cell == NULL)
                return NULL;

            if (!list->add(cell))
            {
                free(cell);
                return NULL;
            }

            cell->a.nLeft   = 0;
            cell->a.nTop    = 0;
            cell->a.nWidth  = 0;
            cell->a.nHeight = 0;
            cell->s.nLeft   = 0;
            cell->s.nTop    = 0;
            cell->s.nWidth  = 0;
            cell->s.nHeight = 0;

            cell->pWidget   = NULL;
            cell->nLeft     = 0;
            cell->nTop      = 0;
            cell->nRows     = 0;
            cell->nCols     = 0;
            cell->nTag      = 0;

            return cell;
        }

        void Grid::free_cell(cell_t *cell)
        {
            if (cell == NULL)
                return;

            cell->pWidget   = NULL;
            free(cell);
        }

        void Grid::free_cells(alloc_t *alloc)
        {
            for (size_t i=0, n=alloc->vCells.size(); i<n; ++i)
                free_cell(alloc->vCells.uget(i));

            alloc->vCells.flush();
            alloc->vTable.flush();
        }

        status_t Grid::init()
        {
            status_t res = WidgetContainer::init();
            if (res != STATUS_OK)
                return res;

            sRows.bind("rows", &sStyle);
            sColumns.bind("columns", &sStyle);
            sHSpacing.bind("hspacing", &sStyle);
            sVSpacing.bind("vspacing", &sStyle);
            sOrientation.bind("orientation", &sStyle);
            sConstraints.bind("size.constraints", &sStyle);

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
            if (sConstraints.is(prop))
                query_resize();
        }

        bool Grid::hidden_widget(const widget_t *w)
        {
            if ((w == NULL) || (w->pWidget == NULL) || (!w->pWidget->valid()))
                return true;
            return !w->pWidget->visibility()->get();
        }

        Widget *Grid::find_widget(ssize_t x, ssize_t y)
        {
            for (size_t i=0, n=sAlloc.vCells.size(); i<n; ++i)
            {
                cell_t *w = sAlloc.vCells.uget(i);
                Widget *pw = w->pWidget;
                if ((pw == NULL) || (!pw->is_visible_child_of(this)))
                    continue;
                if (pw->inside(x, y))
                    return pw;
            }

            return NULL;
        }

        void Grid::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            lsp::Color bg_color;
            ws::rectangle_t xr;
            get_actual_bg_color(bg_color);

            // Check dirty flag
            if (nFlags & REDRAW_SURFACE)
                force = true;

            // Pass 1: render space around nested widgets, optimize for draw calls
            s->clip_begin(area);
            {
                lsp_finally { s->clip_end(); };

                // No widgets to draw?
                if (sAlloc.vTable.is_empty())
                {
                    s->fill_rect(bg_color, SURFMASK_NONE, 0.0f, &sSize);
                    return;
                }

                const float scaling     = lsp_max(0.0f, sScaling.get());
                const ssize_t hspacing  = scaling * sHSpacing.get();
                const ssize_t vspacing  = scaling * sVSpacing.get();

                for (size_t i=0, n=sAlloc.vTable.size(); i<n; ++i)
                {
                    cell_t *w = sAlloc.vTable.uget(i);

                    if (w->pWidget == NULL)
                    {
                        get_actual_bg_color(bg_color);

                        size_t cw   = w->a.nWidth;
                        size_t ch   = w->a.nHeight;
                        if (size_t(w->nLeft + w->nCols) < sAlloc.nCols)
                            cw         += hspacing;
                        if (size_t(w->nTop  + w->nRows) < sAlloc.nRows)
                            ch         += vspacing;

                        s->fill_rect(bg_color, SURFMASK_NONE, 0.0f, w->a.nLeft, w->a.nTop, cw, ch);
                        continue;
                    }

                    // Fill unused space with background
                    if (!force)
                        continue;

                    // Draw widget area
                    if (Size::overlap(area, &w->a))
                    {
                        w->pWidget->get_actual_bg_color(bg_color);
                        s->fill_frame(bg_color, SURFMASK_NONE, 0.0f, &w->a, &w->s);
                    }

                    // Need to draw spacing?
                    get_actual_bg_color(bg_color);
                    if ((hspacing > 0) && ((w->nLeft + w->nCols) < sAlloc.nCols))
                    {
                        xr.nLeft    = w->a.nLeft + w->a.nWidth;
                        xr.nTop     = w->a.nTop;
                        xr.nWidth   = hspacing;
                        xr.nHeight  = w->a.nHeight;

                        if (Size::overlap(area, &xr))
                            s->fill_rect(bg_color, SURFMASK_NONE, 0.0f, &xr);

                        if ((vspacing > 0) && ((w->nTop + w->nRows) < sAlloc.nRows))
                        {
                            xr.nLeft    = w->a.nLeft;
                            xr.nTop     = w->a.nTop + w->a.nHeight;
                            xr.nWidth   = w->a.nWidth + hspacing;
                            xr.nHeight  = vspacing;

                            if (Size::overlap(area, &xr))
                                s->fill_rect(bg_color, SURFMASK_NONE, 0.0f, &xr);
                        }
                    }
                    else if ((vspacing > 0) && ((w->nTop + w->nRows) < sAlloc.nRows))
                    {
                        xr.nLeft    = w->a.nLeft;
                        xr.nTop     = w->a.nTop + w->a.nHeight;
                        xr.nWidth   = w->a.nWidth;
                        xr.nHeight  = vspacing;

                        if (Size::overlap(area, &xr))
                            s->fill_rect(bg_color, SURFMASK_NONE, 0.0f, &xr);
                    }
                }
            } // clip_begin()

            // Pass 2: Render nested widgets
            for (size_t i=0, n=sAlloc.vTable.size(); i<n; ++i)
            {
                cell_t *w = sAlloc.vTable.uget(i);
                if (w->pWidget == NULL)
                    continue;

                if ((force) || (w->pWidget->redraw_pending()))
                {
                    if (Size::intersection(&xr, area, &w->s))
                        w->pWidget->render(s, &xr, force);
                    w->pWidget->commit_redraw();
                }
            }
        }

        status_t Grid::add(Widget *widget)
        {
            return attach_internal(-1, -1, widget, 1, 1);
        }

        status_t Grid::attach(size_t left, size_t top, Widget *widget)
        {
            return attach_internal(left, top, widget, 1, 1);
        }

        status_t Grid::add(Widget *widget, size_t rows, size_t cols)
        {
            return attach_internal(-1, -1, widget, rows, cols);
        }

        status_t Grid::attach(size_t left, size_t top, Widget *widget, size_t rows, size_t cols)
        {
            return attach_internal(left, top, widget, rows, cols);
        }

        status_t Grid::attach_internal(ssize_t left, ssize_t top, Widget *widget, size_t rows, size_t cols)
        {
            // Add widget
            if ((rows < 1) || (cols < 1))
                return STATUS_BAD_ARGUMENTS;

            // Check that widget already exists
            if (widget != NULL)
            {
                for (size_t i=0, n=vItems.size(); i<n; ++i)
                {
                    widget_t *cell      = vItems.uget(i);
                    if (cell->pWidget == widget)
                        return STATUS_ALREADY_EXISTS;
                }
            }

            // Allocate item
            widget_t *item = vItems.add();
            if (item == NULL)
                return STATUS_NO_MEM;

            // Complete item
            item->pWidget   = widget;
            item->nLeft     = left;
            item->nTop      = top;
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

                    free_cells(&sAlloc);
                    unlink_widget(widget);
                    return STATUS_OK;
                }
            }

            return STATUS_NOT_FOUND;
        }

        status_t Grid::remove_all()
        {
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                widget_t *cell    = vItems.uget(i);
                if (cell != NULL)
                    unlink_widget(cell->pWidget);
            }

            free_cells(&sAlloc);
            vItems.flush();

            return STATUS_OK;
        }

        void Grid::realize(const ws::rectangle_t *r)
        {
            free_cells(&sAlloc);

//            lsp_trace("this=%p, size={%d, %d, %d, %d}",
//                    this, int(r->nLeft), int(r->nTop), int(r->nWidth), int(r->nHeight)
//                );
//
            alloc_t a;
            status_t res = allocate_cells(&a);
            if (res != STATUS_OK)
            {
                free_cells(&a);
                return;
            }

            // Distribute the size between rows and columns
            distribute_size(&a.vCols, 0, a.nCols, r->nWidth);
            distribute_size(&a.vRows, 0, a.nRows, r->nHeight);

            // Assign coordinates to cells
            assign_coords(&a, r);

            // Realize widgets
            realize_children(&a);

            // Swap the actual data
            sAlloc.vCells.swap(&a.vCells);
            sAlloc.vTable.swap(&a.vTable);
            sAlloc.vRows.swap(&a.vRows);
            sAlloc.vCols.swap(&a.vCols);
            sAlloc.nRows    = a.nRows;
            sAlloc.nCols    = a.nCols;

            // Call parent method to realize
            WidgetContainer::realize(r);

            // Destroy the previously used data
            free_cells(&a);
        }

        void Grid::size_request(ws::size_limit_t *r)
        {
            alloc_t a;
            float scaling       = lsp_max(0.0f, sScaling.get());

            // Allocate cells
            allocate_cells(&a);

            // Estimate size
            r->nMinWidth        = estimate_size(&a.vCols, 0, a.nCols);
            r->nMinHeight       = estimate_size(&a.vRows, 0, a.nRows);
            r->nMaxWidth        = -1;
            r->nMaxHeight       = -1;
            r->nPreWidth        = -1;
            r->nPreHeight       = -1;

            // Apply size constraints
            sConstraints.apply(r, scaling);

            free_cells(&a);


//            lsp_trace("w={%d, %d}, h={%d, %d}",
//                    int(r->nMinWidth), int(r->nMaxWidth), int(r->nMinHeight), int(r->nMaxHeight)
//            );
        }

        bool Grid::attach_cell(alloc_t *a, widget_t *w, size_t left, size_t top)
        {
            // Check that cell does not go outside the
            if ((left >= a->nCols) || (top >= a->nRows))
                return false;

            size_t xmax = lsp_min(left + w->nCols, a->nCols);
            size_t ymax = lsp_min(top  + w->nRows, a->nRows);

            // Check that we can allocate cells
            for (size_t y=top; y < ymax; ++y)
            {
                size_t off = y * a->nCols;
                for (size_t x=left; x < xmax; ++x)
                    if (a->vTable.get(off + x) != NULL)
                        return false;
            }

            // Allocate cell
            cell_t *cell = alloc_cell(&a->vCells);
            if (cell == NULL)
                return false;

            cell->pWidget   = w->pWidget;
            cell->nLeft     = left;
            cell->nTop      = top;
            cell->nRows     = ymax - top;
            cell->nCols     = xmax - left;
            cell->nTag      = 0;

//            lsp_trace("attach_cell widget=%p, structure={%d, %d, %d, %d}",
//                    cell->pWidget, int(cell->nLeft), int(cell->nTop), int(cell->nRows), int(cell->nCols)
//            );

            // Fill table with the cell
            for (size_t y=top; y < ymax; ++y)
            {
                size_t off = y * a->nCols;
                for (size_t x=left; x < xmax; ++x)
                    a->vTable.set(off + x, cell);
            }

            return true;
        }

        status_t Grid::attach_cells(alloc_t *a)
        {
            // Check size of grid
            a->nRows        = lsp_max(0, sRows.get());
            a->nCols        = lsp_max(0, sColumns.get());
            a->nTag         = 0;
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
                // Fill cells from left to right first, then move to next row
                for (size_t y=0; (y < a->nRows) && (i < n); ++y)
                    for (size_t x=0; (x < a->nCols) && (i < n); ++x)
                    {
                        // Ensure that cell is free
                        cell_t *c = a->vTable.uget(y * a->nCols + x);
                        if (c != NULL)
                            continue;

                        // Try to attach widget to the cell
                        while (i < n)
                        {
                            widget_t *w = vItems.uget(i++);
                            if ((w->nLeft >= 0) || (w->nTop >= 0))
                                continue;
                            if (attach_cell(a, w, x, y))
                                break;
                        }
                    }
            }
            else
            {
                // Fill cells from top to bottom first, then move to next column
                for (size_t x=0; (x < a->nCols) && (i < n); ++x)
                    for (size_t y=0; (y < a->nRows) && (i < n); ++y)
                    {
                        // Ensure that cell is free
                        cell_t *c = a->vTable.uget(y * a->nCols + x);
                        if (c != NULL)
                            continue;

                        // Try to attach widget to the cell
                        while (i < n)
                        {
                            widget_t *w = vItems.uget(i++);
                            if ((w->nLeft >= 0) || (w->nTop >= 0))
                                continue;
                            if (attach_cell(a, w, x, y))
                                break;
                        }
                    }
            }

            return STATUS_OK;
        }

        bool Grid::is_invisible_row(alloc_t *a, size_t row)
        {
            size_t off = row * a->nCols;
            for (size_t x=0; x < a->nCols; ++x, ++off)
            {
                cell_t *w = a->vTable.uget(off);
                if (w == NULL)
                    continue;

                // Is there a visible widget in a row?
                if ((w->pWidget != NULL) && (w->pWidget->visibility()->get()))
                {
                    // If this widget is present in the previous row or in the next row
                    // at the same column, then we can safely omit current row
                    bool shared = false;
                    if ((row > 0) && (a->vTable.uget(off - a->nCols) == w))
                        shared      = true;
                    else if ((row < (a->nRows - 1)) && (a->vTable.uget(off + a->nCols) == w))
                        shared      = true;

                    // We should consider current row not being empty if widget is not shared
                    // across neighbouring rows.
                    if (!shared)
                        return false;
                }
            }

            return true;
        }

        bool Grid::is_invisible_col(alloc_t *a, size_t col)
        {
            size_t off = col;
            for (size_t y=0; y < a->nRows; ++y, off += a->nCols)
            {
                cell_t *w = a->vTable.uget(off);
                if (w == NULL)
                    continue;

                // Is therer a visible widget in a column?
                if ((w->pWidget != NULL) && (w->pWidget->visibility()->get()))
                {
                    bool shared = false;

                    // If this widget is present in the previous column or in the next column
                    // at the same row, then we can safely omit current column
                    if ((col > 0) && (a->vTable.uget(off - 1) == w))
                        shared      = true;
                    else if ((col < (a->nCols - 1)) && (a->vTable.uget(off + 1) == w))
                        shared      = true;

                    // We should consider current column not being empty if widget is not shared
                    // across neighbouring columns.
                    if (!shared)
                        return false;
                }
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
            cell_t *c;
            ++a->nTag;
            size_t off = id*a->nCols;

            for (size_t i=0; i<a->nCols; ++i)
            {
                c           = a->vTable.uget(off + i);
                if ((c != NULL) && (c->nTag != a->nTag))
                {
                    c->nTag     = a->nTag;
                    --c->nRows;
                }
            }

            // Remove row in table and decrement overall number of rows
            a->vTable.remove_n(off, a->nCols);
            a->vRows.remove(id);
            --a->nRows;
        }

        void Grid::remove_col(alloc_t *a, size_t id)
        {
            // Eliminate the column and decrement number of cells used by column
            cell_t *c;
            ++a->nTag;

            for (size_t i=0, off=id; i < a->nRows; ++i, off += a->nCols-1)
            {
                c           = a->vTable.get(off);
                if ((c != NULL) && (c->nTag != a->nTag))
                {
                    c->nTag     = a->nTag;
                    --c->nCols;
                }
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
            float scaling   = lsp_max(0.0f, sScaling.get());
            size_t hspacing = lsp_max(0, scaling * sHSpacing.get());
            size_t vspacing = lsp_max(0, scaling * sVSpacing.get());

            // Initialize row and column descriptors
            for (size_t i=0; i<a->nRows; ++i)
            {
                h               = a->vRows.uget(i);
                h->nSize        = 0;
                h->nWeight      = 1;
                h->nSpacing     = vspacing;
                h->nFlags       = 0;
            }
            for (size_t i=0; i<a->nCols; ++i)
            {
                h               = a->vCols.uget(i);
                h->nSize        = 0;
                h->nWeight      = 1;
                h->nSpacing     = hspacing;
                h->nFlags       = 0;
            }

            // Remove empty rows and columns
            for (size_t y=0; y < a->nRows; )
            {
                if (row_equals(a, y, y+1))
                {
                    remove_row(a, y+1);
                    h               = a->vRows.uget(y);
                    ++h->nWeight;   // Increment weight of current row
                }
                else if (is_invisible_row(a, y))
                    remove_row(a, y);
                else
                    ++y;
            }
            for (size_t x=0; x < a->nCols; )
            {
                if (col_equals(a, x, x+1))
                {
                    remove_col(a, x+1);
                    h               = a->vCols.uget(x);
                    ++h->nWeight;   // Increment weight of current column
                }
                else if (is_invisible_col(a, x))
                    remove_col(a, x);
                else
                    ++x;
            }

            // Replace empty cells with stubs, change coordinates for
            // existing cells
            ++a->nTag;
            for (size_t y=0, off=0; (y < a->nRows); ++y)
            {
                cell_t *prev = NULL;
                for (size_t x=0; (x < a->nCols); ++x, ++off)
                {
                    cell_t **pcell  = a->vTable.upget(off);
                    cell_t *cell    = *pcell;
                    if (cell == NULL)
                    {
                        if (prev == NULL)
                        {
                            // Allocate cell
                            if ((prev = alloc_cell(&a->vCells)) == NULL)
                                return STATUS_NO_MEM;

                            prev->pWidget   = NULL;
                            prev->nLeft     = x;
                            prev->nTop      = y;
                            prev->nRows     = 1;
                            prev->nCols     = 0;
                            prev->nTag      = 0;
                        }

                        ++prev->nCols;
                        *pcell  = prev;
                    }
                    else
                    {
                        // Assign new coordinates to the cell
                        if (cell->nTag != a->nTag)
                        {
                            cell->nTag      = a->nTag;
                            cell->nLeft     = x;
                            cell->nTop      = y;
                        }

                        prev    = NULL;
                    }
                }
            }

            // Mark last row and last column as non-spacing
            if (a->nRows > 0)
            {
                h   = a->vRows.get(a->nRows - 1);
                h->nSpacing     = 0;
            }
            if (a->nCols > 0)
            {
                h   = a->vCols.get(a->nCols - 1);
                h->nSpacing     = 0;
            }

            // Initialize 'expand' and 'reduce' flags
            for (size_t i=0, n=a->vCells.size(); i<n; ++i)
            {
                cell_t *c       = a->vCells.uget(i);
                if (c->pWidget == NULL)
                    continue;

                // Estimate horizontal and vertical flags
                tk::Allocation *alloc = c->pWidget->allocation();
                size_t hflags = 0, vflags = 0;
                if (alloc->hexpand())
                    hflags     |= F_EXPAND;
                if (alloc->vexpand())
                    vflags     |= F_EXPAND;
                if (alloc->hreduce())
                    hflags     |= F_REDUCE;
                if (alloc->vreduce())
                    vflags     |= F_REDUCE;

                // Apply horizontal flags if present
                if (hflags)
                {
                    for (size_t j=0; j<c->nCols; ++j)
                    {
                        h               = a->vCols.uget(c->nLeft + j);
                        h->nFlags      |= hflags;
                    }
                }

                // Apply vertical flags if present
                if (vflags)
                {
                    for (size_t j=0; j<c->nRows; ++j)
                    {
                        h               = a->vRows.uget(c->nTop + j);
                        h->nFlags      |= vflags;
                    }
                }
            }

            return STATUS_OK;
        }

        size_t Grid::estimate_size(lltl::darray<header_t> *hdr, size_t first, size_t count)
        {
            size_t res      = 0;
            for (size_t i=0; i<count; ++i)
            {
                header_t *h     = hdr->uget(first);
                res            += h->nSize;
                if ((++first) < count)
                    res            += h->nSpacing;
            }
            return res;
        }

        void Grid::distribute_size(lltl::darray<header_t> *vh, size_t first, size_t count, size_t size)
        {
            // Check number of elements
            if (count <= 0)
                return;

            // Estimate number of expanded items, reduced items and overall weight
            size_t expanded = 0, reduced = 0, weight = 0, width = 0;
            for (size_t k=0; k<count; ++k)
            {
                header_t *h     = vh->uget(first + k);
                weight         += h->nSize * h->nWeight;
                width          += h->nSize;
                if ((k+1) < count)
                    width          += h->nSpacing;

                if (h->nFlags & F_REDUCE)
                    reduced ++;
                else if (h->nFlags & F_EXPAND)
                    expanded ++;
            }

            if (size <= width)
                return;
            size_t left = size - width;

            // Form list of items for size distribution excluding reduced (if possible)
            lltl::parray<header_t> vl;
            for (size_t k=0; k<count; ++k)
            {
                header_t *h     = vh->uget(first + k);
                if (expanded > 0)
                {
                    if ((!(h->nFlags & F_REDUCE)) && (h->nFlags & F_EXPAND))
                        vl.add(h);
                }
                else if (reduced >= count)
                    vl.add(h);
                else if (!(h->nFlags & F_REDUCE))
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
                    size_t delta    = (h->nSize * h->nWeight * left) / weight;
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

            // Estimate minimum row/column size for 1xN and Mx1 cells
            for (size_t i=0, n=a->vCells.size(); i<n; ++i)
            {
                cell_t *w       = a->vCells.uget(i);
                if ((w->pWidget == NULL) || (!w->pWidget->visibility()->get()))
                    continue;
                else if ((w->nRows != 1) && (w->nCols != 1))
                    continue;

                // Get size limits of the widget
                w->pWidget->get_padded_size_limits(&sr);

                if (w->nRows == 1)
                {
                    h               = a->vRows.uget(w->nTop);
                    h->nSize        = lsp_max(h->nSize, sr.nMinHeight);
                }
                if (w->nCols == 1)
                {
                    h               = a->vCols.uget(w->nLeft);
                    h->nSize        = lsp_max(h->nSize, sr.nMinWidth);
                }
            }

            // Estimate minimum row/column size for N x M cells
            for (size_t i=0, n=a->vCells.size(); i<n; ++i)
            {
                cell_t *w       = a->vCells.uget(i);
                if ((w->pWidget == NULL) || (!w->pWidget->visibility()->get()))
                    continue;
                if ((w->nRows <= 1) && (w->nCols <= 1))
                    continue;

                // Get size limits of the widget
                w->pWidget->get_padded_size_limits(&sr);

                if ((w->nRows > 1) && (sr.nMinHeight > 0))
                    distribute_size(&a->vRows, w->nTop,  w->nRows, sr.nMinHeight);

                if ((w->nCols > 1) && (sr.nMinWidth > 0))
                    distribute_size(&a->vCols, w->nLeft, w->nCols, sr.nMinWidth);
            }

            return STATUS_OK;
        }

        status_t Grid::allocate_cells(alloc_t *a)
        {
            // Attach cells
            status_t res    = attach_cells(a);
            if ((res != STATUS_OK) || (a->nRows < 1) || (a->nCols < 1))
                return res;

            // Estimate row and column parameters
            if ((res = create_row_col_descriptors(a)) != STATUS_OK)
                return res;

            // Estimate cell sizes
            if ((res = estimate_sizes(a)) != STATUS_OK)
                return res;

            return STATUS_OK;
        }

        void Grid::assign_coords(alloc_t *a, const ws::rectangle_t *r)
        {
            ssize_t y       = r->nTop;
            ++a->nTag;

            for (size_t i=0, off=0, n=a->vRows.size(); i<n; ++i)
            {
                header_t *vr    = a->vRows.uget(i);
                ssize_t x       = r->nLeft;

                for (size_t j=0, m=a->vCols.size(); j<m; ++j, ++off)
                {
                    header_t *hr    = a->vCols.uget(j);
                    cell_t *c       = a->vTable.uget(off);

                    // Allocate initial coordinates of the cell
                    if (c->nTag != a->nTag)
                    {
                        c->a.nLeft      = x;
                        c->a.nTop       = y;
                        c->a.nWidth     = 0;
                        c->a.nHeight    = 0;
                        c->nTag         = a->nTag;
                    }

                    // Add horizontal space to cell
                    if (c->nTop == i)
                    {
                        c->a.nWidth    += hr->nSize;
                        if (j < (c->nLeft + c->nCols - 1))
                            c->a.nWidth    += hr->nSpacing;
                    }

                    // Add vertical space to cell
                    if (c->nLeft == j)
                    {
                        c->a.nHeight   += vr->nSize;
                        if (i < (c->nTop + c->nRows - 1))
                            c->a.nHeight   += vr->nSpacing;
                    }

                    // Update X coordinate
                    x              += hr->nSize + hr->nSpacing;
                }

                // Update Y coordinate
                y              += vr->nSize + vr->nSpacing;
            }
        }

        void Grid::realize_children(alloc_t *a)
        {
            ws::size_limit_t sr;
            ws::rectangle_t r;

            for (size_t i=0, n=a->vTable.size(); i<n; ++i)
            {
                // Get widget
                cell_t *w       = a->vTable.uget(i);
                if ((w->pWidget == NULL) || (!w->pWidget->visibility()->get()))
                    continue;

                // Allocated widget area may be too large, restrict it with size constraints
                w->pWidget->get_padded_size_limits(&sr);
                SizeConstraints::apply(&r, &w->a, &sr);

                // Estimate the real widget allocation size
                ssize_t xw      = (w->pWidget->allocation()->hfill()) ? r.nWidth    : lsp_max(0, sr.nMinWidth);
                ssize_t xh      = (w->pWidget->allocation()->vfill()) ? r.nHeight   : lsp_max(0, sr.nMinHeight);

                // Update location of the widget
                w->s.nLeft      = w->a.nLeft + (lsp_max(0, w->a.nWidth  - xw) >> 1);
                w->s.nTop       = w->a.nTop  + (lsp_max(0, w->a.nHeight - xh) >> 1);
                w->s.nWidth     = xw;
                w->s.nHeight    = xh;
                w->pWidget->padding()->enter(&w->s, w->pWidget->scaling()->get());

                // Realize the widget
//                lsp_trace("realize widget %p, id=%d, allocation = {%d, %d, %d, %d}, size = {%d, %d, %d, %d}",
//                        w->pWidget, int(i),
//                        int(w->a.nLeft), int(w->a.nTop), int(w->a.nWidth), int(w->a.nHeight),
//                        int(w->s.nLeft), int(w->s.nTop), int(w->s.nWidth), int(w->s.nHeight)
//                );
                w->pWidget->realize_widget(&w->s);
            }
        }
    
    } /* namespace tk */
} /* namespace lsp */
