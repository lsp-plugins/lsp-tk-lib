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

#ifndef LSP_PLUG_IN_TK_SYS_ATOMS_H_
#define LSP_PLUG_IN_TK_SYS_ATOMS_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/lltl/parray.h>

namespace lsp
{
    namespace tk
    {
        /**
         * Atom collection class
         */
        class Atoms
        {
            private:
                Atoms & operator = (const Atoms &);

            protected:
                lltl::parray<char>      vAtoms;

            public:
                explicit Atoms();
                virtual ~Atoms();

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
                inline const char   *atom_name(atom_t id) const     { return (id >= 0) ? vAtoms.get(id) : NULL ;    }
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* INCLUDE_LSP_PLUG_IN_TK_SYS_ATOMS_H_ */
