/*
 * Allocation.cpp
 *
 *  Created on: 16 мая 2020 г.
 *      Author: sadko
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
            flags = (hor)  ? flags | (1 << F_HFILL) : flags & (~(1 << F_HFILL));
            flags = (vert) ? flags | (1 << F_VFILL) : flags & (~(1 << F_VFILL));
            if (flags == nFlags)
                return;

            sync(flags);
        }

        void Allocation::set_expand(bool hor, bool vert)
        {
            size_t flags = nFlags;
            flags = (hor)  ? flags | (1 << F_HEXPAND) : flags & (~(1 << F_HEXPAND));
            flags = (vert) ? flags | (1 << F_VEXPAND) : flags & (~(1 << F_VEXPAND));
            if (flags == nFlags)
                return;

            sync(flags);
        }

        void Allocation::set(bool hfill, bool vfill, bool hexpand, bool vexpand)
        {
            size_t flags = nFlags;
            flags = (hfill)     ? flags | (1 << F_HFILL)   : flags & (~(1 << F_HFILL));
            flags = (vfill)     ? flags | (1 << F_VFILL)   : flags & (~(1 << F_VFILL));
            flags = (hexpand)   ? flags | (1 << F_HEXPAND) : flags & (~(1 << F_HEXPAND));
            flags = (vexpand)   ? flags | (1 << F_VEXPAND) : flags & (~(1 << F_VEXPAND));
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
        }

    } /* namespace tk */
} /* namespace lsp */
