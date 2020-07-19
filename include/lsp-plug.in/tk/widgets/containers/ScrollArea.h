/*
 * ScrollArea.h
 *
 *  Created on: 17 июл. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_SCROLLAREA_H_
#define LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_SCROLLAREA_H_

namespace lsp
{
    namespace tk
    {
        /**
         * Scrolling area widget, provides container that may scroll it's contents if it does not fit into
         * the window
         */
        class ScrollArea: public WidgetContainer
        {
            public:
                static const w_class_t    metadata;

            protected:
                typedef struct alloc_t
                {
                    ws::size_limit_t        sSize;      // Overall widget size limits
                    ws::size_limit_t        sWidget;    // Size limits of the widget
                    ws::rectangle_t         sArea;      // Area allocated for the widget
                    ws::rectangle_t         sHBar;      // horizontal scroll parameters
                    ws::rectangle_t         sVBar;      // vertical scroll parameters
                    bool                    bHBar;      // horizontal scroll enable
                    bool                    bVBar;      // vertical scroll enable
                    ssize_t                 wMinW;      // widget min width
                    ssize_t                 wMinH;      // widget min height
                } alloc_t;

            protected:
                Widget                 *pWidget;
                ScrollBar               sHBar;
                ScrollBar               sVBar;
                ws::rectangle_t         sArea;

                prop::Layout            sLayout;
                prop::SizeConstraints   sSizeConstraints;
                prop::Scrolling         sHScrollMode;
                prop::Scrolling         sVScrollMode;
                prop::RangeFloat        sHScroll;
                prop::RangeFloat        sVScroll;

            protected:
                void                    do_destroy();
                void                    estimate_size(alloc_t *a, const ws::rectangle_t *xr);

                static status_t         slot_on_scroll_change(Widget *sender, void *ptr, void *data);

            protected:
                virtual Widget         *find_widget(ssize_t x, ssize_t y);
                virtual void            property_changed(Property *prop);
                virtual void            size_request(ws::size_limit_t *r);
                virtual void            realize(const ws::rectangle_t *r);

            public:
                explicit                ScrollArea(Display *dpy);
                virtual                 ~ScrollArea();

                virtual status_t        init();
                virtual void            destroy();

            public:
                LSP_TK_PROPERTY(Layout,             layout,             &sLayout)
                LSP_TK_PROPERTY(SizeConstraints,    size_constraints,   &sSizeConstraints)
                LSP_TK_PROPERTY(Scrolling,          hscroll_mode,       &sHScrollMode)
                LSP_TK_PROPERTY(Scrolling,          vscroll_mode,       &sVScrollMode)

                LSP_TK_PROPERTY(RangeFloat,         hscroll,            &sHScroll)
                LSP_TK_PROPERTY(RangeFloat,         vscroll,            &sVScroll)

            public:
                virtual void            render(ws::ISurface *s, const ws::rectangle_t *area, bool force);

                virtual status_t        add(Widget *widget);

                virtual status_t        remove(Widget *widget);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_SCROLLAREA_H_ */
