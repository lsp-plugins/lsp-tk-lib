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
#include <lsp-plug.in/tk/prop.h>

namespace lsp
{
    namespace tk
    {
        class Display;
        class Widget;
        
        /**
         * Floating-point property interface
         */
        class Float: public Property
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

                    public:
                        virtual void notify(atom_t property);
                };

            protected:
                float               fValue;
                Listener            sListener;

            protected:
                status_t            unbind();
                status_t            bind(prop::Listener *listener, atom_t property, Style *style);
                void                sync();

            protected:
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

                /**
                 * Swap contents
                 * @param dst destination property to perform swap
                 */
                void                swap(Float *dst);
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
                    ~Float();

                public:
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

#endif /* UI_TK_SYS_LSPFLOATPROPERTY_H_ */
