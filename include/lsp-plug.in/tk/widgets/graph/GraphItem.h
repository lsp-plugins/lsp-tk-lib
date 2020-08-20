/*
 * GraphItem.h
 *
 *  Created on: 20 авг. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHITEM_H_
#define LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHITEM_H_

namespace lsp
{
    namespace tk
    {
        class Graph;

        class GraphItem: public Widget
        {
            public:
                static const w_class_t    metadata;

            private:
                GraphItem & operator = (const GraphItem &);

            protected:
                prop::Boolean       sSmooth;

            protected:
                virtual void            property_changed(Property *prop);

            public:
                explicit GraphItem(Display *dpy);
                virtual ~GraphItem();

            public:
                virtual status_t            init();
                virtual void                destroy();

            public:
                LSP_TK_PROPERTY(Boolean,        smooth,         &sSmooth);

            public:
                /**
                 * Get the pointer to parent graph
                 * @return pointer to parent graph or NULL
                 */
                Graph              *graph();

                /**
                 * Check whether mouse pointer is inside of the graph item
                 * @param x horizontal position of mouse pointer
                 * @param y vertical position of mouse pointer
                 * @return true if item is inside of the graph
                 */
                virtual bool        inside(ssize_t x, ssize_t y);
        };
    }
}

#endif /* LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHITEM_H_ */
