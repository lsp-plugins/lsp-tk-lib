/*
 * GraphAxis.cpp
 *
 *  Created on: 20 авг. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/tk/helpers/graphics.h>
#include <lsp-plug.in/dsp/dsp.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t GraphAxis::metadata             = { "GraphAxis", &GraphItem::metadata };

        GraphAxis::GraphAxis(Display *dpy):
            GraphItem(dpy),
            sDirection(&sProperties),
            sMin(&sProperties),
            sMax(&sProperties),
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
            sLogScale.bind("log", &sStyle);
            sBasis.bind("basis", &sStyle);
            sWidth.bind("width", &sStyle);
            sLength.bind("length", &sStyle);
            sOrigin.bind("origin", &sStyle);
            sColor.bind("color", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sDirection.init_cart(sclass, 1.0f, 0.0f);
                sMin.init(sclass, -1.0f);
                sMax.init(sclass, 1.0f);
                sLogScale.init(sclass, false);
                sBasis.init(sclass, true);
                sWidth.init(sclass, 1);
                sLength.init(sclass, -1.0f);
                sOrigin.init(sclass, 0);
                sColor.init(sclass, "#ffffff");
            }

            pClass          = &metadata;

            return STATUS_OK;
        }

        void GraphAxis::property_changed(Property *prop)
        {
            GraphItem::property_changed(prop);

            if (sDirection.is(prop))
                query_draw();
            if (sMin.is(prop))
                query_draw();
            if (sMax.is(prop))
                query_draw();
            if (sLogScale.is(prop))
                query_draw();
            if (sBasis.is(prop))
                query_draw();
            if (sWidth.is(prop))
                query_draw();
            if (sLength.is(prop))
                query_draw();
            if (sOrigin.is(prop))
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
            color.scale_lightness(sBrightness.get());

            // Draw
            float cx = 0.0f, cy = 0.0f;
            cv->origin(sOrigin.get(), &cx, &cy);

            float la, lb, lc;
            if (!locate_line2d(sDirection.dx(), -sDirection.dy(), cx, cy, la, lb, lc))
                return;

            bool aa = s->set_antialiasing(sSmooth.get());
            s->parametric_line(
                    la, lb, lc,
                    cv->canvas_left(), cv->canvas_right(), cv->canvas_top(), cv->canvas_bottom(),
                    width, color
                );
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
                if (!clip_line2d(la, lb, lc,
                        cv->canvas_left(), cv->canvas_right(), cv->canvas_top(), cv->canvas_bottom(),
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

            // Normalize value according to minimum and maximum visible values of the axis
            float a_min = fabsf(sMin.get()), a_max = fabsf(sMax.get());

            // Now we can surely apply deltas
            if (sLogScale.get())
            {
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
                float norm = (a_min > a_max) ? a_min : a_max;
                if (norm == 0.0f)
                    return false;
                norm    = d / norm;

                // Apply delta-vector
                dsp::fmadd_k3(x, dv, norm * fdx, count);
                dsp::fmadd_k3(y, dv, norm * fdy, count);
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
                return false;

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
                    return false;

                float x1, y1, x2, y2;
                if (!clip_line2d(la, lb, lc,
                        cv->canvas_left(), cv->canvas_right(), cv->canvas_top(), cv->canvas_bottom(),
                        x1, y1, x2, y2)
                    )
                    return false;

                float d1    = distance2d(cx, cy, x1, y1);
                float d2    = distance2d(cx, cy, x2, y2);
                d           = (d1 > d2) ? d1 : d2;
            }
            if (d > 1.0f)
                d          -= 0.5f; // Fix rounding errors

            // Normalize value according to minimum and maximum visible values of the axis
            float a_min = fabsf(sMin.get()), a_max = fabsf(sMax.get());

            // Now we can surely apply deltas
            if (sLogScale.get())
            {
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
                float norm = (a_min > a_max) ? a_min : a_max;
                if (norm == 0.0f)
                    return sMin.get();

                return (pv*norm/d) + ((a_min > a_max) ? a_max : a_min);
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
            float fdx   = sDirection.dx(), fdy = -sDirection.dy();
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
    }
}


