/*
 * Graph.cpp
 *
 *  Created on: 20 авг. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t Graph::metadata         =   { "Graph", &WidgetContainer::metadata };

        Graph::Graph(Display *dpy):
            WidgetContainer(dpy),
            vItems(&sProperties, &sIListener)
        {
            pClass              = &metadata;
        }

        Graph::~Graph()
        {
            do_destroy();
        }

        void Graph::destroy()
        {
            WidgetContainer::destroy();
            do_destroy();
        }

        void Graph::do_destroy()
        {
            // Unlink all items
            size_t n            = vItems.size();
            for (size_t i=0; i<n; ++i)
            {
                GraphItem *item     = vItems.uget(i);
                if (item == NULL)
                    continue;

                unlink_widget(item);
            }

            vItems.flush();
        }

        status_t Graph::init()
        {
            status_t result = WidgetContainer::init();
            if (result != STATUS_OK)
                return result;

            // Init listener
            sIListener.bind_all(this, on_add_item, on_remove_item);

            // Init style
            // TODO

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                // TODO
            }

            return STATUS_OK;
        }

        void Graph::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);
            if (vItems.is(prop))
                query_draw();
        }

        void Graph::on_add_item(void *obj, Property *prop, Widget *w)
        {
            GraphItem *item = widget_cast<GraphItem>(w);
            if (item == NULL)
                return;

            Graph *_this = widget_ptrcast<Graph>(obj);
            if (_this == NULL)
                return;

            // TODO
        }

        void Graph::on_remove_item(void *obj, Property *prop, Widget *w)
        {
            GraphItem *item = widget_cast<GraphItem>(w);
            if (item == NULL)
                return;

            Graph *_this = widget_ptrcast<Graph>(obj);
            if (_this == NULL)
                return;

            // TODO
        }

        status_t Graph::add(Widget *child)
        {
            GraphItem *item     = widget_cast<GraphItem>(child);
            return (item != NULL) ? vItems.add(item) : STATUS_BAD_TYPE;
        }

        status_t Graph::remove(Widget *child)
        {
            GraphItem *item     = widget_cast<GraphItem>(child);
            return (item != NULL) ? vItems.premove(item) : STATUS_BAD_TYPE;
        }

        status_t Graph::remove_all()
        {
            vItems.clear();
            return STATUS_OK;
        }
    }
}
