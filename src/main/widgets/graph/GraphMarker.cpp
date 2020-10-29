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
            sOffset(&sProperties),
            sStep(&sProperties),
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
            nXFlags             = 0;
            nMBState            = 0;
            nMouseX             = 0;
            nMouseY             = 0;
            fLastValue          = 0.0f;

            pClass              = &metadata;
        }

        GraphMarker::~GraphMarker()
        {
            nFlags     |= FINALIZED;
        }

        status_t GraphMarker::init()
        {
            status_t res        = GraphItem::init();
            if (res != STATUS_OK)
                return res;

            // Disable automatic limitation of value
            sValue.set_auto_limit(false);

            // Init style
            sOrigin.bind("origin", &sStyle);
            sBasis.bind("basis", &sStyle);
            sParallel.bind("parallel", &sStyle);
            sValue.bind("value", &sStyle);
            sOffset.bind("value.offset", &sStyle);
            sStep.bind("step", &sStyle);
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
                sValue.init(sclass, 0.0f, -1.0f, 1.0f);
                sOffset.init(sclass, 0.0f);
                sStep.init(sclass, 1.0f, 10.0f, 0.1f);
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

                sSmooth.override(sclass, false);
            }

            // Add handler
            handler_id_t id = 0;
            id = sSlots.add(SLOT_CHANGE, slot_on_change, self());
            if (id < 0)
                return -id;

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
            if (sOffset.is(prop))
                query_draw();
            if (sStep.is(prop))
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
            float off = sOffset.get();
            if (off != 0.0f)
            {
                if (!parallel->apply(&x, &y, &off, 1))
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

            mx      = mx - cv->canvas_aleft();
            my      = my - cv->canvas_atop();

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
            float off = sOffset.get();
            if (off != 0.0f)
            {
                if (!parallel->apply(&x, &y, &off, 1))
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

        status_t GraphMarker::on_mouse_down(const ws::event_t *e)
        {
            size_t state = nMBState;
            nMBState    |= 1 << e->nCode;

            if (state == 0)
            {
                nMouseX     = e->nLeft;
                nMouseY     = e->nTop;
                fLastValue  = sValue.get();
                nXFlags    |= F_EDITING;

                if (e->nCode == ws::MCB_RIGHT)
                    nXFlags    |= F_FINE_TUNE;
            }

            apply_motion(e->nLeft, e->nTop, e->nState);

            return STATUS_OK;
        }

        status_t GraphMarker::on_mouse_up(const ws::event_t *e)
        {
            if ((!(nXFlags & F_EDITING)) || (nMBState == 0))
                return STATUS_OK;

            apply_motion(e->nLeft, e->nTop, e->nState);

            nMBState       &= ~(1 << e->nCode);
            if (nMBState == 0)
                nXFlags    &= ~(F_FINE_TUNE | F_EDITING);

            return STATUS_OK;
        }

        status_t GraphMarker::on_mouse_move(const ws::event_t *e)
        {
            if (nMBState != 0)
                apply_motion(e->nLeft, e->nTop, e->nState);

            return STATUS_OK;
        }

        void GraphMarker::apply_motion(ssize_t x, ssize_t y, size_t flags)
        {
            // Get graph
            Graph *cv = graph();
            if (cv == NULL)
                return;

            // Ignore canvas coordinates

            // Get axises
            GraphAxis *basis    = cv->axis(sBasis.get());
            if (basis == NULL)
                return;
            GraphAxis *parallel = cv->axis(sParallel.get());
            if (parallel == NULL)
                return;

            // Check that mouse button state matches
            size_t bflag    = (nXFlags & F_FINE_TUNE) ? ws::MCF_RIGHT : ws::MCF_LEFT;
            if (nMBState != bflag)
            {
                x       = nMouseX;
                y       = nMouseY;
            }

            // Update the difference relative to the sensitivity
            lsp_trace("xy=(%d, %d), mxy=(%d, %d)",
                    int(x), int(y), int(nMouseX), int(nMouseY));

            float step = (nXFlags & F_FINE_TUNE) ?
                sStep.get(flags & ws::MCF_CONTROL, !(flags & ws::MCF_SHIFT)) :
                sStep.get(flags & ws::MCF_CONTROL, flags & ws::MCF_SHIFT);

            float dx = x - nMouseX, dy = y - nMouseY;
            float rx = nMouseX - cv->canvas_aleft() + step * dx;
            float ry = nMouseY - cv->canvas_atop() + step * dy;

            lsp_trace("rxy=(%f, %f)", rx, ry);

            // Modify the value according to X coordinate
            float old       = sValue.get();
            float nvalue    = fLastValue;
            if ((nMouseX == x) && (nMouseY == y))
                nvalue          = fLastValue;
            else if (basis != NULL)
                nvalue          = basis->project(rx, ry);
            nvalue          = sValue.limit(nvalue);

            // Query widget for redraw
            if (nvalue != old)
            {
                sValue.set(nvalue);
                sSlots.execute(SLOT_CHANGE, this);
            }
            query_draw();
        }

        status_t GraphMarker::slot_on_change(Widget *sender, void *ptr, void *data)
        {
            GraphMarker *_this = widget_ptrcast<GraphMarker>(ptr);
            return (_this != NULL) ? _this->on_change() : STATUS_BAD_ARGUMENTS;
        }

        status_t GraphMarker::on_change()
        {
            lsp_trace("value = %f", sValue.get());
            return STATUS_OK;
        }
    }
}


