/*
 * Align.h
 *
 *  Created on: 17 июл. 2017 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_ALIGN_H_
#define LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_ALIGN_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Alignment, implements a single widget container that aligns the child widget
         * according to the layout settings. The container ignores allocation() property
         * of the child widget.
         */
        class Align: public WidgetContainer
        {
            public:
                static const w_class_t    metadata;

            protected:
                Widget                 *pWidget;
                prop::Layout            sLayout;
                prop::SizeConstraints   sSizeConstraints;

            protected:
                void                    do_destroy();

            protected:
                virtual Widget         *find_widget(ssize_t x, ssize_t y);
                virtual void            property_changed(Property *prop);
                virtual void            size_request(ws::size_limit_t *r);
                virtual void            realize(const ws::rectangle_t *r);

            public:
                explicit Align(Display *dpy);
                virtual ~Align();

                virtual status_t        init();
                virtual void            destroy();

            public:
                LSP_TK_PROPERTY(Layout,             layout,             &sLayout)
                LSP_TK_PROPERTY(SizeConstraints,    size_constraints,   &sSizeConstraints)

            public:
                virtual void            render(ws::ISurface *s, const ws::rectangle_t *area, bool force);

                virtual status_t        add(Widget *widget);

                virtual status_t        remove(Widget *widget);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_ALIGN_H_ */
