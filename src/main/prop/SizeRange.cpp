/*
 * SizeRange.cpp
 *
 *  Created on: 23 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const prop::desc_t SizeRange::DESC[] =
        {
            { "",               PT_STRING   },
            { ".min",           PT_INT      },
            { ".max",           PT_INT      },
            { NULL,             PT_UNKNOWN  }
        };

        void SizeRange::Listener::notify(atom_t property)
        {
            pValue->commit(property);
        }

        SizeRange::SizeRange(prop::Listener *listener):
            MultiProperty(listener),
            sListener(this)
        {
            nMin            = -1;
            nMax            = -1;
        }

        SizeRange::~SizeRange()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void SizeRange::commit(atom_t property)
        {
            if ((pStyle == NULL) || (property < 0))
                return;

            ssize_t v;
            if ((property == vAtoms[P_MIN]) && (pStyle->get_int(vAtoms[P_MIN], &v) == STATUS_OK))
                nMin            = lsp_max(v, -1);
            if ((property == vAtoms[P_MAX]) && (pStyle->get_int(vAtoms[P_MAX], &v) == STATUS_OK))
                nMax            = lsp_max(v, -1);

            LSPString s;
            if ((property == vAtoms[P_VALUE]) && (pStyle->get_string(vAtoms[P_VALUE], &s) == STATUS_OK))
            {
                ssize_t xv[2];
                size_t n = Property::parse_ints(xv, 2, &s);
                if (n == 2)
                {
                    nMin            = lsp_max(-1, xv[0]);
                    nMax            = lsp_max(-1, xv[1]);
                }
                else if (n == 2)
                {
                    nMin            = lsp_max(-1, xv[0]);
                    nMax            = nMin;
                }
            }

            if (pListener != NULL)
                pListener->notify(this);
        }

        void SizeRange::sync()
        {
            if (pStyle != NULL)
            {
                pStyle->begin(&sListener);
                {
                    // Simple components
                    if (vAtoms[P_MIN] >= 0)
                        pStyle->set_int(vAtoms[P_MIN], nMin);
                    if (vAtoms[P_MAX] >= 0)
                        pStyle->set_int(vAtoms[P_MAX], nMax);

                    // Compound objects
                    LSPString s;
                    if (vAtoms[P_VALUE] >= 0)
                    {
                        if (s.fmt_ascii("%ld %ld ", long(nMin), long(nMax)))
                            pStyle->set_string(vAtoms[P_VALUE], &s);
                    }
                }
                pStyle->end();
            }
            if (pListener != NULL)
                pListener->notify(this);
        }

        ssize_t SizeRange::set_min(ssize_t value)
        {
            value               = lsp_max(value, -1);
            ssize_t old         = nMin;
            if (old == value)
                return value;

            nMin                = value;
            sync();
            return old;
        }

        ssize_t SizeRange::set_max(ssize_t value)
        {
            value               = lsp_max(value, -1);
            ssize_t old         = nMax;
            if (old == value)
                return value;

            nMax                = value;
            sync();
            return old;
        }

        void SizeRange::set(ssize_t min, ssize_t max)
        {
            min                 = lsp_max(min, -1);
            max                 = lsp_max(max, -1);

            if ((nMin == min) &&
                (nMax == max))
                return;

            nMin                = min;
            nMax                = max;
            sync();
        }

        void SizeRange::set(const SizeRange *src)
        {
            if ((nMin == src->nMin) &&
                (nMax == src->nMax))
                return;

            nMin                = src->nMin;
            nMax                = src->nMax;
            sync();
        }

        void SizeRange::compute(ssize_t *min, ssize_t *max, float scale) const
        {
            scale               = lsp_max(scale, 0.0f);
            *min                = (nMin >= 0) ? nMin * scale : -1;
            *max                = (nMax >= 0) ? nMax * scale : -1;
        }

        void SizeRange::compute(ws::size_limit_t *r, float scale) const
        {
            scale               = lsp_max(scale, 0.0f);

            r->nMinWidth        = (nMin >= 0) ? nMin * scale : -1;
            r->nMaxWidth        = (nMax >= 0) ? nMax * scale : -1;
            if ((r->nMaxWidth >= 0) && (r->nMaxWidth < r->nMinWidth))
                r->nMaxWidth        = r->nMinWidth;

            r->nMinHeight       = r->nMinWidth;
            r->nMinHeight       = r->nMinWidth;
        }

        namespace prop
        {
            status_t SizeRange::init(Style *style, ssize_t min, ssize_t max)
            {
                if (pStyle == NULL)
                    return STATUS_BAD_STATE;

                style->begin();
                {
                    style->create_int(vAtoms[P_MIN], min);
                    style->create_int(vAtoms[P_MAX], max);

                    // Compound objects
                    LSPString s;
                    s.fmt_ascii("%ld %ld ", long(min), long(max));
                    style->create_string(vAtoms[P_VALUE], &s);
                }
                style->end();
                return STATUS_OK;
            }
        }
    } /* namespace tk */
} /* namespace lsp */


