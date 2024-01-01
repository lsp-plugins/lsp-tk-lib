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
        Float::Float(prop::Listener *listener):
            SimpleProperty(listener)
        {
            fValue          = 0.0f;
            pTransform      = NULL;
            pTransformArg   = NULL;
        }

        Float::~Float()
        {
            SimpleProperty::unbind(&sListener);
        }

        void Float::commit(atom_t property)
        {
            // Handle change: remember new value
            if (pStyle->get_float(nAtom, &fValue) != STATUS_OK)
                return;
        }

        void Float::push()
        {
            pStyle->set_float(nAtom, fValue);
        }

        float Float::transform(float v) const
        {
            return (pTransform != NULL) ? pTransform(v, pTransformArg) : v;
        }

        float Float::set(float v)
        {
            float prev = fValue;
            fValue  = transform(v);

            sync();
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

    } /* namespace calc */
} /* namespace lsp */
