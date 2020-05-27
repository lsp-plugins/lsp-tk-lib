/*
 * Boolean.h
 *
 *  Created on: 7 мая 2020 г.
 *      Author: sadko
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

            protected:
                void                commit();

            protected:
                explicit Boolean(prop::Listener *listener = NULL);
                ~Boolean();

            public:
                /**
                 * Get value of the float property
                 * @return value of the float property
                 */
                inline bool         get() const             { return bValue;                }

                /**
                 * Set value of the float property
                 * @param v value of the float property
                 * @return previous value
                 */
                bool                set(bool v = true);

                /**
                 * Swap contents
                 * @param dst destination property to perform swap
                 */
                void                swap(Boolean *dst);
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

                    /**
                     * Commit value
                     * @param value value to commit
                     * @return previous value
                     */
                    bool                 commit(bool value);
            };
        }

    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_BOOLEAN_H_ */
