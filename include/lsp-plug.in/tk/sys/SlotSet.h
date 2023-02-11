/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef LSP_PLUG_IN_TK_SYS_SLOTSET_H_
#define LSP_PLUG_IN_TK_SYS_SLOTSET_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/lltl/parray.h>

namespace lsp
{
    namespace tk
    {
        class Widget;

        /**
         * Set of slots identified by unique slot identifier
         */
        class SlotSet
        {
            protected:
                typedef struct item_t
                {
                    slot_t              nType;
                    Slot                sSlot;
                } item_t;

            protected:
                lltl::parray<item_t>    vSlots;

            public:
                explicit SlotSet();
                ~SlotSet();

            public:
                /** Get slot by identifier
                 *
                 * @param id slot identifier
                 * @return slot or NULL if not present
                 */
                Slot               *slot(slot_t id);

                /** Add slot
                 *
                 * @param id slot identifier
                 * @return added slot or NULL if no memory
                 */
                Slot               *add(slot_t id);

                /** Add slot and bind
                 *
                 * @param id slot identifier
                 * @param handler slot handler
                 * @param arg argument
                 * @param enabled enable flag
                 * @return status of operation
                 */
                handler_id_t        add(slot_t id, event_handler_t handler, void *arg = NULL, bool enabled = true);

                /** Destroy previously allocated structures
                 *
                 */
                void                destroy();

                /**
                 * Check that the slot set contains the specific slot
                 * @param id slot identifier
                 * @return true if the slot set contains the specific slot
                 */
                bool                contains(slot_t id) const;

                /** Bind slot handler to slot
                 *
                 * @param id slot identifier
                 * @param handler slot handler
                 * @param arg slot argument
                 * @param enabled enable flag
                 * @return status of operation
                 */
                handler_id_t        bind(slot_t id, event_handler_t handler, void *arg = NULL, bool enabled = true);

                /** Intercept slot
                 *
                 * @param id slot identifier
                 * @param handler interceptor handler
                 * @param arg slot argument
                 * @param enabled enable flag
                 * @return status of operation
                 */
                handler_id_t        intercept(slot_t id, event_handler_t handler, void *arg = NULL, bool enabled = true);

                /** Unbind slot handler
                 *
                 * @param id slot identifier
                 * @param handler slot handler identifier
                 * @return status of operation
                 */
                status_t            unbind(slot_t id, handler_id_t handler);

                /** Unbind slot handler
                 *
                 * @param id slot identifier
                 * @param handler slot handler routine
                 * @param arg slot argument
                 * @return status of operation
                 */
                handler_id_t        unbind(slot_t id, event_handler_t handler, void *arg = NULL);

                /** Unbind all handlers for the slot
                 *
                 * @param id slot identifier
                 * @return number of handlers removed from bindings
                 */
                size_t              unbind_all(slot_t id);

                /** Disable event handler in the slot
                 *
                 * @param id slot identifier
                 * @param handler handler identifier
                 * @return status of operation
                 */
                status_t            disable(slot_t id, handler_id_t handler);

                /** Disable all event handlers
                 *
                 * @param id slot identifier
                 * @return number of non-disabled handlers that were disabled
                 */
                ssize_t             disable_all(slot_t id);

                /** Enable event handler in the slot
                 *
                 * @param id handler identifier
                 * @return status of operation
                 */
                status_t            enable(slot_t id, handler_id_t handler);

                /** Enable all event handlers for slot
                 *
                 * @param id slot identifier
                 * @return number of non-enabled handlers that were disabled
                 */
                size_t              enable_all(slot_t id);

                /** Execute slot handlers
                 *
                 * @param sender the widget that initiates an event
                 * @param id slot identifier
                 * @param data data to process
                 * @return status of operation
                 */
                status_t            execute(slot_t id, Widget *sender, void *data = NULL);
        };

    }
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_SYS_SLOTSET_H_ */
