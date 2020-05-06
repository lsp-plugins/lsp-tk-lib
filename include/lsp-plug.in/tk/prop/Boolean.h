/*
 * Boolean.h
 *
 *  Created on: 7 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_BOOLEAN_H_
#define LSP_PLUG_IN_TK_PROP_BOOLEAN_H_

#include <lsp-plug.in/tk/version.h>
#include <lsp-plug.in/tk/types.h>

#include <lsp-plug.in/tk/style.h>

namespace lsp
{
    namespace tk
    {
        class Display;
        class Widget;

        /**
         * Booleaning-point property interface
         */
        class Boolean
        {
            private:
                Boolean & operator = (const Boolean *);

            protected:
                class Listener: public IStyleListener
                {
                    protected:
                        Boolean      *pValue;

                    public:
                        inline Listener(Boolean *ptr)             { pValue = ptr;     }
                        ~Listener();

                    public:
                        virtual void notify(atom_t property);
                };

            protected:
                bool                bValue;
                Style              *pStyle;
                IStyleListener     *pListener;
                atom_t              aValue;
                Listener            sListener;

            protected:
                status_t            unbind();
                status_t            bind(Style *style, IStyleListener *listener, atom_t property);

            public:
                Boolean();
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
                 * @return
                 */
                bool                set(bool v);
        };

        namespace prop
        {
            /**
             * Booleaning-point property implementation
             */
            class Boolean: public tk::Boolean
            {
                private:
                    Boolean & operator = (const Boolean *);

                public:
                    explicit Boolean();

                public:
                    /**
                     * Check that property is bound to this object
                     * @param property property identifier
                     * @return true if property is bound
                     */
                    inline bool         is_bound(atom_t property)  { return aValue == property;    }

                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    status_t            bind(Widget *widget, IStyleListener *listener, const char *property);
                    status_t            bind(Widget *widget, IStyleListener *listener, atom_t property);
                    status_t            bind(Display *dpy, Style *style, IStyleListener *listener, const char *property);
                    status_t            bind(Style *style, IStyleListener *listener, atom_t property);

                    /**
                     * Unbind property
                     */
                    void                unbind();
            };
        }

    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_BOOLEAN_H_ */
