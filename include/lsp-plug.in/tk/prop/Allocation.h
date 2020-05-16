/*
 * Allocation.h
 *
 *  Created on: 16 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_ALLOCATION_H_
#define LSP_PLUG_IN_TK_PROP_ALLOCATION_H_

namespace lsp
{
    namespace tk
    {
        /**
         * Widget allocation property
         *   hfill flag forces widget to fill all available horizontal space
         *   vfill flag forces widget to fill all available vertical space
         *   hexpand flag forces widget to take all available horizontal space from container
         *   vexpand flag forces widget to take all available vertical space from container
         */
        class Allocation: public Flags
        {
            private:
                Allocation & operator = (const Allocation *);

            protected:
                static const char * const FLAGS[];

                enum flags_t
                {
                    F_HFILL,
                    F_VFILL,
                    F_HEXPAND,
                    F_VEXPAND,

                    F_TOTAL
                };

            protected:
                atom_t      vAtoms[F_TOTAL];

            protected:
                inline Allocation(prop::Listener *listener = NULL): Flags(FLAGS, vAtoms, listener) {}

            public:
                inline  bool    hfill() const                   { return Flags::get(F_HFILL);           }
                inline  bool    vfill() const                   { return Flags::get(F_VFILL);           }
                inline  bool    hexpand() const                 { return Flags::get(F_HEXPAND);         }
                inline  bool    vexpand() const                 { return Flags::get(F_VEXPAND);         }

                inline bool     set_hfill(bool set = true)      { return Flags::set(F_HFILL, set);      }
                inline bool     set_vfill(bool set = true)      { return Flags::set(F_VFILL, set);      }
                inline bool     set_hexpand(bool set = true)    { return Flags::set(F_HEXPAND, set);    }
                inline bool     set_vexpand(bool set = true)    { return Flags::set(F_VEXPAND, set);    }

                void            set_fill(bool hor, bool vert);
                void            set_expand(bool hor, bool vert);
                void            set(bool hfill, bool vfill, bool hexpand, bool vexpand);

                inline void     set_fill(bool fill = true)      { set_fill(fill, fill);                 }
                inline void     set_expand(bool expand = true)  { set_expand(expand, expand);           }
                inline void     set(bool fill, bool expand)     { set(fill, fill, expand, expand);      }
        };

        namespace prop
        {
            /**
             * Allocation property implementation
             */
            class Allocation: public tk::Allocation
            {
                private:
                    Allocation & operator = (const Allocation *);

                public:
                    explicit Allocation(prop::Listener *listener = NULL): tk::Allocation(listener) {};

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::Allocation::bind(property, style); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::Allocation::bind(property, style); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::Allocation::bind(property, style); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::Allocation::unbind(); };

                    /**
                     * Initialize default values
                     * @return status of operation
                     */
                    status_t            init(Style *style, bool hfill, bool vfill, bool hexpand, bool vexpand);
            };
        }
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_ALLOCATION_H_ */
