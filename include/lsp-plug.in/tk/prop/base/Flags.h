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
            private:
                Flags & operator = (const Flags &);

            protected:
                class Listener: public IStyleListener
                {
                    protected:
                        Flags       *pValue;

                    public:
                        inline Listener(Flags *ptr)         { pValue = ptr;     }

                    public:
                        virtual void notify(atom_t property);
                };

            protected:
                size_t                  nFlags;         // Bit field of flags
                const char * const     *pFlags;         // Flag description
                atom_t                 *vAtoms;         // List of associated atoms
                Listener                sListener;      // Change listener

            protected:
                status_t        unbind();
                status_t        bind(atom_t id, Style *style);
                status_t        bind(const char *id, Style *style);
                status_t        bind(const LSPString *id, Style *style);
                atom_t          lookup(size_t ordinal);
                void            sync(atom_t atom, bool value);
                void            sync(size_t mask);
                void            sync();

                void            set_default();
                void            commit(atom_t property);

                inline bool     get(size_t ordinal) const       { return nFlags & (1 << ordinal);  }
                bool            set(size_t ordinal, bool on = true);
                bool            unset(size_t ordinal);
                bool            toggle(size_t ordinal);

                status_t        init();
                status_t        override();

            protected:
                explicit Flags(const char * const *flags, atom_t *atoms, prop::Listener *listener = NULL);
                ~Flags();
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_BASE_FLAGS_H_ */
