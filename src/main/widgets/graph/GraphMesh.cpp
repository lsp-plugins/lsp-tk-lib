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
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(GraphMesh, GraphItem)
                // Bind
                sOrigin.bind("origin", this);
                sXAxis.bind("haxis", this);
                sYAxis.bind("vaxis", this);
                sWidth.bind("width", this);
                sStrobes.bind("strobes", this);
                sFill.bind("fill", this);
                sColor.bind("color", this);
                sFillColor.bind("fill.color", this);
                sData.bind("data", this);
                // Configure
                sOrigin.set(0);
                sXAxis.set(0);
                sYAxis.set(1);
                sWidth.set(3);
                sStrobes.set(0);
                sFill.set(false);
                sColor.set("#00ff00");
                sFillColor.set("#8800ff00");
                sData.set_size(0);
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(GraphMesh, "GraphMesh", "root");
        }

        const w_class_t GraphMesh::metadata             = { "GraphMesh", &GraphItem::metadata };

        GraphMesh::GraphMesh(Display *dpy):
            GraphItem(dpy),
            sOrigin(&sProperties),
            sXAxis(&sProperties),
            sYAxis(&sProperties),
            sWidth(&sProperties),
            sStrobes(&sProperties),
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
            nFlags     |= FINALIZED;
            do_destroy();
        }

        void GraphMesh::destroy()
        {
            nFlags     |= FINALIZED;
            GraphItem::destroy();
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
            sStrobes.bind("strobes", &sStyle);
            sFill.bind("fill", &sStyle);
            sColor.bind("color", &sStyle);
            sFillColor.bind("fill.color", &sStyle);
            sData.bind("data", &sStyle);

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
            if (sStrobes.is(prop))
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

        size_t GraphMesh::find_offset(size_t *found, const float *v, size_t count, size_t strobes)
        {
            size_t nf       = 0;    // No strobes found
            while (count > 0)
            {
                float s         = v[--count];
                if (s < 0.5f)
                    continue;
                if ((strobes--) <= 0)   // We found start point?
                    break;
                ++nf;                   // Remember that we found the strobe
            }

            if (found != NULL)
                *found = nf;

            return count;
        }

        size_t GraphMesh::get_length(const float *v, size_t off, size_t count)
        {
            size_t start = off;
            while (true)
            {
                if ((++off) >= count)
                    return count - start;
                if (v[off] >= 0.5f)
                    break;
            }

            return off - start;
        }

        void GraphMesh::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            // Get graph
            Graph *cv = graph();
            if (cv == NULL)
                return;

            // Check that data is valid
            if ((!sData.valid()) || (sData.size() < 0))
                return;

            // Get axes
            GraphAxis *xaxis    = cv->axis(sXAxis.get());
            GraphAxis *yaxis    = cv->axis(sYAxis.get());
            if ((xaxis == NULL) || (yaxis == NULL))
                return;

            // Prepare palette
            float scaling   = lsp_max(0.0f, sScaling.get());
            float width     = (sWidth.get() > 0) ? lsp_max(1.0f, sWidth.get() * scaling) : 0.0f;
            float bright    = sBrightness.get();
            lsp::Color line(sColor), fill(sFillColor);
            line.scale_lch_luminance(bright);
            fill.scale_lch_luminance(bright);

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

            // Initialize X and Y vectors
            size_t vec_size     = sData.size();
            float *x_vec        = &vBuffer[0];
            float *y_vec        = &x_vec[vec_size];
            const float *x_src  = sData.x();
            const float *y_src  = sData.y();

            // Now we have dots in x_vec[] and y_vec[]
            bool aa = s->set_antialiasing(sSmooth.get());

            if (sData.strobe())
            {
                const float *s_src  = sData.s();

                // Draw mesh divided into segments using strobes
                size_t strobes      = lsp_max(sStrobes.get(), 0);
                size_t found        = 0;
                size_t off          = (sData.strobe()) ? find_offset(&found, s_src, vec_size, strobes) : 0;
                ssize_t op          = strobes - found + 1;      // Initial opacity coefficient
                float kop           = 1.0f / (strobes + 1.0f);  // Opacity coefficient

                while (off < vec_size)
                {
                    // Get length of the vector before new strobe series starts
                    size_t length       = get_length(s_src, off, vec_size);
                    float ka            = (op++) * kop;

                    // Initialize dimensions as zeros
                    dsp::fill(x_vec, cx, vec_size);
                    dsp::fill(y_vec, cy, vec_size);

                    // Calculate coordinates for each dot
                    if (!xaxis->apply(x_vec, y_vec, &x_src[off], length))
                        return;
                    if (!yaxis->apply(x_vec, y_vec, &y_src[off], length))
                        return;

                    // Draw part of mesh
                    line.copy(sColor);
                    line.alpha(1.0f - (1.0f - line.alpha()) * ka);

                    if (sFill.get())
                    {
                        fill.copy(sFillColor);
                        fill.alpha(1.0f - (1.0f - line.alpha()) * ka);
                        s->draw_poly(fill, line, width, x_vec, y_vec, length);
                    }
                    else if (width > 0)
                        s->wire_poly(line, width, x_vec, y_vec, length);

                    // Update offset
                    off                += length;
                }
            }
            else
            {
                // Initialize dimensions as zeros
                dsp::fill(x_vec, cx, vec_size);
                dsp::fill(y_vec, cy, vec_size);

                // Calculate coordinates for each dot
                if (!xaxis->apply(x_vec, y_vec, x_src, vec_size))
                    return;
                if (!yaxis->apply(x_vec, y_vec, y_src, vec_size))
                    return;

                if (sFill.get())
                    s->draw_poly(fill, line, width, x_vec, y_vec, vec_size);
                else if (width > 0)
                    s->wire_poly(line, width, x_vec, y_vec, vec_size);
            }

            s->set_antialiasing(aa);
        }
    }
}


