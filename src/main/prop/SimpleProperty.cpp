/*
 * SimpleProperty.cpp
 *
 *  Created on: 14 мая 2020 г.
 *      Author: sadko
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
            style->begin(listener);
            {
                res = style->bind(property, type, listener);
                if (res == STATUS_OK)
                {
                    pStyle      = style;
                    nAtom       = property;
                }
            }
            style->end();
            if (pListener != NULL)
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
