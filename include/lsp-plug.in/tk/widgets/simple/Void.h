/*
 * Void.h
 *
 *  Created on: 11 апр. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_VOID_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_VOID_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Void widget that holds some allocated space and fills it with specified color
         */
        class Void: public Widget
        {
            private:
                Void & operator = (const Void &);

            public:
                static const w_class_t    metadata;

            protected:
                prop::SizeConstraints       sConstraints;

            public:
                explicit Void(Display *dpy);
                virtual ~Void();

            public:
                inline SizeConstraints         *constraints()           { return &sConstraints; }
                inline const SizeConstraints   *constraints() const     { return &sConstraints; }

            protected:
                virtual void        size_request(ws::size_limit_t *r);
                virtual void        property_changed(Property *prop);

            public:
                virtual status_t    init();

                virtual void        render(ws::ISurface *s, const ws::rectangle_t *area, bool force);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_VOID_H_ */
