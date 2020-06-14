/*
 * Pointer.h
 *
 *  Created on: 15 июн. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_BASE_PTR_H_
#define LSP_PLUG_IN_TK_PROP_BASE_PTR_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Ptr: public Property
        {
            private:
                Ptr & operator = (const Ptr &);

            protected:
                void                       *pCurr;
                void                       *pDfl;

            protected:
                explicit Ptr(prop::Listener *listener = NULL);
                ~Ptr();

            protected:
                void                       *set(void *ptr);
                void                       *set_default();

            public:
                inline bool                 is_default() const  { return pCurr == pDfl;             }
        };

    }
}

#endif /* LSP_PLUG_IN_TK_PROP_BASE_PTR_H_ */
