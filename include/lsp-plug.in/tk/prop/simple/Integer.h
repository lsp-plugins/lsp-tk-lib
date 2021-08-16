/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 6 мая 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_INTEGER_H_
#define LSP_PLUG_IN_TK_PROP_INTEGER_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Integering property interface
         */
        class Integer: public SimpleProperty
        {
            private:
                Integer & operator = (const Integer &);
                Integer(const Integer &);

            protected:
                ssize_t             nValue;
                Listener            sListener;

            protected:
                virtual void        commit(atom_t property);
                virtual void        push();

            protected:
                explicit Integer(prop::Listener *listener = NULL);
                virtual ~Integer();

            public:
                /**
                 * Get value of the float property
                 * @return value of the float property
                 */
                inline ssize_t      get() const             { return nValue;                }

                /**
                 * Set value of the float property
                 * @param v value of the float property
                 * @return
                 */
                ssize_t             set(ssize_t v);
                inline ssize_t      copy(const Integer *v)  { return set(v->nValue);        }

                inline ssize_t      add(ssize_t v)          { return set(nValue + v);       }
                inline ssize_t      sub(ssize_t v)          { return set(nValue - v);       }

                /**
                 * Swap contents
                 * @param dst destination property to perform swap
                 */
                void                swap(Integer *dst);
        };

        namespace prop
        {
            /**
             * Integering property implementation
             */
            class Integer: public tk::Integer
            {
                private:
                    Integer & operator = (const Integer &);
                    Integer(const Integer &);

                public:
                    explicit Integer(prop::Listener *listener = NULL): tk::Integer(listener) {};

                public:
                    ssize_t             commit_value(ssize_t value);

                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return SimpleProperty::bind(property, style, PT_INT, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return SimpleProperty::bind(property, style, PT_INT, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return SimpleProperty::bind(property, style, PT_INT, &sListener); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return SimpleProperty::unbind(&sListener); };

                    inline void         listener(prop::Listener *listener)  { pListener = listener;                     }
            };
        }
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_INTEGER_H_ */
