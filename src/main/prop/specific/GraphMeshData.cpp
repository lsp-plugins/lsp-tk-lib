/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 1 сент. 2020 г.
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
#include <lsp-plug.in/common/alloc.h>
#include <stdlib.h>

#define DATA_ALIGNMENT          0x40

namespace lsp
{
    namespace tk
    {
        GraphMeshData::GraphMeshData(prop::Listener *listener):
            Property(listener)
        {
            vData       = NULL;
            nSize       = 0;
            nStride     = 0;
            pPtr        = NULL;
        }

        GraphMeshData::~GraphMeshData()
        {
            if (pPtr != NULL)
                lsp::free_aligned(pPtr);

            vData       = NULL;
            nSize       = 0;
            nStride     = 0;
            pPtr        = NULL;
        }

        bool GraphMeshData::set_size(size_t size)
        {
            // Size does not change?
            if (size == nSize)
                return true;

            // Need to re-allocate?
            size_t stride   = lsp::align_size(size*sizeof(float), DATA_ALIGNMENT) / sizeof(float);

            if (stride != nStride)
            {
                size_t count    = (stride < nStride) ? size : nSize;
                size_t n        = stride - size;
                uint8_t *ptr    = NULL;
                float *xp       = lsp::alloc_aligned<float>(ptr, stride*2, DATA_ALIGNMENT);
                if (xp == NULL)
                    return false;

                if (vData != NULL)
                {
                    // Copy data
                    dsp::copy(&xp[0], &vData[0], count);
                    dsp::fill_zero(&xp[count], n);
                    dsp::copy(&xp[stride], &vData[nStride], count);
                    dsp::fill_zero(&xp[stride + count], n);

                    // Free previously allocated data
                    lsp::free_aligned(pPtr);
                }
                else
                    dsp::fill_zero(xp, stride * 2);

                vData           = xp;
                pPtr            = ptr;
                nStride         = stride;
            }
            else
            {
                size_t n        = nStride - size;
                dsp::fill_zero(&vData[size], n);
                dsp::fill_zero(&vData[nStride + size], n);
            }

            // Update size
            nSize       = size;

            // Notify about property change
            if (pListener != NULL)
                pListener->notify(this);

            return true;
        }

        void GraphMeshData::copy_data(float *dst, const float *src, size_t n)
        {
            dsp::copy(dst, src, n);
            if (n < nStride)
                dsp::fill_zero(&dst[n], nStride - n);

            // Notify about property change
            if (pListener != NULL)
                pListener->notify(this);
        }

        bool GraphMeshData::set_x(const float *v, size_t size)
        {
            if (!set_size(size))
                return false;
            if (vData == NULL)
                return false;

            copy_data(&vData[0], v, size);
            return true;
        }

        bool GraphMeshData::set_y(const float *v, size_t size)
        {
            if (!set_size(size))
                return false;
            if (vData == NULL)
                return false;

            copy_data(&vData[nStride], v, size);
            return true;
        }
    }
}

