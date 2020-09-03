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
#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <stdlib.h>

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
            vBuffer             = NULL;
            nCapacity           = 0;

            pClass              = &metadata;
        }

        GraphMesh::~GraphMesh()
        {
            do_destroy();
        }

        void GraphMesh::do_destroy()
        {
            if (vBuffer != NULL)
            {
                ::free(vBuffer);
                vBuffer         = NULL;
            }
            nCapacity       = 0;
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
            sData.bind("data", &sStyle);

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
                sData.init(sclass, 0);
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
            // Get graph
            Graph *cv = graph();
            if (cv == NULL)
                return;

            if ((!sData.valid()) || (sData.size() < 0))
                return;

            // Prepare palette
            float scaling   = lsp_max(0.0f, sScaling.get());
            float width     = (sWidth.get() > 0) ? lsp_max(1.0f, sWidth.get() * scaling) : 0.0f;
            float bright    = sBrightness.get();
            lsp::Color line(sColor), fill(sFillColor);
            line.scale_lightness(bright);
            fill.scale_lightness(bright);

            float cx = 0.0f, cy = 0.0f;
            cv->origin(sOrigin.get(), &cx, &cy);

            // Ensure that we have enough buffer size
            size_t cap_size     = align_size(sData.size() * 2, DEFAULT_ALIGN);
            if (nCapacity < cap_size)
            {
                float *buf          = static_cast<float *>(realloc(vBuffer, cap_size * sizeof(float)));
                if (buf == NULL)
                    return;
                vBuffer             = buf;
                nCapacity           = cap_size;
            }

            // Initialize dimensions as zeros
            size_t vec_size     = sData.size();
            float *x_vec        = &vBuffer[0];
            float *y_vec        = &x_vec[vec_size];
            dsp::fill(x_vec, cx, vec_size);
            dsp::fill(y_vec, cy, vec_size);

            // Calculate coordinates for each dot
            GraphAxis *xaxis    = cv->axis(sXAxis.get());
            GraphAxis *yaxis    = cv->axis(sYAxis.get());
            if ((xaxis == NULL) || (yaxis == NULL))
                return;

            if (!xaxis->apply(x_vec, y_vec, sData.x(), vec_size))
                return;
            if (!yaxis->apply(x_vec, y_vec, sData.y(), vec_size))
                return;

            // Now we have dots in x_vec[] and y_vec[]
            bool aa = s->set_antialiasing(sSmooth.get());
            if (sFill.get())
                s->draw_poly(x_vec, y_vec, vec_size, width, fill, line);
            else if (width > 0)
                s->wire_poly(x_vec, y_vec, vec_size, width, line);

            s->set_antialiasing(aa);
        }
    }
}


