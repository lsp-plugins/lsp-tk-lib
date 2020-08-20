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
            vItems(&sProperties, &sIListener),
            sConstraints(&sProperties),
            sBorder(&sProperties),
            sBorderRadius(&sProperties),
            sGlass(&sProperties),
            sColor(&sProperties),
            sBorderColor(&sProperties),
            sGlassColor(&sProperties)
        {
            pGlass              = NULL;
            sCanvas.nLeft       = 0;
            sCanvas.nTop        = 0;
            sCanvas.nWidth      = 0;
            sCanvas.nHeight     = 0;

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
            sConstraints.bind("size.constraints", &sStyle);
            sBorder.bind("border.size", &sStyle);
            sBorderRadius.bind("border.radius", &sStyle);
            sGlass.bind("glass.visibility", &sStyle);
            sColor.bind("color", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sGlassColor.bind("glass.color", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sConstraints.init(sclass);
                sBorder.init(sclass, 4);
                sBorderRadius.init(sclass, 12);
                sGlass.init(sclass, true);
                sColor.init(sclass, "#000000");
                sBorderColor.init(sclass, "#cccccc");
                sGlassColor.init(sclass, "#ffffff");
            }

            return STATUS_OK;
        }

        void Graph::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);
            if (vItems.is(prop))
                query_draw();
        }

        void Graph::size_request(ws::size_limit_t *r)
        {
        }

        void Graph::realize(const ws::rectangle_t *r)
        {
        }

        Widget *Graph::find_widget(ssize_t x, ssize_t y)
        {
            return NULL;
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
