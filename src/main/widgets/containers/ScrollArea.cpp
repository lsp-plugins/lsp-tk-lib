/*
 * ScrollArea.cpp
 *
 *  Created on: 17 июл. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t ScrollArea::metadata    = { "ScrollArea", &WidgetContainer::metadata };
        
        ScrollArea::ScrollArea(Display *dpy):
            WidgetContainer(dpy),
            sHBar(dpy),
            sVBar(dpy),
            sLayout(&sProperties),
            sSizeConstraints(&sProperties),
            sHScroll(&sProperties),
            sVScroll(&sProperties)
        {
            pWidget     = NULL;
            
            pClass      = &metadata;
        }
        
        ScrollArea::~ScrollArea()
        {
            do_destroy();
        }

        status_t ScrollArea::init()
        {
            // Initialize widgets
            status_t result = WidgetContainer::init();
            if (result == STATUS_OK)
                result  = sHBar.init();
            if (result == STATUS_OK)
                result  = sVBar.init();
            if (result != STATUS_OK)
                return result;

            sLayout.bind("layout", &sStyle);
            sSizeConstraints.bind("size.constraints", &sStyle);
            sHScroll.bind("hscroll", &sStyle);
            sVScroll.bind("vscroll", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sLayout.init(sclass, 0.0f, 0.0f, 0.0f, 0.0f);
                sSizeConstraints.init(sclass);
                sHScroll.init(sclass, SCROLL_OPTIONAL);
                sVScroll.init(sclass, SCROLL_OPTIONAL);
            }

            return STATUS_OK;
        }

        void ScrollArea::destroy()
        {
            do_destroy();
            WidgetContainer::destroy();
        }

        void ScrollArea::do_destroy()
        {
            if (pWidget != NULL)
            {
                unlink_widget(pWidget);
                pWidget = NULL;
            }
        }

        void ScrollArea::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);
            if (sLayout.is(prop))
                query_resize();
            if (sSizeConstraints.is(prop))
                query_resize();
            if (sHScroll.is(prop))
                query_resize();
            if (sVScroll.is(prop))
                query_resize();
        }

        void ScrollArea::size_request(ws::size_limit_t *r)
        {
            // TODO
        }

        void ScrollArea::realize(const ws::rectangle_t *r)
        {
            // TODO
        }

        void ScrollArea::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            // TODO
        }

        status_t ScrollArea::add(Widget *widget)
        {
            if ((widget == NULL) || (widget == this))
                return STATUS_BAD_ARGUMENTS;
            if (pWidget != NULL)
                return STATUS_ALREADY_EXISTS;

            widget->set_parent(this);
            pWidget = widget;
            query_resize();
            return STATUS_OK;
        }

        status_t ScrollArea::remove(Widget *widget)
        {
            if (pWidget != widget)
                return STATUS_NOT_FOUND;

            unlink_widget(pWidget);
            pWidget  = NULL;

            return STATUS_OK;
        }

        Widget *ScrollArea::find_widget(ssize_t x, ssize_t y)
        {
            if (pWidget == NULL)
                return NULL;

            return (pWidget->inside(x, y)) ? pWidget : NULL;
        }

    } /* namespace tk */
} /* namespace lsp */
