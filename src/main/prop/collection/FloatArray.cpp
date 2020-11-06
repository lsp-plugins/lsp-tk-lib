/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 28 сент. 2020 г.
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
#include <lsp-plug.in/dsp/dsp.h>

namespace lsp
{
    namespace tk
    {
        FloatArray::FloatArray(prop::Listener *listener):
            Property(listener)
        {
        }

        FloatArray::~FloatArray()
        {
        }

        float FloatArray::get(size_t index) const
        {
            const float *v = vItems.get(index);
            return (v != NULL) ? *v : 0.0f;
        }

        void FloatArray::clear()
        {
            if (vItems.size() <= 0)
                return;

            vItems.clear();
            sync();
        }

        status_t FloatArray::resize(size_t size)
        {
            size_t xsize = vItems.size();
            if (xsize == size)
                return STATUS_OK;

            if (xsize > size)
            {
                vItems.truncate(size);
                sync();
                return STATUS_OK;
            }

            size       -= xsize;
            float *v    = vItems.append_n(size);
            if (v == NULL)
                return STATUS_NO_MEM;

            dsp::fill_zero(v, size);
            sync();
            return STATUS_OK;
        }

        status_t FloatArray::prepend(const float *v, size_t count)
        {
            float *dst  = vItems.prepend_n(count);
            if (dst == NULL)
                return STATUS_NO_MEM;

            dsp::copy(dst, v, count);
            sync();
            return STATUS_OK;
        }

        status_t FloatArray::append(const float *v, size_t count)
        {
            float *dst  = vItems.append_n(count);
            if (dst == NULL)
                return STATUS_NO_MEM;

            dsp::copy(dst, v, count);
            sync();
            return STATUS_OK;
        }

        status_t FloatArray::insert(size_t idx, const float *v, size_t count)
        {
            float *dst  = vItems.insert_n(idx, count);
            if (dst == NULL)
                return STATUS_NO_MEM;

            dsp::copy(dst, v, count);
            sync();
            return STATUS_OK;
        }

        status_t FloatArray::remove(size_t idx, size_t count)
        {
            if (!vItems.remove_n(idx, count))
                return STATUS_INVALID_VALUE;

            sync();
            return STATUS_OK;
        }

        status_t FloatArray::set(const float *v, size_t count)
        {
            if (!vItems.set_n(count, v))
                return STATUS_NO_MEM;

            sync();
            return STATUS_OK;
        }

        status_t FloatArray::set(size_t idx, float v)
        {
            float *xv = vItems.get(idx);
            if (xv == NULL)
                return STATUS_INVALID_VALUE;
            if (*xv == v)
                return STATUS_OK;

            *xv     = v;
            sync();
            return STATUS_OK;
        }

        status_t FloatArray::set(size_t idx, const float *v, size_t count)
        {
            if (!vItems.set_n(idx, count, v))
                return STATUS_INVALID_VALUE;

            sync();
            return STATUS_OK;
        }

        void FloatArray::swap(FloatArray *src)
        {
            if (src == this)
                return;

            vItems.swap(src->vItems);
            sync();
            src->sync();
        }
    }
}


