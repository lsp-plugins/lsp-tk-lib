/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 6 мая 2020 г.
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

        status_t Integer::init(ssize_t value)
        {
            return pStyle->create_int(nAtom, value);
        }

        status_t Integer::override(ssize_t value)
        {
            return pStyle->override_int(nAtom, value);
        }

        void Integer::commit()
        {
            // Handle change: remember new value
            if (pStyle == NULL)
                return;

            if (pStyle->get_int(nAtom, &nValue) != STATUS_OK)
                return;

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
            if (pListener != NULL)
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

            status_t Integer::override(Style *style, ssize_t value)
            {
                if (pStyle == NULL)
                    return STATUS_BAD_STATE;
                style->override_int(nAtom, value);
                return STATUS_OK;
            }

            ssize_t Integer::commit(ssize_t value)
            {
                ssize_t old = nValue;
                value       = nValue;
                return old;
            }

            status_t Integer::create(const char *name, Style *style, ssize_t value)
            {
                LSP_STATUS_ASSERT(bind(name, style));
                set(value);
                return STATUS_OK;
            }

            status_t Integer::init(const char *name, Style *style, ssize_t value)
            {
                prop::Integer v;
                LSP_STATUS_ASSERT(v.bind(name, style));
                return v.init(value);
            }

            status_t Integer::override(const char *name, Style *style, ssize_t value)
            {
                prop::Integer v;
                LSP_STATUS_ASSERT(v.bind(name, style));
                return v.override(value);
            }
        }
    } /* namespace tk */
} /* namespace lsp */
