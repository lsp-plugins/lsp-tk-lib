/*
 * MultiProperty.cpp
 *
 *  Created on: 8 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        status_t MultiProperty::unbind(atom_t *atoms, const prop::desc_t *desc, IStyleListener *listener)
        {
            if (pStyle == NULL)
                return STATUS_NOT_BOUND;

            // Unbind all atoms
            for ( ; desc->postfix != NULL; ++atoms, ++desc)
            {
                if (*atoms < 0)
                    continue;
                pStyle->unbind(*atoms, listener);
                *atoms      = -1;
            }

            pStyle      = NULL;
            return STATUS_OK;
        }

        status_t MultiProperty::bind(
                atom_t *atoms, const prop::desc_t *desc, IStyleListener *listener,
                const char *property, Style *style, Atoms *xatoms
        )
        {
            if ((style == NULL) || (xatoms == NULL) || (property == NULL))
                return STATUS_BAD_ARGUMENTS;

            if (pStyle == style)
                return STATUS_OK;

            // Unbind from previously used style
            unbind(atoms, desc, listener);

            LSPString key;
            if (!key.set_utf8(property))
                return STATUS_NO_MEM;
            size_t len = key.length();

            // Bind all ports
            status_t res = STATUS_OK;

            style->begin();
            {
                for ( ; desc->postfix != NULL; ++atoms, ++desc)
                {
                    key.set_length(len);
                    if (!key.append_ascii(desc->postfix))
                    {
                        res = STATUS_NO_MEM;
                        break;
                    }
                    atom_t atom = xatoms->atom_id(key.get_utf8());
                    if (atom < 0)
                    {
                        res = STATUS_NO_MEM;
                        break;
                    }
                    res = style->bind(atom, desc->type, listener);
                    if (res != STATUS_OK)
                        break;
                    *atoms      = atom;
                }

                if (res == STATUS_OK)
                    pStyle      = style;
                else
                    unbind(atoms, desc, listener);
            }
            style->end();

            return res;
        }

        void MultiProperty::set_default(atom_t *atoms, const prop::desc_t *desc)
        {
            if (pStyle == NULL)
                return;

            pStyle->begin();

            for ( ; desc->postfix != NULL; ++atoms, ++desc)
            {
                if (*atoms >= 0)
                    pStyle->set_default(*atoms);
            }

            pStyle->end();
        }
    }
}


