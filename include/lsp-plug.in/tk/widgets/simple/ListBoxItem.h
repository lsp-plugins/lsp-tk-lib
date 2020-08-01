/*
 * ListBoxItem.h
 *
 *  Created on: 30 июл. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LISTBOXITEM_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LISTBOXITEM_H_

namespace lsp
{
    namespace tk
    {
        
        class ListBoxItem: public Widget
        {
            public:
                static const w_class_t    metadata;

            public:
                explicit ListBoxItem(Display *dpy);
                virtual ~ListBoxItem();
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LISTBOXITEM_H_ */
