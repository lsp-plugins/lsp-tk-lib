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
        const prop::desc_t Position::DESC[] =
        {
            { "",           PT_STRING   },
            { ".left",      PT_INT      },
            { ".top",       PT_INT      },
            { NULL,         PT_UNKNOWN  }
        };

        Position::Position(prop::Listener *listener):
            MultiProperty(vAtoms, P_COUNT, listener)
        {
            nLeft           = 0;
            nTop            = 0;
        }

        Position::~Position()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void Position::commit(atom_t property)
        {
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
        }

        void Position::push()
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

        bool Position::rinside(const ws::rectangle_t *r, ssize_t x, ssize_t y, ssize_t radius)
        {
            return rminside(r, x, y, SURFMASK_ALL_CORNER, radius);
        }

        bool Position::rminside(const ws::rectangle_t *r, ssize_t x, ssize_t y, size_t mask, ssize_t radius)
        {
            if (!inside(r, x, y))
                return false;

            x -= r->nLeft;
            y -= r->nTop;

            // Check special case: corners
            if (x < radius)
            {
                if (y < radius)
                {
                    if (!(mask & SURFMASK_LT_CORNER))
                        return true;
                    float dx = radius - x, dy = radius - y;
                    return (dx*dx + dy*dy) <= radius * radius;
                }
                else if (y > (r->nHeight - radius))
                {
                    if (!(mask & SURFMASK_LB_CORNER))
                        return true;
                    float dx = radius - x, dy = y - r->nHeight + radius;
                    return (dx*dx + dy*dy) <= radius * radius;
                }
            }
            else if (x > (r->nWidth - radius))
            {
                if (y < radius)
                {
                    if (!(mask & SURFMASK_RT_CORNER))
                        return true;
                    float dx = x - r->nWidth + radius, dy = radius - y;
                    return (dx*dx + dy*dy) <= radius * radius;
                }
                else if (y > (r->nHeight - radius))
                {
                    if (!(mask & SURFMASK_RB_CORNER))
                        return true;
                    float dx = x - r->nWidth + radius, dy = y - r->nHeight + radius;
                    return (dx*dx + dy*dy) <= radius * radius;
                }
            }

            return true;
        }

        namespace prop
        {
            void Position::commit_value(ssize_t left, ssize_t top)
            {
                nLeft   = left;
                nTop    = top;
                sync(false);
            }
        }
    } /* namespace tk */
} /* namespace lsp */


