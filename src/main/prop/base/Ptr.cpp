/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 15 июн. 2020 г.
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


