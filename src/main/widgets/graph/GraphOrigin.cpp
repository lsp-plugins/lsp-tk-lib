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
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(GraphOrigin, GraphItem)
                // Bind
                sLeft.bind("left", this);
                sTop.bind("top", this);
                sRadius.bind("radius", this);
                sColor.bind("color", this);
                // Configure
                sLeft.set_all(0.0f, -1.0f, 1.0f);
                sTop.set_all(0.0f, -1.0f, 1.0f);
                sRadius.set(4.0f);
                sColor.set("#ffffff");
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(GraphOrigin, "GraphOrigin", "root");
        }

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
            nFlags     |= FINALIZED;
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

//            Style *sclass = style_class();
//            if (sclass != NULL)
//            {
//                sLeft.init(sclass, 0.0f, -1.0f, 1.0f);
//                sTop.init(sclass, 0.0f, -1.0f, 1.0f);
//                sRadius.init(sclass, 4.0f);
//                sColor.init(sclass, "#ffffff");
//            }

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
            float scaling   = lsp_max(0.0f, sScaling.get());
            ssize_t radius  = (sRadius.get() > 0) ? lsp_max(1.0f, sRadius.get() * scaling) : 0;
            lsp::Color color(sColor);
            color.scale_lch_luminance(sBrightness.get());

            // Draw circle
            float x=0.0f, y=0.0f;
            cv->origin(this, &x, &y);
            bool aa = s->set_antialiasing(sSmooth.get());
            s->fill_circle(color, x, y, radius);
            s->set_antialiasing(aa);
        }
    }
}


