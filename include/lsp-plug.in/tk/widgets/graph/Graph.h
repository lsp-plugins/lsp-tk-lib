/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 20 авг. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPH_H_
#define LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPH_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class GraphItem;
        class GraphAxis;
        class GraphOrigin;

        // Style definition
        namespace style
        {
            LSP_TK_STYLE_DEF_BEGIN(Graph, WidgetContainer)
                prop::SizeConstraints           sConstraints;   // Size constraints
                prop::Integer                   sBorder;        // Border size
                prop::Integer                   sBorderRadius;  // Border radius
                prop::Boolean                   sBorderFlat;    // Border is flat
                prop::Boolean                   sGlass;         // Draw glass
                prop::Color                     sColor;         // Graph color
                prop::Color                     sBorderColor;   // Color of the border
                prop::Color                     sGlassColor;    // Color of the glass
                prop::Padding                   sIPadding;      // Internal padding
            LSP_TK_STYLE_DEF_END
        }

        /**
         * Graph for drawing 2D charts
         */
        class Graph: public WidgetContainer
        {
            public:
                static const w_class_t    metadata;

            protected:
                typedef struct w_alloc_t
                {
                    ws::rectangle_t     sRect;
                    ssize_t             nGroup;
                    ssize_t             nPriority;
                    GraphItem          *pWidget;
                } w_alloc_t;

            protected:
                prop::WidgetList<GraphItem>     vItems;         // Overall list of graph items
                lltl::parray<GraphAxis>         vAxis;          // List of all axes
                lltl::parray<GraphAxis>         vBasis;         // List of basises
                lltl::parray<GraphOrigin>       vOrigins;       // List of origins
                prop::CollectionListener        sIListener;     // Listener to trigger vItems content change

                prop::SizeConstraints           sConstraints;   // Size constraints
                prop::Integer                   sBorder;        // Border size
                prop::Integer                   sBorderRadius;  // Border radius
                prop::Boolean                   sBorderFlat;    // Border is flat
                prop::Boolean                   sGlass;         // Draw glass
                prop::Color                     sColor;         // Graph color
                prop::Color                     sBorderColor;   // Color of the border
                prop::Color                     sGlassColor;    // Color of the glass
                prop::Padding                   sIPadding;      // Internal padding

                ws::ISurface                   *pGlass;         // Cached glass gradient
                ws::rectangle_t                 sCanvas;        // Actual dimensions of the drawing area (with padding)
                ws::rectangle_t                 sICanvas;       // Actual dimensions of the drawing area (without padding)

            protected:
                void                        do_destroy();

                static void                 on_add_item(void *obj, Property *prop, void *w);
                static void                 on_remove_item(void *obj, Property *prop, void *w);

                static ssize_t              check_collision(const w_alloc_t *a, const w_alloc_t *b);
                static ssize_t              compare_walloc(const w_alloc_t *a, const w_alloc_t *b);

            protected:
                virtual Widget             *find_widget(ssize_t x, ssize_t y) override;
                virtual void                size_request(ws::size_limit_t *r) override;
                virtual void                property_changed(Property *prop) override;
                virtual void                realize(const ws::rectangle_t *r) override;
                virtual void                hide_widget() override;

                void                        sync_lists();
                void                        drop_glass();

            public:
                explicit Graph(Display *dpy);
                Graph(const Graph &) = delete;
                Graph(Graph &&) = delete;
                virtual ~Graph();

                Graph & operator = (const Graph &) = delete;
                Graph & operator = (Graph &&) = delete;

            public:
                virtual status_t            init() override;
                virtual void                destroy() override;

            public:
                LSP_TK_PROPERTY(WidgetList<GraphItem>,      items,              &vItems);
                LSP_TK_PROPERTY(SizeConstraints,            constraints,        &sConstraints);
                LSP_TK_PROPERTY(Integer,                    border_size,        &sBorder);
                LSP_TK_PROPERTY(Integer,                    border_radius,      &sBorderRadius);
                LSP_TK_PROPERTY(Boolean,                    border_flat,        &sBorderFlat);
                LSP_TK_PROPERTY(Boolean,                    glass,              &sGlass);
                LSP_TK_PROPERTY(Color,                      color,              &sColor);
                LSP_TK_PROPERTY(Color,                      border_color,       &sBorderColor);
                LSP_TK_PROPERTY(Color,                      glass_color,        &sGlassColor);
                LSP_TK_PROPERTY(Padding,                    ipadding,           &sIPadding);

            public:
                /**
                 * Get axis by index, valid only at render time
                 *
                 * @param index index of the axis
                 * @return axis or NULL
                 */
                GraphAxis                  *axis(size_t index)          { return vAxis.get(index);      }

                /**
                 * Get index of specific axis
                 * @param axis index of specific axis
                 * @return index of specific axis
                 */
                ssize_t                     indexof_axis(const GraphAxis *axis) { return vAxis.index_of(axis);      }

                /**
                 * Get basis by index, valid only at render time
                 *
                 * @param index index of the basis
                 * @return basis or NULL
                 */
                GraphAxis                  *basis(size_t index)         { return vBasis.get(index);     }

                /**
                 * Get origin by index, valid only at render time
                 *
                 * @param index index of the origin
                 * @return origin or NULL
                 */
                GraphOrigin                *origin(size_t index)        { return vOrigins.get(index);   }

                /**
                 * Get index of specific origin
                 * @param axis index of specific origin
                 * @return index of specific origin
                 */
                ssize_t                     indexof_origin(const GraphOrigin *origin) { return vOrigins.index_of(origin);       }

                bool                        origin(size_t index, float *x, float *y);
                bool                        origin(GraphOrigin *o, float *x, float *y);

                void                        canvas_size(ws::rectangle_t *r);
                inline ssize_t              canvas_left() const         { return sICanvas.nLeft;                    }
                inline ssize_t              canvas_top() const          { return sICanvas.nTop;                     }
                inline ssize_t              canvas_width() const        { return sICanvas.nWidth;                   }
                inline ssize_t              canvas_height() const       { return sICanvas.nHeight;                  }
                inline ssize_t              canvas_right() const        { return sICanvas.nLeft + sICanvas.nWidth;  }
                inline ssize_t              canvas_bottom() const       { return sICanvas.nTop + sICanvas.nHeight;  }

                inline ssize_t              canvas_aleft() const        { return sCanvas.nLeft + sICanvas.nLeft;                    }
                inline ssize_t              canvas_atop() const         { return sCanvas.nTop + sICanvas.nTop;                      }
                inline ssize_t              canvas_aright() const       { return sCanvas.nLeft + sICanvas.nLeft + sICanvas.nWidth;  }
                inline ssize_t              canvas_abottom() const      { return sCanvas.nTop + sICanvas.nTop + sICanvas.nHeight;   }


                /**
                 * Project window coordinates X and Y on the specified axis and return the corresponding value
                 * @param index axis index to perform projection
                 * @param out pointer to store coodrinate of point on the axis
                 * @param in input value to process
                 * @return status of operation
                 */
                status_t                    xy_to_axis(size_t index, float *out, ssize_t x, ssize_t y);

                /**
                 * Get the point on axis and return it's window coordinate X
                 * @param index axist to perform transformation
                 * @param x pointer to store output window X coordinate
                 * @param y pointer to store output window Y coordinate
                 * @param in input value to process
                 * @return status of operation
                 */
                status_t                    axis_to_xy(size_t index, ssize_t *x, ssize_t *y, float value);

            public:
                virtual status_t            add(Widget *child) override;

                virtual status_t            remove(Widget *child) override;

                virtual status_t            remove_all() override;

                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force) override;

                virtual void                draw(ws::ISurface *s) override;
        };

    } /* namespace tk */
} /* namespace lsp */


#endif /* LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPH_H_ */
