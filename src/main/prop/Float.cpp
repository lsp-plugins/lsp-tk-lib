/*
 * Float.cpp
 *
 *  Created on: 9 окт. 2019 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        void Float::Listener::notify(atom_t property)
        {
            if (pValue->nAtom == property)
                pValue->commit();
        }

        Float::Float(prop::Listener *listener):
            SimpleProperty(listener),
            sListener(this)
        {
            fValue      = 0.0f;
        }

        Float::~Float()
        {
            SimpleProperty::unbind(&sListener);
        }

        void Float::commit()
        {
            // Handle change: remember new value
            if (pStyle == NULL)
                return;

            if (pStyle->get_float(nAtom, &fValue) != STATUS_OK)
                return;

            // Delegate event
            if (pListener != NULL)
                pListener->notify(this);
        }

        float Float::set(float v)
        {
            float prev = fValue;
            if (v == fValue)
                return prev;

            fValue  = v;
            if (pStyle != NULL)
            {
                pStyle->begin(&sListener);
                    pStyle->set_float(nAtom, v);
                pStyle->end();
            }
            if (pListener != NULL)
                pListener->notify(this);
            return prev;
        }

        void Float::swap(Float *dst)
        {
            if (dst == this)
                return;

            float tmp = dst->fValue;
            dst->set(fValue);
            set(tmp);
        }

        namespace prop
        {
            status_t Float::init(Style *style, float value)
            {
                if (pStyle == NULL)
                    return STATUS_BAD_STATE;
                style->create_float(nAtom, value);
                return STATUS_OK;
            }

            status_t Float::override(Style *style, float value)
            {
                if (pStyle == NULL)
                    return STATUS_BAD_STATE;
                style->override_float(nAtom, value);
                return STATUS_OK;
            }
        }
    
    } /* namespace calc */
} /* namespace lsp */
