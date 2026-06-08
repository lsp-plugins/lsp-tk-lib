/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 17 июл. 2017 г.
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
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(Align, WidgetContainer)
                // Bind
                sLayout.bind("layout", this);
                sConstraints.bind("size.constraints", this);
                sAggregateSize.bind("size.aggregate", this);
                // Configure
                sLayout.set(0.0f, 0.0f, 0.0f, 0.0f);
                sConstraints.set_all(-1);
                sAggregateSize.set(true);
                // Override
                sLayout.override();
                sConstraints.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(Align, "Align", "root");
        }

        const w_class_t Align::metadata         = { "Align", &WidgetContainer::metadata };

        Align::Align(Display *dpy):
            WidgetContainer(dpy),
            vWidgets(&sProperties, &sIListener),
            sLayout(&sProperties),
            sConstraints(&sProperties),
            sAggregateSize(&sProperties)
        {
            pClass          = &metadata;
        }

        Align::~Align()
        {
            nFlags     |= FINALIZED;
            do_destroy();
        }

        status_t Align::init()
        {
            status_t result = WidgetContainer::init();
            if (result != STATUS_OK)
                return result;

            sIListener.bind_all(this, on_add_widget, on_remove_widget);

            sLayout.bind("layout", &sStyle);
            sConstraints.bind("size.constraints", &sStyle);
            sAggregateSize.bind("size.aggregate", &sStyle);

            return STATUS_OK;
        }

        void Align::destroy()
        {
            nFlags     |= FINALIZED;
            do_destroy();
            WidgetContainer::destroy();
        }

        void Align::do_destroy()
        {
            // Unlink children
            for (size_t i=0, n=vWidgets.size(); i<n; ++i)
            {
                // Get widget
                Widget * const w = vWidgets.get(i);
                if (w != NULL)
                    unlink_widget(w);
            }

            // Free list of children
            vWidgets.flush();
        }

        void Align::on_add_widget(void *obj, Property *prop, void *w)
        {
            Widget * const item = widget_ptrcast<Widget>(w);
            if (item == NULL)
                return;

            Align * const self = widget_ptrcast<Align>(obj);
            if (self != NULL)
            {
                item->set_parent(self);
                self->query_resize();
            }
        }

        void Align::on_remove_widget(void *obj, Property *prop, void *w)
        {
            Widget * const item = widget_ptrcast<Widget>(w);
            if (item == NULL)
                return;

            Align * const self = widget_ptrcast<Align>(obj);
            if (self != NULL)
            {
                self->unlink_widget(item);
                self->query_resize();
            }
        }

        Widget *Align::find_widget(ssize_t x, ssize_t y)
        {
            Widget * const w = current_widget();
            return (w->inside(x, y)) ? w : NULL;
        }

        Widget *Align::current_widget()
        {
            for (size_t i=0, n=vWidgets.size(); i<n; ++i)
            {
                Widget * const w = vWidgets.get(i);
                if (w->is_visible_child_of(this))
                    return w;
            }

            return NULL;
        }

        void Align::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);
            if (prop->one_of(sLayout, sConstraints, sAggregateSize))
                query_resize();
        }

        void Align::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            if (nFlags & REDRAW_SURFACE)
                force = true;

            Widget * const widget = current_widget();

            // Initialize palette
            lsp::Color bg_color;
            get_actual_bg_color(bg_color);

            // Draw background if child is invisible or not present
            if ((widget == NULL) || (!widget->visibility()->get()))
            {
                s->clip_begin(area);
                s->fill_rect(bg_color, SURFMASK_NONE, 0.0f, &sSize);
                s->clip_end();
                return;
            }

            ws::rectangle_t xr;
            widget->get_rectangle(&xr);

            if ((force) || (widget->redraw_bg_pending()))
            {
                if (Size::overlap(area, &sSize))
                {
                    s->clip_begin(area);
                    {
                        widget->get_actual_bg_color(bg_color);
                        s->fill_frame(bg_color, SURFMASK_NONE, 0.0f, &sSize, &xr);
                    }
                    s->clip_end();
                }
            }

            if ((force) || (widget->redraw_pending()))
            {
                // Draw the child only if it is visible in the area
                if (Size::intersection(&xr, area))
                    widget->render(s, &xr, force);
                widget->commit_redraw();
            }
        }

        status_t Align::add(Widget *widget)
        {
            return vWidgets.add(widget);
        }

        status_t Align::remove(Widget *widget)
        {
            return vWidgets.premove(widget);
        }

        void Align::size_request(ws::size_limit_t *r)
        {
            const float scaling = lsp_max(0.0f, sScaling.get());

            // Estimate minimum size
            size_t count = 0;

            if (sAggregateSize.get())
            {
                ws::size_limit_t xr;

                for (size_t i=0, n=vWidgets.size(); i<n; ++i)
                {
                    Widget * const w = vWidgets.get(i);
                    if (w == NULL)
                        continue;

                    if ((count++) > 0)
                    {
                        w->get_padded_size_limits(&xr);
                        SizeConstraints::maximize(r, &xr);
                    }
                    else
                        w->get_padded_size_limits(r);
                }
            }
            else
            {
                Widget * const w = current_widget();
                if (w != NULL)
                {
                    w->get_padded_size_limits(r);
                    ++count;
                }
            }

            // Fill size parameters
            if (count <= 0)
            {
                r->nMinWidth    = -1;
                r->nMinHeight   = -1;
            }

            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;
            r->nPreWidth    = -1;
            r->nPreHeight   = -1;

            // Apply constraints
            sConstraints.apply(r, scaling);
        }

        bool Align::realize(const ws::rectangle_t *r)
        {
//            lsp_trace("width=%d, height=%d", int(r->nWidth), int(r->nHeight));
            Widget * const widget = current_widget();

            bool needs_redraw = WidgetContainer::realize(r);
            if (widget == NULL)
                return needs_redraw;

            // Realize child widget
            ws::rectangle_t xr;
            ws::size_limit_t sr;

            widget->get_padded_size_limits(&sr);
            sLayout.apply(&xr, r, &sr);
            widget->padding()->enter(&xr, widget->scaling()->get());
            if (widget->realize_widget(&xr))
                needs_redraw    = true;

            return needs_redraw;
        }
    } /* namespace tk */
} /* namespace lsp */
