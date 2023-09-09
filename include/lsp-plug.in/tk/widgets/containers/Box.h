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
        // Style definition
        namespace style
        {
            LSP_TK_STYLE_DEF_BEGIN(Box, WidgetContainer)
                prop::Integer               sSpacing;
                prop::Integer               sBorder;            // Border size
                prop::Boolean               sHomogeneous;
                prop::Orientation           sOrientation;
                prop::SizeConstraints       sConstraints;       // Size constraints
                prop::Color                 sBorderColor;       // Border color
                prop::Boolean               sSolid;             // Solid state: acts as a widget
            LSP_TK_STYLE_DEF_END
        }

        /**
         * Implements widget container that allows to pack all elements
         * horizontally or vertically depending on settings
         */
        class Box: public WidgetContainer
        {
            public:
                static const w_class_t    metadata;

            protected:
                typedef struct cell_t
                {
                    ws::rectangle_t     a;          // Allocated space for widget
                    ws::rectangle_t     s;          // Really used space by widget
                    Widget             *pWidget;    // Widget contained in the cell
                } cell_t;

                enum state_t
                {
                    F_MOUSE_IN      = 1 << 0,
                    F_MOUSE_DOWN    = 1 << 1,
                    F_MOUSE_IGN     = 1 << 2,
                };

            protected:
                size_t                      nMFlags;
                size_t                      nState;

                lltl::darray<cell_t>        vVisible;
                prop::WidgetList<Widget>    vItems;
                prop::CollectionListener    sIListener;         // Listener to trigger vItems content change

                prop::Integer               sSpacing;
                prop::Integer               sBorder;            // Border size
                prop::Boolean               sHomogeneous;
                prop::Orientation           sOrientation;
                prop::SizeConstraints       sConstraints;       // Size constraints
                prop::Color                 sBorderColor;       // Border color
                prop::Boolean               sSolid;             // Solid state: acts as a widget

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
                static status_t             slot_on_submit(Widget *sender, void *ptr, void *data);

            protected:
                virtual void                size_request(ws::size_limit_t *r) override;
                virtual void                property_changed(Property *prop) override;
                virtual void                realize(const ws::rectangle_t *r) override;

            public:
                explicit Box(Display *dpy);
                Box(const Box &) = delete;
                Box(Box &&) = delete;
                virtual ~Box();

                Box & operator = (const Box &) = delete;
                Box & operator = (Box &&) = delete;

                virtual status_t            init() override;
                virtual void                destroy() override;

            //---------------------------------------------------------------------------------
            // Properties
            public:
                /**
                 * Spacing between widget allocations
                 * @return spacing property
                 */
                LSP_TK_PROPERTY(Integer,            spacing,            &sSpacing)

                /**
                 * The size of the border around the box
                 * @return size of border around the box
                 */
                LSP_TK_PROPERTY(Integer,            border,             &sBorder)

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
                 * Get the color of the surrounding border
                 * @return color of the surrounding border
                 */
                LSP_TK_PROPERTY(Color,              border_color,       &sBorderColor)

                /**
                 * Get the solid flag of the container. If container is solid, it acts as an widget
                 */
                LSP_TK_PROPERTY(Boolean,            solid,              &sSolid)

                /**
                 * Get collection of widgets
                 * @return collection of widgets
                 */
                LSP_TK_PROPERTY(WidgetList<Widget>, items,              &vItems)

            //---------------------------------------------------------------------------------
            // Manipulation
            public:
                virtual Widget             *find_widget(ssize_t x, ssize_t y) override;

                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force) override;

                virtual status_t            add(Widget *widget) override;
                virtual status_t            remove(Widget *child) override;
                virtual status_t            remove_all() override;

                virtual status_t            on_mouse_in(const ws::event_t *e) override;
                virtual status_t            on_mouse_out(const ws::event_t *e) override;
                virtual status_t            on_mouse_move(const ws::event_t *e) override;
                virtual status_t            on_mouse_down(const ws::event_t *e) override;
                virtual status_t            on_mouse_up(const ws::event_t *e) override;

                virtual status_t            on_submit();

        };
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_BOX_H_ */
