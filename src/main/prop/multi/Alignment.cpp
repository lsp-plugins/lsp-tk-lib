/*
 * Copyright (C) 2021 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2021 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 1 июн. 2021 г.
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
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace tk
    {
        const prop::desc_t Alignment::DESC[] =
        {
            { "",           PT_STRING   },
            { ".align",     PT_FLOAT    },
            { ".scale",     PT_FLOAT    },
            { NULL,         PT_UNKNOWN  }
        };

        Alignment::Alignment(prop::Listener *listener):
            MultiProperty(vAtoms, P_COUNT, listener)
        {
            nAlign      = 0.0f;
            nScale      = 0.0f;
        }

        Alignment::~Alignment()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void Alignment::push()
        {
            // Simple components
            if (vAtoms[P_ALIGN] >= 0)
                pStyle->set_float(vAtoms[P_ALIGN], nAlign);
            if (vAtoms[P_SCALE] >= 0)
                pStyle->set_float(vAtoms[P_SCALE], nScale);

            // Compound objects
            LSPString s;
            if (vAtoms[P_VALUE] >= 0)
            {
                if (s.fmt_ascii("%.4f %.4f", nAlign, nScale))
                    pStyle->set_string(vAtoms[P_VALUE], &s);
            }
        }

        void Alignment::parse(const LSPString *s)
        {
            float v[2];
            size_t n = MultiProperty::parse_floats(v, 2, s);
            switch (n)
            {
                case 1:
                    nAlign      = lsp_limit(v[0], -1.0f, 1.0f);
                    nScale      = 0.0f;
                    break;

                case 2:
                    nAlign      = lsp_limit(v[0], -1.0f, 1.0f);
                    nScale      = lsp_limit(v[1], 0.0f, 1.0f);
                    break;

                default:
                    break;
            }
        }

        void Alignment::commit(atom_t property)
        {
            float v;
            if ((property == vAtoms[P_ALIGN]) && (pStyle->get_float(vAtoms[P_ALIGN], &v) == STATUS_OK))
                nAlign      = lsp_limit(v, -1.0f, 1.0f);
            if ((property == vAtoms[P_SCALE]) && (pStyle->get_float(vAtoms[P_SCALE], &v) == STATUS_OK))
                nScale      = lsp_limit(v, 0.0f, 1.0f);

            LSPString s;
            if ((property == vAtoms[P_VALUE]) && (pStyle->get_string(vAtoms[P_VALUE], &s) == STATUS_OK))
                parse(&s);
        }

        float Alignment::set_align(float v)
        {
            float prev  = nAlign;
            v           = lsp_limit(v, -1.0f, 1.0f);
            if (prev == v)
                return prev;

            nAlign      = v;
            sync();

            return prev;
        }

        float Alignment::set_scale(float v)
        {
            float prev  = nScale;
            v           = lsp_limit(v, 0.0f, 1.0f);
            if (prev == v)
                return prev;

            nScale      = v;
            sync();

            return prev;
        }

        void Alignment::set(float align, float scale)
        {
            align       = lsp_limit(align, -1.0f, 1.0f);
            scale       = lsp_limit(scale, 0.0f, 1.0f);

            if ((nAlign == align) && (nScale == scale))
                return;

            nAlign      = align;
            nScale      = scale;
            sync();
        }

        void Alignment::happly(ws::rectangle_t *dst, const ws::rectangle_t *src, const ws::size_limit_t *req)
        {
            // Estimate the minimum area size and the amount of free space
            ssize_t w       = lsp_max(0, req->nMinWidth);
            ssize_t xgap    = lsp_max(0, src->nWidth - w);

            // Scale the area size (if possible)
            w              += lsp_max(xgap * nScale, 0.0f);
            if (req->nMaxWidth > 0)
                w              = lsp_min(w, req->nMaxWidth);

            // Estimate the amount of free space and update position (if possible)
            xgap            = lsp_max(0, src->nWidth  - w);

            // Scale position (if possible) and store results
            dst->nLeft      = src->nLeft + xgap * (nAlign + 1.0f) * 0.5f;
            dst->nTop       = src->nTop;
            dst->nWidth     = w;
            dst->nHeight    = src->nHeight;
        }

        void Alignment::happly(ws::rectangle_t *dst, const ws::rectangle_t *src, ssize_t avail)
        {
            // Estimate the minimum area size and the amount of free space
            ssize_t w       = lsp_max(0, src->nWidth);
            ssize_t xgap    = lsp_max(0, avail - w);

            // Scale the area size (if possible)
            w              += lsp_max(xgap * nScale, 0.0f);

            // Estimate the amount of free space and update position (if possible)
            xgap            = lsp_max(0, avail  - w);

            // Scale position (if possible) and store results
            dst->nLeft      = src->nLeft + xgap * (nAlign + 1.0f) * 0.5f;
            dst->nTop       = src->nTop;
            dst->nWidth     = w;
            dst->nHeight    = src->nHeight;
        }

        void Alignment::vapply(ws::rectangle_t *dst, const ws::rectangle_t *src, const ws::size_limit_t *req)
        {
            // Estimate the minimum area size and the amount of free space
            ssize_t h       = lsp_max(0, req->nMinHeight);
            ssize_t ygap    = lsp_max(0, src->nHeight - h);

            // Scale the area size (if possible)
            h              += lsp_max(ygap * nScale, 0.0f);
            if (req->nMaxHeight > 0)
                h              = lsp_min(h, req->nMaxHeight);

            // Estimate the amount of free space and update position (if possible)
            ygap            = lsp_max(0, src->nHeight - h);

            // Scale position (if possible) and store results
            dst->nLeft      = src->nLeft;
            dst->nTop       = src->nTop  + ygap * (nAlign + 1.0f) * 0.5f;
            dst->nWidth     = src->nWidth;
            dst->nHeight    = h;
        }

        void Alignment::vapply(ws::rectangle_t *dst, const ws::rectangle_t *src, ssize_t avail)
        {
            // Estimate the minimum area size and the amount of free space
            ssize_t h       = lsp_max(0, src->nHeight);
            ssize_t ygap    = lsp_max(0, avail - h);

            // Scale the area size (if possible)
            h              += lsp_max(ygap * nScale, 0.0f);

            // Estimate the amount of free space and update position (if possible)
            ygap            = lsp_max(0, avail - h);

            // Scale position (if possible) and store results
            dst->nLeft      = src->nLeft;
            dst->nTop       = src->nTop  + ygap * (nAlign + 1.0f) * 0.5f;
            dst->nWidth     = src->nWidth;
            dst->nHeight    = h;
        }

        void Alignment::apply(ws::rectangle_t *dst, const ws::rectangle_t *src, const ws::size_limit_t *req, orientation_t orientation)
        {
            if (orientation == O_HORIZONTAL)
                happly(dst, src, req);
            else
                vapply(dst, src, req);
        }

        void Alignment::apply(ws::rectangle_t *dst, const ws::rectangle_t *src, ssize_t avail, orientation_t orientation)
        {
            if (orientation == O_HORIZONTAL)
                happly(dst, src, avail);
            else
                vapply(dst, src, avail);
        }

        void Alignment::apply(ws::rectangle_t *dst, const ws::size_limit_t *req, orientation_t orientation)
        {
            if (orientation == O_HORIZONTAL)
                happly(dst, dst, req);
            else
                vapply(dst, dst, req);
        }

        void Alignment::apply(ws::rectangle_t *dst, ssize_t avail, orientation_t orientation)
        {
            if (orientation == O_HORIZONTAL)
                happly(dst, dst, avail);
            else
                vapply(dst, dst, avail);
        }

    } /* namespace tk */
} /* namespace lsp */




