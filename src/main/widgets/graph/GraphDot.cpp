/*
 * GraphDot.cpp
 *
 *  Created on: 20 авг. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t GraphDot::metadata             = { "GraphDot", &GraphItem::metadata };

        GraphDot::GraphDot(Display *dpy):
            GraphItem(dpy)
        {

            pClass              = &metadata;
        }

        GraphDot::~GraphDot()
        {
        }

        void GraphDot::destroy()
        {
        }

        status_t GraphDot::init()
        {
            status_t res        = GraphItem::init();
            if (res != STATUS_OK)
                return res;

            // Init style
            // TODO

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                // TODO
            }

            return STATUS_OK;
        }

        void GraphDot::property_changed(Property *prop)
        {
        }

        void GraphDot::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
        }
    }
}


