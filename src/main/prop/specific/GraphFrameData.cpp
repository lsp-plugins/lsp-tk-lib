/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 2 сент. 2020 г.
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
#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/dsp/dsp.h>

#define DATA_ALIGNMENT          0x40

namespace lsp
{
    namespace tk
    {
        const prop::desc_t GraphFrameData::DESC[] =
        {
            { ".rows",          PT_INT      },
            { ".columns",       PT_INT      },
            { ".size",          PT_STRING   },
            { ".min",           PT_FLOAT    },
            { ".max",           PT_FLOAT    },
            { ".default",       PT_FLOAT    },
            NULL
        };

        void GraphFrameData::Listener::notify(atom_t property)
        {
            pValue->commit(property);
        }

        GraphFrameData::GraphFrameData(prop::Listener *listener):
            MultiProperty(vAtoms, P_COUNT, listener),
            sListener(this)
        {
            vData       = NULL;
            nRows       = 0;
            nCols       = 0;
            nRowId      = 0;
            nPendingId  = 0;
            nStride     = 0;
            nCapacity   = 0;
            fMin        = 0.0f;
            fMax        = 1.0f;
            fDfl        = 0.0f;
            pPtr        = NULL;
        }

        GraphFrameData::~GraphFrameData()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);

            if (pPtr != NULL)
                lsp::free_aligned(pPtr);

            vData       = NULL;
            nRows       = 0;
            nCols       = 0;
            nRowId      = 0;
            nPendingId  = 0;
            nStride     = 0;
            nCapacity   = 0;
            fMin        = 0.0f;
            fMax        = 1.0f;
            fDfl        = 0.0f;
            pPtr        = NULL;
        }

        void GraphFrameData::sync()
        {
        }

        void GraphFrameData::commit(atom_t property)
        {
        }

        bool GraphFrameData::resize_buffer(size_t rows, size_t cols)
        {
            // TODO
            return false;
        }

        ssize_t GraphFrameData::row_index(uint32_t id) const
        {
            if ((vData == NULL) || (id > nPendingId))
                return -1;

            uint32_t delta  = (nPendingId >= nRowId) ? nRows : -nRows;
            uint32_t first  = nPendingId - delta;

            return (id > first) ? id & (nCapacity - 1) : -1;
        }

        const float *GraphFrameData::row(uint32_t id) const
        {
            ssize_t rowid   = row_index(id);
            return (rowid >= 0) ? &vData[rowid * nStride] : NULL;
        }

        bool GraphFrameData::set_size(size_t rows, size_t cols)
        {
            if (!resize_buffer(rows, cols))
                return false;

            sync();
            return true;
        }

        float GraphFrameData::dfl() const
        {
            return (fMin <= fMax) ?
                    lsp_limit(fDfl, fMin, fMax) :
                    lsp_limit(fDfl, fMax, fMin);
        }

        bool GraphFrameData::set_row(uint32_t id, const float *data, size_t columns)
        {
            ssize_t rowid   = row_index(id);
            if (rowid < 0)
                return false;

            ssize_t count = nCols - columns;
            if (count < 0)
            {
                if (!resize_buffer(nRows, columns))
                    return false;
                count = 0;
            }

            if (vData != NULL)
            {
                float *dst = &vData[rowid * nStride];
                dsp::copy(dst, data, columns);
                dsp::fill(&dst[columns], dfl(), count);
            }

            sync();
            return true;
        }

        void GraphFrameData::clear()
        {
            dsp::fill(vData, dfl(), nCapacity * nStride);
            nRowId      = nPendingId - nRows;

            if (pListener != NULL)
                pListener->notify(this);
        }

        namespace prop
        {
            void GraphFrameData::advance()
            {
            }

            void GraphFrameData::advance(size_t rows)
            {
            }

            status_t GraphFrameData::init(Style *style, size_t rows, size_t cols)
            {
                return STATUS_OK;
            }

            status_t GraphFrameData::override(Style *style, size_t rows, size_t cols)
            {
                return STATUS_OK;
            }
        }
    }
}


