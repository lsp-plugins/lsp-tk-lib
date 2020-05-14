/*
 * BitEnum.cpp
 *
 *  Created on: 8 мая 2020 г.
 *      Author: sadko
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

        bool BitEnum::format(LSPString *dst, size_t v)
        {
            bool success = true;
            for (const prop::enum_t *e = pEnum; (e != NULL) && (e->name != NULL); ++e)
            {
                if (!(v & e->value))
                    continue;
                if (dst->length() > 0)
                {
                    if (!(success = dst->append(',')))
                        break;
                }
                if (!(success = dst->append_ascii(e->name)))
                    break;
            }
            return success;
        }

        void BitEnum::sync()
        {
            if (pStyle != NULL)
            {
                pStyle->begin(&sListener);
                {
                    LSPString s;
                    if (format(&s, nValue))
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
            if (!format(&s, nValue))
                return STATUS_NO_MEM;

            style->create_string(nAtom, &s);
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


