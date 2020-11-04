/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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
            private:
                Boolean & operator = (const Boolean &);

            protected:
                class Listener: public IStyleListener
                {
                    protected:
                        Boolean      *pValue;

                    public:
                        inline Listener(Boolean *ptr)             { pValue = ptr;     }

                    public:
                        virtual void notify(atom_t property);
                };

            protected:
                bool                bValue;
                Listener            sListener;

                status_t            init(bool value);
                status_t            override(bool value);

            protected:
                void                commit();

            protected:
                explicit Boolean(prop::Listener *listener = NULL);
                ~Boolean();

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

                public:
                    explicit inline Boolean(prop::Listener *listener = NULL): tk::Boolean(listener) {};

                protected:
                    using tk::Boolean::init;
                    using tk::Boolean::override;

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
                     * Init default value
                     * @param style style
                     * @param value default value
                     * @return status of operation
                     */
                    status_t            init(Style *style, bool value);
                    status_t            override(Style *style, bool value);

                    static status_t     init(const char *name, Style *style, bool value);
                    static status_t     override(const char *name, Style *style, bool value);

                    status_t            create(const char *name, Style *style, bool value);

                    /**
                     * Commit value
                     * @param value value to commit
                     * @return previous value
                     */
                    bool                commit(bool value);

                    inline void         listener(prop::Listener *listener)  { pListener = listener;                     }
            };
        }

    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_BOOLEAN_H_ */
