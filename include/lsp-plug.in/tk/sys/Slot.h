/*
 * Slot.h
 *
 *  Created on: 12 июн. 2017 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_SYS_SLOTS_H_
#define LSP_PLUG_IN_TK_SYS_SLOTS_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/lltl/darray.h>

namespace lsp
{
    namespace tk
    {
        /**
         * Slot class for event handling in publisher-subscriber mode
         */
        class Slot
        {
            protected:
                enum bind_flags_t
                {
                    BIND_DFL            = 0,
                    BIND_ENABLED        = 1 << 0,
                    BIND_INTERCEPT      = 1 << 1
                };

                typedef struct item_t
                {
                    handler_id_t        nID;        // Identifier of handler
                    size_t              nFlags;     // Additional flags
                    event_handler_t     pHandler;   // Handler
                    void               *pPtr;       // Additional argument to pass
                } item_t;

            protected:
                lltl::darray<item_t>    vItems;
                handler_id_t            nID;        // ID generator

            protected:
                inline item_t          *find_item(handler_id_t id);
                handler_id_t            bind(event_handler_t handler, bool intercept, void *arg, bool enabled);
                size_t                  disable_all(bool handler, bool interceptor);
                size_t                  enable_all(bool handler, bool interceptor);

            public:
                explicit Slot();
                ~Slot();

            public:
                /** Bind slot
                 *
                 * @param handler event handler routine
                 * @param arg argument
                 * @param enabled binding is enabled
                 * @return identifier of handler or negative status code
                 */
                handler_id_t        bind(event_handler_t handler, void *arg = NULL, bool enabled = true);

                /** Intercept slot
                 *
                 * @param handler interceptor handler routine
                 * @param arg argument
                 * @param enabled interceptor is enabled
                 * @return identifier of interceptor or negative status code
                 */
                handler_id_t        intercept(event_handler_t handler, void *arg = NULL, bool enabled = true);

                /** Unbind handler or interceptor by identifier
                 *
                 * @param id handler identifier
                 * @return status of operation
                 */
                status_t            unbind(handler_id_t id);

                /** Unbind handler or interceptor by contents. Removes first occured binding
                 *
                 * @param handler event handler routine
                 * @param arg argument
                 * @return identifier of removed handler on success, negative status code on error
                 */
                handler_id_t        unbind(event_handler_t handler, void *arg = NULL);

                /** Unbind all handlers and interceptors for the slot
                 *
                 * @return number of handlers removed from bindings
                 */
                size_t              unbind_all();

                /** Disable event handler or interceptor for the slot
                 *
                 * @param id handler identifier
                 * @return status of operation
                 */
                status_t            disable(handler_id_t id);

                /** Disable all event handlers or interceptors
                 *
                 * @return number of non-disabled handlers that were disabled
                 */
                size_t              disable_all();

                /** Disable all event handlers only
                 *
                 * @return number of non-disabled handlers that were disabled
                 */
                size_t              disable_all_bindings();

                /** Disable all event interceptors only
                 *
                 * @return number of non-disabled handlers that were disabled
                 */
                size_t              disable_all_interceptors();

                /** Enable event handler in the slot
                 *
                 * @param id handler identifier
                 * @return status of operation
                 */
                status_t            enable(handler_id_t id);

                /** Enable all event handlers and interceptors
                 *
                 * @return number of non-enabled handlers that were disabled
                 */
                size_t              enable_all();

                /** Enable all interceptors
                 *
                 * @return number of non-enabled interceptors that were enabled
                 */
                size_t              enable_all_interceptors();

                /** Enable all bindings
                 *
                 * @return number of non-enabled handlers that were enabled
                 */
                size_t              enable_all_bindings();
    
                /** Execute slot handlers
                 *
                 * @param sender the object that initiated event
                 * @param data data to process
                 * @return status of operation
                 */
                status_t            execute(Widget *sender, void *data);
        };
    }
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_SYS_SLOTS_H_ */
