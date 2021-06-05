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
                Ptr(const Ptr &);

            protected:
                void                       *pCurr;
                void                       *pDfl;

            protected:
                explicit Ptr(prop::Listener *listener = NULL);
                virtual ~Ptr();

            protected:
                void                       *set(void *ptr);
                void                       *set_default();

            public:
                inline bool                 is_default() const  { return pCurr == pDfl;             }

                inline bool                 is_set() const      { return pCurr != NULL;             }
        };

    }
}

#endif /* LSP_PLUG_IN_TK_PROP_BASE_PTR_H_ */
