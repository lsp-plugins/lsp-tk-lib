/*
 * Copyright (C) 2021 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2021 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 13 июн. 2021 г.
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
#include <lsp-plug.in/stdlib/locale.h>
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace tk
    {
        const prop::desc_t Arrangement::DESC[] =
        {
            { "",           PT_STRING   },
            { ".halign",    PT_FLOAT    },
            { ".valign",    PT_FLOAT    },
            { NULL,         PT_UNKNOWN  }
        };

        Arrangement::Arrangement(prop::Listener *listener):
            MultiProperty(vAtoms, P_COUNT, listener)
        {
            hAlign      = 0.0f;
            vAlign      = 0.0f;
        }

        Arrangement::~Arrangement()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void Arrangement::push()
        {
            // Simple components
            if (vAtoms[P_HALIGN] >= 0)
                pStyle->set_float(vAtoms[P_HALIGN], hAlign);
            if (vAtoms[P_VALIGN] >= 0)
                pStyle->set_float(vAtoms[P_VALIGN], vAlign);

            // Compound objects
            LSPString s;
            if (vAtoms[P_VALUE] >= 0)
            {
                SET_LOCALE_SCOPED(LC_NUMERIC, "C");
                if (s.fmt_ascii("%.4f %.4f", hAlign, vAlign))
                    pStyle->set_string(vAtoms[P_VALUE], &s);
            }
        }

        void Arrangement::parse(const LSPString *s)
        {
            float v[2];
            size_t n = MultiProperty::parse_floats(v, 2, s);
            switch (n)
            {
                case 1:
                    hAlign      = lsp_limit(v[0], -1.0f, 1.0f);
                    vAlign      = hAlign;
                    break;

                case 2:
                    hAlign      = lsp_limit(v[0], -1.0f, 1.0f);
                    vAlign      = lsp_limit(v[1], -1.0f, 1.0f);
                    break;

                default:
                    break;
            }
        }

        void Arrangement::commit(atom_t property)
        {
            float v;
            if ((property == vAtoms[P_HALIGN]) && (pStyle->get_float(vAtoms[P_HALIGN], &v) == STATUS_OK))
                hAlign      = lsp_limit(v, -1.0f, 1.0f);
            if ((property == vAtoms[P_VALIGN]) && (pStyle->get_float(vAtoms[P_VALIGN], &v) == STATUS_OK))
                vAlign      = lsp_limit(v, -1.0f, 1.0f);

            LSPString s;
            if ((property == vAtoms[P_VALUE]) && (pStyle->get_string(vAtoms[P_VALUE], &s) == STATUS_OK))
                parse(&s);
        }

        float Arrangement::set_halign(float v)
        {
            float prev  = hAlign;
            v           = lsp_limit(v, -1.0f, 1.0f);
            if (prev == v)
                return prev;

            hAlign      = v;
            sync();

            return prev;
        }

        float Arrangement::set_valign(float v)
        {
            float prev  = vAlign;
            v           = lsp_limit(v, -1.0f, 1.0f);
            if (prev == v)
                return prev;

            vAlign      = v;
            sync();

            return prev;
        }

        void Arrangement::set(float h, float v)
        {
            h           = lsp_limit(h, -1.0f, 1.0f);
            v           = lsp_limit(v, -1.0f, 1.0f);
            if ((hAlign == h) && (vAlign == v))
                return;

            hAlign      = h;
            vAlign      = v;
            sync();
        }


        void Arrangement::apply(ws::rectangle_t *dst, const ws::rectangle_t *src, const ws::rectangle_t *area)
        {
            ssize_t xgap    = lsp_max(0, area->nWidth - src->nWidth);
            ssize_t ygap    = lsp_max(0, area->nHeight - src->nHeight);

            // Scale the area size (if possible)
            dst->nWidth     = src->nWidth;
            dst->nHeight    = src->nHeight;
            dst->nLeft      = area->nLeft + xgap * (hAlign + 1.0f) * 0.5f;
            dst->nTop       = area->nTop  + ygap * (vAlign + 1.0f) * 0.5f;
        }

    } /* namespace tk */
} /* namespace lsp */



