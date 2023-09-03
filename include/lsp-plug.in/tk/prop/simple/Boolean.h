/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 7 мая 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_BOOLEAN_H_
#define LSP_PLUG_IN_TK_PROP_BOOLEAN_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Booleaning-point property interface
         */
        class Boolean: public SimpleProperty
        {
            protected:
                bool                bValue;

            protected:
                virtual void        commit(atom_t property);
                virtual void        push();

            protected:
                explicit Boolean(prop::Listener *listener = NULL);
                Boolean(const Boolean &) = delete;
                Boolean(Boolean &&) = delete;
                virtual ~Boolean();

                Boolean & operator = (const Boolean &) = delete;
                Boolean & operator = (Boolean &&) = delete;

            public:
                /**
                 * Get value of the boolean property
                 * @return value of the boolean property
                 */
                inline bool         get() const             { return bValue;                }

                /**
                 * Get inverse value of the boolean property
                 * @return inverse value of the boolean property
                 */
                inline bool         iget() const            { return !bValue;               }

                /**
                 * Set value of the boolean property
                 * @param v value of the boolean property
                 * @return previous value
                 */
                bool                set(bool v = true);

                /**
                 * Invert the value of the boolean property
                 * @return previous value
                 */
                inline bool         toggle()                { return set(!bValue);          }

                /**
                 * Swap contents
                 * @param dst destination property to perform swap
                 */
                void                swap(Boolean *dst);

                /**
                 * Commit boolean flag as some bit in a bit set
                 * @param set bit set to take as source
                 * @param flag flag that indicates boolean value
                 * @return updated bit set
                 */
                size_t              add_as_flag(size_t set, size_t flag);
        };

        namespace prop
        {
            /**
             * Booleaning property implementation
             */
            class Boolean: public tk::Boolean
            {
                private:
                    Boolean & operator = (const Boolean &);
                    Boolean(const Boolean &);

                public:
                    explicit inline Boolean(prop::Listener *listener = NULL): tk::Boolean(listener) {};

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return SimpleProperty::bind(property, style, PT_BOOL, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return SimpleProperty::bind(property, style, PT_BOOL, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return SimpleProperty::bind(property, style, PT_BOOL, &sListener); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return SimpleProperty::unbind(&sListener); };

                    /**
                     * Commit value
                     * @param value value to commit
                     * @return previous value
                     */
                    bool                commit_value(bool value);

                    inline void         listener(prop::Listener *listener)  { pListener = listener;                     }
            };
        } /* namespace prop */

    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_BOOLEAN_H_ */
