/*
 * ComplexWidget.h
 *
 *  Created on: 10 авг. 2017 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_BASE_COMPLEXWIDGET_H_
#define LSP_PLUG_IN_TK_BASE_COMPLEXWIDGET_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class ComplexWidget: public Widget
        {
            private:
                ComplexWidget & operator = (const ComplexWidget &);

            public:
                static const w_class_t    metadata;

            protected:
                ssize_t             nKey;
                Widget             *pKey;

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
