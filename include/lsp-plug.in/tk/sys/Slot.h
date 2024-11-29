/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 12 июн. 2017 г.
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
                bool                    bTracking;  // Tracking skip messages

            protected:
                inline item_t          *find_item(handler_id_t id);
                handler_id_t            bind(event_handler_t handler, bool intercept, void *arg, bool enabled);
                size_t                  disable_all(bool handler, bool interceptor);
                size_t                  enable_all(bool handler, bool interceptor);
                status_t                track_result(status_t result) const;

            public:
                explicit Slot(bool tracking = true);
                Slot(const Slot &) = delete;
                Slot(Slot &&) = delete;
                ~Slot();

                Slot & operator = (const Slot &) = delete;
                Slot & operator = (Slot &&) = delete;

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
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_SYS_SLOTS_H_ */
