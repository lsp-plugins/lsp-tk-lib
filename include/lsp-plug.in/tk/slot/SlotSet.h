/*
 * SlotSet.h
 *
 *  Created on: 12 июн. 2017 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_SLOT_SLOTSET_H_
#define LSP_PLUG_IN_TK_SLOT_SLOTSET_H_

#include <lsp-plug.in/tk/version.h>
#include <lsp-plug.in/tk/types.h>
#include <lsp-plug.in/tk/slot/types.h>
#include <lsp-plug.in/tk/slot/Slot.h>
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

#endif /* LSP_PLUG_IN_TK_SLOT_SLOTSET_H_ */
