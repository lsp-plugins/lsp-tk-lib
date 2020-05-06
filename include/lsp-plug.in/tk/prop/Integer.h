/*
 * Integer.h
 *
 *  Created on: 6 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_INTEGER_H_
#define LSP_PLUG_IN_TK_PROP_INTEGER_H_

#include <lsp-plug.in/tk/version.h>
#include <lsp-plug.in/tk/types.h>

#include <lsp-plug.in/tk/style.h>
#include <lsp-plug.in/tk/prop/Property.h>

namespace lsp
{
    namespace tk
    {
        class Display;
        class Widget;
        
        /**
         * Integering-point property interface
         */
        class Integer: public Property
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
                status_t            unbind();
                status_t            bind(prop::Listener *listener, atom_t property, Style *style);

            public:
                Integer();
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
             * Integering-point property implementation
             */
            class Integer: public tk::Integer
            {
                private:
                    Integer & operator = (const Integer *);

                public:
                    explicit Integer();

                public:
                    /**
                     * Check that property is bound to this object
                     * @param property property identifier
                     * @return true if property is bound
                     */
                    inline bool         is_bound(atom_t property)  { return nAtom == property;    }

                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    status_t            bind(prop::Listener *listener, const char *property, Widget *widget);
                    status_t            bind(prop::Listener *listener, atom_t property, Widget *widget);
                    status_t            bind(prop::Listener *listener, const char *property, Display *dpy, Style *style);
                    status_t            bind(prop::Listener *listener, atom_t property, Style *style);

                    /**
                     * Unbind property
                     */
                    void                unbind();
            };
        }
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_INTEGER_H_ */
