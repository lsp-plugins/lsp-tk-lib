/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 13 мая 2020 г.
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
        Atoms::Atoms()
        {
        }
        
        Atoms::~Atoms()
        {
            // Destroy atoms
            for (size_t i=0, n=vAtoms.size(); i<n; ++i)
            {
                char *ptr = vAtoms.uget(i);
                if (ptr != NULL)
                    ::free(ptr);
            }
            vAtoms.flush();
        }

        atom_t Atoms::atom_id(const char *name)
        {
            if (name == NULL)
                return -STATUS_BAD_ARGUMENTS;

            // Find existing atom
            size_t last = vAtoms.size();
            for (size_t i=0; i<last; ++i)
            {
                const char *aname = vAtoms.uget(i);
                if (!::strcmp(aname, name))
                    return i;
            }

            // Allocate new atom name
            char *aname         = ::strdup(name);
            if (aname == NULL)
                return -STATUS_NO_MEM;

            // Insert atom name to the found position
            if (!vAtoms.add(aname))
            {
                ::free(aname);
                return -STATUS_NO_MEM;
            }

            return last;
        }
    
    } /* namespace tk */
} /* namespace lsp */
