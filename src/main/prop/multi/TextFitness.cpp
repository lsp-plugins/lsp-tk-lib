/*
 * TextFitness.cpp
 *
 *  Created on: 12 авг. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const prop::desc_t TextFitness::DESC[] =
        {
            { "",           PT_STRING   },
            { ".hfit",      PT_FLOAT    },
            { ".vfit",      PT_FLOAT    },
            { NULL,         PT_UNKNOWN  }
        };

        void TextFitness::Listener::notify(atom_t property)
        {
            pValue->commit(property);
        }

        TextFitness::TextFitness(prop::Listener *listener):
            MultiProperty(vAtoms, P_COUNT, listener),
            sListener(this)
        {
            hFit        = 1.0f;
            vFit        = 1.0f;
        }

        TextFitness::~TextFitness()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void TextFitness::sync()
        {
            if (pStyle != NULL)
            {
                pStyle->begin(&sListener);
                {
                    // Simple components
                    if (vAtoms[P_HFIT] >= 0)
                        pStyle->set_float(vAtoms[P_HFIT], hFit);
                    if (vAtoms[P_VFIT] >= 0)
                        pStyle->set_float(vAtoms[P_VFIT], vFit);

                    // Compound objects
                    LSPString s;
                    if (vAtoms[P_VALUE] >= 0)
                    {
                        if (s.fmt_ascii("%.4f %.4f", hFit, vFit))
                            pStyle->set_string(vAtoms[P_VALUE], &s);
                    }
                }
                pStyle->end();
            }

            if (pListener != NULL)
                pListener->notify(this);
        }

        void TextFitness::parse(const LSPString *s)
        {
            float v[2];
            size_t n = MultiProperty::parse_floats(v, 2, s);
            switch (n)
            {
                case 1:
                    hFit        = lsp_limit(v[0], -1.0f, 1.0f);
                    vFit        = hFit;
                    break;

                case 2:
                    hFit        = lsp_limit(v[0], -1.0f, 1.0f);
                    vFit        = lsp_limit(v[1], -1.0f, 1.0f);
                    break;

                default:
                    break;
            }
        }

        void TextFitness::commit(atom_t property)
        {
            if ((pStyle == NULL) || (property < 0))
                return;

            float v;
            if ((property == vAtoms[P_HFIT]) && (pStyle->get_float(vAtoms[P_HFIT], &v) == STATUS_OK))
                hFit        = lsp_limit(v, -1.0f, 1.0f);
            if ((property == vAtoms[P_VFIT]) && (pStyle->get_float(vAtoms[P_VFIT], &v) == STATUS_OK))
                vFit        = lsp_limit(v, -1.0f, 1.0f);

            LSPString s;
            if ((property == vAtoms[P_VALUE]) && (pStyle->get_string(vAtoms[P_VALUE], &s) == STATUS_OK))
                parse(&s);

            if (pListener != NULL)
                pListener->notify(this);
        }

        float TextFitness::set_hfit(float v)
        {
            float prev  = hFit;
            v           = lsp_max(0.0f, v);
            if (prev == v)
                return prev;

            hFit        = v;
            sync();

            return prev;
        }

        float TextFitness::set_vfit(float v)
        {
            float prev  = vFit;
            v           = lsp_max(0.0f, v);
            if (prev == v)
                return prev;

            vFit        = v;
            sync();

            return prev;
        }

        void TextFitness::set(float h, float v)
        {
            h           = lsp_max(0.0f, h);
            v           = lsp_max(0.0f, v);
            if ((hFit == h) && (vFit == v))
                return;

            hFit        = h;
            vFit        = v;
            sync();
        }

        void TextFitness::apply(ws::rectangle_t *r)
        {
            r->nWidth   = lsp_max(0.0f, hFit * r->nWidth);
            r->nHeight  = lsp_max(0.0f, hFit * r->nHeight);
        }

        namespace prop
        {
            status_t TextFitness::init(Style *style, float halign, float valign)
            {
                if (pStyle == NULL)
                    return STATUS_BAD_STATE;

                style->begin();
                {
                    style->create_float(vAtoms[P_HFIT], halign);
                    style->create_float(vAtoms[P_VFIT], valign);

                    // Compound objects
                    LSPString s;
                    s.fmt_ascii("%.4f %.4f", halign, valign);
                    style->create_string(vAtoms[P_VALUE], &s);
                }
                style->end();
                return STATUS_OK;
            }
        }

    } /* namespace tk */
} /* namespace lsp */






