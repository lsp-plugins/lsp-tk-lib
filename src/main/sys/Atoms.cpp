/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
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
            for (size_t i=0, n=vAtomList.size(); i<n; ++i)
            {
                atom_id_t *ptr = vAtomList.uget(i);
                if (ptr != NULL)
                    ::free(ptr);
            }
            vAtoms.flush();
            vAtomList.flush();
        }

        ssize_t Atoms::index_of(const char *name)
        {
            const atom_id_t *mp;
            ssize_t first = 0, last = vAtoms.size() - 1, mid;
            if (last < 0)
                return -1;

            while (first < last)
            {
                mid             = (first + last) >> 1;
                mp              = vAtoms.uget(mid);
                int cmp         = strcmp(name, mp->name);

                if (cmp < 0)
                    last            = mid - 1;
                else if (cmp > 0)
                    first           = mid + 1;
                else
                    return mid;
            }

            return first;
        }

        Atoms::atom_id_t *Atoms::make_atom(const char *name)
        {
            size_t len      = strlen(name) + 1;
            atom_id_t *atom = static_cast<atom_id_t *>(malloc(sizeof(atom_id_t) + len));
            if (atom == NULL)
                return NULL;

            atom->id        = vAtomList.size();
            memcpy(atom->name, name, len);

            return atom;
        }

        const char *Atoms::atom_name(atom_t id) const
        {
            if (id < 0)
                return NULL;

            const atom_id_t *atom = vAtomList.get(id);
            return (atom != NULL) ? atom->name : NULL;
        }

        atom_t Atoms::atom_id(const char *name)
        {
            if (name == NULL)
                return -STATUS_BAD_ARGUMENTS;

            // Find existing atom or place to paste the atom to
            ssize_t index   = index_of(name);
            if (index >= 0)
            {
                atom_id_t *atom = vAtoms.uget(index);
                int cmp         = strcmp(name, atom->name);
                if (cmp == 0)
                    return atom->id;
                else if (cmp > 0)
                    ++index;
            }
            else
                index       = 0;

            // Create atom object
            Atoms::atom_id_t *atom  = make_atom(name);
            if (atom == NULL)
                return -STATUS_NO_MEM;

            // Insert atom name at the found position
            if (!vAtoms.insert(index, atom))
            {
                ::free(atom);
                return -STATUS_NO_MEM;
            }

            // Add atom to the list of atoms
            if (!vAtomList.append(atom))
            {
                vAtoms.remove(index);
                ::free(atom);
                return -STATUS_NO_MEM;
            }

            return atom->id;
        }
    
    } /* namespace tk */
} /* namespace lsp */
