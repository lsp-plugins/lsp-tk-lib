/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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
            NULL
        };
    
        void Allocation::set_fill(bool hor, bool vert)
        {
            size_t flags = nFlags;
            flags = lsp_setflag(flags, 1 << F_HFILL, hor);
            flags = lsp_setflag(flags, 1 << F_VFILL, vert);
            if (flags == nFlags)
                return;

            sync(flags);
        }

        void Allocation::set_expand(bool hor, bool vert)
        {
            size_t flags = nFlags;
            flags = lsp_setflag(flags, 1 << F_HEXPAND, hor);
            flags = lsp_setflag(flags, 1 << F_VEXPAND, vert);
            if (flags == nFlags)
                return;

            sync(flags);
        }

        void Allocation::set(bool hfill, bool vfill, bool hexpand, bool vexpand)
        {
            size_t flags = nFlags;
            flags   = lsp_setflag(flags, 1 << F_HFILL, hfill);
            flags   = lsp_setflag(flags, 1 << F_VFILL, vfill);
            flags   = lsp_setflag(flags, 1 << F_HEXPAND, hexpand);
            flags   = lsp_setflag(flags, 1 << F_VEXPAND, vexpand);
            if (flags == nFlags)
                return;

            sync(flags);
        }

        namespace prop
        {
            status_t Allocation::init(Style *style, bool hfill, bool vfill, bool hexpand, bool vexpand)
            {
                if (pStyle == NULL)
                    return STATUS_BAD_STATE;

                style->begin();
                {
                    style->create_bool(vAtoms[F_HFILL], hfill);
                    style->create_bool(vAtoms[F_VFILL], vfill);
                    style->create_bool(vAtoms[F_HEXPAND], hexpand);
                    style->create_bool(vAtoms[F_VEXPAND], vexpand);
                }
                style->end();
                return STATUS_OK;
            }

            status_t Allocation::override(Style *style, bool hfill, bool vfill, bool hexpand, bool vexpand)
            {
                if (pStyle == NULL)
                    return STATUS_BAD_STATE;

                style->begin();
                {
                    style->override_bool(vAtoms[F_HFILL], hfill);
                    style->override_bool(vAtoms[F_VFILL], vfill);
                    style->override_bool(vAtoms[F_HEXPAND], hexpand);
                    style->override_bool(vAtoms[F_VEXPAND], vexpand);
                }
                style->end();
                return STATUS_OK;
            }


            status_t Allocation::init(const char *name, Style *style, bool hfill, bool vfill, bool hexpand, bool vexpand)
            {
                prop::Allocation v;
                LSP_STATUS_ASSERT(v.bind(name, style));
                v.set(hfill, vfill, hexpand, vexpand);
                return v.init();
            }

            status_t Allocation::override(const char *name, Style *style, bool hfill, bool vfill, bool hexpand, bool vexpand)
            {
                prop::Allocation v;
                LSP_STATUS_ASSERT(v.bind(name, style));
                v.set(hfill, vfill, hexpand, vexpand);
                return v.override();
            }
        }

    } /* namespace tk */
} /* namespace lsp */
