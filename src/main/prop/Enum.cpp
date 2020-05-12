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
            Property(listener),
            sListener(this)
        {
            nAtom       = -1;
            nValue      = 0.0f;
            pEnum       = xenum;
        }

        Enum::~Enum()
        {
            unbind();
        }

        status_t Enum::unbind()
        {
            if ((pStyle != NULL) && (nAtom >= 0))
            {
                status_t res = pStyle->unbind(nAtom, &sListener);
                if (res != STATUS_OK)
                    return res;
            }

            pStyle      = NULL;
            nAtom       = -1;

            return STATUS_NOT_BOUND;
        }

        status_t Enum::bind(atom_t property, Style *style)
        {
            if ((style == NULL) || (property < 0))
                return STATUS_BAD_ARGUMENTS;

            // Unbind first
            status_t res;
            if ((pStyle != NULL) && (nAtom >= 0))
            {
                res = pStyle->unbind(nAtom, &sListener);
                if (res != STATUS_OK)
                    return res;
            }

            // Bind to new handler
            style->begin();
            {
                res = style->bind(property, PT_STRING, &sListener);
                if (res == STATUS_OK)
                {
                    pStyle      = style;
                    nAtom       = property;
                }
            }
            style->end();

            return res;
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
            if (n > 0)
            {
                // Apply successfully parsed value
                nValue = v;

                // Delegate event
                if (pListener != NULL)
                    pListener->notify(this);
            }
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
                    else if (pListener != NULL)
                        pListener->notify(this);
                    return prev;
                }
            }

            return prev;
        }

    } /* namespace tk */
} /* namespace lsp */


