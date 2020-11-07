/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 16 мая 2020 г.
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
        Flags::Flags(const char * const *flags, atom_t *atoms, prop::Listener *listener):
            Property(listener)
        {
            nFlags      = 0;
            pFlags      = flags;
            vAtoms      = atoms;
        }

        Flags::~Flags()
        {
            unbind();
        }

        status_t Flags::unbind()
        {
            if (pStyle == NULL)
                return STATUS_NOT_BOUND;

            // Unbind all atoms
            atom_t *atoms = vAtoms;
            for (const char * const *flags = pFlags; *flags != NULL; ++atoms, ++flags)
            {
                if (*atoms < 0)
                    continue;
                pStyle->unbind(*atoms, &sListener);
                *atoms      = -1;
            }

            pStyle      = NULL;
            return STATUS_OK;
        }

        status_t Flags::bind(const char *id, Style *style)
        {
            if ((style == NULL) || (id == NULL))
                return STATUS_BAD_ARGUMENTS;

            if (pStyle == style)
                return STATUS_OK;

            // Unbind from previously used style
            unbind();

            LSPString key;
            if (!key.set_utf8(id))
                return STATUS_NO_MEM;
            size_t len = key.length();

            // Bind all ports
            status_t res = STATUS_OK;

            style->begin();
            {
                atom_t *atoms = vAtoms;
                for (const char * const *flags = pFlags; *flags != NULL; ++atoms, ++flags)
                {
                    key.set_length(len);
                    if (!key.append_ascii(*flags))
                    {
                        res = STATUS_NO_MEM;
                        break;
                    }
                    atom_t atom = style->atom_id(key.get_utf8());
                    if (atom < 0)
                    {
                        res = STATUS_NO_MEM;
                        break;
                    }
                    res = style->bind(atom, PT_BOOL, &sListener);
                    if (res != STATUS_OK)
                        break;
                    *atoms      = atom;
                }

                if (res == STATUS_OK)
                    pStyle      = style;
                else
                    unbind();
            }
            style->end();

            // Push configuration to style if required
            if ((pStyle != NULL) && (pStyle->sync_mode()))
                this->sync();
            else if (pListener != NULL)
                pListener->notify(this);

            return res;
        }

        status_t Flags::bind(atom_t id, Style *style)
        {
            if (style == NULL)
                return STATUS_BAD_ARGUMENTS;
            return bind(style->atom_name(id), style);
        }

        status_t Flags::bind(const LSPString *id, Style *style)
        {
            if (id == NULL)
                return STATUS_BAD_ARGUMENTS;
            return bind(id->get_utf8(), style);
        }

        void Flags::commit(atom_t property)
        {
            // Parse property
            atom_t *atoms = vAtoms;
            size_t bit = 1;
            for (const char * const *flags = pFlags; *flags != NULL; ++atoms, ++flags, bit <<= 1)
            {
                bool b;
                if (*atoms == property)
                {
                    if (pStyle->get_bool(property, &b) == STATUS_OK)
                        nFlags  = (b) ? (nFlags | bit) : (nFlags & (~bit));
                    break;
                }
            }
        }

        void Flags::psync(atom_t atom, bool value)
        {
            // Commit data to the style
            if (pStyle != NULL)
            {
                pStyle->begin(&sListener);
                    pStyle->set_bool(atom, value);
                pStyle->end();
            }

            // Notify listener about change
            if (pListener != NULL)
                pListener->notify(this);
        }

        void Flags::psync(size_t mask)
        {
            // Update flags
            size_t changes  = nFlags ^ mask;
            nFlags          = mask;

            // Commit data to the style
            if (pStyle != NULL)
            {
                pStyle->begin(&sListener);
                    atom_t *atoms = vAtoms;
                    size_t bit = 1;
                    for (const char * const *flags = pFlags; *flags != NULL; ++atoms, ++flags, bit <<= 1)
                    {
                        if ((changes & bit) && (*atoms >= 0))
                            pStyle->set_bool(*atoms, nFlags & bit);
                    }
                pStyle->end();
            }

            // Notify listener about change
            if (pListener != NULL)
                pListener->notify(this);
        }

        void Flags::push()
        {
            psync(size_t(0) - 1); // Sync all items
        }

        void Flags::set_default()
        {
            if (pStyle == NULL)
                return;

            pStyle->begin();
            {
                atom_t *atoms = vAtoms;
                for (const char * const *flags = pFlags; *flags != NULL; ++atoms, ++flags)
                {
                    if (*atoms >= 0)
                        pStyle->set_default(*atoms);
                }
            }
            pStyle->end();
        }

        atom_t Flags::lookup(size_t ordinal)
        {
            // Lookup for the flag
            atom_t *atoms = vAtoms;
            for (const char * const *flags = pFlags; *flags != NULL; ++atoms, ++flags)
            {
                if ((ordinal--) == 0) // Found match?
                    return (pStyle != NULL) ? *atoms : ordinal; // Return always non-negative value
            }
            return -1;
        }

        bool Flags::set(size_t ordinal, bool on)
        {
            atom_t atom = lookup(ordinal);
            if (atom < 0)
                return false;

            ordinal     = 1 << ordinal;
            size_t v    = lsp_setflag(nFlags, ordinal, on);
            if (v == nFlags)
                return on;

            nFlags      = v;
            psync(atom, on);

            return ! on;
        }

        bool Flags::unset(size_t ordinal)
        {
            atom_t atom = lookup(ordinal);
            if (atom < 0)
                return false;

            ordinal = 1 << ordinal;
            size_t v = nFlags & (~ordinal);
            if (v == nFlags)
                return false;

            nFlags      = v;
            psync(atom, false);

            return true;
        }

        bool Flags::toggle(size_t ordinal)
        {
            atom_t atom = lookup(ordinal);
            if (atom < 0)
                return false;

            ordinal     = 1 << ordinal;
            bool prev   = nFlags & ordinal;
            nFlags     ^= ordinal;
            psync(atom, !prev);

            return prev;
        }
    
    } /* namespace tk */
} /* namespace lsp */
