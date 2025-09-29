/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 29 сент. 2025 г.
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

#include <lsp-plug.in/common/debug.h>
#include <lsp-plug.in/tk/tk.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        // Style definition
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(GraphEmbed, GraphItem)
                // bind
                sOrigin.bind("origin", this);
                sHAxis.bind("haxis", this);
                sVAxis.bind("vaxis", this);
                sHStartValue.bind("hvalue.start", this);
                sVStartValue.bind("vvalue.start", this);
                sHEndValue.bind("hvalue.end", this);
                sVEndValue.bind("vvalue.end", this);
                sLayout.bind("layout", this);
                sTransparency.bind("transparency", this);

                // Configure
                sOrigin.set(0);
                sHAxis.set(0);
                sVAxis.set(1);
                sHStartValue.set(0.0f);
                sVStartValue.set(0.0f);
                sHEndValue.set(1.0f);
                sVEndValue.set(1.0f);
                sLayout.set(0.0f, 0.0f, 1.0f, 1.0f);
                sTransparency.set(0.5f);
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(GraphEmbed, "GraphEmbed", "root");
        } /* namespace style */

        const w_class_t GraphEmbed::metadata    = { "GraphEmbed", &WidgetContainer::metadata };

        GraphEmbed::GraphEmbed(Display *dpy):
            WidgetContainer(dpy),
            sOrigin(&sProperties),
            sHAxis(&sProperties),
            sVAxis(&sProperties),
            sHStartValue(&sProperties),
            sVStartValue(&sProperties),
            sHEndValue(&sProperties),
            sVEndValue(&sProperties),
            sLayout(&sProperties),
            sTransparency(&sProperties)
        {
            pClass              = &metadata;

            wWidget             = NULL;
            sLocation.nLeft     = -1;
            sLocation.nTop      = -1;
            sLocation.nWidth    = -1;
            sLocation.nHeight   = -1;
        }

        GraphEmbed::~GraphEmbed()
        {
        }

        status_t GraphEmbed::init()
        {
            status_t res = WidgetContainer::init();
            if (res != STATUS_OK)
                return res;

            sOrigin.bind("origin", &sStyle);
            sHAxis.bind("haxis", &sStyle);
            sVAxis.bind("vaxis", &sStyle);
            sHStartValue.bind("hvalue.start", &sStyle);
            sVStartValue.bind("vvalue.start", &sStyle);
            sHEndValue.bind("hvalue.end", &sStyle);
            sVEndValue.bind("vvalue.end", &sStyle);
            sLayout.bind("layout", &sStyle);
            sTransparency.bind("transparency", &sStyle);

            return STATUS_OK;
        }

        void GraphEmbed::destroy()
        {
            do_destroy();
            WidgetContainer::destroy();
        }

        void GraphEmbed::do_destroy()
        {
            if (wWidget != NULL)
            {
                unlink_widget(wWidget);
                wWidget = NULL;
            }
        }

        void GraphEmbed::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);

            if (prop->one_of(sOrigin, sHAxis, sVAxis, sHStartValue, sVStartValue, sHEndValue, sVEndValue, sLayout))
            {
                invalidate_location();
                query_draw();
            }

            if (prop->one_of(sTransparency))
                query_draw();
        }

        status_t GraphEmbed::calc_point(ssize_t *x, ssize_t *y, Graph *cv, float hvalue, float vvalue)
        {
            // Get axises
            GraphAxis *basis        = cv->axis(sHAxis.get());
            if (basis == NULL)
                return STATUS_BAD_STATE;
            GraphAxis *parallel     = cv->axis(sVAxis.get());
            if (parallel == NULL)
                return STATUS_BAD_STATE;

            // Locate the point at the center
            float xx = 0.0f, yy = 0.0f;
            cv->origin(sOrigin.get(), &xx, &yy);

            // Translate point and get the owner line
            basis->apply(&xx, &yy, &hvalue, 1);
            parallel->apply(&xx, &yy, &vvalue, 1);

            *x                      = xx;
            *y                      = yy;

            return STATUS_OK;
        }

        void GraphEmbed::invalidate_location()
        {
            sLocation.nLeft     = -1;
            sLocation.nTop      = -1;
            sLocation.nWidth    = -1;
            sLocation.nHeight   = -1;
        }

        void GraphEmbed::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            // Check visibility of nested widget
            if ((wWidget == NULL) || (!wWidget->visibility()->get()))
                return;

            // Get graph
            Graph *cv = graph();
            if (cv == NULL)
                return;

            // Realize widget contents
            status_t res;
            ws::rectangle_t rect, wrect;
            rect.nLeft              = 0;
            rect.nTop               = 0;
            rect.nWidth             = 0;
            rect.nHeight            = 0;

            if ((res = calc_point(&rect.nLeft, &rect.nTop, cv, sHStartValue.get(), sVStartValue.get())) != STATUS_OK)
                return;
            if ((res = calc_point(&rect.nWidth, &rect.nHeight, cv, sHEndValue.get(), sVEndValue.get())) != STATUS_OK)
                return;

            if (rect.nLeft >= rect.nWidth)
                lsp::swap(rect.nLeft, rect.nWidth);
            if (rect.nTop >= rect.nHeight)
                lsp::swap(rect.nTop, rect.nHeight);

            rect.nWidth            -= rect.nLeft;
            rect.nHeight           -= rect.nTop;

            realize_child(cv, &wrect, &rect);
