/*
 * GraphAxis.cpp
 *
 *  Created on: 20 авг. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t GraphAxis::metadata             = { "GraphAxis", &GraphItem::metadata };

        GraphAxis::GraphAxis(Display *dpy):
            GraphItem(dpy)
        {

            pClass              = &metadata;
        }

        GraphAxis::~GraphAxis()
        {
        }

        void GraphAxis::destroy()
        {
        }

        status_t GraphAxis::init()
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

        void GraphAxis::property_changed(Property *prop)
        {
        }

        void GraphAxis::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
        }
    }
}


