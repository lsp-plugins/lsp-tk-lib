/*
 * ComplexWidget.h
 *
 *  Created on: 10 авг. 2017 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_BASE_COMPLEXWIDGET_H_
#define LSP_PLUG_IN_TK_BASE_COMPLEXWIDGET_H_

#include <lsp-plug.in/tk/types.h>
#include <lsp-plug.in/tk/version.h>

#include <lsp-plug.in/tk/base/Widget.h>

namespace lsp
{
    namespace tk
    {
        class ComplexWidget: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                ssize_t             nMouse;
                ssize_t             nKey;
                Widget             *pKey;
                Widget             *pMouse;

            protected:
                Widget             *acquire_mouse_handler(const ws::event_t *e);
                void                release_mouse_handler(const ws::event_t *e);

            //---------------------------------------------------------------------------------
            // Manipulation
            protected:
                virtual Widget         *find_widget(ssize_t x, ssize_t y);

                virtual status_t        handle_event_internal(const ws::event_t *e);

            //---------------------------------------------------------------------------------
            // Construction and destruction
            public:
                explicit ComplexWidget(Display *dpy);

                virtual ~ComplexWidget();

            //---------------------------------------------------------------------------------
            // Manipulation
            public:
                /** Handle event from window system
                 *
                 * @param e event to handle
                 * @return status of operation
                 */
                virtual status_t    handle_event(const ws::event_t *e);

        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_BASE_COMPLEXWIDGET_H_ */
