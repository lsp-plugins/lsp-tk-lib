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

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        status_t SimpleProperty::bind(atom_t property, Style *style, property_type_t type, IStyleListener *listener)
        {
            if ((style == NULL) || (property < 0))
                return STATUS_BAD_ARGUMENTS;

            // Unbind first
            status_t res;
            if ((pStyle != NULL) && (nAtom >= 0))
            {
                res = pStyle->unbind(nAtom, listener);
                if (res != STATUS_OK)
                    return res;
            }

            // Bind to new handler
            style->begin();
            {
                res = style->bind(property, type, listener);
                if (res == STATUS_OK)
                {
                    pStyle      = style;
                    nAtom       = property;
                }
            }
            style->end();

            // Push configuration to style if required
            if ((pStyle != NULL) && (pStyle->sync()))
                this->sync();
            else if (pListener != NULL)
                pListener->notify(this);

            return res;
        }

        status_t SimpleProperty::bind(const char *property, Style *style, property_type_t type, IStyleListener *listener)
        {
            if ((style == NULL) || (property == NULL))
                return STATUS_BAD_ARGUMENTS;
            atom_t atom = style->atom_id(property);
            return (atom >= 0) ? bind(atom, style, type, listener) : STATUS_UNKNOWN_ERR;
        }

        status_t SimpleProperty::bind(const LSPString *property, Style *style, property_type_t type, IStyleListener *listener)
        {
            if ((style == NULL) || (property == NULL))
                return STATUS_BAD_ARGUMENTS;
            atom_t atom = style->atom_id(property);
            return (atom >= 0) ? bind(atom, style, type, listener) : STATUS_UNKNOWN_ERR;
        }

        status_t SimpleProperty::unbind(IStyleListener *listener)
        {
            if ((pStyle != NULL) && (nAtom >= 0))
            {
                status_t res = pStyle->unbind(nAtom, listener);
                if (res != STATUS_OK)
                    return res;
            }

            pStyle      = NULL;
            nAtom       = -1;

            return STATUS_NOT_BOUND;
        }
    
    } /* namespace tk */
} /* namespace lsp */
