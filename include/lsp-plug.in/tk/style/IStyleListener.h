/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 6 мая 2020 г.
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
            private:
                IStyleListener & operator = (const IStyleListener &);
                IStyleListener(const IStyleListener &);

            public:
                explicit IStyleListener();
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
