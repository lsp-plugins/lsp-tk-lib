/*
 * GraphItem.cpp
 *
 *  Created on: 20 авг. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t GraphItem::metadata     =   { "GraphItem", &Widget::metadata };

        GraphItem::GraphItem(Display *dpy):
            Widget(dpy),
            sSmooth(&sProperties)
        {
        }

        GraphItem::~GraphItem()
        {
        }

        status_t GraphItem::init()
        {
            status_t result = Widget::init();
            if (result != STATUS_OK)
                return result;

            // Init style
            sSmooth.bind("smooth", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sSmooth.init(sclass, true);
            }

            return STATUS_OK;
        }

        void GraphItem::destroy()
        {
            Widget::destroy();
        }

        void GraphItem::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (sSmooth.is(prop))
                query_draw();
        }

        Graph *GraphItem::graph()
        {
            return widget_cast<Graph>(pParent);
        }

        bool GraphItem::inside(ssize_t x, ssize_t y)
        {
            return false;
        }

        void GraphItem::query_draw(size_t flags)
        {
            Widget::query_draw(flags);
            if (!sVisibility.get())
                return;

            // Force graph to redraw it's surface
            if (flags & (REDRAW_SURFACE | REDRAW_CHILD))
            {
                Graph *gr = graph();
                if (gr != NULL)
                    gr->query_draw(REDRAW_SURFACE);
            }
        }
    }
}



