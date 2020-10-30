/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 15 мая 2020 г.
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
        const prop::desc_t Layout::DESC[] =
        {
            { "",           PT_STRING   },
            { ".halign",    PT_FLOAT    },
            { ".valign",    PT_FLOAT    },
            { ".hscale",    PT_FLOAT    },
            { ".vscale",    PT_FLOAT    },
            { NULL,         PT_UNKNOWN  }
        };

        void Layout::Listener::notify(atom_t property)
        {
            pValue->commit(property);
        }

        Layout::Layout(prop::Listener *listener):
            MultiProperty(vAtoms, P_COUNT, listener),
            sListener(this)
        {
            hAlign      = 0.0f;
            vAlign      = 0.0f;
            hScale      = 0.0f;
            vScale      = 0.0f;
        }

        Layout::~Layout()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void Layout::sync()
        {
            if (pStyle != NULL)
            {
                pStyle->begin(&sListener);
                {
                    // Simple components
                    if (vAtoms[P_HALIGN] >= 0)
                        pStyle->set_float(vAtoms[P_HALIGN], hAlign);
                    if (vAtoms[P_VALIGN] >= 0)
                        pStyle->set_float(vAtoms[P_VALIGN], vAlign);
                    if (vAtoms[P_HSCALE] >= 0)
                        pStyle->set_float(vAtoms[P_HSCALE], hScale);
                    if (vAtoms[P_VSCALE] >= 0)
                        pStyle->set_float(vAtoms[P_VSCALE], vScale);

                    // Compound objects
                    LSPString s;
                    if (vAtoms[P_VALUE] >= 0)
                    {
                        if (s.fmt_ascii("%.4f %.4f %.4f %.4f", hAlign, vAlign, hScale, vScale))
                            pStyle->set_string(vAtoms[P_VALUE], &s);
                    }
                }
                pStyle->end();
            }

            if (pListener != NULL)
                pListener->notify(this);
        }

        void Layout::parse(const LSPString *s)
        {
            float v[4];
            size_t n = MultiProperty::parse_floats(v, 4, s);
            switch (n)
            {
                case 1:
                    hAlign      = lsp_limit(v[0], -1.0f, 1.0f);
                    vAlign      = hAlign;
                    hScale      = 0.0f;
                    vScale      = 0.0f;
                    break;

                case 2:
                    hAlign      = lsp_limit(v[0], -1.0f, 1.0f);
                    vAlign      = lsp_limit(v[1], -1.0f, 1.0f);
                    hScale      = 0.0f;
                    vScale      = 0.0f;
                    break;

                case 3:
                    hAlign      = lsp_limit(v[0], -1.0f, 1.0f);
                    vAlign      = lsp_limit(v[1], -1.0f, 1.0f);
                    hScale      = lsp_limit(v[2], 0.0f, 1.0f);
                    vScale      = hScale;
                    break;

                case 4:
                    hAlign      = lsp_limit(v[0], -1.0f, 1.0f);
                    vAlign      = lsp_limit(v[1], -1.0f, 1.0f);
                    hScale      = lsp_limit(v[2], 0.0f, 1.0f);
                    vScale      = lsp_limit(v[3], 0.0f, 1.0f);
                    break;

                default:
                    break;
            }
        }

        void Layout::commit(atom_t property)
        {
            if ((pStyle == NULL) || (property < 0))
                return;

            float v;
            if ((property == vAtoms[P_HALIGN]) && (pStyle->get_float(vAtoms[P_HALIGN], &v) == STATUS_OK))
                hAlign      = lsp_limit(v, -1.0f, 1.0f);
            if ((property == vAtoms[P_VALIGN]) && (pStyle->get_float(vAtoms[P_VALIGN], &v) == STATUS_OK))
                vAlign      = lsp_limit(v, -1.0f, 1.0f);
            if ((property == vAtoms[P_HSCALE]) && (pStyle->get_float(vAtoms[P_HSCALE], &v) == STATUS_OK))
                hScale      = lsp_limit(v, 0.0f, 1.0f);
            if ((property == vAtoms[P_VSCALE]) && (pStyle->get_float(vAtoms[P_VSCALE], &v) == STATUS_OK))
                vScale      = lsp_limit(v, 0.0f, 1.0f);

            LSPString s;
            if ((property == vAtoms[P_VALUE]) && (pStyle->get_string(vAtoms[P_VALUE], &s) == STATUS_OK))
                parse(&s);

            if (pListener != NULL)
                pListener->notify(this);
        }

        float Layout::set_halign(float v)
        {
            float prev  = hAlign;
            v           = lsp_limit(v, -1.0f, 1.0f);
            if (prev == v)
                return prev;

            hAlign      = v;
            sync();

            return prev;
        }

        float Layout::set_valign(float v)
        {
            float prev  = vAlign;
            v           = lsp_limit(v, -1.0f, 1.0f);
            if (prev == v)
                return prev;

            vAlign      = v;
            sync();

            return prev;
        }

        float Layout::set_hscale(float v)
        {
            float prev  = hScale;
            v           = lsp_limit(v, 0.0f, 1.0f);
            if (prev == v)
                return prev;

            hScale      = v;
            sync();

            return prev;
        }

        float Layout::set_vscale(float v)
        {
            float prev  = vScale;
            v           = lsp_limit(v, 0.0f, 1.0f);
            if (prev == v)
                return prev;

            vScale      = v;
            sync();

            return prev;
        }

        void Layout::set_align(float h, float v)
        {
            h           = lsp_limit(h, -1.0f, 1.0f);
            v           = lsp_limit(v, -1.0f, 1.0f);
            if ((hAlign == h) && (vAlign == v))
                return;

            hAlign      = h;
            vAlign      = v;
            sync();
        }

        void Layout::set_scale(float h, float v)
        {
            h           = lsp_limit(h, 0.0f, 1.0f);
            v           = lsp_limit(v, 0.0f, 1.0f);
            if ((hScale == h) && (vScale == v))
                return;

            hScale      = h;
            vScale      = v;
            sync();
        }

        void Layout::set(float halign, float valign, float hscale, float vscale)
        {
            halign      = lsp_limit(halign, -1.0f, 1.0f);
            valign      = lsp_limit(valign, -1.0f, 1.0f);
            hscale      = lsp_limit(hscale, 0.0f, 1.0f);
            vscale      = lsp_limit(vscale, 0.0f, 1.0f);

            if ((hAlign == halign) && (vAlign == valign) &&
                (hScale == hscale) && (vScale == vscale))
                return;

            hAlign      = halign;
            vAlign      = valign;
            hScale      = hscale;
            vScale      = vscale;
            sync();
        }

        void Layout::apply(ws::rectangle_t *dst, const ws::rectangle_t *src, const ws::size_limit_t *req)
        {
//            lsp_trace("before: x=%d, y=%d, w=%d, h=%d", int(src->nLeft), int(src->nTop), int(src->nWidth), int(src->nHeight));
            // Estimate the minimum area size and the amount of free space
            ssize_t w       = lsp_max(0, req->nMinWidth);
            ssize_t h       = lsp_max(0, req->nMinHeight);
            ssize_t xgap    = lsp_max(0, src->nWidth - w);
            ssize_t ygap    = lsp_max(0, src->nHeight - h);

            // Scale the area size (if possible)
            w              += lsp_max(xgap * hScale, 0.0f);
            h              += lsp_max(ygap * hScale, 0.0f);
            if (req->nMaxWidth > 0)
                w              = lsp_min(w, req->nMaxWidth);
            if (req->nMaxHeight > 0)
                h              = lsp_min(h, req->nMaxHeight);

            // Estimate the amount of free space and update position (if possible)
            xgap            = lsp_max(0, src->nWidth  - w);
            ygap            = lsp_max(0, src->nHeight - h);

            // Scale position (if possible) and store results
            dst->nLeft      = src->nLeft + xgap * (hAlign + 1.0f) * 0.5f;
            dst->nTop       = src->nTop  + ygap * (vAlign + 1.0f) * 0.5f;
            dst->nWidth     = w;
            dst->nHeight    = h;
//            lsp_trace("after: x=%d, y=%d, w=%d, h=%d", int(dst->nLeft), int(dst->nTop), int(dst->nWidth), int(dst->nHeight));
        }

        namespace prop
        {
            status_t Layout::init(Style *style, float halign, float valign, float hscale, float vscale)
            {
                if (pStyle == NULL)
                    return STATUS_BAD_STATE;

                style->begin();
                {
                    style->create_float(vAtoms[P_HALIGN], halign);
                    style->create_float(vAtoms[P_VALIGN], valign);
                    style->create_float(vAtoms[P_HSCALE], hscale);
                    style->create_float(vAtoms[P_VSCALE], vscale);

                    // Compound objects
                    LSPString s;
                    s.fmt_ascii("%.4f %.4f %.4f %.4f", halign, valign, hscale, vscale);
                    style->create_string(vAtoms[P_VALUE], &s);
                }
                style->end();
                return STATUS_OK;
            }

            status_t Layout::override(Style *style, float halign, float valign, float hscale, float vscale)
            {
                if (pStyle == NULL)
                    return STATUS_BAD_STATE;

                style->begin();
                {
                    style->override_float(vAtoms[P_HALIGN], halign);
                    style->override_float(vAtoms[P_VALIGN], valign);
                    style->override_float(vAtoms[P_HSCALE], hscale);
                    style->override_float(vAtoms[P_VSCALE], vscale);

                    // Compound objects
                    LSPString s;
                    s.fmt_ascii("%.4f %.4f %.4f %.4f", halign, valign, hscale, vscale);
                    style->override_string(vAtoms[P_VALUE], &s);
                }
                style->end();
                return STATUS_OK;
            }
        }

    } /* namespace tk */
} /* namespace lsp */
