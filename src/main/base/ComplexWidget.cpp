/*
 * ComplexWidget.cpp
 *
 *  Created on: 10 авг. 2017 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t ComplexWidget::metadata = { "ComplexWidget", &Widget::metadata };

        ComplexWidget::ComplexWidget(Display *dpy):
            Widget(dpy)
        {
            nKey        = 0;
            pKey        = NULL;
            pClass      = &metadata;
        }

        ComplexWidget::~ComplexWidget()
        {
        }

        status_t ComplexWidget::handle_event(const ws::event_t *e)
        {
            switch (e->nType)
            {
                case ws::UIE_KEY_UP:
                {
                    Widget *child  = (pKey == NULL) ? find_widget(e->nLeft, e->nTop) : pKey;
                    if (child == NULL)
                        return Widget::handle_event(e);

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
                        return Widget::handle_event(e);

                    // Handle key press event
                    child->handle_event(e);
                    nKey++;
                    pKey        = child;

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
                    return Widget::handle_event(e);
            }

            return STATUS_OK;
        }
    } /* namespace tk */
} /* namespace lsp */
