/*
 * Void.h
 *
 *  Created on: 11 апр. 2020 г.
 *      Author: sadko
 */

#ifndef UI_TK_WIDGETS_LSPVOID_H_
#define UI_TK_WIDGETS_LSPVOID_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Void space
         */
        class Void: public Widget
        {
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

            public:
                virtual status_t    init();

                virtual void        render(ws::ISurface *s, bool force);

                virtual void        realize(const ws::rectangle_t *r);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_LSPVOID_H_ */
