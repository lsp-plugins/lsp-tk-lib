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
        const prop::desc_t GraphMeshData::DESC[] =
        {
            { ".size",      PT_INT      },
            { ".strobe",    PT_BOOL     },
            NULL
        };

        void GraphMeshData::Listener::notify(atom_t property)
        {
            pValue->commit(property);
        }

        GraphMeshData::GraphMeshData(prop::Listener *listener):
            MultiProperty(vAtoms, P_COUNT, listener),
            sListener(this)
        {
            vData       = NULL;
            nSize       = 0;
            nStride     = 0;
            bStrobe     = false;
            pPtr        = NULL;
        }

        GraphMeshData::~GraphMeshData()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);

            if (pPtr != NULL)
                lsp::free_aligned(pPtr);

            vData       = NULL;
            nSize       = 0;
            bStrobe     = false;
            nStride     = 0;
            pPtr        = NULL;
        }

        void GraphMeshData::commit(atom_t property)
        {
            if ((pStyle == NULL) || (property < 0))
                return;

            ssize_t v;
            if ((property == vAtoms[P_SIZE]) && (pStyle->get_int(vAtoms[P_SIZE], &v) == STATUS_OK))
                resize_buffer(v, bStrobe);

            bool strobe;
            if ((property == vAtoms[P_STROBE]) && (pStyle->get_bool(vAtoms[P_STROBE], &strobe) == STATUS_OK))
                resize_buffer(nSize, strobe);

            // Update/notify listeners
            if (pStyle->config_mode())
                this->sync();
            else if (pListener != NULL)
                pListener->notify(this);
        }

        void GraphMeshData::sync()
        {
            // Update settings
            if (pStyle != NULL)
            {
                pStyle->begin(&sListener);
                {
                    if (vAtoms[P_SIZE] >= 0)
                        pStyle->set_int(vAtoms[P_SIZE], nSize);
                    if (vAtoms[P_STROBE] >= 0)
                        pStyle->set_bool(vAtoms[P_STROBE], bStrobe);
                }
                pStyle->end();
            }

            // Notify about property change
            if (pListener != NULL)
                pListener->notify(this);
        }

        bool GraphMeshData::resize_buffer(size_t size, bool strobe)
        {
            // Did not even changed?
            if ((nSize == size) && (bStrobe == strobe))
                return true;

            // Need to re-allocate?
            size_t stride   = lsp::align_size(size*sizeof(float), DATA_ALIGNMENT) / sizeof(float);

            if ((stride != nStride) || (strobe != bStrobe))
            {
                size_t count    = (stride < nStride) ? size : nSize;
                size_t n        = stride - size;
                uint8_t *ptr    = NULL;
                size_t nc       = (strobe) ? 3 : 2;
                float *xp       = lsp::alloc_aligned<float>(ptr, stride * nc, DATA_ALIGNMENT);
                if (xp == NULL)
                    return false;

                if (vData != NULL)
                {
                    // Copy data
                    dsp::copy(&xp[0], &vData[0], count);
                    dsp::fill_zero(&xp[count], n);
                    dsp::copy(&xp[stride], &vData[nStride], count);
                    dsp::fill_zero(&xp[stride + count], n);

                    // Copy strobe data if present
                    if (strobe)
                    {
                        if (bStrobe)
                        {
                            dsp::copy(&xp[stride*2], &vData[nStride], count);
                            dsp::fill_zero(&xp[stride*2 + count], n);
                        }
                        else
                            dsp::fill_zero(&xp[stride*2], stride);
                    }

                    // Free previously allocated data
                    lsp::free_aligned(pPtr);
                }
                else
                    dsp::fill_zero(xp, stride * nc);

                vData           = xp;
                pPtr            = ptr;
                nStride         = stride;
            }
            else
            {
                size_t n        = nStride - size;
                dsp::fill_zero(&vData[size], n);
                dsp::fill_zero(&vData[nStride + size], n);
                if (strobe)
                    dsp::fill_zero(&vData[nStride*2 + size], n);
            }

            // Update size
            nSize       = size;
            return true;
        }

        bool GraphMeshData::set_size(size_t size, bool strobe)
        {
            // Try to resize the buffer
            bool res = resize_buffer(size, strobe);
            if (res)
                sync();

            return res;
        }

        bool GraphMeshData::set_size(size_t size)
        {
            // Try to resize the buffer
            bool res = resize_buffer(size, bStrobe);
            if (res)
                sync();

            return res;
        }

        bool GraphMeshData::set_strobe(bool strobe)
        {
            bool res = resize_buffer(nSize, strobe);
            if (res)
                sync();
            return res;
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
            if (!resize_buffer(size, bStrobe))
                return false;

            if (vData != NULL)
                copy_data(&vData[0], v, size);
            sync();

            return true;
        }

        bool GraphMeshData::set_y(const float *v, size_t size)
        {
            if (!resize_buffer(size, bStrobe))
                return false;

            if (vData != NULL)
                copy_data(&vData[nStride], v, size);
            sync();

            return true;
        }

        bool GraphMeshData::set_s(const float *v, size_t size)
        {
            if (!bStrobe)
                return false;
            if (!resize_buffer(size, bStrobe))
                return false;

            if (vData != NULL)
                copy_data(&vData[nStride*2], v, size);
            sync();

            return true;
        }

        bool GraphMeshData::set(const float *x, const float *y, size_t size)
        {
            if (!resize_buffer(size, bStrobe))
                return false;

            if (vData != NULL)
            {
                copy_data(&vData[0], x, size);
                copy_data(&vData[nStride], y, size);
            }
            sync();

            return true;
        }

    }
}

