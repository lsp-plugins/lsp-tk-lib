/*
 * GraphText.h
 *
 *  Created on: 20 авг. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHTEXT_H_
#define LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHTEXT_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class GraphText: public GraphItem
        {
            public:
                static const w_class_t    metadata;

            private:
                GraphText & operator = (const GraphText &);

            protected:
                virtual void                property_changed(Property *prop);

            public:
                explicit GraphText(Display *dpy);
                virtual ~GraphText();

                virtual status_t            init();
                virtual void                destroy();

            public:
                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force);
        };
    }
}


#endif /* LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHTEXT_H_ */
