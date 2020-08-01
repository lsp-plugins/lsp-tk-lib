/*
 * ListBoxItem.cpp
 *
 *  Created on: 30 июл. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t ListBoxItem::metadata       = { "ListBoxItem", &Widget::metadata };

        ListBoxItem::ListBoxItem(Display *dpy):
            Widget(dpy)
        {
            pClass = &metadata;
        }
        
        ListBoxItem::~ListBoxItem()
        {
        }
    
    } /* namespace tk */
} /* namespace lsp */
