/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 17 июн. 2023 г.
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
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(GraphLineSegment, GraphItem)
                // Bind
                sOrigin.bind("origin", this);
                sAbscissa.bind("abscissa", this);
                sOrdinate.bind("ordinate", this);
                sValue.bind("value", this);
                sStep.bind("step", this);
                sBegin.bind("begin", this);
                sEnd.bind("end", this);
                sWidth.bind("width", this);
                sHWidth.bind("hover.width", this);
                sEditable.bind("editable", this);
                sLBorder.bind("border.left.size", this);
                sRBorder.bind("border.right.size", this);
                sHLBorder.bind("hover.border.left.size", this);
                sHRBorder.bind("hover.border.right.size", this);
                sColor.bind("color", this);
                sHColor.bind("hover.color", this);
                sLBorderColor.bind("border.left.color", this);
                sRBorderColor.bind("border.right.color", this);
                sHLBorderColor.bind("hover.border.left.color", this);
                sHRBorderColor.bind("hover.border.right.color", this);
                // Configure
                sOrigin.set(0);
                sAbscissa.set(0);
                sOrdinate.set(1);
                sValue.set_all(0.0f, -1.0f, 1.0f);
                sStep.set(1.0f, 10.0f, 0.1f);
                sBegin.set(0.0f, 0.0f);
                sEnd.set(1.0f, 0.0f);
                sWidth.set(1);
                sHWidth.set(3);
                sEditable.set(false);
                sLBorder.set(0);
                sRBorder.set(0);
                sHLBorder.set(0);
                sHRBorder.set(0);
                sColor.set("#ffffff");
                sHColor.set("#ffffff");
                sLBorderColor.set("#ffffff");
                sRBorderColor.set("#ffffff");
                sHLBorderColor.set("#ffffff");
                sHRBorderColor.set("#ffffff");
                // Override
                sSmooth.set(false);
                // Commit
                sSmooth.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(GraphLineSegment, "GraphLineSegment", "root");
        }

        const w_class_t GraphLineSegment::metadata             = { "GraphLineSegment", &GraphItem::metadata };

        GraphLineSegment::GraphLineSegment(Display *dpy):
            GraphItem(dpy),
            sOrigin(&sProperties),
            sAbscissa(&sProperties),
            sOrdinate(&sProperties),
            sValue(&sProperties),
            sStep(&sProperties),
            sBegin(&sProperties),
            sEnd(&sProperties),
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

        GraphLineSegment::~GraphLineSegment()
        {
            nFlags     |= FINALIZED;
        }

        status_t GraphLineSegment::init()
        {
            status_t res        = GraphItem::init();
            if (res != STATUS_OK)
                return res;

            // Disable automatic limitation of value
            sValue.set_auto_limit(false);

            // Init style
            sOrigin.bind("origin", &sStyle);
            sAbscissa.bind("basis", &sStyle);
            sOrdinate.bind("parallel", &sStyle);
            sValue.bind("value", &sStyle);
            sStep.bind("step", &sStyle);
            sBegin.bind("begin", &sStyle);
            sEnd.bind("end", &sStyle);
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

            // Add handler
            handler_id_t id = 0;
            id = sSlots.add(SLOT_CHANGE, slot_on_change, self());
            if (id >= 0) id = sSlots.add(SLOT_BEGIN_EDIT, slot_begin_edit, self());
            if (id >= 0) id = sSlots.add(SLOT_END_EDIT, slot_end_edit, self());
            if (id < 0)
                return -id;

            return STATUS_OK;
        }

        void GraphLineSegment::property_changed(Property *prop)
        {
            GraphItem::property_changed(prop);

            if (prop->one_of(sOrigin, sAbscissa, sOrdinate, sValue, sBegin, sEnd))
                query_draw();
            if (sStep.is(prop))
                {/* nothing */}
            if (prop->one_of(sWidth, sHWidth, sLBorder, sRBorder, sHLBorder, sHRBorder))
                query_draw();
            if (sEditable.is(prop))
                query_draw();
            if (prop->one_of(sColor, sHColor, sLBorderColor, sRBorderColor, sHLBorderColor, sHRBorderColor))
                query_draw();
        }

        void GraphLineSegment::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
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

            bcol_l.scale_lch_luminance(brightness);
            bcol_r.scale_lch_luminance(brightness);
            color.scale_lch_luminance(brightness);

            // Get basis
            GraphAxis *abscissa = cv->axis(sAbscissa.get());
            if (abscissa == NULL)
                return;
            GraphAxis *ordinate = cv->axis(sOrdinate.get());
            if (ordinate == NULL)
                return;

            // Compute point coordinates
            float x[2]  = { 0.0f, 0.0f };
            float y[2]  = { 0.0f, 0.0f };
            float dx[2] = { sBegin.x(), sEnd.x() };
            float dy[2] = { sBegin.y(), sEnd.y() };
            cv->origin(sOrigin.get(), &x[0], &y[0]);
            x[1]    = x[0];
            y[1]    = y[0];

            if (!abscissa->apply(x, y, dx, 2))
                return;
            if (!ordinate->apply(x, y, dy, 2))
                return;

            // Draw line
            bool aa     = s->set_antialiasing(sSmooth.get());
            lsp_finally { s->set_antialiasing(aa); };

            prop::Integer *w = (nXFlags & F_HIGHLIGHT) ? &sHWidth : &sWidth;
            ssize_t l_width  = (w->get() > 0) ? lsp_max(1.0f, w->get() * scaling) : 0;

