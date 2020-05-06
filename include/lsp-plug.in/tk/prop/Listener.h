/*
 * Listener.h
 *
 *  Created on: 7 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_LISTENER_H_
#define LSP_PLUG_IN_TK_PROP_LISTENER_H_

#include <lsp-plug.in/tk/version.h>
#include <lsp-plug.in/tk/types.h>

#include <lsp-plug.in/tk/prop/Property.h>

namespace lsp
{
    namespace tk
    {
        class Property;

        namespace prop
        {
            /**
             * Property listener interface
             */
            class Listener
            {
                public:
                    /**
                     * Virtual destructor
                     */
                    virtual ~Listener();

                public:
                    /**
                     * Change callback
                     * @param prop property that has been changed
                     */
                    virtual void notify(Property *prop);
            };
        }
    }
}

#endif /* LSP_PLUG_IN_TK_PROP_LISTENER_H_ */
