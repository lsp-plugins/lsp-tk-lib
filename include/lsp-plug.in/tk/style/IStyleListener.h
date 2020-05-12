/*
 * IStyleListener.h
 *
 *  Created on: 6 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_STYLE_ISTYLELISTENER_H_
#define LSP_PLUG_IN_TK_STYLE_ISTYLELISTENER_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Style Listener
         */
        class IStyleListener
        {
            public:
                virtual ~IStyleListener();

            public:
                /**
                 * Notify about property value change
                 * @param property property identifier
                 */
                virtual void notify(atom_t property);
        };
    }
}

#endif /* LSP_PLUG_IN_TK_STYLE_ISTYLELISTENER_H_ */
