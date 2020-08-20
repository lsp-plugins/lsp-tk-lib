/*
 * Graph.h
 *
 *  Created on: 20 авг. 2020 г.
 *      Author: sadko
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

                ws::ISurface                   *pGlass;         // Cached glass gradient
                ws::rectangle_t                 sCanvas;        // Actual dimensions of the drawing area

            protected:
                void                        do_destroy();

                static void                 on_add_item(void *obj, Property *prop, Widget *w);
                static void                 on_remove_item(void *obj, Property *prop, Widget *w);

            protected:
                virtual Widget             *find_widget(ssize_t x, ssize_t y);
                virtual void                size_request(ws::size_limit_t *r);
                virtual void                property_changed(Property *prop);
                virtual void                realize(const ws::rectangle_t *r);

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

            public:
                virtual status_t            add(Widget *child);

                virtual status_t            remove(Widget *child);

                virtual status_t            remove_all();
        };
    }
}


#endif /* LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPH_H_ */
