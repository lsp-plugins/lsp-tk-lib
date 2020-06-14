/*
 * Ptr.cpp
 *
 *  Created on: 15 июн. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/io/InStringSequence.h>
#include <lsp-plug.in/expr/Tokenizer.h>

namespace lsp
{
    namespace tk
    {
        Ptr::Ptr(prop::Listener *listener): Property(listener)
        {
            pCurr       = NULL;
            pDfl        = NULL;
        }

        Ptr::~Ptr()
        {
            pCurr       = NULL;
            pDfl        = NULL;
        }

        void *Ptr::set(void *ptr)
        {
            void *old = pCurr;
            if (old == ptr)
                return old;

            pCurr = ptr;
            if (pListener != NULL)
                pListener->notify(this);
            return old;
        }

        void *Ptr::set_default()
        {
            void *old = pCurr;
            if (old == pDfl)
                return old;

            pCurr = pDfl;
            if (pListener != NULL)
                pListener->notify(this);
            return old;
        }
    }
}


