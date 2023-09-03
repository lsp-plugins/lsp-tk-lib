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

#ifndef LSP_PLUG_IN_TK_PROP_BASE_FLAGS_H_
#define LSP_PLUG_IN_TK_PROP_BASE_FLAGS_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        
        class Flags: public Property
        {
            protected:
                size_t                  nFlags;         // Bit field of flags
                const char * const     *pFlags;         // Flag description
                atom_t                 *vAtoms;         // List of associated atoms

            protected:
                virtual void    push();
                virtual void    commit(atom_t property);

                status_t        unbind();
                status_t        bind(atom_t id, Style *style);
                status_t        bind(const char *id, Style *style);
                status_t        bind(const LSPString *id, Style *style);
                atom_t          lookup(size_t ordinal);
                void            psync(atom_t atom, bool value);
                void            psync(size_t mask);
                void            set_default();
                inline bool     get(size_t ordinal) const       { return nFlags & (1 << ordinal);  }
                bool            set(size_t ordinal, bool on = true);
                bool            unset(size_t ordinal);
                bool            toggle(size_t ordinal);

            protected:
                explicit Flags(const char * const *flags, atom_t *atoms, prop::Listener *listener = NULL);
                Flags(const Flags &) = delete;
                Flags(Flags &&) = delete;
                virtual ~Flags() override;

                Flags & operator = (const Flags &) = delete;
                Flags & operator = (Flags &&) = delete;
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_BASE_FLAGS_H_ */
