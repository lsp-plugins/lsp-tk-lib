/*
 * GraphFrameBuffer.cpp
 *
 *  Created on: 20 авг. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t GraphFrameBuffer::metadata             = { "GraphFrameBuffer", &GraphItem::metadata };

        GraphFrameBuffer::GraphFrameBuffer(Display *dpy):
            GraphItem(dpy)
        {

            pClass              = &metadata;
        }

        GraphFrameBuffer::~GraphFrameBuffer()
        {
        }

        void GraphFrameBuffer::destroy()
        {
        }

        status_t GraphFrameBuffer::init()
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

        void GraphFrameBuffer::property_changed(Property *prop)
        {
            GraphItem::property_changed(prop);
        }

        void GraphFrameBuffer::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
        }
    }
}


