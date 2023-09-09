/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 17 июл. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_SCROLLAREA_H_
#define LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_SCROLLAREA_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        // Style definition
        namespace style
        {
            LSP_TK_STYLE_DEF_BEGIN(ScrollArea, WidgetContainer)
                prop::Layout            sLayout;
                prop::SizeConstraints   sSizeConstraints;
                prop::Scrolling         sHScrollMode;
                prop::Scrolling         sVScrollMode;
                prop::RangeFloat        sHScroll;
                prop::RangeFloat        sVScroll;
            LSP_TK_STYLE_DEF_END
        }

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
                virtual void            property_changed(Property *prop) override;
                virtual void            size_request(ws::size_limit_t *r) override;
                virtual void            realize(const ws::rectangle_t *r) override;

            public:
                explicit                ScrollArea(Display *dpy);
                ScrollArea(const ScrollArea &) = delete;
                ScrollArea(ScrollArea &&) = delete;
                virtual                 ~ScrollArea() override;

                ScrollArea & operator = (const ScrollArea &) = delete;
                ScrollArea & operator = (ScrollArea &&) = delete;

                virtual status_t        init() override;
                virtual void            destroy() override;

            public:
                inline void             get_area(ws::rectangle_t *area) { *area = sArea;            }

            public:
                LSP_TK_PROPERTY(Layout,             layout,             &sLayout)
                LSP_TK_PROPERTY(SizeConstraints,    constraints,        &sSizeConstraints)
                LSP_TK_PROPERTY(Scrolling,          hscroll_mode,       &sHScrollMode)
                LSP_TK_PROPERTY(Scrolling,          vscroll_mode,       &sVScrollMode)

                LSP_TK_PROPERTY(RangeFloat,         hscroll,            &sHScroll)
                LSP_TK_PROPERTY(RangeFloat,         vscroll,            &sVScroll)
                LSP_TK_PROPERTY(StepFloat,          hstep,               sHBar.step())
                LSP_TK_PROPERTY(StepFloat,          vstep,               sVBar.step())
                LSP_TK_PROPERTY(StepFloat,          haccel_step,         sHBar.accel_step())
                LSP_TK_PROPERTY(StepFloat,          vaccel_step,         sVBar.accel_step())

            public:
                virtual void            render(ws::ISurface *s, const ws::rectangle_t *area, bool force) override;

                virtual status_t        add(Widget *widget) override;
                virtual status_t        remove(Widget *widget) override;

                virtual Widget         *find_widget(ssize_t x, ssize_t y) override;

                virtual status_t        on_mouse_scroll(const ws::event_t *e) override;
        };
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_SCROLLAREA_H_ */
