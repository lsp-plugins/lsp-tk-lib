/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 9 окт. 2019 г.
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

        status_t Float::init(float value)
        {
            return pStyle->create_float(nAtom, value);
        }

        status_t Float::override(float value)
        {
            return pStyle->override_float(nAtom, value);
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

            status_t Float::init(const char *name, Style *style, float value)
            {
                prop::Float v;
                LSP_STATUS_ASSERT(v.bind(name, style));
                return v.init(value);
            }

            status_t Float::override(const char *name, Style *style, float value)
            {
                prop::Float v;
                LSP_STATUS_ASSERT(v.bind(name, style));
                return v.override(value);
            }
        }
    
    } /* namespace calc */
} /* namespace lsp */
