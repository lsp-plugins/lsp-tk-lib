/*
 * GraphMarker.cpp
 *
 *  Created on: 20 авг. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/tk/helpers/graphics.h>
#include <lsp-plug.in/common/debug.h>

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
            sStep(&sProperties),
            sMin(&sProperties),
            sMax(&sProperties),
            sDirection(&sProperties),
            sWidth(&sProperties),
            sHWidth(&sProperties),
            sEditable(&sProperties),
            sLBorder(&sProperties),
            sRBorder(&sProperties),
            sHLBorder(&sProperties),
            sHRBorder(&sProperties),
            sColor(&sProperties),
            sHColor(&sProperties),
            sLBorderColor(&sProperties),
            sRBorderColor(&sProperties),
            sHLBorderColor(&sProperties),
            sHRBorderColor(&sProperties)
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
            sStep.bind("step", &sStyle);
            sMin.bind("min", &sStyle);
            sMax.bind("max", &sStyle);
            sDirection.bind("direction", &sStyle);
            sWidth.bind("width", &sStyle);
            sHWidth.bind("hover.width", &sStyle);
            sEditable.bind("editable", &sStyle);
            sLBorder.bind("border.left.size", &sStyle);
            sRBorder.bind("border.right.size", &sStyle);
            sHLBorder.bind("hover.border.left.size", &sStyle);
            sHRBorder.bind("hover.border.right.size", &sStyle);
            sColor.bind("color", &sStyle);
            sHColor.bind("hover.color", &sStyle);
            sLBorderColor.bind("border.left.color", &sStyle);
            sRBorderColor.bind("border.right.color", &sStyle);
            sHLBorderColor.bind("hover.border.left.color", &sStyle);
            sHRBorderColor.bind("hover.border.right.color", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sOrigin.init(sclass, 0);
                sBasis.init(sclass, 0);
                sParallel.init(sclass, 1);
                sValue.init(sclass, 0.0f);
                sStep.init(sclass, 0.01f);
                sMin.init(sclass, -1.0f);
                sMax.init(sclass, 1.0f);
                sDirection.init_cart(sclass, 1.0f, 0.0f);
                sWidth.init(sclass, 1);
                sHWidth.init(sclass, 3);
                sEditable.init(sclass, false);
                sLBorder.init(sclass, 0);
                sRBorder.init(sclass, 0);
                sHLBorder.init(sclass, 0);
                sHRBorder.init(sclass, 0);
                sColor.init(sclass, "#ffffff");
                sHColor.init(sclass, "#ffffff");
                sLBorderColor.init(sclass, "#ffffff");
                sRBorderColor.init(sclass, "#ffffff");
                sHLBorderColor.init(sclass, "#ffffff");
                sHRBorderColor.init(sclass, "#ffffff");
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
            if (sStep.is(prop))
                {/* nothing */}
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
            if (sLBorder.is(prop))
                query_draw();
            if (sRBorder.is(prop))
                query_draw();
            if (sHLBorder.is(prop))
                query_draw();
            if (sHRBorder.is(prop))
                query_draw();
            if (sColor.is(prop))
                query_draw();
            if (sHColor.is(prop))
                query_draw();
            if (sLBorderColor.is(prop))
                query_draw();
            if (sRBorderColor.is(prop))
                query_draw();
            if (sHLBorderColor.is(prop))
                query_draw();
            if (sHRBorderColor.is(prop))
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

            lsp::Color  color, bcol_l, bcol_r;
            if (nXFlags & F_HIGHLIGHT)
            {
                color.copy(sHColor);
                bcol_l.copy(sHLBorderColor);
                bcol_r.copy(sHRBorderColor);
            }
            else
            {
                color.copy(sColor);
                bcol_l.copy(sLBorderColor);
                bcol_r.copy(sRBorderColor);
            }

            bcol_l.scale_lightness(brightness);
            bcol_r.scale_lightness(brightness);
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
            float ll[3], bl[3], br[3];
            float nl[2], nr[2];
            float fangle    = sDirection.rphi();
            ssize_t lborder = (nXFlags & F_HIGHLIGHT) ? sHLBorder.get() : sLBorder.get();
            ssize_t rborder = (nXFlags & F_HIGHLIGHT) ? sHRBorder.get() : sRBorder.get();
            lborder         = (lborder > 0) ? lsp_max(1.0f, lborder * scaling) : 0;
            rborder         = (rborder > 0) ? lsp_max(1.0f, rborder * scaling) : 0;

            if (fangle == 0.0f)
            {
                if (!parallel->parallel(x, y, ll[0], ll[1], ll[2]))
                    return;
                if (lborder != 0)
                {
                    parallel->ortogonal_shift(x, y, lborder, nl[0], nl[1]);
                    if (!parallel->parallel(nl[0], nl[1], bl[0], bl[1], bl[2]))
                        return;
                }
                if (rborder != 0)
                {
                    parallel->ortogonal_shift(x, y, -rborder, nr[0], nr[1]);
                    if (!parallel->parallel(nr[0], nr[1], br[0], br[1], br[2]))
                        return;
                }
            }
            else
            {
                if (!parallel->angle(x, y, fangle, ll[0], ll[1], ll[2]))
                    return;
                if (lborder != 0)
                {
                    parallel->rotate_shift(x, y, fangle, lborder, nl[0], nl[1]);
                    if (!parallel->angle(x, y, fangle, bl[0], bl[1], bl[2]))
                        return;
                }
                if (rborder != 0)
                {
                    parallel->rotate_shift(x, y, fangle, -rborder, nr[0], nr[1]);
                    if (!parallel->angle(x, y, fangle, br[0], br[1], br[2]))
                        return;
                }
            }

            // Draw line
            bool aa     = s->set_antialiasing(sSmooth.get());

            prop::Integer *w = (nXFlags & F_HIGHLIGHT) ? &sHWidth : &sWidth;
            ssize_t l_width  = (w->get() > 0) ? lsp_max(1.0f, w->get() * scaling) : 0;

            if (lborder > 0)
            {
                ws::IGradient *g = s->linear_gradient(x, y, nl[0], nl[1]);
                if (g != NULL)
                {
                    g->add_color(0.0f, bcol_l);
                    g->add_color(1.0f, bcol_l, 1.0f);

                    s->parametric_bar(
                        ll[0], ll[1], ll[2], bl[0], bl[1], bl[2],
                        cv->canvas_left(), cv->canvas_right(), cv->canvas_bottom(), cv->canvas_top(),
                        g
                    );

                    delete g;
                }
            }
            if (rborder > 0)
            {
                ws::IGradient *g = s->linear_gradient(x, y, nr[0], nr[1]);
                if (g != NULL)
                {
                    g->add_color(0.0f, bcol_r);
                    g->add_color(1.0f, bcol_r, 1.0f);

                    s->parametric_bar(
                        ll[0], ll[1], ll[2], br[0], br[1], br[2],
                        cv->canvas_left(), cv->canvas_right(), cv->canvas_bottom(), cv->canvas_top(),
                        g
                    );

                    delete g;
                }
            }

            s->parametric_line(
                ll[0], ll[1], ll[2],
                cv->canvas_left(), cv->canvas_right(), cv->canvas_bottom(), cv->canvas_top(),
                l_width, color
            );

            s->set_antialiasing(aa);
        }

        bool GraphMarker::inside(ssize_t mx, ssize_t my)
        {
            if (!sEditable.get())
                return false;

            // Get graph
            Graph *cv   = graph();
            if (cv == NULL)
                return false;

            mx     -= cv->canvas_aleft();
            my      = (cv->canvas_height()) - (my - cv->canvas_atop());

            lsp_trace("mx = %d, my = %d", int(mx), int(my));

            // Get basis
            GraphAxis *basis        = cv->axis(sBasis.get());
            if (basis == NULL)
                return false;
            GraphAxis *parallel     = cv->axis(sParallel.get());
            if (parallel == NULL)
                return false;

            float fvalue            = sValue.get();
            float x = 0.0f, y = 0.0f;
            cv->origin(sOrigin.get(), &x, &y);

            // Translate point and get the coordinates of point that lays on the target line
            if (!basis->apply(&x, &y, &fvalue, 1))
                return false;
            if (fOffset != 0.0f)
            {
                if (!parallel->apply(&x, &y, &fOffset, 1))
                    return false;
            }

            // Get equation of the line that contains calculated point
            float a1, b1, c1;
            float a2, b2, c2;
            float nx, ny;

            float scaling = lsp_max(0.0f, sScaling.get());
            ssize_t width = (nXFlags & F_HIGHLIGHT) ? sHWidth.get() : sWidth.get();
            if (width <= 0)
                return false;

            float range   = lsp_max(3.0f, width * scaling); // Minimum 3 pixels width, otherwise we won't be able to catch up the marker

            if (!parallel->parallel(x, y, a1, b1, c1))
                return false;
            if (!basis->parallel(mx, my, a2, b2, c2))
                return false;
            if (!line2d_intersection(a1, b1, c1, a2, b2, c2, nx, ny))
                return false;

            return distance2d(nx, ny, mx, my) <= range;
        }

        status_t GraphMarker::on_mouse_in(const ws::event_t *e)
        {
            if (!sEditable.get())
                return STATUS_OK;

            nXFlags |= F_HIGHLIGHT;
            query_draw();

            return GraphItem::on_mouse_in(e);
        }

        status_t GraphMarker::on_mouse_out(const ws::event_t *e)
        {
            if (!sEditable.get())
                return STATUS_OK;

            nXFlags &= ~F_HIGHLIGHT;
            query_draw();

            return GraphItem::on_mouse_out(e);
        }
    }
}


