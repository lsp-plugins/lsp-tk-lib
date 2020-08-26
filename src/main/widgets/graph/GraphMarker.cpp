/*
 * GraphMarker.cpp
 *
 *  Created on: 20 авг. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t GraphMarker::metadata             = { "GraphMarker", &GraphItem::metadata };

        GraphMarker::GraphMarker(Display *dpy):
            GraphItem(dpy),
            sOrigin(&sProperties),
            sBasis(&sProperties),
            sParallel(&sProperties),
            sValue(&sProperties),
            sMin(&sProperties),
            sMax(&sProperties),
            sDirection(&sProperties),
            sWidth(&sProperties),
            sEditable(&sProperties),
            sHighlight(&sProperties),
            sBorder(&sProperties),
            sColor(&sProperties),
            sSelColor(&sProperties),
            sHColor(&sProperties)
        {
            pClass              = &metadata;
        }

        GraphMarker::~GraphMarker()
        {
        }

        status_t GraphMarker::init()
        {
            status_t res        = GraphItem::init();
            if (res != STATUS_OK)
                return res;

            // Init style
            sOrigin.bind("origin", &sStyle);
            sBasis.bind("basis", &sStyle);
            sParallel.bind("parallel", &sStyle);
            sValue.bind("value", &sStyle);
            sMin.bind("min", &sStyle);
            sMax.bind("max", &sStyle);
            sDirection.bind("direction", &sStyle);
            sWidth.bind("width", &sStyle);
            sEditable.bind("editable", &sStyle);
            sHighlight.bind("highlight", &sStyle);
            sBorder.bind("border.size", &sStyle);
            sColor.bind("color", &sStyle);
            sSelColor.bind("hover.color", &sStyle);
            sHColor.bind("highlight.color", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sOrigin.init(sclass, 0);
                sBasis.init(sclass, 0);
                sParallel.init(sclass, 1);
                sValue.init(sclass, 0.0f);
                sMin.init(sclass, -1.0f);
                sMax.init(sclass, 1.0f);
                sDirection.init_cart(sclass, 1.0f, 0.0f);
                sWidth.init(sclass, 1);
                sEditable.init(sclass, false);
                sHighlight.init(sclass, false);
                sBorder.init(sclass, 0);
                sColor.init(sclass, "#ffffff");
                sSelColor.init(sclass, "#ffffff");
                sHColor.init(sclass, "#ffffff");
            }

            return STATUS_OK;
        }

        void GraphMarker::property_changed(Property *prop)
        {
            GraphItem::property_changed(prop);

            if (sOrigin.is(prop))
                query_draw();
            if (sBasis.is(prop))
                query_draw();
            if (sParallel.is(prop))
                query_draw();
            if (sValue.is(prop))
                query_draw();
            if (sMin.is(prop))
                {/* nothing */}
            if (sMax.is(prop))
                {/* nothing */}
            if (sDirection.is(prop))
                {/* nothing */}
            if (sWidth.is(prop))
                query_draw();
            if (sEditable.is(prop))
                query_draw();
            if (sHighlight.is(prop))
                query_draw();
            if (sBorder.is(prop))
                query_draw();
            if (sColor.is(prop))
                query_draw();
            if (sSelColor.is(prop))
                query_draw();
            if (sHColor.is(prop))
                query_draw();
        }

        void GraphMarker::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
        }
    }
}


