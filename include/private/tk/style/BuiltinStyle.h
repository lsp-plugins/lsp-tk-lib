/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 3 нояб. 2020 г.
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

#ifndef PRIVATE_TK_STYLE_BUILTINSTYLE_H_
#define PRIVATE_TK_STYLE_BUILTINSTYLE_H_

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        class BuiltinStyle
        {
            private:
                static BuiltinStyle        *pRoot;
                BuiltinStyle               *pNext;
                IStyleFactory              *pInit;

            public:
                explicit BuiltinStyle(IStyleFactory *init);

            public:
                static inline BuiltinStyle *root() { return pRoot; }
                inline BuiltinStyle        *next() { return pNext; }
                inline IStyleFactory       *init() { return pInit; }
        };

        #define LSP_BUILTIN_STYLE_DEPRECATED(widget)

        #define LSP_TK_BUILTIN_STYLE_VAR(Name) Style ## Builtin

        #define LSP_TK_BUILTIN_STYLE(Style, Name) \
            LSP_SYMBOL_HIDDEN \
            StyleFactory<Style> Style ## Factory(Name); \
            \
            LSP_SYMBOL_HIDDEN \
            BuiltinStyle Style ## Builtin(& Style ## Factory);


    }
}

#endif /* PRIVATE_TK_STYLE_BUILTINSTYLE_H_ */
