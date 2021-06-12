/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_GRID_H_
#define LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_GRID_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        // Style definition
        namespace style
        {
            LSP_TK_STYLE_DEF_BEGIN(Grid, WidgetContainer)
                prop::Integer               sRows;
                prop::Integer               sColumns;
                prop::Integer               sHSpacing;
                prop::Integer               sVSpacing;
                prop::Orientation           sOrientation;
                prop::SizeConstraints       sConstraints;
            LSP_TK_STYLE_DEF_END
        }

        /**
         * Grid widget
         */
        class Grid: public WidgetContainer
        {
            private:
                Grid & operator = (const Grid &);
                Grid(const Grid &);

            public:
                static const w_class_t    metadata;

            protected:
                enum flags_t
                {
                    F_EXPAND        = 1 << 0,       // Widget in the cell has 'expand' flag
                };

                typedef struct cell_t
                {
                    ws::rectangle_t     a;          // Allocated area for the cell
                    ws::rectangle_t     s;          // Real area used by widget
                    Widget             *pWidget;    // Widget
                    size_t              nLeft;      // Horizontal position
                    size_t              nTop;       // Vertical position
                    size_t              nRows;      // Number of rows taken by cell
                    size_t              nCols;      // Number of columns taken by cell
                    size_t              nTag;       // Tag
                } cell_t;

                typedef struct header_t
                {
                    ssize_t             nSize;      // Size of the header
                    size_t              nWeight;    // Weight of the header
                    size_t              nSpacing;   // Additional spacing
                    size_t              nFlags;     // Additional flags
                } header_t;

                typedef struct widget_t
                {
                    Widget             *pWidget;    // Pointer to widget
                    ssize_t             nLeft;      // Attached left position, negative for add()
                    ssize_t             nTop;       // Attached top position, negative for add()
                    size_t              nRows;      // Number of rows taken by widget, should be positive
                    size_t              nCols;      // Number of columns taken by widget, should be positive
                } widget_t;

                typedef struct alloc_t
                {
                    lltl::darray<cell_t>    vCells;
                    lltl::parray<cell_t>    vTable;
                    lltl::darray<header_t>  vRows;
                    lltl::darray<header_t>  vCols;
                    size_t                  nRows;
                    size_t                  nCols;
                    size_t                  nTag;
                } alloc_t;

            protected:
                lltl::darray<widget_t>      vItems;     // All list of items
                alloc_t                     sAlloc;     // Allocation

                prop::Integer               sRows;
                prop::Integer               sColumns;
                prop::Integer               sHSpacing;
                prop::Integer               sVSpacing;
                prop::Orientation           sOrientation;
                prop::SizeConstraints       sConstraints;

            protected:
                void                        do_destroy();
                static inline bool          hidden_widget(const widget_t *w);
                status_t                    allocate_cells(alloc_t *a);
                status_t                    attach_cells(alloc_t *a);
                static bool                 attach_cell(alloc_t *a, widget_t *w, size_t left, size_t top);
                static bool                 is_invisible_row(alloc_t *a, size_t row);
                static bool                 is_invisible_col(alloc_t *a, size_t col);
                static bool                 row_equals(alloc_t *a, size_t r1, size_t r2);
                static bool                 col_equals(alloc_t *a, size_t c1, size_t c2);
                static void                 remove_row(alloc_t *a, size_t id);
                static void                 remove_col(alloc_t *a, size_t id);
                static size_t               estimate_size(lltl::darray<header_t> *hdr, size_t first, size_t count);
                static void                 distribute_size(lltl::darray<header_t> *vh, size_t first, size_t count, size_t size);
                status_t                    estimate_sizes(alloc_t *a);
                status_t                    create_row_col_descriptors(alloc_t *a);
                static void                 assign_coords(alloc_t *a, const ws::rectangle_t *r);
                static void                 realize_children(alloc_t *a);
                status_t                    attach_internal(ssize_t left, ssize_t top, Widget *widget, size_t rows, size_t cols);

            protected:
                virtual Widget             *find_widget(ssize_t x, ssize_t y);
                virtual void                size_request(ws::size_limit_t *r);
                virtual void                property_changed(Property *prop);
                virtual void                realize(const ws::rectangle_t *r);

            public:
                explicit Grid(Display *dpy);
                virtual ~Grid();

                virtual status_t            init();
                virtual void                destroy();

            //---------------------------------------------------------------------------------
            // Properties
            public:
                /**
                 * Get number of rows
                 * @return number of rows
                 */
                LSP_TK_PROPERTY(Integer,            rows,               &sRows)

                /**
                 * Get number of columns
                 * @return number of columns
                 */
                LSP_TK_PROPERTY(Integer,            columns,            &sColumns)

                /**
                 * Get grid orientation: if orientation is horizontal, widgets are allocated
                 * from left to right. Otherwise, widgets are allocated from top to bottom
                 * @return orientation
                 */
                LSP_TK_PROPERTY(Orientation,        orientation,        &sOrientation)

                /**
                 * Get horizontal spacing between cells
                 *
                 * @return horizontal spacing between cells
                 */
                LSP_TK_PROPERTY(Integer,            hspacing,           &sHSpacing)

                /**
                 * Get vertical spacing between cells
                 *
                 * @return vertical spacing between cells
                 */
                LSP_TK_PROPERTY(Integer,            vspacing,           &sVSpacing)

                /**
                 * Get size constraints
                 *
                 * @return size constraings
                 */
                LSP_TK_PROPERTY(SizeConstraints,    constraints,        &sConstraints)

            //---------------------------------------------------------------------------------
            // Manipulation
            public:
                /** Render widget
                 *
                 * @param s surface to render
                 * @param force force flag
                 */
                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force);

                /** Add widget to the grid
                 *
                 * @param widget widget to add
                 * @return status of operation
                 */
                virtual status_t            add(Widget *widget);

                /** Add widget to the grid
                 *
                 * @param widget widget to add
                 * @param rows number of rows taken by widget
                 * @param cols number of columns taken by widget
                 * @return status of operation
                 */
                virtual status_t            add(Widget *widget, size_t rows, size_t cols);

                /**
                 * Attach widget to the grid
                 * @param left left coordinate of the widget
                 * @param top top coordinate of the widget
                 * @param widget widget to attach
                 * @return status of operation
                 */
                virtual status_t            attach(size_t left, size_t top, Widget *widget);

                /**
                 * Attach widget to the grid
                 * @param left left coordinate of the widget
                 * @param top top coordinate of the widget
                 * @param rows number of rows taken by widget
                 * @param cols number of columns taken by widget
                 * @return status of operation
                 */
                virtual status_t            attach(size_t left, size_t top, Widget *widget, size_t rows, size_t cols);

                /** Remove widget from grid
                 *
                 * @param widget widget to remove
                 * @return status of operation
                 */
                virtual status_t            remove(Widget *widget);
        };
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_GRID_H_ */