//            lsp_trace("wrect: x=%d, y=%d, w=%d, h=%d", int(wrect.nLeft), int(wrect.nTop), int(wrect.nWidth), int(wrect.nHeight));

            ws::ISurface *ws        = get_surface(s, wrect.nWidth, wrect.nHeight);
            if (ws == NULL)
                return;

            const float transp      = lsp_limit(sTransparency.get(), 0.0f, 1.0f);
            s->draw(ws,
                wrect.nLeft - cv->canvas_aleft(), wrect.nTop - cv->canvas_atop(),
                1.0f, 1.0f,
                transp);
        }

        void GraphEmbed::draw(ws::ISurface *s, bool force)
        {
            // Get graph
            Graph *cv = graph();
            if (cv == NULL)
                return;

            // Set up draw translation from window coordinates to surface coordinates
            ws::rectangle_t xr;
            wWidget->get_rectangle(&xr);

            const ws::point_t origin = s->set_origin(-xr.nLeft, -xr.nTop);
            lsp_finally { s->set_origin(origin); };

            // Draw the child widget
            wWidget->render(s, &xr, force);
            wWidget->commit_redraw();
        }

        void GraphEmbed::realize_child(Graph *cv, ws::rectangle_t *widget, const ws::rectangle_t *location)
        {
            if ((sLocation.nLeft == location->nLeft) &&
                (sLocation.nTop == location->nTop) &&
                (sLocation.nWidth == location->nWidth) &&
                (sLocation.nHeight == location->nHeight))
            {
                *widget         = sSize;
                return;
            }

            ws::size_limit_t sr;
            wWidget->get_size_limits(&sr);
            sLayout.apply(widget, location, &sr);

            widget->nLeft  += cv->canvas_aleft();
            widget->nTop   += cv->canvas_atop();
            sSize           = *widget;
            sLocation       = *location;

            wWidget->realize_widget(widget);
        }

        Graph *GraphEmbed::graph()
        {
            return widget_cast<Graph>(pParent);
        }

        status_t GraphEmbed::add(Widget *widget)
        {
            if ((widget == NULL) || (widget == this))
                return STATUS_BAD_ARGUMENTS;
            if (wWidget != NULL)
                return STATUS_ALREADY_EXISTS;

            widget->set_parent(this);
            wWidget = widget;
            query_resize();
            return STATUS_OK;
        }

        status_t GraphEmbed::remove(Widget *widget)
        {
            if (wWidget != widget)
                return STATUS_NOT_FOUND;

            unlink_widget(wWidget);
            wWidget  = NULL;
            query_resize();

            return STATUS_OK;
        }

        Widget *GraphEmbed::find_widget(ssize_t x, ssize_t y)
        {
            if ((wWidget == NULL) || (!wWidget->is_visible_child_of(this)))
                return NULL;

            return (wWidget->inside(x, y)) ? wWidget : NULL;
        }

    } /* namespace tk */
} /* namespace lsp */

