/*
 * BitEnum.cpp
 *
 *  Created on: 8 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/prop.h>
#include <lsp-plug.in/tk/sys/Display.h>
#include <lsp-plug.in/tk/base.h>

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
            Property(listener),
            sListener(this)
        {
            nAtom       = -1;
            nValue      = 0.0f;
            pEnum       = xenum;
        }

        BitEnum::~BitEnum()
        {
            unbind();
        }

        status_t BitEnum::unbind()
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

        status_t BitEnum::bind(atom_t property, Style *style)
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
            if (pStyle == NULL)
            {
                if (pListener != NULL)
                    pListener->notify(this);
                return;
            }

            pStyle->begin(&sListener);
            {
                LSPString s;
                bool success = true;
                for (const prop::enum_t *e = pEnum; (e != NULL) && (e->name != NULL); ++e)
                {
                    if (!(nValue & e->value))
                        continue;
                    if (s.length() > 0)
                    {
                        if (!(success = s.append(',')))
                            break;
                    }
                    if (!(success = s.append_ascii(e->name)))
                        break;
                }

                if (success)
                    pStyle->set_string(nAtom, &s);
            }
            pStyle->end();
        }

        size_t BitEnum::xset_all(size_t v)
        {
            size_t prev = nValue;
            if (prev == nValue)
                return prev;

            nValue      = v;
            sync();
            return prev;
        }

        size_t BitEnum::xset(size_t v)
        {
            size_t prev = nValue;
            nValue     |= v;
            if (prev == nValue)
                return prev;

            sync();
            return prev;
        }

        size_t BitEnum::xunset(size_t v)
        {
            size_t prev = nValue;
            nValue     &= ~v;
            if (prev == nValue)
                return prev;

            sync();
            return prev;
        }

        size_t BitEnum::xtoggle(size_t v)
        {
            size_t prev = nValue;
            nValue     ^= v;
            if (prev == nValue)
                return prev;

            sync();
            return prev;
        }

        size_t BitEnum::xset(size_t v, bool flag)
        {
            size_t prev = nValue;
            nValue = (flag) ? (nValue | v) : (nValue & (~v));
            if (prev == nValue)
                return prev;

            sync();
            return prev;
        }

    } /* namespace tk */
} /* namespace lsp */


