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
        Enum::Enum(const prop::enum_t *xenum, size_t dfl, prop::Listener *listener):
            SimpleProperty(listener)
        {
            nValue      = dfl;
            pEnum       = xenum;
        }

        Enum::~Enum()
        {
            SimpleProperty::unbind(&sListener);
        }

        status_t Enum::unbind()
        {
            return SimpleProperty::unbind(&sListener);
        }

        status_t Enum::bind(atom_t property, Style *style)
        {
            return SimpleProperty::bind(property, style, PT_STRING, &sListener);
        }

        status_t Enum::bind(const char *property, Style *style)
        {
            return SimpleProperty::bind(property, style, PT_STRING, &sListener);
        }

        status_t Enum::bind(const LSPString *property, Style *style)
        {
            return SimpleProperty::bind(property, style, PT_STRING, &sListener);
        }

        void Enum::commit(atom_t property)
        {
            LSPString s;
            if (pStyle->get_string(nAtom, &s) != STATUS_OK)
                return;

            // Decode enum value
            ssize_t v;
            size_t n = Property::parse_enums(&v, 1, &s, pEnum);
            if (n <= 0)
                return;

            // Apply successfully parsed value
            nValue = v;
        }

        status_t Enum::parse(const char *value)
        {
            LSPString s;
            if (!s.set_utf8(value))
                return STATUS_NO_MEM;
            return parse(&s);
        }

        status_t Enum::parse(const LSPString *value)
        {
            ssize_t v;
            size_t n = Property::parse_enums(&v, 1, value, pEnum);
            if (n <= 0)
                return STATUS_INVALID_VALUE;
            else if (nValue == v)
                return true;

            nValue  = v;
            sync();
            return true;
        }

        void Enum::push()
        {
            const prop::enum_t *e = Property::find_enum(nValue, pEnum);
            if (e != NULL)
                pStyle->set_string(nAtom, e->name);
        }

        ssize_t Enum::set(ssize_t v)
        {
            float prev = v;
            if (v == nValue)
                return prev;

            const prop::enum_t *e = Property::find_enum(v, pEnum);
            if (e != NULL)
            {
                nValue  = v;
                sync();
            }

            return prev;
        }

    } /* namespace tk */
} /* namespace lsp */


