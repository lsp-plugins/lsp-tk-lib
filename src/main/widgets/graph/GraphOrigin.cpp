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
            GraphItem(dpy),
            sLeft(&sProperties),
            sTop(&sProperties),
            sRadius(&sProperties),
            sColor(&sProperties)
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
            sLeft.bind("left", &sStyle);
            sTop.bind("top", &sStyle);
            sRadius.bind("radius", &sStyle);
            sColor.bind("color", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sLeft.init(sclass, 0.0f, -1.0f, 1.0f);
                sTop.init(sclass, 0.0f, -1.0f, 1.0f);
                sRadius.init(sclass, 4.0f);
                sColor.init(sclass, "#ffffff");
            }

            return STATUS_OK;
        }

        void GraphOrigin::property_changed(Property *prop)
        {
            GraphItem::property_changed(prop);

            if (sLeft.is(prop))
                query_draw();
            if (sTop.is(prop))
                query_draw();
            if (sRadius.is(prop))
                query_draw();
            if (sColor.is(prop))
                query_draw();
        }

        void GraphOrigin::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            // Get graph
            Graph *cv = graph();
            if (cv == NULL)
                return;

            // Generate palette
            float scaling = lsp_max(0.0f, sScaling.get());
            lsp::Color color(sColor);
            color.scale_lightness(sBrightness.get());

            // Draw circle
            float x=0.0, y=0.0;
            cv->origin(this, &x, &y);
            bool aa = s->set_antialiasing(sSmooth.get());
            s->fill_circle(x, y, sRadius.get() * scaling, color);
            s->set_antialiasing(aa);
        }
    }
}


