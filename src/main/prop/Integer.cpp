/*
 * Integer.cpp
 *
 *  Created on: 6 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        void Integer::Listener::notify(atom_t property)
        {
            if (property == pValue->nAtom)
                pValue->commit();
        }

        Integer::Integer(prop::Listener *listener):
            SimpleProperty(listener),
            sListener(this)
        {
            nAtom       = -1;
            nValue      = 0.0f;
        }

        Integer::~Integer()
        {
            unbind(&sListener);
        }

        void Integer::commit()
        {
            // Handle change: remember new value
            if (pStyle != NULL)
            {
                if (pStyle->get_int(nAtom, &nValue) != STATUS_OK)
                    return;
            }

            // Delegate event
            if (pListener != NULL)
                pListener->notify(this);
        }

        ssize_t Integer::set(ssize_t v)
        {
            float prev = nValue;
            if (v == nValue)
                return prev;

            nValue  = v;
            if (pStyle != NULL)
            {
                pStyle->begin(&sListener);
                pStyle->set_int(nAtom, v);
                pStyle->end();
            }
            else if (pListener != NULL)
                pListener->notify(this);
            return prev;
        }

        void Integer::swap(Integer *dst)
        {
            if (dst == this)
                return;

            ssize_t tmp = dst->nValue;
            dst->set(nValue);
            set(tmp);
        }
    
        namespace prop
        {
            status_t Integer::init(Style *style, ssize_t value)
            {
                if (pStyle == NULL)
                    return STATUS_BAD_STATE;
                style->create_int(nAtom, value);
                return STATUS_OK;
            }
        }
    } /* namespace tk */
} /* namespace lsp */
