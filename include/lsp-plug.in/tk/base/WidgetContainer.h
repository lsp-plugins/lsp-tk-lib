/*
 * WidgetContainer.h
 *
 *  Created on: 16 июн. 2017 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_BASE_WIDGETCONTAINER_H_
#define LSP_PLUG_IN_TK_BASE_WIDGETCONTAINER_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /** This is a basic widget class for any widget that contains sub-widgets
         *
         */
        class WidgetContainer: public ComplexWidget
        {
            public:
                static const w_class_t    metadata;

            //---------------------------------------------------------------------------------
            // Construction and destruction
            public:
                explicit WidgetContainer(Display *dpy);

                virtual ~WidgetContainer();

            //---------------------------------------------------------------------------------
            // Manipulation
            public:
                /** Add child to widget container
                 *
                 * @param child child widget to add
                 * @return status of operation
                 */
                virtual status_t    add(Widget *child);

                /** Remove child from widget container
                 *
                 * @param child child widget to remove
                 * @return status of operation
                 */
                virtual status_t    remove(Widget *child);

                /** Remove all widgets
                 *
                 * @return status of operation
                 */
                virtual status_t    remove_all();
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_BASE_WIDGETCONTAINER_H_ */
