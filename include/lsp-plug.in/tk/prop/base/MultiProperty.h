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

#ifndef LSP_PLUG_IN_TK_PROP_BASE_MULTIPROPERTY_H_
#define LSP_PLUG_IN_TK_PROP_BASE_MULTIPROPERTY_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Style;
        class Display;

        /**
         * Floating-point property interface
         */
        class MultiProperty: public Property
        {
            private:
                MultiProperty & operator = (const MultiProperty &);

            protected:
                status_t        unbind(atom_t *atoms, const prop::desc_t *desc, IStyleListener *listener);
                status_t        bind(atom_t id, Style *style, atom_t *atoms, const prop::desc_t *desc, IStyleListener *listener);
                status_t        bind(const char *id, Style *style, atom_t *atoms, const prop::desc_t *desc, IStyleListener *listener);
                status_t        bind(const LSPString *id, Style *style, atom_t *atoms, const prop::desc_t *desc, IStyleListener *listener);

                void            set_default(atom_t *atoms, const prop::desc_t *desc);

            protected:
                explicit MultiProperty(atom_t *atoms, size_t size, prop::Listener *listener = NULL);
        };

    }
}

#endif /* LSP_PLUG_IN_TK_PROP_BASE_MULTIPROPERTY_H_ */
