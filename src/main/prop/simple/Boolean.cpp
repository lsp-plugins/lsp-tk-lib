/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 7 мая 2020 г.
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

        status_t Boolean::init(bool value)
        {
            return pStyle->create_bool(nAtom, value);
        }

        status_t Boolean::override(bool value)
        {
            return pStyle->override_bool(nAtom, value);
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

        size_t Boolean::add_as_flag(size_t set, size_t flag)
        {
            return (bValue) ? set | flag : set & (~flag);
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

            status_t Boolean::override(Style *style, bool value)
            {
                if (pStyle == NULL)
                    return STATUS_BAD_STATE;
                style->override_bool(nAtom, value);
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

            status_t Boolean::create(const char *name, Style *style, bool value)
            {
                LSP_STATUS_ASSERT(bind(name, style));
                set(value);
                return STATUS_OK;
            }

            status_t Boolean::init(const char *name, Style *style, bool value)
            {
                prop::Boolean v;
                LSP_STATUS_ASSERT(v.bind(name, style));
                return v.init(value);
            }

            status_t Boolean::override(const char *name, Style *style, bool value)
            {
                prop::Boolean v;
                LSP_STATUS_ASSERT(v.bind(name, style));
                return v.override(value);
            }
        }
    } /* namespace calc */
} /* namespace lsp */



