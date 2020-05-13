/*
 * IAtoms.cpp
 *
 *  Created on: 13 мая 2020 г.
 *      Author: sadko
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
