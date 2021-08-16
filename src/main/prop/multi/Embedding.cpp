/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 26 июн. 2020 г.
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
        const prop::desc_t Embedding::DESC[] =
        {
            { "",           PT_STRING   },
            { ".left",      PT_BOOL     },
            { ".right",     PT_BOOL     },
            { ".top",       PT_BOOL     },
            { ".bottom",    PT_BOOL     },
            NULL
        };

        Embedding::Embedding(prop::Listener *listener):
            MultiProperty(vAtoms, P_COUNT, listener)
        {
            nFlags      = 0;
        }

        Embedding::~Embedding()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void Embedding::commit(atom_t property)
        {
            bool v;
            if ((property == vAtoms[P_LEFT]) && (pStyle->get_bool(vAtoms[P_LEFT], &v) == STATUS_OK))
                nFlags      = lsp_setflag(nFlags, M_LEFT, v);
            if ((property == vAtoms[P_RIGHT]) && (pStyle->get_bool(vAtoms[P_RIGHT], &v) == STATUS_OK))
                nFlags      = lsp_setflag(nFlags, M_RIGHT, v);
            if ((property == vAtoms[P_TOP]) && (pStyle->get_bool(vAtoms[P_TOP], &v) == STATUS_OK))
                nFlags      = lsp_setflag(nFlags, M_TOP, v);
            if ((property == vAtoms[P_BOTTOM]) && (pStyle->get_bool(vAtoms[P_BOTTOM], &v) == STATUS_OK))
                nFlags      = lsp_setflag(nFlags, M_BOTTOM, v);

            LSPString s;
            if ((property == vAtoms[P_VALUE]) && (pStyle->get_string(vAtoms[P_VALUE], &s) == STATUS_OK))
            {
                bool xv[4];
                size_t n = Property::parse_bools(xv, 4, &s);
                switch (n)
                {
                    case 1:
                        nFlags      = lsp_setflag(nFlags, M_ALL, xv[0]);
                        break;
                    case 2:
                        nFlags      = lsp_setflag(nFlags, M_HOR,  xv[0]);
                        nFlags      = lsp_setflag(nFlags, M_VERT, xv[1]);
                        break;
                    case 3:
                        nFlags      = lsp_setflag(nFlags, M_LEFT,  xv[0]);
                        nFlags      = lsp_setflag(nFlags, M_RIGHT, xv[1]);
                        nFlags      = lsp_setflag(nFlags, M_VERT,  xv[2]);
                        break;
                    case 4:
                        nFlags      = lsp_setflag(nFlags, M_LEFT,  xv[0]);
                        nFlags      = lsp_setflag(nFlags, M_RIGHT, xv[1]);
                        nFlags      = lsp_setflag(nFlags, M_TOP,   xv[2]);
                        nFlags      = lsp_setflag(nFlags, M_BOTTOM,xv[3]);
                        break;
                    default:
                        break;
                }
            }
        }

        void Embedding::push()
        {
            // Simple components
            if (vAtoms[P_LEFT] >= 0)
                pStyle->set_bool(vAtoms[P_LEFT], left());
            if (vAtoms[P_RIGHT] >= 0)
                pStyle->set_bool(vAtoms[P_RIGHT], right());
            if (vAtoms[P_TOP] >= 0)
                pStyle->set_bool(vAtoms[P_TOP], top());
            if (vAtoms[P_BOTTOM] >= 0)
                pStyle->set_bool(vAtoms[P_BOTTOM], bottom());

            // Compound objects
            LSPString s;
            if (vAtoms[P_VALUE] >= 0)
            {
                if (s.fmt_ascii("%s %s %s %s",
                        (left()) ? "true" : "false",
                        (right()) ? "true" : "false",
                        (top()) ? "true" : "false",
                        (bottom()) ? "true" : "false"
                    ))
                    pStyle->set_string(vAtoms[P_VALUE], &s);
            }
        }

        void Embedding::set(bool left, bool right, bool top, bool bottom)
        {
            size_t flags    = nFlags;
            flags = lsp_setflag(flags, M_LEFT, left);
            flags = lsp_setflag(flags, M_RIGHT, right);
            flags = lsp_setflag(flags, M_TOP, top);
            flags = lsp_setflag(flags, M_BOTTOM, bottom);

            if (flags == nFlags)
                return;

            nFlags  = flags;
            sync();
        }

        void Embedding::set(bool hor, bool vert)
        {
            size_t flags    = nFlags;
            flags = lsp_setflag(flags, M_HOR, hor);
            flags = lsp_setflag(flags, M_VERT, vert);
            if (flags == nFlags)
                return;

            nFlags  = flags;
            sync();
        }

        void Embedding::set_horizontal(bool hor)
        {
            size_t flags    = nFlags;
            flags = lsp_setflag(flags, M_HOR, hor);
            if (flags == nFlags)
                return;

            nFlags  = flags;
            sync();
        }

        void Embedding::set_vertical(bool vert)
        {
            size_t flags    = nFlags;
            flags = lsp_setflag(flags, M_VERT, vert);
            if (flags == nFlags)
                return;

            nFlags  = flags;
            sync();
        }

        void Embedding::set(bool on)
        {
            size_t flags    = nFlags;
            flags = lsp_setflag(flags, M_ALL, on);
            if (flags == nFlags)
                return;

            nFlags  = flags;
            sync();
        }

        bool Embedding::set_flag(size_t flag, bool set)
        {
            bool old        = nFlags & flag;
            size_t flags    = lsp_setflag(nFlags, flag, set);
            if (flags == nFlags)
                return old;

            nFlags  = flags;
            sync();

            return old;
        }

    }
}


