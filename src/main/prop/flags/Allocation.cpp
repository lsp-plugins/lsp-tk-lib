/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 16 мая 2020 г.
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
        const char * const Allocation::FLAGS[] =
        {
            ".hfill",
            ".vfill",
            ".hexpand",
            ".vexpand",
            ".hreduce",
            ".vreduce",
            NULL
        };

        Allocation::Allocation(prop::Listener *listener):
            Flags(FLAGS, vAtoms, listener)
        {
            for (size_t i=0; i<F_TOTAL; ++i)
                vAtoms[i]   = -1;
        }
    
        void Allocation::set_fill(bool hor, bool vert)
        {
            size_t flags = nFlags;
            flags = lsp_setflag(flags, 1 << F_HFILL, hor);
            flags = lsp_setflag(flags, 1 << F_VFILL, vert);
            if (flags == nFlags)
                return;

            psync(flags);
        }

        void Allocation::set_expand(bool hor, bool vert)
        {
            size_t flags = nFlags;
            flags = lsp_setflag(flags, 1 << F_HEXPAND, hor);
            flags = lsp_setflag(flags, 1 << F_VEXPAND, vert);
            if (flags == nFlags)
                return;

            psync(flags);
        }

        void Allocation::set_reduce(bool hor, bool vert)
        {
            size_t flags = nFlags;
            flags = lsp_setflag(flags, 1 << F_HREDUCE, hor);
            flags = lsp_setflag(flags, 1 << F_VREDUCE, vert);
            if (flags == nFlags)
                return;

            psync(flags);
        }

        void Allocation::set(bool hfill, bool vfill, bool hexpand, bool vexpand)
        {
            set(hfill, vfill, hexpand, vexpand, false, false);
        }

        void Allocation::set(bool hfill, bool vfill, bool hexpand, bool vexpand, bool hreduce, bool vreduce)
        {
            size_t flags = nFlags;
            flags   = lsp_setflag(flags, 1 << F_HFILL, hfill);
            flags   = lsp_setflag(flags, 1 << F_VFILL, vfill);
            flags   = lsp_setflag(flags, 1 << F_HEXPAND, hexpand);
            flags   = lsp_setflag(flags, 1 << F_VEXPAND, vexpand);
            flags   = lsp_setflag(flags, 1 << F_HREDUCE, hreduce);
            flags   = lsp_setflag(flags, 1 << F_VREDUCE, vreduce);
            if (flags == nFlags)
                return;

            psync(flags);
        }

    } /* namespace tk */
} /* namespace lsp */
