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
#include <lsp-plug.in/common/debug.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        STYLE_INITIALIZER_BEGIN(GraphDot, GraphItem);

            prop::Integer::init("origin", style, 0);
            prop::Integer::init("haxis", style, 0);
            prop::Integer::init("vaxis", style, 1);
            prop::Integer::init("size", style, 4);
            prop::Integer::init("hover.size", style, 4);
            prop::Integer::init("border.size", style, 0);
            prop::Integer::init("hover.border.size", style, 12);
            prop::Integer::init("gap", style, 1);
            prop::Integer::init("hover.gap", style, 1);
            prop::Color::init("color", style, "#cccccc");
            prop::Color::init("hover.color", style, "#ffffff");
            prop::Color::init("border.color", style, "#cccccc");
            prop::Color::init("hover.border.color", style, "#ffffff");
            prop::Color::init("gap.color", style, "#000000");
            prop::Color::init("hover.gap.color", style, "#000000");

            prop::Boolean::init("hvalue.editable", style, false);
            prop::RangeFloat::init("hvalue.value", style, 0.0f, -1.0f, 1.0f);
            prop::StepFloat::init("hvalue.step", style, 1.0f, 10.0f, 0.1f);
            prop::Boolean::init("vvalue.editable", style, false);
            prop::RangeFloat::init("vvalue.value", style, 0.0f, -1.0f, 1.0f);
            prop::StepFloat::init("vvalue.step", style, 1.0f, 10.0f, 0.1f);
            prop::Boolean::init("zvalue.editable", style, false);
            prop::RangeFloat::init("zvalue.value", style, 0.0f, -1.0f, 1.0f);
            prop::StepFloat::init("zvalue.step", style, 1.0f, 10.0f, 0.1f);

        STYLE_INITIALIZER_END(GraphDot, "GraphDot");
        LSP_BUILTIN_STYLE(GraphDot);

        const w_class_t GraphDot::metadata             = { "GraphDot", &GraphItem::metadata };

        GraphDot::param_t::param_t(GraphDot *dot, prop::Listener *lst):
            sEditable(lst),
            sValue(lst),
            sStep(lst)
        {
            pDot        = dot;
        }

        void GraphDot::param_t::property_changed(Property *prop)
        {
            if (sEditable.is(prop))
            {
                bool editable =
                    (pDot->sHValue.sEditable.get()) ||
                    (pDot->sVValue.sEditable.get()) ||
                    (pDot->sZValue.sEditable.get());

                pDot->nXFlags = lsp_setflag(pDot->nXFlags, F_EDITABLE, editable);
                pDot->query_draw();
            }
            if (sValue.is(prop))
                pDot->query_draw();
            if (sStep.is(prop))
                { /* Nothing */ }
        }

        void GraphDot::param_t::init(const char *prefix)
        {
            Style *style  = pDot->style();
            LSPString id;
            id.fmt_ascii("%s.editable", prefix);
            sEditable.bind(&id, style);
            id.fmt_ascii("%s.value", prefix);
            sValue.bind(&id, style);
            id.fmt_ascii("%s.step", prefix);
            sStep.bind(&id, style);

            Style *sclass = pDot->style_class();
            if (sclass != NULL)
            {
                id.fmt_ascii("%s.editable", prefix);
                sEditable.init(sclass, false);
                id.fmt_ascii("%s.value", prefix);
                sValue.init(sclass, 0.0f, -1.0f, 1.0f);
                id.fmt_ascii("%s.step", prefix);
                sStep.init(sclass, 1.0f, 10.0f, 0.1f);
            }
        }

        GraphDot::GraphDot(Display *dpy):
            GraphItem(dpy),
            sHValue(this, &sProperties),
            sVValue(this, &sProperties),
            sZValue(this, &sProperties),
            sOrigin(&sProperties),
            sHAxis(&sProperties),
            sVAxis(&sProperties),
            sSize(&sProperties),
            sHoverSize(&sProperties),
            sBorderSize(&sProperties),
            sHoverBorderSize(&sProperties),
            sGap(&sProperties),
            sHoverGap(&sProperties),
            sColor(&sProperties),
            sHoverColor(&sProperties),
            sBorderColor(&sProperties),
            sHoverBorderColor(&sProperties),
            sGapColor(&sProperties),
            sHoverGapColor(&sProperties)
        {
            nXFlags             = 0;
            nMBState            = 0;
            nMouseX             = 0;
            nMouseY             = 0;
            fLastX              = 0;
            fLastY              = 0;

            pClass              = &metadata;
        }

        GraphDot::~GraphDot()
        {
            nFlags     |= FINALIZED;
        }

        status_t GraphDot::init()
        {
            status_t res        = GraphItem::init();
            if (res != STATUS_OK)
                return res;

            // Init values
            sHValue.init("hvalue");
            sVValue.init("vvalue");
            sZValue.init("zvalue");

            // Init style
            sOrigin.bind("origin", &sStyle);
            sHAxis.bind("haxis", &sStyle);
            sVAxis.bind("vaxis", &sStyle);
            sSize.bind("size", &sStyle);
            sHoverSize.bind("hover.size", &sStyle);
            sBorderSize.bind("border.size", &sStyle);
            sHoverBorderSize.bind("hover.border.size", &sStyle);
            sGap.bind("gap", &sStyle);
            sHoverGap.bind("hover.gap", &sStyle);
            sColor.bind("color", &sStyle);
            sHoverColor.bind("hover.color", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sHoverBorderColor.bind("hover.border.color", &sStyle);
            sGapColor.bind("gap.color", &sStyle);
            sHoverGapColor.bind("hover.gap.color", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sOrigin.init(sclass, 0);
                sHAxis.init(sclass, 0);
                sVAxis.init(sclass, 1);
                sSize.init(sclass, 4);
                sHoverSize.init(sclass, 4);
                sBorderSize.init(sclass, 0);
                sHoverBorderSize.init(sclass, 12);
                sGap.init(sclass, 1);
                sHoverGap.init(sclass, 1);
                sColor.init(sclass, "#cccccc");
                sHoverColor.init(sclass, "#ffffff");
                sBorderColor.init(sclass, "#cccccc");
                sHoverBorderColor.init(sclass, "#ffffff");
                sGapColor.init(sclass, "#000000");
                sHoverGapColor.init(sclass, "#000000");
            }

            // Add handler
            handler_id_t id = 0;
            id = sSlots.add(SLOT_CHANGE, slot_on_change, self());
            if (id < 0)
                return -id;

            return STATUS_OK;
        }

        void GraphDot::property_changed(Property *prop)
        {
            GraphItem::property_changed(prop);
            sHValue.property_changed(prop);
            sVValue.property_changed(prop);
            sZValue.property_changed(prop);

            if (sOrigin.is(prop))
                query_draw();
            if (sHAxis.is(prop))
                query_draw();
            if (sVAxis.is(prop))
                query_draw();
            if (sSize.is(prop))
                query_draw();
            if (sHoverSize.is(prop))
                query_draw();
            if (sBorderSize.is(prop))
                query_draw();
            if (sHoverBorderSize.is(prop))
                query_draw();
            if (sGap.is(prop))
                query_draw();
            if (sHoverGap.is(prop))
                query_draw();
            if (sColor.is(prop))
                query_draw();
            if (sHoverColor.is(prop))
                query_draw();
            if (sBorderColor.is(prop))
                query_draw();
            if (sHoverBorderColor.is(prop))
                query_draw();
        }

        void GraphDot::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            // Get graph
            Graph *cv = graph();
            if (cv == NULL)
                return;

            // Generate palette
            float scaling   = lsp_max(0.0f, sScaling.get());
            float bright    = sBrightness.get();

            // Prepare arguments
            float left  = sHValue.sValue.get();
            float top   = sVValue.sValue.get();

            // Get axises
            GraphAxis *basis    = cv->axis(sHAxis.get());
            if (basis == NULL)
                return;
            GraphAxis *parallel = cv->axis(sVAxis.get());
            if (parallel == NULL)
                return;

            // Locate the point at the center
            float x = 0.0f, y = 0.0f;
            cv->origin(sOrigin.get(), &x, &y);

            // Translate point and get the owner line
            basis->apply(&x, &y, &left, 1);
            parallel->apply(&x, &y, &top, 1);

            // Store real coordinates
            x = truncf(x);
            y = truncf(y);

            // Draw the dot
            ssize_t dot     = (nXFlags & F_HIGHLIGHT) ? sHoverSize.get() : sSize.get();
            ssize_t pad     = (nXFlags & F_HIGHLIGHT) ? sHoverGap.get() : sGap.get();
            ssize_t border  = (nXFlags & F_HIGHLIGHT) ? sHoverBorderSize.get() : sBorderSize.get();

            float fdot      = (dot > 0) ? lsp_max(1.0f, dot * scaling) : 0.0f;
            float fpad      = ((border > 0) && (pad > 0)) ? lsp_max(1.0f, pad * scaling) : 0.0f;
            float fborder   = (border > 0) ? lsp_max(1.0f, border * scaling) : 0.0f;

            bool aa         = s->set_antialiasing(true);

            if (border > 0)
            {
                float radius    = fdot + fpad + fborder;
                lsp::Color gcol((nXFlags & F_HIGHLIGHT) ? sHoverBorderColor : sBorderColor);
                gcol.scale_lightness(bright);

                // Draw border
                ws::IGradient *gr   = s->radial_gradient(x, y, 0.0f, x, y, radius);
                if (gr != NULL)
                {
                    gr->add_color(0.0f, gcol);
                    gr->add_color(1.0f, gcol, 1.0f);
                    s->fill_circle(x, y, radius, gr);
                    delete gr;
                }

                // Draw padding
                if (pad > 0)
                {
                    s->set_antialiasing(sSmooth.get());
                    lsp::Color hole((nXFlags & F_HIGHLIGHT) ? sHoverGapColor : sGapColor);
                    hole.scale_lightness(bright);
                    s->set_antialiasing(sSmooth.get());
                    s->fill_circle(x, y, fpad + fdot, hole);
                }
            }

            // Draw the inner contents
            lsp::Color color((nXFlags & F_HIGHLIGHT) ? sHoverColor : sColor);
            color.scale_lightness(bright);
            s->set_antialiasing(sSmooth.get());
            s->fill_circle(x, y, fdot, color);
            s->set_antialiasing(aa);
        }

        status_t GraphDot::slot_on_change(Widget *sender, void *ptr, void *data)
        {
            GraphDot *_this = widget_ptrcast<GraphDot>(ptr);
            return (_this != NULL) ? _this->on_change() : STATUS_BAD_ARGUMENTS;
        }

        status_t GraphDot::on_change()
        {
            lsp_trace("hvalue = %f, vvalue=%f, zvalue=%f",
                    sHValue.sValue.get(),
                    sVValue.sValue.get(),
                    sZValue.sValue.get()
                );
            return STATUS_OK;
        }

        bool GraphDot::inside(ssize_t mx, ssize_t my)
        {
            if (!(nXFlags & F_EDITABLE))
                return false;

            // Get graph
            Graph *cv = graph();
            if (cv == NULL)
                return false;

            // Prepare arguments
            float left  = sHValue.sValue.get();
            float top   = sVValue.sValue.get();

            // Get axises
            GraphAxis *basis    = cv->axis(sHAxis.get());
            if (basis == NULL)
                return false;
            GraphAxis *parallel = cv->axis(sVAxis.get());
            if (parallel == NULL)
                return false;

            // Locate the point at the center
            float x = 0.0f, y = 0.0f;
            cv->origin(sOrigin.get(), &x, &y);

            // Translate point and get the owner line
            basis->apply(&x, &y, &left, 1);
            parallel->apply(&x, &y, &top, 1);

            // Estimate size of the dot
            float scaling   = lsp_max(0.0f, sScaling.get());
            ssize_t border  = (nXFlags & F_HIGHLIGHT) ? sHoverBorderSize.get() : sBorderSize.get();
            ssize_t dot     = (nXFlags & F_HIGHLIGHT) ? sHoverSize.get() : sSize.get();
            ssize_t pad     = (nXFlags & F_HIGHLIGHT) ? sHoverGap.get() : sGap.get();

            float fdot      = (dot > 0) ? lsp_max(1.0f, dot * scaling) : 0.0f;
            float fpad      = ((border > 0) && (pad > 0)) ? lsp_max(1.0f, pad * scaling) : 0.0f;
            float r    = lsp_max(2.0f, fdot + fpad);

            // Update coordinates
            lsp_trace("mx = %d, my = %d", int(mx), int(my));

            float dx        = mx - cv->canvas_aleft() - x;
            float dy        = my - cv->canvas_atop()  - y;

            lsp_trace("x = %f, y = %f", x, y);
            lsp_trace("dx = %f, dy = %f", dx, dy);

            return dx*dx + dy*dy <= r*r;
        }

        void GraphDot::apply_motion(ssize_t x, ssize_t y, size_t flags)
        {
            // Get graph
            Graph *cv = graph();
            if (cv == NULL)
                return;

            // Get axises
            GraphAxis *xaxis    = cv->axis(sHAxis.get());
            GraphAxis *yaxis   = cv->axis(sVAxis.get());

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

            float dx = x - nMouseX, dy = y - nMouseY;
            bool modified = false;

            // Update HValue
            if (sHValue.sEditable.get())
            {
                float step = (nXFlags & F_FINE_TUNE) ?
                    sHValue.sStep.get(flags & ws::MCF_CONTROL, !(flags & ws::MCF_SHIFT)) :
                    sHValue.sStep.get(flags & ws::MCF_CONTROL, flags & ws::MCF_SHIFT);

                float rx = nMouseX - cv->canvas_aleft() + step * dx;
                float ry = nMouseY - cv->canvas_atop() + step * dy;

                float old       = sHValue.sValue.get();
                float nvalue    = fLastX;
                if ((nMouseX == x) && (nMouseY == y))
                    nvalue          = fLastX;
                else if (xaxis != NULL)
                    nvalue          = xaxis->project(rx, ry);
                nvalue          = sHValue.sValue.limit(nvalue);

                if (nvalue != old)
                {
                    sHValue.sValue.set(nvalue);
                    modified        = true;
                }
            }

            // Update VValue
            if (sHValue.sEditable.get())
            {
                float step = (nXFlags & F_FINE_TUNE) ?
                    sVValue.sStep.get(flags & ws::MCF_CONTROL, !(flags & ws::MCF_SHIFT)) :
                    sVValue.sStep.get(flags & ws::MCF_CONTROL, flags & ws::MCF_SHIFT);

                float rx = nMouseX - cv->canvas_aleft() + step * dx;
                float ry = nMouseY - cv->canvas_atop()  + step * dy;

                float old       = sVValue.sValue.get();
                float nvalue    = fLastY;
                if ((nMouseX == x) && (nMouseY == y))
                    nvalue          = fLastY;
                else if (xaxis != NULL)
                    nvalue          = yaxis->project(rx, ry);
                nvalue          = sVValue.sValue.limit(nvalue);

                if (nvalue != old)
                {
                    sVValue.sValue.set(nvalue);
                    modified        = true;
                }
            }

            // Notify
            if (modified)
                sSlots.execute(SLOT_CHANGE, this);
        }

        status_t GraphDot::on_mouse_in(const ws::event_t *e)
        {
            if (!(nXFlags & F_EDITABLE))
                return STATUS_OK;

            nXFlags |= F_HIGHLIGHT;
            query_draw();

            return GraphItem::on_mouse_in(e);
        }

        status_t GraphDot::on_mouse_out(const ws::event_t *e)
        {
            if (!(nXFlags & F_EDITABLE))
                return STATUS_OK;

            nXFlags &= ~F_HIGHLIGHT;
            query_draw();

            return GraphItem::on_mouse_out(e);
        }

        status_t GraphDot::on_mouse_down(const ws::event_t *e)
        {
            size_t state = nMBState;
            nMBState    |= 1 << e->nCode;

            if (state == 0)
            {
                nMouseX     = e->nLeft;
                nMouseY     = e->nTop;
                fLastX      = sHValue.sValue.get();
                fLastY      = sVValue.sValue.get();
                nXFlags    |= F_EDITING;

                if (e->nCode == ws::MCB_RIGHT)
                    nXFlags    |= F_FINE_TUNE;
            }

            apply_motion(e->nLeft, e->nTop, e->nState);

            return STATUS_OK;
        }

        status_t GraphDot::on_mouse_up(const ws::event_t *e)
        {
            if ((!(nXFlags & F_EDITING)) || (nMBState == 0))
                return STATUS_OK;

            apply_motion(e->nLeft, e->nTop, e->nState);

            nMBState       &= ~(1 << e->nCode);
            if (nMBState == 0)
                nXFlags    &= ~(F_FINE_TUNE | F_EDITING);

            return STATUS_OK;
        }

        status_t GraphDot::on_mouse_move(const ws::event_t *e)
        {
            if (nMBState != 0)
                apply_motion(e->nLeft, e->nTop, e->nState);

            return STATUS_OK;
        }

        status_t GraphDot::on_mouse_scroll(const ws::event_t *e)
        {
            if (!(nXFlags & F_EDITABLE))
                return STATUS_OK;
            if (!sZValue.sEditable.get())
                return STATUS_OK;

            float step      = sZValue.sStep.get(e->nState & ws::MCF_CONTROL, e->nState & ws::MCF_SHIFT);
            float delta     = (e->nCode == ws::MCD_UP) ? -step : step;

            float old       = sZValue.sValue.get();
            sZValue.sValue.add(delta);
            if (old != sZValue.sValue.get())
                sSlots.execute(SLOT_CHANGE, this);

            return STATUS_OK;
        }
    }
}


