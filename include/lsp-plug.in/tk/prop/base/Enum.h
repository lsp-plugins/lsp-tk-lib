/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 8 мая 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_BASE_ENUM_H_
#define LSP_PLUG_IN_TK_PROP_BASE_ENUM_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Integering property interface
         */
        class Enum: public SimpleProperty
        {
            private:
                Enum & operator = (const Enum &);

            protected:
                class Listener: public IStyleListener
                {
                    protected:
                        Enum       *pValue;

                    public:
                        inline Listener(Enum *ptr)             { pValue = ptr;     }

                    public:
                        virtual void notify(atom_t property);
                };

            protected:
                ssize_t             nValue;
                Listener            sListener;
                const prop::enum_t *pEnum;

            protected:
                status_t            unbind();
                status_t            bind(atom_t property, Style *style);
                status_t            bind(const char *property, Style *style);
                status_t            bind(const LSPString *property, Style *style);
                void                commit();
                ssize_t             set(ssize_t v);
                ssize_t             init(Style *style, ssize_t v);
                ssize_t             override(Style *style, ssize_t v);
                ssize_t             init(ssize_t v);
                ssize_t             override(ssize_t v);

            protected:
                explicit Enum(const prop::enum_t *xenum, prop::Listener *listener = NULL);
                ~Enum();

        };
    }
}



#endif /* LSP_PLUG_IN_TK_PROP_BASE_ENUM_H_ */
