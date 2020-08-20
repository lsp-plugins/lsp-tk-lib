/*
 * Graph.h
 *
 *  Created on: 20 авг. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPH_H_
#define LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPH_H_

namespace lsp
{
    namespace tk
    {
        class Graph: public WidgetContainer
        {
            public:
                static const w_class_t    metadata;

            private:
                Graph & operator    = (const Graph &);

            protected:
                prop::WidgetList<GraphItem>     vItems;
                prop::CollectionListener        sIListener;

            protected:
                void                        do_destroy();

                static void                 on_add_item(void *obj, Property *prop, Widget *w);
                static void                 on_remove_item(void *obj, Property *prop, Widget *w);

            protected:
                virtual void                property_changed(Property *prop);

            public:
                explicit Graph(Display *dpy);
                virtual ~Graph();

            public:
                virtual status_t            init();
                virtual void                destroy();

            public:
                LSP_TK_PROPERTY(WidgetList<GraphItem>,      items,      &vItems);

            public:
                virtual status_t            add(Widget *child);

                virtual status_t            remove(Widget *child);

                virtual status_t            remove_all();
        };
    }
}


#endif /* LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPH_H_ */
