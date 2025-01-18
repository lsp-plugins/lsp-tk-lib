/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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
        static const char *prop_editable[] =
        {
            "hvalue.editable",
            "vvalue.editable",
            "zvalue.editable"
        };

        static const char *prop_value[] =
        {
            "hvalue.value",
            "vvalue.value",
            "zvalue.value"
        };

        static const char *prop_step[] =
        {
            "hvalue.step",
            "vvalue.step",
            "zvalue.step"
        };

        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(GraphDot, GraphItem)
                // bind
                sOrigin.bind("origin", this);
                sHAxis.bind("haxis", this);
                sVAxis.bind("vaxis", this);
                sSize.bind("size", this);
                sHoverSize.bind("hover.size", this);
                sBorderSize.bind("border.size", this);
                sHoverBorderSize.bind("hover.border.size", this);
                sGap.bind("gap", this);
                sHoverGap.bind("hover.gap", this);
                sInvertMouseVScroll.bind("mouse.vscroll.invert", this);

                sColor.bind("color", this);
                sHoverColor.bind("hover.color", this);
                sBorderColor.bind("border.color", this);
                sHoverBorderColor.bind("hover.border.color", this);
                sGapColor.bind("gap.color", this);
                sHoverGapColor.bind("hover.gap.color", this);

                for (size_t i=0; i<3; ++i)
                {
                    sEditable[i].bind(prop_editable[i], this);
                    sValue[i].bind(prop_value[i], this);
                    sStep[i].bind(prop_step[i], this);
                }

                // Configure
                sOrigin.set(0);
                sHAxis.set(0);
                sVAxis.set(1);
                sSize.set(4);
                sHoverSize.set(4);
                sBorderSize.set(0);
                sHoverBorderSize.set(12);
                sGap.set(1);
                sHoverGap.set(1);
                sInvertMouseVScroll.set(false);

                sColor.set("#cccccc");
                sHoverColor.set("#ffffff");
                sBorderColor.set("#cccccc");
                sHoverBorderColor.set("#ffffff");
                sGapColor.set("#000000");
                sHoverGapColor.set("#000000");

                for (size_t i=0; i<3; ++i)
                {
                    sEditable[i].set(false);
                    sValue[i].set_all(0.0f, -1.0f, 1.0f);
                    sStep[i].set(1.0f, 10.0f, 0.1f);
                }

            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(GraphDot, "GraphDot", "root");
        }

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
            sInvertMouseVScroll(&sProperties),
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
            sInvertMouseVScroll.bind("mouse.vscroll.invert", &sStyle);

            sColor.bind("color", &sStyle);
            sHoverColor.bind("hover.color", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sHoverBorderColor.bind("hover.border.color", &sStyle);
            sGapColor.bind("gap.color", &sStyle);
            sHoverGapColor.bind("hover.gap.color", &sStyle);

            // Add handler
            handler_id_t id = 0;
            id = sSlots.add(SLOT_CHANGE, slot_on_change, self());
            if (id >= 0) id = sSlots.add(SLOT_BEGIN_EDIT, slot_begin_edit, self());
            if (id >= 0) id = sSlots.add(SLOT_END_EDIT, slot_end_edit, self());
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

            if (prop->one_of(sOrigin, sHAxis, sVAxis, sSize, sHoverSize))
                query_draw();
            if (prop->one_of(sBorderSize, sHoverBorderSize, sGap, sHoverGap))
                query_draw();
            if (prop->one_of(sColor, sHoverColor, sBorderColor, sHoverBorderColor))
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
                gcol.scale_lch_luminance(bright);

                // Draw border
                ws::IGradient *gr   = s->radial_gradient(x, y, x, y, radius);
                if (gr != NULL)
                {
                    gr->set_start(gcol);
                    gr->set_stop(gcol, 1.0f);
                    s->fill_circle(gr, x, y, radius);
                    delete gr;
                }

                // Draw padding
                if (pad > 0)
                {
                    s->set_antialiasing(sSmooth.get());
                    lsp::Color hole((nXFlags & F_HIGHLIGHT) ? sHoverGapColor : sGapColor);
                    hole.scale_lch_luminance(bright);
                    s->set_antialiasing(sSmooth.get());
                    s->fill_circle(hole, x, y, fpad + fdot);
                }
            }

            // Draw the inner contents
            lsp::Color color((nXFlags & F_HIGHLIGHT) ? sHoverColor : sColor);
            color.scale_lch_luminance(bright);
            s->set_antialiasing(sSmooth.get());
            s->fill_circle(color, x, y, fdot);
            s->set_antialiasing(aa);
        }

        status_t GraphDot::slot_on_change(Widget *sender, void *ptr, void *data)
        {
            GraphDot *_this = widget_ptrcast<GraphDot>(ptr);
            return (_this != NULL) ? _this->on_change() : STATUS_BAD_ARGUMENTS;
        }

        status_t GraphDot::slot_begin_edit(Widget *sender, void *ptr, void *data)
        {
            GraphDot *_this = widget_ptrcast<GraphDot>(ptr);
            return (_this != NULL) ? _this->on_begin_edit() : STATUS_BAD_ARGUMENTS;
        }

        status_t GraphDot::slot_end_edit(Widget *sender, void *ptr, void *data)
        {
            GraphDot *_this = widget_ptrcast<GraphDot>(ptr);
            return (_this != NULL) ? _this->on_end_edit() : STATUS_BAD_ARGUMENTS;
        }

        status_t GraphDot::on_change()
        {
            return STATUS_OK;
        }

        status_t GraphDot::on_begin_edit()
        {
            return STATUS_OK;
        }

        status_t GraphDot::on_end_edit()
        {
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
//            lsp_trace("mx = %d, my = %d", int(mx), int(my));

            float dx        = mx - cv->canvas_aleft() - x;
            float dy        = my - cv->canvas_atop()  - y;

//            lsp_trace("x = %f, y = %f", x, y);
//            lsp_trace("dx = %f, dy = %f", dx, dy);

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
            GraphAxis *yaxis    = cv->axis(sVAxis.get());

            // Check that mouse button state matches
            size_t bflag    = (nXFlags & F_FINE_TUNE) ? ws::MCF_RIGHT : ws::MCF_LEFT;
            if (nMBState != bflag)
            {
                x       = nMouseX;
                y       = nMouseY;
            }

            // Update the difference relative to the sensitivity
//            lsp_trace("xy=(%d, %d), mxy=(%d, %d)",
//                    int(x), int(y), int(nMouseX), int(nMouseY));

            float dx = x - nMouseX, dy = y - nMouseY;
            bool modified = false;

            // Update HValue
            if (sHValue.sEditable.get())
            {
                float step = (nXFlags & F_FINE_TUNE) ?
                    sHValue.sStep.get(flags & ws::MCF_CONTROL, !(flags & ws::MCF_SHIFT)) :
                    sHValue.sStep.get(flags & ws::MCF_CONTROL, flags & ws::MCF_SHIFT);

                float rx = nMouseX - cv->canvas_aleft() + step * dx;
                float ry = nMouseY - cv->canvas_atop()  + step * dy;

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
            if (sVValue.sEditable.get())
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
                else if (yaxis != NULL)
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
            nMBState    |= size_t(1) << e->nCode;

            if (state == 0)
            {
                nMouseX     = e->nLeft;
                nMouseY     = e->nTop;
                fLastX      = sHValue.sValue.get();
                fLastY      = sVValue.sValue.get();
                nXFlags    |= F_EDITING;

                if (e->nCode == ws::MCB_RIGHT)
                    nXFlags    |= F_FINE_TUNE;

                sSlots.execute(SLOT_BEGIN_EDIT, this);
            }

            apply_motion(e->nLeft, e->nTop, e->nState);

            return STATUS_OK;
        }

        status_t GraphDot::on_mouse_up(const ws::event_t *e)
        {
            if ((!(nXFlags & F_EDITING)) || (nMBState == 0))
                return STATUS_OK;

            apply_motion(e->nLeft, e->nTop, e->nState);

            nMBState       &= ~(size_t(1) << e->nCode);
            if (nMBState == 0)
            {
                nXFlags    &= ~(F_FINE_TUNE | F_EDITING);
                sSlots.execute(SLOT_END_EDIT, this);
            }

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
            if (sInvertMouseVScroll.get())
                step            = -step;

            // Compute the delta value
            float delta     = 0.0f;
            if (e->nCode == ws::MCD_UP)
                delta   = step;
            else if (e->nCode == ws::MCD_DOWN)
                delta   = -step;
            else
                return STATUS_OK;

            // Commit the value
            float old       = sZValue.sValue.get();
            sZValue.sValue.add(delta);
            if (old != sZValue.sValue.get())
                sSlots.execute(SLOT_CHANGE, this);

            return STATUS_OK;
        }

    } /* namespace tk */
} /* namespace lsp */


