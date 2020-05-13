/*
 * IAtoms.h
 *
 *  Created on: 13 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_SYS_IATOMS_H_
#define LSP_PLUG_IN_TK_SYS_IATOMS_H_

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

#endif /* INCLUDE_LSP_PLUG_IN_TK_SYS_IATOMS_H_ */
