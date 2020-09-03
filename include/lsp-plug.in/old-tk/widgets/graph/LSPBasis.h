/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 19 июл. 2017 г.
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

#ifndef UI_TK_LSPBASIS_H_
#define UI_TK_LSPBASIS_H_

#include <lsp-plug.in/old-tk/widgets/graph/LSPGraphItem.h>
#include <lsp-plug.in/old-tk/widgets/LSPGraph.h>
#include <lsp-plug.in/tk-old/types.h>
#include <lsp-plug.in/tk-old/version.h>

namespace lsp
{
    namespace tk
    {
        class LSPBasis: public LSPGraphItem
        {
            public:
                static const w_class_t    metadata;

            protected:
                ssize_t     nID;

            public:
                explicit LSPBasis(LSPDisplay *dpy);
                virtual ~LSPBasis();

            public:
                inline ssize_t get_id() const { return nID; }
                inline void set_id(ssize_t id) { nID = id; }
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPBASIS_H_ */
