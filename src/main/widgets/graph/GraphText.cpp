/*
 * GraphText.cpp
 *
 *  Created on: 20 авг. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t GraphText::metadata             = { "GraphText", &GraphItem::metadata };

        GraphText::GraphText(Display *dpy):
            GraphItem(dpy)
        {

            pClass              = &metadata;
        }

        GraphText::~GraphText()
        {
        }

        void GraphText::destroy()
        {
        }

        status_t GraphText::init()
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

        void GraphText::property_changed(Property *prop)
        {
        }

        void GraphText::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
        }
    }
}


