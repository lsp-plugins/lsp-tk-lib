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

#ifndef LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_BOX_H_
#define LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_BOX_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Implements widget container that allows to pack all elements
         * horizontally or vertically depending on settings
         */
        class Box: public WidgetContainer
        {
            private:
                Box & operator = (const Box &);

            public:
                static const w_class_t    metadata;

            protected:
                typedef struct cell_t
                {
                    ws::rectangle_t     a;          // Allocated space for widget
                    ws::rectangle_t     s;          // Really used space by widget
                    Widget             *pWidget;    // Widget contained in the cell
                } cell_t;

            protected:
                lltl::darray<cell_t>        vVisible;
                prop::WidgetList<Widget>    vItems;
                prop::CollectionListener    sIListener;         // Listener to trigger vItems content change

                prop::Integer               sSpacing;
                prop::Boolean               sHomogeneous;
                prop::Orientation           sOrientation;
                prop::SizeConstraints       sConstraints;        // Size constraints

            protected:
                status_t                    visible_items(lltl::darray<cell_t> *out);
                void                        do_destroy();

                static void                 on_add_item(void *obj, Property *prop, void *w);
                static void                 on_remove_item(void *obj, Property *prop, void *w);

                status_t                    allocate_homogeneous(const ws::rectangle_t *r, lltl::darray<cell_t> &visible);
                status_t                    allocate_proportional(const ws::rectangle_t *r, lltl::darray<cell_t> &visible);
                void                        allocate_widget_space(const ws::rectangle_t *r, lltl::darray<cell_t> &visible, ssize_t spacing);
                void                        realize_children(lltl::darray<cell_t> &visible);

            protected:
                virtual void                size_request(ws::size_limit_t *r);
                virtual void                property_changed(Property *prop);
                virtual void                realize(const ws::rectangle_t *r);

            public:
                explicit Box(Display *dpy);
                virtual ~Box();

                virtual status_t            init();
                virtual void                destroy();

            //---------------------------------------------------------------------------------
            // Properties
            public:
                /**
                 * Spacing between widget allocations
                 * @return spacing property
                 */
                LSP_TK_PROPERTY(Integer,            spacing,            &sSpacing)

                /**
                 * Get proportional flag
                 * @return proportional flag property
                 */
                LSP_TK_PROPERTY(Boolean,            homogeneous,        &sHomogeneous)

                /**
                 * Get box orientation
                 * @return box orientation
                 */
                LSP_TK_PROPERTY(Orientation,        orientation,        &sOrientation)

                /**
                 * Get size constraints of the box
                 * @return size constraints of the box
                 */
                LSP_TK_PROPERTY(SizeConstraints,    constraints,        &sConstraints)

                /**
                 * Get collection of widgets
                 * @return collection of widgets
                 */
                LSP_TK_PROPERTY(WidgetList<Widget>, items,              &vItems)

            //---------------------------------------------------------------------------------
            // Manipulation
            public:
                virtual Widget             *find_widget(ssize_t x, ssize_t y);

                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force);

                virtual status_t            add(Widget *widget);

                virtual status_t            remove(Widget *child);

                virtual status_t            remove_all();

        };
    
        STYLE_INITIALIZER_DEF(Box, WidgetContainer);
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_BOX_H_ */
