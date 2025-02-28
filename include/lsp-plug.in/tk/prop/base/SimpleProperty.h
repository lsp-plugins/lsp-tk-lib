/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef LSP_PLUG_IN_TK_PROP_BASE_SIMPLEPROPERTY_H_
#define LSP_PLUG_IN_TK_PROP_BASE_SIMPLEPROPERTY_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class SimpleProperty: public Property
        {
            protected:
                atom_t              nAtom;

            protected:
                status_t            bind(atom_t property, Style *style, property_type_t type, IStyleListener *listener);
                status_t            bind(const char *property, Style *style, property_type_t type, IStyleListener *listener);
                status_t            bind(const LSPString *property, Style *style, property_type_t type, IStyleListener *listener);
                status_t            unbind(IStyleListener *listener);

            protected:
                inline SimpleProperty(prop::Listener *listener = NULL): Property(listener) { nAtom = -1; };
                SimpleProperty(const SimpleProperty &) = delete;
                SimpleProperty(SimpleProperty &&) = delete;

                SimpleProperty & operator = (const SimpleProperty &) = delete;
                SimpleProperty & operator = (SimpleProperty &&) = delete;
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_BASE_SIMPLEPROPERTY_H_ */
