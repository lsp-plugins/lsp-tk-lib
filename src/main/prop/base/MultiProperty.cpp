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

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        MultiProperty::MultiProperty(atom_t *atoms, size_t size, prop::Listener *listener):
                Property(listener)
        {
            for (size_t i=0; i<size; ++i)
                atoms[i]    = -1;   // By default, atom is not bound
        }

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

        status_t MultiProperty::bind(const char *id, Style *style, atom_t *atoms, const prop::desc_t *desc, IStyleListener *listener)
        {
            if ((style == NULL) || (id == NULL))
                return STATUS_BAD_ARGUMENTS;

            if (pStyle == style)
                return STATUS_OK;

            // Unbind from previously used style
            unbind(atoms, desc, listener);

            LSPString key;
            if (!key.set_utf8(id))
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
                    atom_t atom = style->atom_id(&key);
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

            // Push configuration to style if required
            if ((pStyle != NULL) && (pStyle->config_mode()))
                this->sync();
            else if (pListener != NULL)
                pListener->notify(this);

            return res;
        }

        status_t MultiProperty::bind(atom_t id, Style *style, atom_t *atoms, const prop::desc_t *desc, IStyleListener *listener)
        {
            if (style == NULL)
                return STATUS_BAD_ARGUMENTS;
            return bind(style->atom_name(id), style, atoms, desc, listener);
        }

        status_t MultiProperty::bind(const LSPString *id, Style *style, atom_t *atoms, const prop::desc_t *desc, IStyleListener *listener)
        {
            if (id == NULL)
                return STATUS_BAD_ARGUMENTS;
            return bind(id->get_utf8(), style, atoms, desc, listener);
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


