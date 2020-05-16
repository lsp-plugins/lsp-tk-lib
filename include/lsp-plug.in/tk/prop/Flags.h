/*
 * Flags.h
 *
 *  Created on: 16 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_FLAGS_H_
#define LSP_PLUG_IN_TK_PROP_FLAGS_H_

namespace lsp
{
    namespace tk
    {
        
        class Flags: public Property
        {
            private:
                Flags & operator = (const Flags *);

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

                void            set_default();
                void            commit(atom_t property);

                inline bool     get(size_t ordinal) const       { return nFlags & (1 << ordinal);  }
                bool            set(size_t ordinal, bool on = true);
                bool            unset(size_t ordinal);
                bool            toggle(size_t ordinal);

            protected:
                explicit Flags(const char * const *flags, atom_t *atoms, prop::Listener *listener = NULL);
                ~Flags();
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_FLAGS_H_ */
