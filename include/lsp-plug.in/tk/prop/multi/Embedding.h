/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 26 июн. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_FLAGS_EMBEDDING_H_
#define LSP_PLUG_IN_TK_PROP_FLAGS_EMBEDDING_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Widget embedding property
         */
        class Embedding: public MultiProperty
        {
            protected:
                static const prop::desc_t   DESC[];

                enum property_t
                {
                    P_VALUE,
                    P_LEFT,
                    P_RIGHT,
                    P_TOP,
                    P_BOTTOM,

                    P_COUNT
                };

                enum flags_t
                {
                    F_LEFT,
                    F_RIGHT,
                    F_TOP,
                    F_BOTTOM,

                    M_LEFT      = 1 << F_LEFT,
                    M_RIGHT     = 1 << F_RIGHT,
                    M_TOP       = 1 << F_TOP,
                    M_BOTTOM    = 1 << F_BOTTOM,

                    M_HOR       = M_LEFT | M_RIGHT,
                    M_VERT      = M_TOP | M_BOTTOM,
                    M_ALL       = M_LEFT | M_RIGHT | M_TOP | M_BOTTOM
                };

            protected:
                atom_t          vAtoms[P_COUNT];    // Atoms
                size_t          nFlags;             // Flags

            protected:
                bool            set_flag(size_t flag, bool set);

                virtual void    push() override;
                virtual void    commit(atom_t property) override;

            protected:
                Embedding(prop::Listener *listener = NULL);
                Embedding(const Embedding &) = delete;
                Embedding(Embedding &&) = delete;
                virtual ~Embedding() override;

                Embedding & operator = (const Embedding &) = delete;
                Embedding & operator = (Embedding &&) = delete;

            public:
                inline bool     left() const                    { return nFlags & M_LEFT;               }
                inline bool     right() const                   { return nFlags & M_RIGHT;              }
                inline bool     top() const                     { return nFlags & M_TOP;                }
                inline bool     bottom() const                  { return nFlags & M_BOTTOM;             }

                inline bool     set_left(bool set = true)       { return set_flag(F_LEFT, set);         }
                inline bool     set_right(bool set = true)      { return set_flag(F_RIGHT, set);        }
                inline bool     set_top(bool set = true)        { return set_flag(F_TOP, set);          }
                inline bool     set_bottom(bool set = true)     { return set_flag(F_BOTTOM, set);       }

                void            set(bool left, bool right, bool top, bool bottom);
                void            set(bool hor, bool vert);
                void            set(bool on);
                void            set_horizontal(bool hor);
                void            set_vertical(bool vert);
        };

        namespace prop
        {
            /**
             * Embedding property implementation
             */
            class Embedding: public tk::Embedding
            {
                public:
                    explicit Embedding(prop::Listener *listener = NULL): tk::Embedding(listener) {}
                    Embedding(const Embedding &) = delete;
                    Embedding(Embedding &&) = delete;

                    Embedding & operator = (const Embedding &) = delete;
                    Embedding & operator = (Embedding &&) = delete;

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::Embedding::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::Embedding::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::Embedding::bind(property, style, vAtoms, DESC, &sListener); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::Embedding::unbind(vAtoms, DESC, &sListener); };

            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */




#endif /* LSP_PLUG_IN_TK_PROP_FLAGS_EMBEDDING_H_ */
