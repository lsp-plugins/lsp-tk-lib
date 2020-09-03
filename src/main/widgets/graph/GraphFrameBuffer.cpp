/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 20 авг. 2020 г.
 *
 * lsp-tk-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-tk-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-tk-lib. If not, see <https://www.gnu.org/licenses/>.
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


