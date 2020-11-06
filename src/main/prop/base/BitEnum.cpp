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
        BitEnum::BitEnum(const prop::enum_t *xenum, prop::Listener *listener):
            SimpleProperty(listener)
        {
            nValue      = 0.0f;
            pEnum       = xenum;
        }

        BitEnum::~BitEnum()
        {
            SimpleProperty::unbind(&sListener);
        }

        status_t BitEnum::unbind()
        {
            return SimpleProperty::unbind(&sListener);
        }

        status_t BitEnum::bind(atom_t property, Style *style)
        {
            return SimpleProperty::bind(property, style, PT_STRING, &sListener);
        }

        status_t BitEnum::bind(const char *property, Style *style)
        {
            return SimpleProperty::bind(property, style, PT_STRING, &sListener);
        }

        status_t BitEnum::bind(const LSPString *property, Style *style)
        {
            return SimpleProperty::bind(property, style, PT_STRING, &sListener);
        }

        void BitEnum::commit(atom_t property)
        {
            LSPString s;
            if (pStyle->get_string(nAtom, &s) != STATUS_OK)
                return;

            // Decode enum value
            size_t v = 0;
            if (Property::parse_bit_enums(&v, &s, pEnum) >= 0)
                nValue      = v;
        }

        void BitEnum::push()
        {
            LSPString s;
            if (Property::fmt_bit_enums(&s, pEnum, nValue))
                pStyle->set_string(nAtom, &s);
        }

        size_t BitEnum::xset_all(size_t v)
        {
            size_t prev = nValue;
            if (prev == v)
                return prev;

            nValue      = v;
            sync();
            return prev;
        }

        size_t BitEnum::xset(size_t v)
        {
            size_t prev = nValue;
            nValue     |= v;
            if (prev == v)
                return prev;

            sync();
            return prev;
        }

        size_t BitEnum::xunset(size_t v)
        {
            size_t prev = nValue;
            nValue     &= ~v;
            if (prev == v)
                return prev;

            sync();
            return prev;
        }

        size_t BitEnum::xtoggle(size_t v)
        {
            size_t prev = nValue;
            nValue     ^= v;
            if (prev == v)
                return prev;

            sync();
            return prev;
        }

        size_t BitEnum::xset(size_t v, bool flag)
        {
            size_t prev = nValue;
            nValue = (flag) ? (nValue | v) : (nValue & (~v));
            if (prev == v)
                return prev;

            sync();
            return prev;
        }

    } /* namespace tk */
} /* namespace lsp */


