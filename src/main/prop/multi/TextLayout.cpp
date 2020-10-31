/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 2 июн. 2020 г.
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
        const prop::desc_t TextLayout::DESC[] =
        {
            { "",           PT_STRING   },
            { ".halign",    PT_FLOAT    },
            { ".valign",    PT_FLOAT    },
            { NULL,         PT_UNKNOWN  }
        };

        void TextLayout::Listener::notify(atom_t property)
        {
            pValue->commit(property);
        }

        TextLayout::TextLayout(prop::Listener *listener):
            MultiProperty(vAtoms, P_COUNT, listener),
            sListener(this)
        {
            hAlign      = 0.0f;
            vAlign      = 0.0f;
        }

        TextLayout::~TextLayout()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void TextLayout::sync()
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

                    // Compound objects
                    LSPString s;
                    if (vAtoms[P_VALUE] >= 0)
                    {
                        if (s.fmt_ascii("%.4f %.4f", hAlign, vAlign))
                            pStyle->set_string(vAtoms[P_VALUE], &s);
                    }
                }
                pStyle->end();
            }

            if (pListener != NULL)
                pListener->notify(this);
        }

        void TextLayout::parse(const LSPString *s)
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

        void TextLayout::commit(atom_t property)
        {
            if ((pStyle == NULL) || (property < 0))
                return;

            float v;
            if ((property == vAtoms[P_HALIGN]) && (pStyle->get_float(vAtoms[P_HALIGN], &v) == STATUS_OK))
                hAlign      = lsp_limit(v, -1.0f, 1.0f);
            if ((property == vAtoms[P_VALIGN]) && (pStyle->get_float(vAtoms[P_VALIGN], &v) == STATUS_OK))
                vAlign      = lsp_limit(v, -1.0f, 1.0f);

            LSPString s;
            if ((property == vAtoms[P_VALUE]) && (pStyle->get_string(vAtoms[P_VALUE], &s) == STATUS_OK))
                parse(&s);

            if (pListener != NULL)
                pListener->notify(this);
        }

        float TextLayout::set_halign(float v)
        {
            float prev  = hAlign;
            v           = lsp_limit(v, -1.0f, 1.0f);
            if (prev == v)
                return prev;

            hAlign      = v;
            sync();

            return prev;
        }

        float TextLayout::set_valign(float v)
        {
            float prev  = vAlign;
            v           = lsp_limit(v, -1.0f, 1.0f);
            if (prev == v)
                return prev;

            vAlign      = v;
            sync();

            return prev;
        }

        void TextLayout::set(float h, float v)
        {
            h           = lsp_limit(h, -1.0f, 1.0f);
            v           = lsp_limit(v, -1.0f, 1.0f);
            if ((hAlign == h) && (vAlign == v))
                return;

            hAlign      = h;
            vAlign      = v;
            sync();
        }

        status_t TextLayout::init()
        {
            pStyle->begin();
            {
                pStyle->create_float(vAtoms[P_HALIGN], halign());
                pStyle->create_float(vAtoms[P_VALIGN], valign());

                // Compound objects
                LSPString s;
                s.fmt_ascii("%.4f %.4f", halign(), valign());
                pStyle->create_string(vAtoms[P_VALUE], &s);
            }
            pStyle->end();
            return STATUS_OK;
        }

        status_t TextLayout::override()
        {
            pStyle->begin();
            {
                pStyle->override_float(vAtoms[P_HALIGN], halign());
                pStyle->override_float(vAtoms[P_VALIGN], valign());

                // Compound objects
                LSPString s;
                s.fmt_ascii("%.4f %.4f", halign(), valign());
                pStyle->override_string(vAtoms[P_VALUE], &s);
            }
            pStyle->end();
            return STATUS_OK;
        }

        namespace prop
        {
            status_t TextLayout::init(Style *style, float halign, float valign)
            {
                if (pStyle == NULL)
                    return STATUS_BAD_STATE;

                style->begin();
                {
                    style->create_float(vAtoms[P_HALIGN], halign);
                    style->create_float(vAtoms[P_VALIGN], valign);

                    // Compound objects
                    LSPString s;
                    s.fmt_ascii("%.4f %.4f", halign, valign);
                    style->create_string(vAtoms[P_VALUE], &s);
                }
                style->end();
                return STATUS_OK;
            }

            status_t TextLayout::init(const char *name, Style *style, float halign, float valign)
            {
                prop::TextLayout v;
                LSP_STATUS_ASSERT(v.bind(name, style));
                v.set(halign, valign);
                return v.init();
            }

            status_t TextLayout::override(const char *name, Style *style, float halign, float valign)
            {
                prop::TextLayout v;
                LSP_STATUS_ASSERT(v.bind(name, style));
                v.set(halign, valign);
                return v.override();
            }
        }

    } /* namespace tk */
} /* namespace lsp */



