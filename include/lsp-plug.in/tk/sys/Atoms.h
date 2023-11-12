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

#ifndef LSP_PLUG_IN_TK_SYS_ATOMS_H_
#define LSP_PLUG_IN_TK_SYS_ATOMS_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/lltl/darray.h>

namespace lsp
{
    namespace tk
    {
        /**
         * Atom collection class
         */
        class Atoms
        {
            protected:
                typedef struct atom_id_t
                {
                    atom_t  id;
                    char    name[];
                } atom_id_t;

                lltl::parray<atom_id_t> vAtoms;
                lltl::parray<atom_id_t> vAtomList;

            protected:
                ssize_t                 index_of(const char *name);
                atom_id_t              *make_atom(const char *name);

            public:
                explicit Atoms();
                Atoms(const Atoms &) = delete;
                Atoms(Atoms &&) = delete;
                virtual ~Atoms();

                Atoms & operator = (const Atoms &) = delete;
                Atoms & operator = (Atoms &&) = delete;

            public:

                /**
                 * Get atom identifier by name
                 * @param name atom name
                 * @return atom identifier or negative error code
                 */
                atom_t              atom_id(const char *name);

                /**
                 * Get atom identifier by name
                 * @param name atom name
                 * @return atom identifier or negative error code
                 */
                inline atom_t       atom_id(const LSPString *name)  { return atom_id(name->get_utf8());             }

                /**
                 * Get atom name by identifier
                 * @param name atom name or NULL
                 * @return atom identifier
                 */
                const char         *atom_name(atom_t id) const;
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_SYS_ATOMS_H_ */
