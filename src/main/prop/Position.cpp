/*
 * Position.cpp
 *
 *  Created on: 10 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const prop::desc_t Position::DESC[] =
        {
            { "",           PT_STRING   },
            { ".left",      PT_INT      },
            { ".top",       PT_INT      },
            { NULL,         PT_UNKNOWN  }
        };

        void Position::Listener::notify(atom_t property)
        {
            pValue->commit(property);
        }

        Position::Position(prop::Listener *listener):
            MultiProperty(listener),
            sListener(this)
        {
            nLeft              = 0;
            nTop             = 0;
        }

        Position::~Position()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void Position::commit(atom_t property)
        {
            if ((pStyle == NULL) || (property < 0))
                return;

            ssize_t v;
            if ((property == vAtoms[P_LEFT]) && (pStyle->get_int(vAtoms[P_LEFT], &v) == STATUS_OK))
                nLeft       = v;
            if ((property == vAtoms[P_TOP]) && (pStyle->get_int(vAtoms[P_TOP], &v) == STATUS_OK))
                nTop        = v;

            LSPString s;
            if ((property == vAtoms[P_VALUE]) && (pStyle->get_string(vAtoms[P_VALUE], &s) == STATUS_OK))
            {
                ssize_t xv[2];
                size_t n = Property::parse_ints(xv, 2, &s);
                if (n == 2)
                {
                    nLeft       = xv[0];
                    nTop        = xv[1];
                }
            }

            if (pListener != NULL)
                pListener->notify(this);
        }

        void Position::sync(bool notify)
        {
            if (pStyle != NULL)
            {
                pStyle->begin(&sListener);
                {
                    // Simple components
                    if (vAtoms[P_LEFT] >= 0)
                        pStyle->set_int(vAtoms[P_LEFT], nLeft);
                    if (vAtoms[P_TOP] >= 0)
                        pStyle->set_int(vAtoms[P_TOP], nTop);

                    // Compound objects
                    LSPString s;
                    if (vAtoms[P_VALUE] >= 0)
                    {
                        if (s.fmt_ascii("%ld %ld", long(nLeft), long(nTop)))
                            pStyle->set_string(vAtoms[P_VALUE], &s);
                    }
                }
                pStyle->end();
            }

            if ((pListener != NULL) && (notify))
                pListener->notify(this);
        }

        ssize_t Position::set_left(ssize_t value)
        {
            ssize_t old     = nLeft;
            if (old == value)
                return value;

            nLeft          = value;
            sync(true);
            return old;
        }

        ssize_t Position::set_top(ssize_t value)
        {
            ssize_t old     = nTop;
            if (old == value)
                return value;

            nTop            = value;
            sync(true);
            return old;
        }

        void Position::set(ssize_t left, ssize_t top)
        {
            if ((nLeft == left) &&
                (nTop == top))
                return;

            nLeft       = left;
            nTop        = top;
            sync(true);
        }

        void Position::set(const Position *p)
        {
            if ((nLeft == p->nLeft) &&
                (nTop == p->nTop))
                return;

            nLeft       = p->nLeft;
            nTop        = p->nTop;
            sync(true);
        }

        bool Position::inside(const ws::rectangle_t *rect, ssize_t left, ssize_t top)
        {
            if (left < rect->nLeft)
                return false;
            if (top < rect->nTop)
                return false;
            if (left >= (rect->nLeft + rect->nWidth))
                return false;
            if (top >= (rect->nTop + rect->nHeight))
                return false;

            return true;
        }

        namespace prop
        {
            status_t Position::init(Style *style, ssize_t left, ssize_t top)
            {
                if (pStyle == NULL)
                    return STATUS_BAD_STATE;

                style->begin();
                {
                    style->create_int(vAtoms[P_LEFT], left);
                    style->create_int(vAtoms[P_TOP], top);
                }
                style->end();
                return STATUS_OK;
            }

            void Position::commit(ssize_t left, ssize_t top)
            {
                nLeft   = left;
                nTop    = top;
                sync(false);
            }
        }
    } /* namespace tk */
} /* namespace lsp */


