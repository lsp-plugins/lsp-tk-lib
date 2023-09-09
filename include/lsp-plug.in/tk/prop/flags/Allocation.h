/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef LSP_PLUG_IN_TK_PROP_FLAGS_ALLOCATION_H_
#define LSP_PLUG_IN_TK_PROP_FLAGS_ALLOCATION_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

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
            protected:
                static const char * const FLAGS[];

                enum flags_t
                {
                    F_HFILL,
                    F_VFILL,
                    F_HEXPAND,
                    F_VEXPAND,
                    F_HREDUCE,
                    F_VREDUCE,

                    F_TOTAL
                };

            protected:
                atom_t      vAtoms[F_TOTAL];

            protected:
                Allocation(prop::Listener *listener = NULL);
                Allocation(const Allocation &) = delete;
                Allocation(Allocation &&) = delete;

                Allocation & operator = (const Allocation &) = delete;
                Allocation & operator = (Allocation &&) = delete;

            public:
                inline  bool    hfill() const                   { return Flags::get(F_HFILL);           }
                inline  bool    vfill() const                   { return Flags::get(F_VFILL);           }
                inline  bool    hexpand() const                 { return Flags::get(F_HEXPAND);         }
                inline  bool    vexpand() const                 { return Flags::get(F_VEXPAND);         }
                inline  bool    hreduce() const                 { return Flags::get(F_HREDUCE);         }
                inline  bool    vreduce() const                 { return Flags::get(F_VREDUCE);         }

                inline bool     set_hfill(bool set = true)      { return Flags::set(F_HFILL, set);      }
                inline bool     set_vfill(bool set = true)      { return Flags::set(F_VFILL, set);      }
                inline bool     set_hexpand(bool set = true)    { return Flags::set(F_HEXPAND, set);    }
                inline bool     set_vexpand(bool set = true)    { return Flags::set(F_VEXPAND, set);    }
                inline bool     set_hreduce(bool set = true)    { return Flags::set(F_HREDUCE, set);    }
                inline bool     set_vreduce(bool set = true)    { return Flags::set(F_VREDUCE, set);    }

                void            set_fill(bool hor, bool vert);
                void            set_expand(bool hor, bool vert);
                void            set_reduce(bool hor, bool vert);
                void            set(bool hfill, bool vfill, bool hexpand, bool vexpand);
                void            set(bool hfill, bool vfill, bool hexpand, bool vexpand, bool hreduce, bool vreduce);

                inline void     set_fill(bool fill = true)      { set_fill(fill, fill);                 }
                inline void     set_expand(bool expand = true)  { set_expand(expand, expand);           }
                inline void     set_reduce(bool reduce= true)   { set_reduce(reduce, reduce);           }
                inline void     set(bool fill, bool expand)     { set(fill, fill, expand, expand);      }
                inline void     set(bool fill, bool expand, bool reduce)    { set(fill, fill, expand, expand, reduce, reduce);      }
        };

        namespace prop
        {
            /**
             * Allocation property implementation
             */
            class Allocation: public tk::Allocation
            {
                private:
                    Allocation & operator = (const Allocation &);
                    Allocation(const Allocation &);

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
            };
        } /* namespace prop */
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_FLAGS_ALLOCATION_H_ */
