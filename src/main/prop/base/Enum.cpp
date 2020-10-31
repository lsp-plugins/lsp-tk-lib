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
        void Enum::Listener::notify(atom_t property)
        {
            if (property == pValue->nAtom)
                pValue->commit();
        }

        Enum::Enum(const prop::enum_t *xenum, prop::Listener *listener):
            SimpleProperty(listener),
            sListener(this)
        {
            nValue      = 0.0f;
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

        void Enum::commit()
        {
            // Handle change: remember new value
            if (pStyle == NULL)
                return;

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

            // Delegate event
            if (pListener != NULL)
                pListener->notify(this);
        }

        ssize_t Enum::set(ssize_t v)
        {
            float prev = v;
            if (v == nValue)
                return prev;

            for (const prop::enum_t *e = pEnum; (e != NULL) && (e->name != NULL); ++e)
            {
                if (v == e->value)
                {
                    nValue  = v;
                    if (pStyle != NULL)
                    {
                        pStyle->begin(&sListener);
                        pStyle->set_string(nAtom, e->name);
                        pStyle->end();
                    }
                    if (pListener != NULL)
                        pListener->notify(this);
                    return prev;
                }
            }

            return prev;
        }

        ssize_t Enum::init(Style *style, ssize_t v)
        {
            if (pStyle == NULL)
                return STATUS_BAD_STATE;

            for (const prop::enum_t *e = pEnum; (e != NULL) && (e->name != NULL); ++e)
            {
                if (v == e->value)
                    style->create_string(nAtom, e->name);
            }
            return STATUS_OK;
        }

        ssize_t Enum::override(Style *style, ssize_t v)
        {
            if (pStyle == NULL)
                return STATUS_BAD_STATE;

            for (const prop::enum_t *e = pEnum; (e != NULL) && (e->name != NULL); ++e)
            {
                if (v == e->value)
                    style->override_string(nAtom, e->name);
            }
            return STATUS_OK;
        }


        ssize_t Enum::init(ssize_t v)
        {
            if (pStyle == NULL)
                return STATUS_BAD_STATE;

            for (const prop::enum_t *e = pEnum; (e != NULL) && (e->name != NULL); ++e)
            {
                if (v == e->value)
                    pStyle->create_string(nAtom, e->name);
            }
            return STATUS_OK;
        }

        ssize_t Enum::override(ssize_t v)
        {
            if (pStyle == NULL)
                return STATUS_BAD_STATE;

            for (const prop::enum_t *e = pEnum; (e != NULL) && (e->name != NULL); ++e)
            {
                if (v == e->value)
                    pStyle->override_string(nAtom, e->name);
            }
            return STATUS_OK;
        }

    } /* namespace tk */
} /* namespace lsp */


