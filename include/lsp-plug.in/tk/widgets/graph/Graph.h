/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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

        /**
         * Graph for drawing 2D charts
         */
        class Graph: public WidgetContainer
        {
            public:
                static const w_class_t    metadata;

            private:
                Graph & operator    = (const Graph &);

            protected:
                prop::WidgetList<GraphItem>     vItems;         // Overall list of graph items
                lltl::parray<GraphAxis>         vAxis;          // List of all axes
                lltl::parray<GraphAxis>         vBasis;         // List of basises
                lltl::parray<GraphOrigin>       vOrigins;       // List of origins
                prop::CollectionListener        sIListener;     // Listener to trigger vItems content change

                prop::SizeConstraints           sConstraints;   // Size constraints
                prop::Integer                   sBorder;        // Border size
                prop::Integer                   sBorderRadius;  // Border radius
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

                static void                 on_add_item(void *obj, Property *prop, Widget *w);
                static void                 on_remove_item(void *obj, Property *prop, Widget *w);

            protected:
                virtual Widget             *find_widget(ssize_t x, ssize_t y);
                virtual void                size_request(ws::size_limit_t *r);
                virtual void                property_changed(Property *prop);
                virtual void                realize(const ws::rectangle_t *r);

                void                        sync_lists();

            public:
                explicit Graph(Display *dpy);
                virtual ~Graph();

            public:
                virtual status_t            init();
                virtual void                destroy();

            public:
                LSP_TK_PROPERTY(WidgetList<GraphItem>,      items,              &vItems);
                LSP_TK_PROPERTY(SizeConstraints,            constraints,        &sConstraints);
                LSP_TK_PROPERTY(Integer,                    border_size,        &sBorder);
                LSP_TK_PROPERTY(Integer,                    border_radius,      &sBorderRadius);
                LSP_TK_PROPERTY(Boolean,                    glass,              &sGlass);
                LSP_TK_PROPERTY(Color,                      color,              &sColor);
                LSP_TK_PROPERTY(Color,                      border_color,       &sBorderColor);
                LSP_TK_PROPERTY(Color,                      glass_color,        &sGlassColor);
                LSP_TK_PROPERTY(Padding,                    internal_padding,   &sIPadding);

            public:
                /**
                 * Get axis by index, valid only at render time
                 *
                 * @param index index of the axis
                 * @return axis or NULL
                 */
                GraphAxis                  *axis(size_t index)          { return vAxis.get(index);      }

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

                bool                        origin(size_t index, float *x, float *y);
                bool                        origin(GraphOrigin *o, float *x, float *y);

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

            public:
                virtual status_t            add(Widget *child);

                virtual status_t            remove(Widget *child);

                virtual status_t            remove_all();

                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force);

                virtual void                draw(ws::ISurface *s);
        };
    }
}


#endif /* LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPH_H_ */
