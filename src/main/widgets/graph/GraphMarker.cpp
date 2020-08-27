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
            sHWidth(&sProperties),
            sEditable(&sProperties),
            sBorder(&sProperties),
            sColor(&sProperties),
            sHColor(&sProperties),
            sBorderColor(&sProperties)
        {
            fOffset             = 0.0f;
            nXFlags             = 0;

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
            sHWidth.bind("hover.width", &sStyle);
            sEditable.bind("editable", &sStyle);
            sBorder.bind("border.size", &sStyle);
            sColor.bind("color", &sStyle);
            sHColor.bind("hover.color", &sStyle);
            sBorderColor.bind("border.color", &sStyle);

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
                sHWidth.init(sclass, 3);
                sEditable.init(sclass, false);
                sBorder.init(sclass, 0);
                sColor.init(sclass, "#ffffff");
                sHColor.init(sclass, "#ffffff");
                sBorderColor.init(sclass, "#ffffff");
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
            if (sHWidth.is(prop))
                query_draw();
            if (sEditable.is(prop))
                query_draw();
            if (sBorder.is(prop))
                query_draw();
            if (sColor.is(prop))
                query_draw();
            if (sHColor.is(prop))
                query_draw();
            if (sBorderColor.is(prop))
                query_draw();
        }

        void GraphMarker::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            // Get graph
            Graph *cv           = graph();
            if (cv == NULL)
                return;

            // Prepare palette
            float scaling       = lsp_max(0.0f, sScaling.get());
            float brightness    = sBrightness.get();

            lsp::Color color(sColor);
            color.scale_lightness(brightness);

            // Get basis
            GraphAxis *basis    = cv->axis(sBasis.get());
            if (basis == NULL)
                return;
            GraphAxis *parallel = cv->axis(sParallel.get());
            if (parallel == NULL)
                return;

            float x = 0.0f, y = 0.0f;
            cv->origin(sOrigin.get(), &x, &y);

            // Translate point and get the coordinates of point that lays on the target line
            float fvalue    = sValue.get();

            if (!basis->apply(&x, &y, &fvalue, 1))
                return;
            if (fOffset != 0.0f)
            {
                if (!parallel->apply(&x, &y, &fOffset, 1))
                    return;
            }

            // Get equation of the line that contains calculated point
            float a, b, c;
            float nx, ny;
            float a2, b2, c2;
            float fangle    = sDirection.rphi();
            ssize_t nborder = (sBorder.get() > 0) ? lsp_max(1.0f, sBorder.get() * scaling) : 0;

            if (fangle == 0.0f)
            {
                if (!parallel->parallel(x, y, a, b, c))
                    return;
                if (nborder != 0)
                {
                    parallel->ortogonal_shift(x, y, nborder, nx, ny);
                    if (!parallel->parallel(nx, ny, a2, b2, c2))
                        return;
                }
            }
            else
            {
                if (!parallel->angle(x, y, fangle, a, b, c))
                    return;
                if (nborder != 0)
                {
                    parallel->rotate_shift(x, y, fangle, nborder, nx, ny);
                    if (!parallel->angle(x, y, fangle, a2, b2, c2))
                        return;
                }
            }

            // Draw line
            bool aa     = s->set_antialiasing(sSmooth.get());
            lsp::Color col(sColor, 0.0f);
            col.scale_lightness(brightness);

            prop::Integer *w = (nXFlags & F_HIGHLIGHT) ? &sHWidth : &sWidth;
            ssize_t l_width  = (w->get() > 0) ? lsp_max(1.0f, w->get() * scaling) : 0;

            if (nborder > 0)
            {
                ws::IGradient *g = s->linear_gradient(x, y, nx, ny);
                if (g != NULL)
                {
                    g->add_color(0.0f, color, 0.25f  + 0.5f * (1.0f - color.alpha()));
                    g->add_color(1.0f, color, 1.0f);

                    s->parametric_bar(
                        a, b, c, a2, b2, c2,
                        cv->canvas_left(), cv->canvas_right(), cv->canvas_top(), cv->canvas_bottom(),
                        g
                    );
                    s->parametric_line(a, b, c,
                            cv->canvas_left(), cv->canvas_right(), cv->canvas_top(), cv->canvas_bottom(),
                            l_width, col
                    );

                    delete g;
                }
            }
            else {
                s->parametric_line(a, b, c,
                        cv->canvas_left(), cv->canvas_right(), cv->canvas_top(), cv->canvas_bottom(),
                        l_width, col
                );
            }
            s->set_antialiasing(aa);
        }
    }
}