//            if (lborder > 0)
//            {
//                ws::IGradient *g = s->linear_gradient(x, y, nl[0], nl[1]);
//                if (g != NULL)
//                {
//                    g->add_color(0.0f, bcol_l);
//                    g->add_color(1.0f, bcol_l, 1.0f);
//
//                    s->parametric_bar(
//                        g,
//                        ll[0], ll[1], ll[2], bl[0], bl[1], bl[2],
//                        cv->canvas_left(), cv->canvas_right(), cv->canvas_bottom(), cv->canvas_top());
//
//                    delete g;
//                }
//            }
//            if (rborder > 0)
//            {
//                ws::IGradient *g = s->linear_gradient(x, y, nr[0], nr[1]);
//                if (g != NULL)
//                {
//                    g->add_color(0.0f, bcol_r);
//                    g->add_color(1.0f, bcol_r, 1.0f);
//
//                    s->parametric_bar(
//                        g,
//                        ll[0], ll[1], ll[2], br[0], br[1], br[2],
//                        cv->canvas_left(), cv->canvas_right(), cv->canvas_bottom(), cv->canvas_top());
//
//                    delete g;
//                }
//            }

            s->line(
                color,
                x[0], y[0], x[1], y[1],
                l_width);

//
//            // Get equation of the line that contains calculated point
//            float ll[3], bl[3], br[3];
//            float nl[2], nr[2];
//            float fangle    = sDirection.rphi();
//            ssize_t lborder = (nXFlags & F_HIGHLIGHT) ? sHLBorder.get() : sLBorder.get();
//            ssize_t rborder = (nXFlags & F_HIGHLIGHT) ? sHRBorder.get() : sRBorder.get();
//            lborder         = (lborder > 0) ? lsp_max(1.0f, lborder * scaling) : 0;
//            rborder         = (rborder > 0) ? lsp_max(1.0f, rborder * scaling) : 0;
//
//            if (fangle == 0.0f)
//            {
//                if (!parallel->parallel(x, y, ll[0], ll[1], ll[2]))
//                    return;
//                if (lborder != 0)
//                {
//                    parallel->ortogonal_shift(x, y, lborder, nl[0], nl[1]);
//                    if (!parallel->parallel(nl[0], nl[1], bl[0], bl[1], bl[2]))
//                        return;
//                }
//                if (rborder != 0)
//                {
//                    parallel->ortogonal_shift(x, y, -rborder, nr[0], nr[1]);
//                    if (!parallel->parallel(nr[0], nr[1], br[0], br[1], br[2]))
//                        return;
//                }
//            }
//            else
//            {
//                if (!parallel->angle(x, y, fangle, ll[0], ll[1], ll[2]))
//                    return;
//                if (lborder != 0)
//                {
//                    parallel->rotate_shift(x, y, fangle, lborder, nl[0], nl[1]);
//                    if (!parallel->angle(x, y, fangle, bl[0], bl[1], bl[2]))
//                        return;
//                }
//                if (rborder != 0)
//                {
//                    parallel->rotate_shift(x, y, fangle, -rborder, nr[0], nr[1]);
//                    if (!parallel->angle(x, y, fangle, br[0], br[1], br[2]))
//                        return;
//                }
//            }
//
//            // Draw line
//            bool aa     = s->set_antialiasing(sSmooth.get());
//
//            prop::Integer *w = (nXFlags & F_HIGHLIGHT) ? &sHWidth : &sWidth;
//            ssize_t l_width  = (w->get() > 0) ? lsp_max(1.0f, w->get() * scaling) : 0;
//
//            if (lborder > 0)
//            {
//                ws::IGradient *g = s->linear_gradient(x, y, nl[0], nl[1]);
//                if (g != NULL)
//                {
//                    g->add_color(0.0f, bcol_l);
//                    g->add_color(1.0f, bcol_l, 1.0f);
//
//                    s->parametric_bar(
//                        g,
//                        ll[0], ll[1], ll[2], bl[0], bl[1], bl[2],
//                        cv->canvas_left(), cv->canvas_right(), cv->canvas_bottom(), cv->canvas_top());
//
//                    delete g;
//                }
//            }
//            if (rborder > 0)
//            {
//                ws::IGradient *g = s->linear_gradient(x, y, nr[0], nr[1]);
//                if (g != NULL)
//                {
//                    g->add_color(0.0f, bcol_r);
//                    g->add_color(1.0f, bcol_r, 1.0f);
//
//                    s->parametric_bar(
//                        g,
//                        ll[0], ll[1], ll[2], br[0], br[1], br[2],
//                        cv->canvas_left(), cv->canvas_right(), cv->canvas_bottom(), cv->canvas_top());
//
//                    delete g;
//                }
//            }
//
//            s->parametric_line(
//                color,
//                ll[0], ll[1], ll[2],
//                cv->canvas_left(), cv->canvas_right(), cv->canvas_bottom(), cv->canvas_top(),
//                l_width);
//
//            s->set_antialiasing(aa);
        }

        bool GraphLineSegment::inside(ssize_t mx, ssize_t my)
        {
            if (!sEditable.get())
                return false;

            // Get graph
            Graph *cv   = graph();
            if (cv == NULL)
                return false;

//            mx      = mx - cv->canvas_aleft();
//            my      = my - cv->canvas_atop();
//
//            // Get basis
//            GraphAxis *basis        = cv->axis(sBasis.get());
//            if (basis == NULL)
//                return false;
//            GraphAxis *parallel     = cv->axis(sParallel.get());
//            if (parallel == NULL)
//                return false;
//
//            float fvalue            = sValue.get();
//            float x = 0.0f, y = 0.0f;
//            cv->origin(sOrigin.get(), &x, &y);
//
//            // Translate point and get the coordinates of point that lays on the target line
//            if (!basis->apply(&x, &y, &fvalue, 1))
//                return false;
//            float off = sOffset.get();
//            if (off != 0.0f)
//            {
//                if (!parallel->apply(&x, &y, &off, 1))
//                    return false;
//            }
//
//            // Get equation of the line that contains calculated point
//            float a1, b1, c1;
//            float a2, b2, c2;
//            float nx, ny;
//
//            float scaling = lsp_max(0.0f, sScaling.get());
//            ssize_t width = (nXFlags & F_HIGHLIGHT) ? sHWidth.get() : sWidth.get();
//            if (width <= 0)
//                return false;
//
//            float range   = lsp_max(3.0f, width * scaling); // Minimum 3 pixels width, otherwise we won't be able to catch up the marker
//
//            if (!parallel->parallel(x, y, a1, b1, c1))
//                return false;
//            if (!basis->parallel(mx, my, a2, b2, c2))
//                return false;
//            if (!line2d_intersection(a1, b1, c1, a2, b2, c2, nx, ny))
//                return false;
//
//            return distance2d(nx, ny, mx, my) <= range;

            return false;
        }

        status_t GraphLineSegment::on_mouse_in(const ws::event_t *e)
        {
            if (!sEditable.get())
                return STATUS_OK;

            nXFlags |= F_HIGHLIGHT;
            query_draw();

            return GraphItem::on_mouse_in(e);
        }

        status_t GraphLineSegment::on_mouse_out(const ws::event_t *e)
        {
            if (!sEditable.get())
                return STATUS_OK;

            nXFlags &= ~F_HIGHLIGHT;
            query_draw();

            return GraphItem::on_mouse_out(e);
        }

        status_t GraphLineSegment::on_mouse_down(const ws::event_t *e)
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

                sSlots.execute(SLOT_BEGIN_EDIT, this);
            }

            apply_motion(e->nLeft, e->nTop, e->nState);

            return STATUS_OK;
        }

        status_t GraphLineSegment::on_mouse_up(const ws::event_t *e)
        {
            if ((!(nXFlags & F_EDITING)) || (nMBState == 0))
                return STATUS_OK;

            apply_motion(e->nLeft, e->nTop, e->nState);

            nMBState       &= ~(1 << e->nCode);
            if (nMBState == 0)
            {
                nXFlags    &= ~(F_FINE_TUNE | F_EDITING);
                sSlots.execute(SLOT_END_EDIT, this);
            }

            return STATUS_OK;
        }

        status_t GraphLineSegment::on_mouse_move(const ws::event_t *e)
        {
            if (nMBState != 0)
                apply_motion(e->nLeft, e->nTop, e->nState);

            return STATUS_OK;
        }

        void GraphLineSegment::apply_motion(ssize_t x, ssize_t y, size_t flags)
        {
            // Get graph
            Graph *cv = graph();
            if (cv == NULL)
                return;

            // Ignore canvas coordinates

            // Get axises
//            GraphAxis *basis    = cv->axis(sBasis.get());
//            if (basis == NULL)
//                return;
//            GraphAxis *parallel = cv->axis(sParallel.get());
//            if (parallel == NULL)
//                return;
//
//            // Check that mouse button state matches
//            size_t bflag    = (nXFlags & F_FINE_TUNE) ? ws::MCF_RIGHT : ws::MCF_LEFT;
//            if (nMBState != bflag)
//            {
//                x       = nMouseX;
//                y       = nMouseY;
//            }
//
//            // Update the difference relative to the sensitivity
////            lsp_trace("xy=(%d, %d), mxy=(%d, %d)",
////                    int(x), int(y), int(nMouseX), int(nMouseY));
//
//            float step = (nXFlags & F_FINE_TUNE) ?
//                sStep.get(flags & ws::MCF_CONTROL, !(flags & ws::MCF_SHIFT)) :
//                sStep.get(flags & ws::MCF_CONTROL, flags & ws::MCF_SHIFT);
//
//            float dx = x - nMouseX, dy = y - nMouseY;
//            float rx = nMouseX - cv->canvas_aleft() + step * dx;
//            float ry = nMouseY - cv->canvas_atop() + step * dy;
//
////            lsp_trace("rxy=(%f, %f)", rx, ry);
//
//            // Modify the value according to X coordinate
//            float old       = sValue.get();
//            float nvalue    = fLastValue;
//            if ((nMouseX == x) && (nMouseY == y))
//                nvalue          = fLastValue;
//            else if (basis != NULL)
//                nvalue          = basis->project(rx, ry);
//            nvalue          = sValue.limit(nvalue);
//
//            // Query widget for redraw
//            if (nvalue != old)
//            {
//                sValue.set(nvalue);
//                sSlots.execute(SLOT_CHANGE, this);
//            }
//            query_draw();
        }

        status_t GraphLineSegment::slot_on_change(Widget *sender, void *ptr, void *data)
        {
            GraphLineSegment *_this = widget_ptrcast<GraphLineSegment>(ptr);
            return (_this != NULL) ? _this->on_change() : STATUS_BAD_ARGUMENTS;
        }

        status_t GraphLineSegment::slot_begin_edit(Widget *sender, void *ptr, void *data)
        {
            GraphLineSegment *_this = widget_ptrcast<GraphLineSegment>(ptr);
            return (_this != NULL) ? _this->on_begin_edit() : STATUS_BAD_ARGUMENTS;
        }

        status_t GraphLineSegment::slot_end_edit(Widget *sender, void *ptr, void *data)
        {
            GraphLineSegment *_this = widget_ptrcast<GraphLineSegment>(ptr);
            return (_this != NULL) ? _this->on_end_edit() : STATUS_BAD_ARGUMENTS;
        }

        status_t GraphLineSegment::on_change()
        {
            return STATUS_OK;
        }

        status_t GraphLineSegment::on_begin_edit()
        {
            return STATUS_OK;
        }

        status_t GraphLineSegment::on_end_edit()
        {
            return STATUS_OK;
        }

    } /* namespace tk */
} /* namespace lsp */



