/*
 * GraphOrigin.cpp
 *
 *  Created on: 20 авг. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t GraphOrigin::metadata             = { "GraphOrigin", &GraphItem::metadata };

        GraphOrigin::GraphOrigin(Display *dpy):
            GraphItem(dpy)
        {

            pClass              = &metadata;
        }

        GraphOrigin::~GraphOrigin()
        {
        }

        void GraphOrigin::destroy()
        {
        }

        status_t GraphOrigin::init()
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

        void GraphOrigin::property_changed(Property *prop)
        {
        }

        void GraphOrigin::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
        }
    }
}


