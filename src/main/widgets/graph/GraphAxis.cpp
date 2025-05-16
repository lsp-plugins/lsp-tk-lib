/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
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
#include <lsp-plug.in/tk/helpers/graphics.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(GraphAxis, GraphItem)
                // Bind
                sDirection.bind("direction", this);
                sMin.bind("min", this);
                sMax.bind("max", this);
                sZero.bind("zero", this);
                sLogScale.bind("log", this);
                sBasis.bind("basis", this);
                sWidth.bind("width", this);
                sLength.bind("length", this);
                sOrigin.bind("origin", this);
                sColor.bind("color", this);

                // Configure
                sDirection.set_cart(1.0f, 0.0f);
                sMin.set(-1.0f);
                sMax.set(1.0f);
                sZero.set(0.0f);
                sLogScale.set(false);
                sBasis.set(true);
                sWidth.set(1);
                sLength.set(-1.0f);
                sOrigin.set(0);
                sColor.set("#ffffff");
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(GraphAxis, "GraphAxis", "root");
        }

        const w_class_t GraphAxis::metadata             = { "GraphAxis", &GraphItem::metadata };

        GraphAxis::GraphAxis(Display *dpy):
            GraphItem(dpy),
            sDirection(&sProperties),
            sMin(&sProperties),
            sMax(&sProperties),
            sZero(&sProperties),
            sLogScale(&sProperties),
            sBasis(&sProperties),
            sWidth(&sProperties),
            sLength(&sProperties),
            sOrigin(&sProperties),
            sColor(&sProperties)
        {
            pClass              = &metadata;
        }

        GraphAxis::~GraphAxis()
        {
            nFlags     |= FINALIZED;
        }

        status_t GraphAxis::init()
        {
            status_t res        = GraphItem::init();
            if (res != STATUS_OK)
                return res;

            // Init style
            sDirection.bind("direction", &sStyle);
            sMin.bind("min", &sStyle);
            sMax.bind("max", &sStyle);
            sZero.bind("zero", &sStyle);
            sLogScale.bind("log", &sStyle);
            sBasis.bind("basis", &sStyle);
            sWidth.bind("width", &sStyle);
            sLength.bind("length", &sStyle);
            sOrigin.bind("origin", &sStyle);
            sColor.bind("color", &sStyle);

            pClass          = &metadata;

            return STATUS_OK;
        }

        void GraphAxis::property_changed(Property *prop)
        {
            GraphItem::property_changed(prop);

            if (prop->one_of(sMin, sMax, sZero))
                query_draw();
            if (prop->one_of(sBasis, sOrigin, sDirection))
                query_draw();
            if (prop->one_of(sWidth, sLength))
                query_draw();
            if (sLogScale.is(prop))
                query_draw();
            if (sColor.is(prop))
                query_draw();
        }

        void GraphAxis::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            // Get graph
            Graph *cv = graph();
            if (cv == NULL)
                return;

            // Prepare palette
            float scaling = lsp_max(0.0f, sScaling.get());
            float width   = (sWidth.get() > 0) ? lsp_max(1.0f, sWidth.get() * scaling) : 0;
            lsp::Color color(sColor);
            color.scale_lch_luminance(select_brightness());

            // Draw
            float cx = 0.0f, cy = 0.0f;
            cv->origin(sOrigin.get(), &cx, &cy);

            float la, lb, lc;
            if (!locate_line2d(sDirection.dx(), -sDirection.dy(), cx, cy, la, lb, lc))
                return;

            bool aa = s->set_antialiasing(sSmooth.get());
            s->parametric_line(
                color,
                la, lb, lc,
                cv->canvas_left(), cv->canvas_right(), cv->canvas_bottom(), cv->canvas_top(),
                width);
            s->set_antialiasing(aa);
        }

        bool GraphAxis::apply(float *x, float *y, const float *dv, size_t count)
        {
            // Get graph
            Graph *cv = graph();
            if (cv == NULL)
                return false;

            float cx    = 0.0f, cy = 0.0f;
            float fdx   = sDirection.dx(), fdy = -sDirection.dy();
            cv->origin(sOrigin.get(), &cx, &cy);

            float d     = sLength.get();
            if (d < 0.0f)
            {
                float la, lb, lc;

                if (!locate_line2d(fdx, fdy, cx, cy, la, lb, lc))
                    return false;

                float x1, y1, x2, y2;

                if (!clip_line2d_eq(
                        la, lb, lc,
                        cv->canvas_left(), cv->canvas_right(), cv->canvas_bottom(), cv->canvas_top(),
                        2.0f,
                        x1, y1, x2, y2
                        )
                    )
                    return false;

                float d1    = distance2d(cx, cy, x1, y1);
                float d2    = distance2d(cx, cy, x2, y2);
                d           = (d1 > d2) ? d1 : d2;
            }
//            if (d > 1.0f)
//                d          -= 0.5f; // Fix rounding errors

            // Now we can surely apply deltas
            if (sLogScale.get())
            {
                float a_min = fabsf(sMin.get());
                float a_max = fabsf(sMax.get());

                if (a_min <= 0.0f)
                    a_min   = 1e-10f;
                if (a_max <= 0.0f)
                    a_max   = 1e-10f;
                float norm = (a_min > a_max) ? logf(a_min / a_max) : logf(a_max / a_min);
                if (norm == 0.0f)
                    return false;

                norm            = d / norm;
                a_min           = 1.0f / a_min;

                dsp::axis_apply_log2(x, y, dv, a_min, norm * fdx, norm * fdy, count);
            }
            else
            {
                float zero  = - sZero.get();
                float a_min = fabsf(sMin.get() + zero);
                float a_max = fabsf(sMax.get() + zero);

                float norm = (a_min > a_max) ? a_min : a_max;
                if (norm == 0.0f)
                    return false;
                norm    = d / norm;

                // Apply delta-vector
                dsp::axis_apply_lin1(x, dv, zero, norm * fdx, count);
                dsp::axis_apply_lin1(y, dv, zero, norm * fdy, count);
            }

            // Saturate values
            dsp::saturate(x, count);
            dsp::saturate(y, count);

            return true;
        }

        float GraphAxis::project(float x, float y)
        {
            // Get graph
            Graph *cv = graph();
            if (cv == NULL)
                return sMin.get();

            // Get the center of coordinates
            float cx    = 0.0f, cy = 0.0f;
            float fdx   = sDirection.dx(), fdy = -sDirection.dy();
            cv->origin(sOrigin.get(), &cx, &cy);

            // Calculate the difference relative to the center and the projection vector length
            float dx    = x - cx, dy = y - cy;
            float pv    = dx*fdx + dy*fdy;

            float d     = sLength.get();
            if (d < 0.0f)
            {
                // Now prepare the image of the line
                float la, lb, lc;
                if (!locate_line2d(fdx, fdy, cx, cy, la, lb, lc))
                    return sMin.get();

                float x1, y1, x2, y2;
                if (!clip_line2d_eq(
                        la, lb, lc,
                        cv->canvas_left(), cv->canvas_right(), cv->canvas_bottom(), cv->canvas_top(),
                        2.0f,
                        x1, y1, x2, y2)
                    )
                    return sMin.get();

                float d1    = distance2d(cx, cy, x1, y1);
                float d2    = distance2d(cx, cy, x2, y2);
                d           = (d1 > d2) ? d1 : d2;
            }
            if (d > 1.0f)
                d          -= 0.5f; // Fix rounding errors

            // Now we can surely apply deltas
            if (sLogScale.get())
            {
                float a_min = fabsf(sMin.get()), a_max = fabsf(sMax.get());

                if (a_min <= 0.0f)
                    a_min   = 1e-10f;
                if (a_max <= 0.0f)
                    a_max   = 1e-10f;
                float norm = (a_min > a_max) ? logf(a_min / a_max) : logf(a_max / a_min);
                if (norm == 0.0f)
                    return sMin.get();

                return expf(pv * norm / d) * ((a_min > a_max) ? a_max : a_min);
            }
            else
            {
                float zero  = - sZero.get();
                float a_min = fabsf(sMin.get() + zero);
                float a_max = fabsf(sMax.get() + zero);

                float norm = (a_min > a_max) ? a_min : a_max;
                if (norm == 0.0f)
                    return sMin.get();

                return (pv*norm/d) - zero + ((a_min > a_max) ? a_max : a_min);
            }

            return sMin.get();
        }

        bool GraphAxis::parallel(float x, float y, float &a, float &b, float &c)
        {
            float fdx   = sDirection.dx(), fdy = -sDirection.dy();
            return locate_line2d(fdx, fdy, x, y, a, b, c);
        }

        void GraphAxis::ortogonal_shift(float x, float y, float shift, float &nx, float &ny)
        {
            // When rotating 90 degrees left, we get: dy' = dx, dx' = -dy
            float fdx   = -sDirection.dy(), fdy = sDirection.dx();
            nx               = x + shift * fdx;
            ny               = y - shift * fdy;
        }

        bool GraphAxis::angle(float x, float y, float angle, float &a, float &b, float &c)
        {
            float fdx       = sDirection.dx();
            float fdy       = -sDirection.dy();
            float c_sin     = sinf(angle);
            float c_cos     = cosf(angle);
            float dx        = fdx*c_cos - fdy*c_sin;
            float dy        = fdx*c_sin + fdy*c_cos;

            return locate_line2d(dx, -dy, x, y, a, b, c);
        }

        void GraphAxis::rotate_shift(float x, float y, float angle, float shift, float &nx, float &ny)
        {
            float fdx       = sDirection.dx();
            float fdy       = -sDirection.dy();
            float c_sin     = sinf(angle);
            float c_cos     = cosf(angle);
            float dx        = fdx*c_cos - fdy*c_sin;
            float dy        = fdx*c_sin + fdy*c_cos;

            nx              = x + shift * dy;
            ny              = y - shift * dx;
        }
    } /* namespace tk */
} /* namespace lsp */


