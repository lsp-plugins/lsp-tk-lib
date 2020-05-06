/*
 * ComplexWidget.cpp
 *
 *  Created on: 10 авг. 2017 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/base/ComplexWidget.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t ComplexWidget::metadata = { "ComplexWidget", &Widget::metadata };

        ComplexWidget::ComplexWidget(Display *dpy):
            Widget(dpy)
        {
            nMouse      = 0;
            nKey        = 0;
            pMouse      = NULL;
            pKey        = NULL;
            pClass      = &metadata;
        }

        ComplexWidget::~ComplexWidget()
        {
        }

        Widget *ComplexWidget::acquire_mouse_handler(const ws::event_t *e)
        {
            // Check that we work in exclusive mode
            if ((nMouse != 0) && (pMouse != NULL))
                return pMouse;

            Widget *child  = find_widget(e->nLeft, e->nTop);
            if (child == pMouse)
                return pMouse;

            // New child has been found
            if (pMouse != NULL)
            {
                ws::event_t ev = *e;
                ev.nType    = ws::UIE_MOUSE_OUT;
                pMouse->handle_event(&ev);
                mark_pointed();
            }
            if (child != NULL)
            {
                ws::event_t ev = *e;
                ev.nType    = ws::UIE_MOUSE_IN;
                child->handle_event(&ev);
            }

            return pMouse = child;
        }

        void ComplexWidget::release_mouse_handler(const ws::event_t *e)
        {
            if (pMouse == NULL)
            {
                nMouse = 0;
                return;
            }

            if (e->nState & ws::MCF_BTN_MASK)
                return;

            // Check that cursor is out of the widget
            if (pMouse != find_widget(e->nLeft, e->nTop))
            {
                ws::event_t ev = *e;
                ev.nType    = ws::UIE_MOUSE_OUT;
                pMouse->handle_event(&ev);
                pMouse      = NULL;
            }
        }

        Widget *ComplexWidget::find_widget(ssize_t x, ssize_t y)
        {
            return NULL;
        }

        status_t ComplexWidget::handle_event_internal(const ws::event_t *e)
        {
            return Widget::handle_event(e);
        }

        status_t ComplexWidget::handle_event(const ws::event_t *e)
        {
            switch (e->nType)
            {
                case ws::UIE_KEY_UP:
                {
                    Widget *child  = (pKey == NULL) ? find_widget(e->nLeft, e->nTop) : pKey;
                    if (child == NULL)
                        return handle_event_internal(e);

                    // Handle key release event and free if possible
                    child->handle_event(e);
                    if ((--nKey) <= 0)
                        pKey        = NULL;

                    break;
                }

                case ws::UIE_KEY_DOWN:
                {
                    Widget *child  = (pKey == NULL) ? find_widget(e->nLeft, e->nTop) : pKey;
                    if (child == NULL)
                        return handle_event_internal(e);

                    // Handle key press event
                    child->handle_event(e);
                    nKey++;
                    pKey        = child;

                    break;
                }

                case ws::UIE_MOUSE_UP:
                {
                    Widget *child = acquire_mouse_handler(e);
                    nMouse     &= ~(1 << e->nCode);
                    if (child == NULL)
                        return handle_event_internal(e);

                    // Handle mouse release event and free if possible
                    child->handle_event(e);
                    release_mouse_handler(e);
                    break;
                }

                case ws::UIE_MOUSE_DOWN:
                {
                    Widget *child = acquire_mouse_handler(e);
                    nMouse     |= 1 << e->nCode;
                    if (child == NULL)
                        return handle_event_internal(e);

                    // Handle mouse button press event
                    return child->handle_event(e);
                }

                case ws::UIE_MOUSE_DBL_CLICK:
                case ws::UIE_MOUSE_TRI_CLICK:
                case ws::UIE_MOUSE_SCROLL:
                {
                    Widget *child  = acquire_mouse_handler(e);
                    if (child == NULL)
                        handle_event_internal(e);
                    else
                        child->handle_event(e);
                    release_mouse_handler(e);
                    break;
                }

                case ws::UIE_MOUSE_MOVE:
                {
                    Widget *child  = acquire_mouse_handler(e);
                    nMouse      = e->nState & ws::MCF_BTN_MASK;
                    if (child == NULL)
                        return handle_event_internal(e);

                    child->handle_event(e);
                    break;
                }

                case ws::UIE_MOUSE_OUT:
                {
                    // Check that cursor is out of the widget
                    if ((nMouse == 0) && (pMouse != NULL))
                    {
                        ws::event_t ev = *e;
                        ev.nType    = ws::UIE_MOUSE_OUT;
                        pMouse->handle_event(&ev);
                        pMouse      = NULL;
                    }
                    break;
                }

                case ws::UIE_DRAG_REQUEST:
                {
                    Widget *child = find_widget(e->nLeft, e->nTop);
                    if (child != NULL) // Pass event to the child
                        child->handle_event(e);
                    else
                    {
                        ws::event_t tmp = *e;
                        sSlots.execute(SLOT_DRAG_REQUEST, this, &tmp);
                    }
                    break;
                }

                default:
                    return handle_event_internal(e);
            }

            return STATUS_OK;
        }
    } /* namespace tk */
} /* namespace lsp */
