/*
 * Align.cpp
 *
 *  Created on: 17 июл. 2017 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t Align::metadata         = { "Align", &WidgetContainer::metadata };

        Align::Align(Display *dpy):
            WidgetContainer(dpy),
            sLayout(&sProperties)
        {
            pWidget         = NULL;
            pClass          = &metadata;
        }

        Align::~Align()
        {
            do_destroy();
        }

        status_t Align::init()
        {
            status_t result = WidgetContainer::init();
            if (result != STATUS_OK)
                return result;

            sLayout.bind("layout", &sStyle);
            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sLayout.init(sclass, 0.0f, 0.0f, 0.0f, 0.0f);
            }

            return STATUS_OK;
        }

        void Align::destroy()
        {
            do_destroy();
            WidgetContainer::destroy();
        }

        void Align::do_destroy()
        {
            if (pWidget != NULL)
            {
                unlink_widget(pWidget);
                pWidget = NULL;
            }
        }

        Widget *Align::find_widget(ssize_t x, ssize_t y)
        {
            if (pWidget == NULL)
                return NULL;

            return (pWidget->inside(x, y)) ? pWidget : NULL;
        }

        void Align::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);
            if (sLayout.is(prop))
                query_resize();
        }

        void Align::render(ws::ISurface *s, bool force)
        {
            if (nFlags & REDRAW_SURFACE)
                force = true;

            // Initialize palette
            lsp::Color bg_color(sBgColor);

            // Draw background if child is invisible or not present
            if ((pWidget == NULL) || (!pWidget->visibility()->get()))
            {
                s->fill_rect(sSize.nLeft, sSize.nTop, sSize.nWidth, sSize.nHeight, bg_color);
                return;
            }

            if ((force) || (pWidget->redraw_pending()))
            {
                pWidget->render(s, force);
                pWidget->commit_redraw();
            }

            if (force)
            {
                ws::rectangle_t pr, cr;
                pWidget->get_padded_rectangle(&pr);
                pWidget->get_rectangle(&cr);

                s->fill_frame(
                    0, 0, sSize.nWidth, sSize.nHeight,
                    pr.nLeft, pr.nTop, pr.nWidth, pr.nHeight,
                    bg_color
                );
                s->fill_frame(
                    pr.nLeft, pr.nTop, pr.nWidth, pr.nHeight,
                    cr.nLeft, cr.nTop, cr.nWidth, cr.nHeight,
                    pWidget->bg_color()->color()
                );
            }
        }

        status_t Align::add(Widget *widget)
        {
            if (widget == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (pWidget != NULL)
                return STATUS_ALREADY_EXISTS;

            widget->set_parent(this);
            pWidget = widget;
            query_resize();
            return STATUS_OK;
        }

        status_t Align::remove(Widget *widget)
        {
            if (pWidget != widget)
                return STATUS_NOT_FOUND;

            unlink_widget(pWidget);
            pWidget  = NULL;

            return STATUS_OK;
        }

        void Align::size_request(ws::size_limit_t *r)
        {
            // Default size request
            r->nMinWidth    = -1;
            r->nMinHeight   = -1;
            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;

            if ((pWidget == NULL) || (!pWidget->visibility()->get()))
                return;

            pWidget->get_padded_size_limits(r);
        }

        void Align::realize(const ws::rectangle_t *r)
        {
            lsp_trace("width=%d, height=%d", int(r->nWidth), int(r->nHeight));
            WidgetContainer::realize(r);

            if ((pWidget == NULL) || (!pWidget->visibility()->get()))
                return;

            // Realize child widget
            ws::rectangle_t xr;
            ws::size_limit_t sr;

            pWidget->get_padded_size_limits(&sr);
            sLayout.apply(&xr, r, &sr);
            pWidget->padding()->enter(&xr, pWidget->scaling()->get());
            pWidget->realize_widget(&xr);
        }
    } /* namespace tk */
} /* namespace lsp */
