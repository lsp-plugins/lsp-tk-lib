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
        void BitEnum::Listener::notify(atom_t property)
        {
            if (property == pValue->nAtom)
                pValue->commit();
        }

        BitEnum::BitEnum(const prop::enum_t *xenum, prop::Listener *listener):
            SimpleProperty(listener),
            sListener(this)
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

        void BitEnum::commit()
        {
            // Handle change: remember new value
            if (pStyle == NULL)
                return;

            LSPString s;
            if (pStyle->get_string(nAtom, &s) != STATUS_OK)
                return;

            // Decode enum value
            size_t v = 0;
            if (Property::parse_bit_enums(&v, &s, pEnum) >= 0)
                nValue      = v;

            // Delegate event
            if (pListener != NULL)
                pListener->notify(this);
        }

        void BitEnum::sync()
        {
            if (pStyle != NULL)
            {
                pStyle->begin(&sListener);
                {
                    LSPString s;
                    if (Property::fmt_bit_enums(&s, pEnum, nValue))
                        pStyle->set_string(nAtom, &s);
                }
                pStyle->end();
            }
            if (pListener != NULL)
                pListener->notify(this);
        }

        status_t BitEnum::init(Style *style, size_t v)
        {
            if (pStyle == NULL)
                return STATUS_BAD_STATE;

            LSPString s;
            if (!Property::fmt_bit_enums(&s, pEnum, v))
                return STATUS_NO_MEM;

            style->create_string(nAtom, &s);
            return STATUS_OK;
        }

        status_t BitEnum::override(Style *style, size_t v)
        {
            if (pStyle == NULL)
                return STATUS_BAD_STATE;

            LSPString s;
            if (!Property::fmt_bit_enums(&s, pEnum, v))
                return STATUS_NO_MEM;

            style->override_string(nAtom, &s);
            return STATUS_OK;
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


