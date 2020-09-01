/*
 * GraphMesh.h
 *
 *  Created on: 20 авг. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHMESH_H_
#define LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHMESH_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class GraphMesh: public GraphItem
        {
            public:
                static const w_class_t    metadata;

            private:
                GraphMesh & operator = (const GraphMesh &);

            protected:
                prop::Integer               sOrigin;        // Index of origin
                prop::Integer               sXAxis;         // Index of X axis
                prop::Integer               sYAxis;         // Index of Y axis
                prop::Integer               sWidth;         // Width of the mesh line
                prop::Boolean               sFill;          // Fill poly
                prop::Color                 sColor;         // Mesh color
                prop::Color                 sFillColor;     // Fill color
                prop::GraphMeshData         sData;          // Graph mesh data

            protected:
                virtual void                property_changed(Property *prop);

                void                        do_destroy();

            public:
                explicit GraphMesh(Display *dpy);
                virtual ~GraphMesh();

                virtual status_t            init();
                virtual void                destroy();

            public:
                LSP_TK_PROPERTY(Integer,            origin,                     &sOrigin)
                LSP_TK_PROPERTY(Integer,            haxis,                      &sXAxis)
                LSP_TK_PROPERTY(Integer,            vaxis,                      &sYAxis)
                LSP_TK_PROPERTY(Integer,            width,                      &sWidth)
                LSP_TK_PROPERTY(Boolean,            fill,                       &sFill)
                LSP_TK_PROPERTY(Color,              color,                      &sColor)
                LSP_TK_PROPERTY(Color,              fill_color,                 &sFillColor)
                LSP_TK_PROPERTY(GraphMeshData,      data,                       &sData)

            public:
                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force);
        };
    }
}


#endif /* LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHMESH_H_ */
