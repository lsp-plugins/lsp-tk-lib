/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 31 окт. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_STYLE_STYLEFACTORY_H_
#define LSP_PLUG_IN_TK_STYLE_STYLEFACTORY_H_

namespace lsp
{
    namespace tk
    {
        class Style;

        class StyleFactory
        {
            protected:
                static StyleFactory    *pRoot;
                StyleFactory           *pNext;
                const char             *pClass;

            public:
                explicit StyleFactory(const char *style);
                virtual ~StyleFactory();

            public:
                static inline StyleFactory     *root()  { return pRoot;     }

            public:
                inline const char  *name() const        { return pClass;    }
                virtual status_t    init(Style *style);
        };
    }
}

#endif /* LSP_PLUG_IN_TK_STYLE_STYLEFACTORY_H_ */
