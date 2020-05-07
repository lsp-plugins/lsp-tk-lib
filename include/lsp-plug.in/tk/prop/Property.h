/*
 * Property.h
 *
 *  Created on: 7 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_PROPERTY_H_
#define LSP_PLUG_IN_TK_PROP_PROPERTY_H_

#include <lsp-plug.in/tk/version.h>
#include <lsp-plug.in/tk/types.h>

#include <lsp-plug.in/tk/style.h>
#include <lsp-plug.in/tk/prop/types.h>
#include <lsp-plug.in/tk/prop/Listener.h>

namespace lsp
{
    namespace tk
    {
        namespace prop
        {
            class Listener;
        }

        /**
         * Floating-point property interface
         */
        class Property
        {
            private:
                Property & operator = (const Property *);

            protected:
                Style              *pStyle;
                prop::Listener     *pListener;

            protected:
                inline Property()
                {
                    pStyle          = NULL;
                    pListener       = NULL;
                }

            public:
                /**
                 * Check that property matches another property
                 * @param prop pointer to property to check
                 * @return true if property matches
                 */
                inline bool         is(const Property *prop) const { return prop == this; }

                /**
                 * Check that property matches another property
                 * @param prop reference to property to check
                 * @return true if property matches
                 */
                inline bool         is(const Property &prop) const { return &prop == this; }
        };

    }
}


#endif /* LSP_PLUG_IN_TK_PROP_PROPERTY_H_ */
