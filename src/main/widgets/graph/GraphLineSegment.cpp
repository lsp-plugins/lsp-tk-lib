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
                sHAxis.bind("haxis", this);
                sVAxis.bind("vaxis", this);
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
                sHAxis.set(0);
                sVAxis.set(1);
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

        } /* namespace style */

        const w_class_t GraphLineSegment::metadata             = { "GraphLineSegment", &GraphItem::metadata };

        GraphLineSegment::GraphLineSegment(Display *dpy):
            GraphItem(dpy),
            sOrigin(&sProperties),
            sHAxis(&sProperties),
            sVAxis(&sProperties),
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
            sHAxis.bind("haxis", &sStyle);
            sVAxis.bind("vaxis", &sStyle);
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

            if (prop->one_of(sOrigin, sHAxis, sVAxis, sValue, sBegin, sEnd))
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

            // Get axes
            GraphAxis *abscissa = cv->axis(sHAxis.get());
            if (abscissa == NULL)
                return;
            GraphAxis *ordinate = cv->axis(sVAxis.get());
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

            prop::Integer *w = (nXFlags & F_HIGHLIGHT) ? &sHWidth : &sWidth;
            ssize_t l_width  = (w->get() > 0) ? lsp_max(1.0f, w->get() * scaling) : 0;

            // Draw borders
            ssize_t lborder = (nXFlags & F_HIGHLIGHT) ? sHLBorder.get() : sLBorder.get();
            ssize_t rborder = (nXFlags & F_HIGHLIGHT) ? sHRBorder.get() : sRBorder.get();
            lborder         = (lborder > 0) ? lsp_max(1.0f, lborder * scaling) : 0;
            rborder         = (rborder > 0) ? lsp_max(1.0f, rborder * scaling) : 0;

            if ((lborder > 0) || (rborder > 0))
            {
                vec2f_t dir;
                if (normalized_vec2f(dir, x[0], y[0], x[1], y[1]))
                {
                    vec2f_t perp    = perp2f(dir);
                    float hw        = lsp_min(0.0f, l_width * 0.5f);

                    if (lborder > 0)
                    {
                        dot2f_t p1      = shift2f(x[0], y[0], perp, -hw);
                        dot2f_t p2      = shift2f(x[0], y[0], perp, -(lborder + hw));
                        ws::IGradient *g = s->linear_gradient(p1.x, p1.y, p2.x, p2.y);
                        if (g != NULL)
                        {
                            lsp_finally { delete g; };
                            g->add_color(0.0f, bcol_l);
                            g->add_color(1.0f, bcol_l, 1.0f);

                            dot2f_t p3      = shift2f(x[1], y[1], perp, -(lborder + hw));
                            dot2f_t p4      = shift2f(x[1], y[1], perp, -hw);
                            float px[5]     = { p1.x, p2.x, p3.x, p4.x, p1.x };
                            float py[5]     = { p1.y, p2.y, p3.y, p4.y, p1.y };
                            s->fill_poly(g, px, py, 5);
                        }
                    }

                    if (rborder > 0)
                    {
                        dot2f_t p1      = shift2f(x[0], y[0], perp, hw);
                        dot2f_t p2      = shift2f(x[0], y[0], perp, (rborder + hw));
                        ws::IGradient *g = s->linear_gradient(p1.x, p1.y, p2.x, p2.y);
                        if (g != NULL)
                        {
                            lsp_finally { delete g; };
                            g->add_color(0.0f, bcol_l);
                            g->add_color(1.0f, bcol_l, 1.0f);

                            dot2f_t p3      = shift2f(x[1], y[1], perp, (rborder + hw));
                            dot2f_t p4      = shift2f(x[1], y[1], perp, hw);
                            float px[5]     = { p1.x, p2.x, p3.x, p4.x, p1.x };
                            float py[5]     = { p1.y, p2.y, p3.y, p4.y, p1.y };
                            s->fill_poly(g, px, py, 5);
                        }
                    }
                }
            }

            // Draw line
            bool aa     = s->set_antialiasing(sSmooth.get());
            lsp_finally { s->set_antialiasing(aa); };
            s->line(
                color,
                x[0], y[0], x[1], y[1],
                l_width);

            // DEBUG: Draw surrounding triangles (for debug) if possible
