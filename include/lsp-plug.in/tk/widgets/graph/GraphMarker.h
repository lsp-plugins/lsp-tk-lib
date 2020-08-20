/*
 * GraphMarker.h
 *
 *  Created on: 20 авг. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHMARKER_H_
#define LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHMARKER_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class GraphMarker: public GraphItem
        {
            public:
                static const w_class_t    metadata;

            private:
                GraphMarker & operator = (const GraphMarker &);

            protected:
                virtual void                property_changed(Property *prop);

            public:
                explicit GraphMarker(Display *dpy);
                virtual ~GraphMarker();

                virtual status_t            init();
                virtual void                destroy();

            public:
                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force);
        };
    }
}


#endif /* LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHMARKER_H_ */
