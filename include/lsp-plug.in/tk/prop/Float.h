/*
 * FloatProperty.h
 *
 *  Created on: 9 окт. 2019 г.
 *      Author: sadko
 */

#ifndef UI_TK_SYS_LSPFLOAT_H_
#define UI_TK_SYS_LSPFLOAT_H_

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
         * Floating-point property interface
         */
        class Float
        {
            private:
                Float & operator = (const Float *);

            protected:
                class Listener: public IStyleListener
                {
                    protected:
                        Float      *pValue;

                    public:
                        inline Listener(Float *ptr)             { pValue = ptr;     }
                        ~Listener();

                    public:
                        virtual void notify(atom_t property);
                };

            protected:
                float               fValue;
                Style              *pStyle;
                IStyleListener     *pListener;
                atom_t              aValue;
                Listener            sListener;

            protected:
                status_t            unbind();
                status_t            bind(Style *style, IStyleListener *listener, atom_t property);

            public:
                Float();
                ~Float();

            public:
                /**
                 * Get value of the float property
                 * @return value of the float property
                 */
                inline float        get() const             { return fValue;                }

                /**
                 * Set value of the float property
                 * @param v value of the float property
                 * @return
                 */
                float               set(float v);
        };

        namespace prop
        {
            /**
             * Floating-point property implementation
             */
            class Float: public tk::Float
            {
                private:
                    Float & operator = (const Float *);

                public:
                    explicit Float();

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

#endif /* UI_TK_SYS_LSPFLOATPROPERTY_H_ */
