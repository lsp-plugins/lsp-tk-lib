/*
 * Integer.h
 *
 *  Created on: 6 мая 2020 г.
 *      Author: sadko
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
                Integer & operator = (const Integer *);

            protected:
                class Listener: public IStyleListener
                {
                    protected:
                        Integer      *pValue;

                    public:
                        inline Listener(Integer *ptr)             { pValue = ptr;     }

                    public:
                        virtual void notify(atom_t property);
                };

            protected:
                ssize_t             nValue;
                Listener            sListener;

            protected:
                void                commit();

            protected:
                explicit Integer(prop::Listener *listener = NULL);
                ~Integer();

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
                    Integer & operator = (const Integer *);

                public:
                    explicit Integer(prop::Listener *listener = NULL): tk::Integer(listener) {};

                public:
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
            };
        }
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_INTEGER_H_ */
