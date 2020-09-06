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
#include <lsp-plug.in/common/bits.h>
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
            { ".min",           PT_FLOAT    },
            { ".max",           PT_FLOAT    },
            { ".default",       PT_FLOAT    },
            { ".size",          PT_STRING   },
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
            nChanges    = 0;
            nRowId      = 0;
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
            nChanges    = 0;
            nRowId      = 0;
            nStride     = 0;
            nCapacity   = 0;
            fMin        = 0.0f;
            fMax        = 1.0f;
            fDfl        = 0.0f;
            pPtr        = NULL;
        }


        void GraphFrameData::commit(atom_t property)
        {
            if ((pStyle == NULL) || (property < 0))
                return;

            float fv;
            ssize_t iv;
            LSPString s;

            if ((property == vAtoms[P_ROWS]) && (pStyle->get_int(vAtoms[P_ROWS], &iv) == STATUS_OK))
                resize_buffer(lsp_max(0, iv), nCols);
            if ((property == vAtoms[P_COLS]) && (pStyle->get_int(vAtoms[P_COLS], &iv) == STATUS_OK))
                resize_buffer(nRows, lsp_max(0, iv));
            if ((property == vAtoms[P_MIN]) && (pStyle->get_float(vAtoms[P_MIN], &fv) == STATUS_OK))
                fMin = fv;
            if ((property == vAtoms[P_MAX]) && (pStyle->get_float(vAtoms[P_MAX], &fv) == STATUS_OK))
                fMax = fv;
            if ((property == vAtoms[P_DFL]) && (pStyle->get_float(vAtoms[P_DFL], &fv) == STATUS_OK))
                fDfl = fv;

            if ((property == vAtoms[P_SIZE]) && (pStyle->get_string(vAtoms[P_SIZE], &s) == STATUS_OK))
            {
                ssize_t xv[2];

                size_t n = Property::parse_ints(xv, 2, &s);
                switch (n)
                {
                    case 1:
                        xv[0]       = lsp_max(0, xv[0]);
                        resize_buffer(xv[0], xv[0]);
                        break;
                    case 2:
                        xv[0]       = lsp_max(0, xv[0]);
                        xv[1]       = lsp_max(0, xv[1]);
                        resize_buffer(xv[0], xv[1]);
                        break;
                    default:
                        break;
                }
            }

            if (pListener != NULL)
                pListener->notify(this);
        }

        void GraphFrameData::sync()
        {
            if (pStyle != NULL)
            {
                pStyle->begin(&sListener);
                {
                    if (vAtoms[P_ROWS] >= 0)
                        pStyle->set_int(vAtoms[P_ROWS], nRows);
                    if (vAtoms[P_COLS] >= 0)
                        pStyle->set_int(vAtoms[P_COLS], nCols);
                    if (vAtoms[P_MIN] >= 0)
                        pStyle->set_float(vAtoms[P_MIN], fMin);
                    if (vAtoms[P_MAX] >= 0)
                        pStyle->set_float(vAtoms[P_MAX], fMax);
                    if (vAtoms[P_DFL] >= 0)
                        pStyle->set_float(vAtoms[P_DFL], fDfl);

                    // Mixed components
                    LSPString s;
                    if (vAtoms[P_SIZE] >= 0)
                    {
                        s.fmt_ascii("%d %d", int(nRows), int(nCols));
                        pStyle->set_string(vAtoms[P_SIZE], &s);
                    }
                }
                pStyle->end();
            }

            if (pListener != NULL)
                pListener->notify(this);
        }

        bool GraphFrameData::resize_buffer(size_t rows, size_t cols)
        {
            // Did not even changed?
            if ((nRows == rows) && (nCols == cols))
                return true;

            size_t stride   = lsp::align_size(cols*sizeof(float), DATA_ALIGNMENT) / sizeof(float);
            size_t cap      = (rows > 0) ? (1 << lsp::int_log2(rows)) : 0;
            if (cap < rows)
                cap            <<= 1;

            float min       = lsp_min(fMin, fMax);
            float max       = lsp_max(fMin, fMax);
            float dfl       = lsp_limit(fDfl, min, max);

            // Need to re-allocate data?
            if ((cap != nCapacity) || (stride != nStride))
            {
                uint8_t *ptr = NULL;
                float *data  = lsp::alloc_aligned<float>(ptr, cap * stride, DATA_ALIGNMENT);
                if (data == NULL)
                    return false;

                dsp::fill(data, dfl, cap * stride); // Clean up the buffer
                if (vData != NULL)
                {
                    size_t ncols = lsp_min(nCols, cols);
                    size_t nrows = lsp_min(nRows, rows);

                    if (ncols > 0)
                    {
                        // Copy rows
                        for (size_t i=0, row=nRowId - nrows; i<nrows; ++i)
                        {
                            size_t didx         = (row + i) & (cap - 1);
                            size_t sidx         = (row + i) & (nCapacity - 1);
                            dsp::limit2(&data[didx * stride], &vData[sidx * nStride], min, max, ncols);
                        }
                    }

                    // Free previously allocated data
                    lsp::free_aligned(pPtr);
                }

                // Update pointers
                pPtr        = ptr;
                vData       = data;
                nStride     = stride;
                nCapacity   = cap;
            }

            nRows   = rows;
            nCols   = cols;
            nChanges= nRows;

            return true;
        }

        ssize_t GraphFrameData::row_index(uint32_t id, size_t range) const
        {
            if (vData == NULL)
                return -1;

            uint32_t first = nRowId - range;

            if (first < nRowId)
            {
                if ((id < first) || (id >= nRowId))
                    return -1;
            }
            else
            {
                if ((id < first) && (id >= nRowId))
                    return -1;
            }

            return id & (nCapacity - 1);
        }

        const float *GraphFrameData::row(uint32_t id) const
        {
            ssize_t rowid   = row_index(id, nCapacity);
            return (rowid >= 0) ? &vData[rowid * nStride] : NULL;
        }

        bool GraphFrameData::set_size(size_t rows, size_t cols)
        {
            if (!resize_buffer(rows, cols))
                return false;

            sync();
            return true;
        }

        float GraphFrameData::get_default() const
        {
            return (fMin <= fMax) ?
                    lsp_limit(fDfl, fMin, fMax) :
                    lsp_limit(fDfl, fMax, fMin);
        }

        bool GraphFrameData::set_row(uint32_t id, const float *data, size_t columns)
        {
            if (vData == NULL)
                return false;

            // Check that we need to extend number of columns for the buffer
            if (columns > nCols)
            {
                if (!resize_buffer(nRows, columns))
                    return false;
            }

            // Estimate the minimum, maximum and default values
            float min       = lsp_min(fMin, fMax);
            float max       = lsp_max(fMin, fMax);
            float dfl       = lsp_limit(fDfl, min, max);

            // Check that row is outside of the allowed range
            ssize_t index   = row_index(id, nCapacity);
            if (index < 0) // Row is outside of the range and we need to append data?
            {
                // Need to append some row?
                uint32_t delta  = (id >= nRowId) ? nRowId : -nRowId;
                uint32_t offset = id - delta;

                if (offset < nCapacity)
                {
                    // Need to fill the buffer with empty data
                    for (size_t i=0; i<offset; ++i)
                    {
                        index           = (nRowId + i) & (nCapacity - 1);
                        dsp::fill(&vData[index * nStride], dfl, nStride);
                    }
                    nChanges        = lsp_min(nChanges + offset + 1, nRows);
                }
                else // Clear the buffer since we've already out of it
                {
                    dsp::fill(vData, get_default(), nCapacity * nStride);
                    nChanges        = nRows;
                }

                index           = id & (nCapacity - 1);
                nRowId          = id+1;
            }

            // Copy data to the destination row
            float *dst      = &vData[index * nStride];

            dsp::limit2(dst, data, min, max, columns);
            dsp::fill(&dst[columns], dfl, nStride - columns);

            sync();
            return true;
        }

        float GraphFrameData::set_min(float v)
        {
            float old = fMin;
            if (old == v)
                return old;

            fMin        = v;
            sync();

            return old;
        }

        float GraphFrameData::set_max(float v)
        {
            float old = fMax;
            if (old == v)
                return old;

            fMax        = v;
            sync();

            return old;
        }

        float GraphFrameData::set_default(float v)
        {
            float old = fDfl;
            if (old == v)
                return old;

            fDfl        = v;
            sync();

            return old;
        }

        void GraphFrameData::set_range(float min, float max, float dfl)
        {
            if ((min == fMin) && (max == fMax) && (dfl == fDfl))
                return;

            fMin        = min;
            fMax        = max;
            fDfl        = dfl;

            sync();
        }

        void GraphFrameData::clear()
        {
            dsp::fill(vData, get_default(), nCapacity * nStride);
            nChanges    = 0;

            if (pListener != NULL)
                pListener->notify(this);
        }

        void GraphFrameData::advance()
        {
            nChanges    = 0;
        }

        namespace prop
        {
            status_t GraphFrameData::init(Style *style, size_t rows, size_t cols, float min, float max, float dfl)
            {
                if (style == NULL)
                    return STATUS_BAD_ARGUMENTS;
                else if (pStyle == NULL)
                    return STATUS_BAD_STATE;

                style->begin();
                {
                    style->create_int(vAtoms[P_ROWS], rows);
                    style->create_int(vAtoms[P_COLS], cols);
                    style->create_float(vAtoms[P_MIN], min);
                    style->create_float(vAtoms[P_MAX], max);
                    style->create_float(vAtoms[P_DFL], dfl);

                    LSPString s;
                    s.fmt_ascii("%d %d", int(rows), int(cols));
                    style->create_string(vAtoms[P_SIZE], &s);
                }
                style->end();

                return STATUS_OK;
            }

            status_t GraphFrameData::override(Style *style, size_t rows, size_t cols, float min, float max, float dfl)
            {
                if (pStyle == NULL)
                    return STATUS_BAD_STATE;

                style->begin();
                {
                    style->override_int(vAtoms[P_ROWS], rows);
                    style->override_int(vAtoms[P_COLS], cols);
                    style->override_float(vAtoms[P_MIN], min);
                    style->override_float(vAtoms[P_MAX], max);
                    style->override_float(vAtoms[P_DFL], dfl);

                    LSPString s;
                    s.fmt_ascii("%d %d", int(rows), int(cols));
                    style->override_string(vAtoms[P_SIZE], &s);
                }
                style->end();

                return STATUS_OK;
            }
        }
    }
}


