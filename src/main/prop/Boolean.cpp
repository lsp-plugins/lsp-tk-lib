/*
 * Boolean.cpp
 *
 *  Created on: 7 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        void Boolean::Listener::notify(atom_t property)
        {
            if (pValue->nAtom == property)
                pValue->commit();
        }

        Boolean::Boolean(prop::Listener *listener):
            SimpleProperty(listener),
            sListener(this)
        {
            bValue      = 0.0f;
        }

        Boolean::~Boolean()
        {
            SimpleProperty::unbind(&sListener);
        }

        void Boolean::commit()
        {
            // Handle change: remember new value
            if (pStyle == NULL)
                return;

            if (pStyle->get_bool(nAtom, &bValue) != STATUS_OK)
                return;

            // Delegate event
            if (pListener != NULL)
                pListener->notify(this);
        }

        bool Boolean::set(bool v)
        {
            bool prev = bValue;
            if (v == bValue)
                return prev;

            bValue  = v;
            if (pStyle != NULL)
            {
                pStyle->begin(&sListener);
                    pStyle->set_bool(nAtom, v);
                pStyle->end();
            }
            if (pListener != NULL)
                pListener->notify(this);
            return prev;
        }

        void Boolean::swap(Boolean *dst)
        {
            if (dst == this)
                return;

            bool tmp = dst->bValue;
            dst->set(bValue);
            set(tmp);
        }

        namespace prop
        {
            status_t Boolean::init(Style *style, bool value)
            {
                if (pStyle == NULL)
                    return STATUS_BAD_STATE;
                style->create_bool(nAtom, value);
                return STATUS_OK;
            }

            bool Boolean::commit(bool v)
            {
                bool prev = bValue;
                if (v == bValue)
                    return prev;

                bValue  = v;
                if (pStyle != NULL)
                {
                    pStyle->begin(&sListener);
                        pStyle->set_bool(nAtom, v);
                    pStyle->end();
                }

                return prev;
            }
        }
    } /* namespace calc */
} /* namespace lsp */