//            vec2f_t dir;
//            if (normalized_vec2f(dir, x[0], y[0], x[1], y[1]))
//            {
//                vec2f_t perp    = perp2f(dir);
//                // Compute the half-width with scaling applied, minimum 3 pixels width, otherwise we won't catch up the widget
//                float hw        = lsp_max(3.0f, l_width) * 0.5f;
//
//                // Compute dots shifted by 1 pixel before the line segment and after line segment by it's line
//                dot2f_t p1      = shift2f(x[0], y[0], dir, -1.0f);
//                dot2f_t p2      = shift2f(x[1], y[1], dir,  1.0f);
//
//                // Compute the surrounding rectangle dots
//                dot2f_t poly[4];
//                poly[0]         = shift2f(p1, perp, hw);
//                poly[1]         = shift2f(p2, perp, hw);
//                poly[2]         = shift2f(p2, perp, -hw);
//                poly[3]         = shift2f(p1, perp, -hw);
//
//                // Triangulize the rectangle
//                triangle2f_t t1 = {
//                    poly[0], poly[1], poly[2]
//                };
//                triangle2f_t t2 = {
//                    poly[0], poly[2], poly[3]
//                };
//
//                // Draw the rectangle
//                lsp::Color dbg;
//
//                dbg.set_rgb24(0xff0000);
//                s->line(dbg, t1.v[0].x, t1.v[0].y, t1.v[1].x, t1.v[1].y, 1.0f);
//                s->line(dbg, t1.v[1].x, t1.v[1].y, t1.v[2].x, t1.v[2].y, 1.0f);
//                s->line(dbg, t1.v[2].x, t1.v[2].y, t1.v[0].x, t1.v[0].y, 1.0f);
//                s->line(dbg, t2.v[0].x, t2.v[0].y, t2.v[1].x, t2.v[1].y, 1.0f);
//                s->line(dbg, t2.v[1].x, t2.v[1].y, t2.v[2].x, t2.v[2].y, 1.0f);
//                s->line(dbg, t2.v[2].x, t2.v[2].y, t2.v[0].x, t2.v[0].y, 1.0f);
//            }
        }

        bool GraphLineSegment::inside(ssize_t mx, ssize_t my)
        {
            if (!sEditable.get())
                return false;

            // Get graph
            Graph *cv   = graph();
            if (cv == NULL)
                return false;

            // Get axes
            GraphAxis *abscissa = cv->axis(sHAxis.get());
            if (abscissa == NULL)
                return false;
            GraphAxis *ordinate = cv->axis(sVAxis.get());
            if (ordinate == NULL)
                return false;

            // Compute point coordinates
            float x[2]  = { 0.0f, 0.0f };
            float y[2]  = { 0.0f, 0.0f };
            float dx[2] = { sBegin.x(), sEnd.x() };
            float dy[2] = { sBegin.y(), sEnd.y() };
            cv->origin(sOrigin.get(), &x[0], &y[0]);
            x[1]    = x[0];
            y[1]    = y[0];

            if (!abscissa->apply(x, y, dx, 2))
                return false;
            if (!ordinate->apply(x, y, dy, 2))
                return false;

            dot2f_t mp = {
                float(mx - cv->canvas_aleft()),
                float(my - cv->canvas_atop())
            };

            // Check whether the mouse pointer is inside of the line segment
            // We can operate only on the non-zero length of the line segment
            vec2f_t dir;
            if (!normalized_vec2f(dir, x[0], y[0], x[1], y[1]))
                return false;

            // Compute the half-width with scaling applied, minimum 3 pixels width, otherwise we won't catch up the widget
            float scaling   = lsp_max(0.0f, sScaling.get());
            float hw        = lsp_max(1.5f, sWidth.get() * scaling * 0.5f);

            // Compute dots shifted by 1 pixel before the line segment and after line segment by it's line
            vec2f_t perp    = perp2f(dir);
            dot2f_t p1      = shift2f(x[0], y[0], dir, -1.0f);
            dot2f_t p2      = shift2f(x[1], y[1], dir,  1.0f);

            // Compute the surrounding rectangle dots
            dot2f_t p[4];
            p[0]            = shift2f(p1, perp, hw);
            p[1]            = shift2f(p2, perp, hw);
            p[2]            = shift2f(p2, perp, -hw);

            // If we're inside of at least of one of triangles, then all is OK
            if (tk::inside(p[0], p[1], p[2], mp))
                return true;

            p[3]            = shift2f(p1, perp, -hw);
            if (tk::inside(p[0], p[2], p[3], mp))
                return true;

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



