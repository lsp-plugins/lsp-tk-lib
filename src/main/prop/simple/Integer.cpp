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

        void Integer::commit(atom_t property)
        {
            if (pStyle->get_int(nAtom, &nValue) != STATUS_OK)
                return;
        }

        void Integer::push()
        {
            pStyle->set_int(nAtom, nValue);
        }

        ssize_t Integer::set(ssize_t v)
        {
            float prev = nValue;
            if (v == nValue)
                return prev;

            nValue  = v;
            sync();
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
            ssize_t Integer::commit_value(ssize_t value)
            {
                ssize_t old = nValue;
                value       = nValue;
                return old;
            }
        }
    } /* namespace tk */
} /* namespace lsp */
