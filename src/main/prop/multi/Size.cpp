/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 10 мая 2020 г.
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
        const prop::desc_t Size::DESC[] =
        {
            { "",           PT_STRING   },
            { ".width",     PT_INT      },
            { ".height",    PT_INT      },
            { NULL,         PT_UNKNOWN  }
        };

        Size::Size(prop::Listener *listener):
            MultiProperty(vAtoms, P_COUNT, listener)
        {
            nWidth              = 0;
            nHeight             = 0;
        }

        Size::~Size()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void Size::commit(atom_t property)
        {
            ssize_t v;
            if ((property == vAtoms[P_WIDTH]) && (pStyle->get_int(vAtoms[P_WIDTH], &v) == STATUS_OK))
                nWidth      = lsp_max(v, 0);
            if ((property == vAtoms[P_HEIGHT]) && (pStyle->get_int(vAtoms[P_HEIGHT], &v) == STATUS_OK))
                nHeight     = lsp_max(v, 0);

            LSPString s;
            if ((property == vAtoms[P_VALUE]) && (pStyle->get_string(vAtoms[P_VALUE], &s) == STATUS_OK))
            {
                ssize_t xv[2];
                size_t n = Property::parse_ints(xv, 2, &s);
                if (n == 2)
                {
                    nWidth      = lsp_max(xv[0], 0);
                    nHeight     = lsp_max(xv[1], 0);
                }
            }
        }

        void Size::push()
        {
            // Simple components
            if (vAtoms[P_WIDTH] >= 0)
                pStyle->set_int(vAtoms[P_WIDTH], nWidth);
            if (vAtoms[P_HEIGHT] >= 0)
                pStyle->set_int(vAtoms[P_HEIGHT], nHeight);

            // Compound objects
            LSPString s;
            if (vAtoms[P_VALUE] >= 0)
            {
                if (s.fmt_ascii("%ld %ld", long(nWidth), long(nHeight)))
                    pStyle->set_string(vAtoms[P_VALUE], &s);
            }
        }

        size_t Size::set_width(size_t value)
        {
            size_t old      = nWidth;
            if (old == value)
                return value;

            nWidth          = value;
            sync();
            return old;
        }

        size_t Size::set_height(size_t value)
        {
            size_t old      = nHeight;
            if (old == value)
                return value;

            nHeight         = value;
            sync();
            return old;
        }

        void Size::set(size_t width, size_t height)
        {
            if ((nWidth == width) &&
                (nHeight == height))
                return;

            nWidth      = width;
            nHeight     = height;
            sync();
        }

        void Size::set(size_t width, size_t height, float scale)
        {
            if (scale > 0.0f)
            {
                width   = size_t(float(width) / scale);
                height  = size_t(float(height) / scale);
            }

            set(width, height);
        }

        void Size::compute(ws::rectangle_t *dst, float scale)
        {
            scale           = lsp_max(0.0f, scale);
            dst->nWidth     = lsp_max(0, float(nWidth)  * scale);
            dst->nHeight    = lsp_max(0, float(nHeight) * scale);
        }

        void Size::set(const Size *p)
        {
            if ((nWidth == p->nWidth) &&
                (nHeight == p->nHeight))
                return;

            nWidth      = p->nWidth;
            nHeight     = p->nHeight;
            sync();
        }

        bool Size::intersection(ws::rectangle_t *dst, const ws::rectangle_t *a, const ws::rectangle_t *b)
        {
            // Check horizontal intersection
            ssize_t left    = lsp_max(a->nLeft, b->nLeft);
            ssize_t right   = lsp_min(a->nLeft + a->nWidth, b->nLeft + b->nWidth);
            if (left >= right)
                return false;

            // Check vertial intersection
            ssize_t top     = lsp_max(a->nTop, b->nTop);
            ssize_t bottom  = lsp_min(a->nTop + a->nHeight, b->nTop + b->nHeight);
            if (top >= bottom)
                return false;

            // A is crossing B, compute boundaries
            dst->nLeft      = left;
            dst->nTop       = top;
            dst->nWidth     = right - left;
            dst->nHeight    = bottom - top;

            return true;
        }

        bool Size::overlap(const ws::rectangle_t *a, const ws::rectangle_t *b)
        {
            // Check horizontal intersection
            ssize_t left    = lsp_max(a->nLeft, b->nLeft);
            ssize_t right   = lsp_min(a->nLeft + a->nWidth, b->nLeft + b->nWidth);
            if (left >= right)
                return false;

            // Check vertial intersection
            ssize_t top     = lsp_max(a->nTop, b->nTop);
            ssize_t bottom  = lsp_min(a->nTop + a->nHeight, b->nTop + b->nHeight);
            if (top >= bottom)
                return false;

            return true;
        }

        bool Size::is_empty(const ws::rectangle_t *r)
        {
            return (r->nWidth <= 0) || (r->nHeight <= 0);
        }

        void Size::init(ws::rectangle_t *dst, ssize_t left, ssize_t top, ssize_t width, ssize_t height)
        {
            dst->nLeft          = left;
            dst->nTop           = top;
            dst->nWidth         = width;
            dst->nHeight        = height;
        }

        namespace prop
        {
            void Size::commit_value(size_t width, size_t height, float scale)
            {
                if (scale > 0.0f)
                {
                    nWidth  = size_t(float(width) / scale);
                    nHeight = size_t(float(height) / scale);
                }
                else
                {
                    nWidth  = width;
                    nHeight = height;
                }
                sync(false);
            }

            void Size::commit_value(const ws::rectangle_t *rect, float scale)
            {
                commit_value(rect->nWidth, rect->nHeight, scale);
            }
        }
    } /* namespace tk */
} /* namespace lsp */



