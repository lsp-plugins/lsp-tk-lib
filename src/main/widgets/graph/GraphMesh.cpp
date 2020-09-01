/*
 * GraphMesh.cpp
 *
 *  Created on: 20 авг. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t GraphMesh::metadata             = { "GraphMesh", &GraphItem::metadata };

        GraphMesh::GraphMesh(Display *dpy):
            GraphItem(dpy),
            sOrigin(&sProperties),
            sXAxis(&sProperties),
            sYAxis(&sProperties),
            sWidth(&sProperties),
            sFill(&sProperties),
            sColor(&sProperties),
            sFillColor(&sProperties),
            sData(&sProperties)
        {
            pClass              = &metadata;
        }

        GraphMesh::~GraphMesh()
        {
            do_destroy();
        }

        void GraphMesh::do_destroy()
        {
        }

        void GraphMesh::destroy()
        {
            do_destroy();
        }

        status_t GraphMesh::init()
        {
            status_t res        = GraphItem::init();
            if (res != STATUS_OK)
                return res;

            // Init style
            sOrigin.bind("origin", &sStyle);
            sXAxis.bind("haxis", &sStyle);
            sYAxis.bind("vaxis", &sStyle);
            sWidth.bind("width", &sStyle);
            sFill.bind("fill", &sStyle);
            sColor.bind("color", &sStyle);
            sFillColor.bind("fill.color", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sOrigin.init(sclass, 0);
                sXAxis.init(sclass, 0);
                sYAxis.init(sclass, 1);
                sWidth.init(sclass, 3);
                sFill.init(sclass, false);
                sColor.init(sclass, "#00ff00");
                sFillColor.init(sclass, "#8800ff00");
            }

            return STATUS_OK;
        }

        void GraphMesh::property_changed(Property *prop)
        {
            GraphItem::property_changed(prop);

            if (sOrigin.is(prop))
                query_draw();
            if (sXAxis.is(prop))
                query_draw();
            if (sYAxis.is(prop))
                query_draw();
            if (sWidth.is(prop))
                query_draw();
            if (sFill.is(prop))
                query_draw();
            if (sColor.is(prop))
                query_draw();
            if ((sFillColor.is(prop)) && (sFill.get()))
                query_draw();
            if (sData.is(prop))
                query_draw();
        }

        void GraphMesh::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
        }
    }
}


