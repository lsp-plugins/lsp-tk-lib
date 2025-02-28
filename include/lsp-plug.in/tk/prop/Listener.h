/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 7 мая 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_LISTENER_H_
#define LSP_PLUG_IN_TK_PROP_LISTENER_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Property;

        namespace prop
        {
            /**
             * Property listener interface
             */
            class Listener
            {
                public:
                    explicit Listener();
                    Listener(const Listener &) = delete;
                    Listener(Listener &&) = delete;
                    virtual ~Listener();

                    Listener & operator = (const Listener &) = delete;
                    Listener & operator = (Listener &&) = delete;

                public:
                    /**
                     * Change callback
                     * @param prop property that has been changed
                     */
                    virtual void notify(Property *prop);
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_LISTENER_H_ */
