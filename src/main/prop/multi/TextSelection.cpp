/*
 * TextSelection.cpp
 *
 *  Created on: 9 июн. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const prop::desc_t TextSelection::DESC[] =
        {
            { "",           PT_STRING   },
            { ".first",     PT_INT      },
            { ".last",      PT_INT      },
            { NULL,         PT_UNKNOWN  }
        };

        void TextSelection::Listener::notify(atom_t property)
        {
            pValue->commit(property);
        }

        TextSelection::TextSelection(prop::Listener *listener):
            MultiProperty(vAtoms, P_COUNT, listener),
            sListener(this)
        {
            nFirst      = -1;
            nLast       = -1;
            nLimit      = 0;
        }

        TextSelection::~TextSelection()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void TextSelection::sync()
        {
            if (pStyle != NULL)
            {
                pStyle->begin(&sListener);
                {
                    // Simple components
                    if (vAtoms[P_FIRST] >= 0)
                        pStyle->set_int(vAtoms[P_FIRST], nFirst);
                    if (vAtoms[P_LAST] >= 0)
                        pStyle->set_int(vAtoms[P_LAST], nLast);

                    // Compound objects
                    LSPString s;
                    if (vAtoms[P_VALUE] >= 0)
                    {
                        if (s.fmt_ascii("%ld %ld", long(nFirst), long(nLast)))
                            pStyle->set_string(vAtoms[P_VALUE], &s);
                    }
                }
                pStyle->end();
            }

            if (pListener != NULL)
                pListener->notify(this);
        }

        void TextSelection::parse(const LSPString *s)
        {
            ssize_t v[2];
            size_t n = MultiProperty::parse_ints(v, 2, s);
            switch (n)
            {
                case 1:
                    nFirst      = lsp_limit(v[0], -1, nLimit);
                    nLast       = nFirst;
                    break;

                case 2:
                    nFirst      = lsp_limit(v[0], -1, nLimit);
                    nLast       = lsp_limit(v[1], -1, nLimit);
                    break;

                default:
                    break;
            }
        }

        void TextSelection::commit(atom_t property)
        {
            if ((pStyle == NULL) || (property < 0))
                return;

            ssize_t v;
            if ((property == vAtoms[P_FIRST]) && (pStyle->get_int(vAtoms[P_FIRST], &v) == STATUS_OK))
                nFirst      = lsp_limit(v, -1, nLimit);
            if ((property == vAtoms[P_LAST]) && (pStyle->get_int(vAtoms[P_LAST], &v) == STATUS_OK))
                nLast       = lsp_limit(v, -1, nLimit);

            LSPString s;
            if ((property == vAtoms[P_VALUE]) && (pStyle->get_string(vAtoms[P_VALUE], &s) == STATUS_OK))
                parse(&s);

            if (pListener != NULL)
                pListener->notify(this);
        }

        void TextSelection::set(ssize_t first, ssize_t last)
        {
            first   = lsp_limit(first, -1, nLimit);
            last    = lsp_limit(last,  -1, nLimit);

            if ((first == nFirst) && (last == nLast))
                return;

            nFirst  = first;
            nLast   = last;

            sync();
        }

        void TextSelection::set(ssize_t first)
        {
            first   = lsp_limit(first, -1, nLimit);

            if ((first == nFirst) && (first == nLast))
                return;

            nFirst  = first;
            nLast   = first;

            sync();
        }

        ssize_t TextSelection::set_first(ssize_t value)
        {
            ssize_t old     = nFirst;
            value           = lsp_limit(value, -1, nLimit);
            if (old == value)
                return old;

            nFirst          = value;
            sync();

            return old;
        }

        ssize_t TextSelection::set_last(ssize_t value)
        {
            ssize_t old     = nLast;
            value           = lsp_limit(value, -1, nLimit);
            if (old == value)
                return old;

            nLast           = value;
            sync();

            return old;
        }

        void TextSelection::set_all()
        {
            ssize_t first   = lsp_min(0, nLimit);
            ssize_t last    = nLimit;

            if ((nFirst == first) && (nLast == last))
                return;

            nFirst      = first;
            nLast       = last;
            sync();
        }

        void TextSelection::truncate()
        {
            ssize_t first   = lsp_limit(nFirst, -1, nLimit);
            if ((first == nFirst) && (first == nLast))
                return;

            nFirst      = first;
            nLast       = first;
            sync();
        }

        void TextSelection::unset()
        {
            if ((nLast == -1) && (nFirst == -1))
                return;

            nLast   = -1;
            nFirst  = -1;
            sync();
        }

        namespace prop
        {
            void TextSelection::set_limit(ssize_t limit)
            {
                nLimit      = limit;
                set(nFirst, nLast);
            }

            status_t TextSelection::init(Style *style, float first, float last)
            {
                if (pStyle == NULL)
                    return STATUS_BAD_STATE;

                style->begin();
                {
                    style->create_int(vAtoms[P_FIRST], first);
                    style->create_int(vAtoms[P_LAST], last);

                    // Compound objects
                    LSPString s;
                    s.fmt_ascii("%ld %ld", long(first), long(last));
                    style->create_string(vAtoms[P_VALUE], &s);
                }
                style->end();
                return STATUS_OK;
            }
        }

    } /* namespace tk */
} /* namespace lsp */



