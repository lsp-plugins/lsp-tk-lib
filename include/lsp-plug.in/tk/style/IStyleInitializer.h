/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 14 мая 2020 г.
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

#ifndef LSP_PLUG_IN_TK_STYLE_ISTYLEINITIALIZER_H_
#define LSP_PLUG_IN_TK_STYLE_ISTYLEINITIALIZER_H_

namespace lsp
{
    namespace tk
    {
        class Style;
        class Schema;

        /**
         * Style initialization interface
         */
        class IStyleInitializer
        {
            private:
                IStyleInitializer & operator = (const IStyleInitializer &);

            public:
                explicit IStyleInitializer();
                virtual ~IStyleInitializer();

            public:
                /**
                 * Initialize style of the chema
                 * @param style style to initialize
                 * @return status of operation
                 */
                virtual status_t init(Style *style);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_STYLE_ISTYLEINITIALIZER_H_ */
