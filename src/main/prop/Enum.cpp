/*
 * Enum.cpp
 *
 *  Created on: 8 мая 2020 г.
 *      Author: sadko
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
            nAtom       = -1;
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

    } /* namespace tk */
} /* namespace lsp */


