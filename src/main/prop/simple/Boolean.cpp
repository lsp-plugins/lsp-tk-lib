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
        Boolean::Boolean(prop::Listener *listener):
            SimpleProperty(listener)
        {
            bValue      = 0.0f;
        }

        Boolean::~Boolean()
        {
            SimpleProperty::unbind(&sListener);
        }

        void Boolean::commit(atom_t property)
        {
            if (pStyle->get_bool(nAtom, &bValue) != STATUS_OK)
                return;
        }

        void Boolean::push()
        {
            pStyle->set_bool(nAtom, bValue);
        }

        bool Boolean::set(bool v)
        {
            bool prev = bValue;
            if (v == bValue)
                return prev;

            bValue  = v;
            sync();

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
            bool Boolean::commit_value(bool v)
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



