/*
 * Slot.cpp
 *
 *  Created on: 12 июн. 2017 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

#define ID_GEN_MASK         0x7fffff

namespace lsp
{
    namespace tk
    {
        Slot::Slot()
        {
            nID         = 0;
        }

        Slot::~Slot()
        {
            unbind_all();
        }

        inline Slot::item_t *Slot::find_item(handler_id_t id)
        {
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                item_t *ptr = vItems.uget(i);
                if (ptr->nID == id)
                    return ptr;
            }

            return NULL;
        }
    
        handler_id_t Slot::bind(event_handler_t handler, void *arg, bool enabled)
        {
            return bind(handler, false, arg, enabled);
        }

        handler_id_t Slot::intercept(event_handler_t handler, void *arg, bool enabled)
        {
            return bind(handler, true, arg, enabled);
        }

        handler_id_t Slot::bind(event_handler_t handler, bool intercept, void *arg, bool enabled)
        {
            // Check data
            if (handler == NULL)
                return -STATUS_BAD_ARGUMENTS;

            // Now try to allocate new data
            item_t item;

            // Generate handler identifier
            do
            {
                item.nID    = nID;
                nID         = (nID + 1) & ID_GEN_MASK;
            } while (find_item(item.nID) != NULL);

            // Initialize item and bind it
            size_t mask        = (intercept) ? BIND_DFL : BIND_DFL | BIND_INTERCEPT;
            item.nFlags        = (enabled) ? mask | BIND_ENABLED : mask;
            item.pHandler      = handler;
            item.pPtr          = arg;

            // Now try to allocate new data
            return (vItems.add(&item)) ? item.nID : -STATUS_NO_MEM;
        }

        status_t Slot::unbind(handler_id_t id)
        {
            // Check data
            if (id < 0)
                return STATUS_BAD_ARGUMENTS;

            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                item_t *ptr = vItems.uget(i);
                if (ptr->nID == id)
                {
                    vItems.remove(i);
                    return STATUS_OK;
                }
            }

            return -STATUS_NOT_FOUND;
        }

        handler_id_t Slot::unbind(event_handler_t handler, void *arg)
        {
            // Check data
            if (handler == NULL)
                return - STATUS_BAD_ARGUMENTS;

            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                item_t *ptr = vItems.uget(i);
                if ((ptr->pHandler == handler) && (ptr->pPtr == arg))
                {
                    handler_id_t id  = ptr->nID;
                    vItems.remove(i);
                    return id;;
                }
            }

            return -STATUS_NOT_FOUND;
        }

        size_t Slot::unbind_all()
        {
            size_t removed = vItems.size();
            vItems.flush();
            return removed;
        }

        status_t Slot::disable(handler_id_t id)
        {
            // Check data
            if (id < 0)
                return STATUS_BAD_ARGUMENTS;

            item_t *ptr             = find_item(id);
            if (ptr == NULL)
                return STATUS_NOT_FOUND;

            ptr->nFlags            &= ~BIND_ENABLED;
            return STATUS_OK;
        }

        size_t Slot::disable_all()
        {
            return disable_all(true, true);
        }

        size_t Slot::disable_all_interceptors()
        {
            return disable_all(false, true);
        }

        size_t Slot::disable_all_bindings()
        {
            return disable_all(true, false);
        }

        size_t Slot::disable_all(bool handler, bool interceptor)
        {
            if ((!handler) && (!interceptor))
                return 0;

            size_t disabled         = 0;
            size_t mask             = (handler && interceptor) ? BIND_ENABLED : BIND_ENABLED | BIND_INTERCEPT;
            size_t check            = ((!handler) && interceptor) ? BIND_INTERCEPT | BIND_ENABLED : BIND_ENABLED;

            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                item_t *ptr = vItems.uget(i);
                if ((ptr->nFlags & mask) == check)
                {
                    ptr->nFlags    &= ~BIND_ENABLED;
                    disabled        ++;
                }
            }

            return disabled;
        }

        status_t Slot::enable(handler_id_t id)
        {
            // Check data
            if (id < 0)
                return STATUS_BAD_ARGUMENTS;

            item_t *ptr             = find_item(id);
            if (ptr == NULL)
                return STATUS_NOT_FOUND;

            ptr->nFlags            |= BIND_ENABLED;
            return STATUS_OK;
        }

        size_t Slot::enable_all(bool handler, bool interceptor)
        {
            size_t enabled          = 0;
            size_t mask             = (handler && interceptor) ? BIND_ENABLED : BIND_ENABLED | BIND_INTERCEPT;
            size_t check            = ((!handler) && interceptor) ? BIND_INTERCEPT : 0;

            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                item_t *ptr = vItems.uget(i);
                if ((ptr->nFlags & mask) == check)
                {
                    ptr->nFlags    |= BIND_ENABLED;
                    enabled         ++;
                }
            }

            return enabled;
        }

        size_t Slot::enable_all()
        {
            return enable_all(true, true);
        }

        size_t Slot::enable_all_bindings()
        {
            return enable_all(true, false);
        }

        size_t Slot::enable_all_interceptors()
        {
            return enable_all(false, true);
        }

        status_t Slot::execute(Widget *sender, void *data)
        {
            // Make a copy of handlers first
            lltl::darray<item_t> copy;
            if (!copy.set(&vItems))
                return STATUS_NO_MEM;

            // First iteration, iterate all interceptors
            for (size_t i=0, n=copy.size(); i<n; ++i)
            {
                // Execute interceptor in the chain
                item_t *ptr = copy.uget(i);
                if ((ptr->nFlags & (BIND_ENABLED | BIND_INTERCEPT)) == (BIND_ENABLED | BIND_INTERCEPT))
                {
                    status_t result      = ptr->pHandler(sender, ptr->pPtr, data);
                    if (result != STATUS_OK)
                        return (result == STATUS_SKIP) ? STATUS_OK : result;
                }
            }

            // Second iteration, iterate all handlers
            for (size_t i=0, n=copy.size(); i<n; ++i)
            {
                // Execute handler in the chain
                item_t *ptr = copy.uget(i);
                if ((ptr->nFlags & (BIND_ENABLED | BIND_INTERCEPT)) == BIND_ENABLED)
                {
                    status_t result      = ptr->pHandler(sender, ptr->pPtr, data);
                    if (result != STATUS_OK)
                        return result;
                }
            }

            return STATUS_OK;
        }

    }
} /* namespace lsp */
