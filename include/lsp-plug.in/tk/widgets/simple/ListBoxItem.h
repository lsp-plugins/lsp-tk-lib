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

            protected:
                prop::String                sText;
                prop::Color                 sBgSelectedColor;
                prop::Color                 sTextColor;
                prop::Color                 sTextSelectedColor;

            public:
                explicit ListBoxItem(Display *dpy);
                virtual ~ListBoxItem();

            public:
                LSP_TK_PROPERTY(String,                     text,                       &sText)
                LSP_TK_PROPERTY(Color,                      bg_selected_color,          &sBgSelectedColor)
                LSP_TK_PROPERTY(Color,                      text_color,                 &sTextColor)
                LSP_TK_PROPERTY(Color,                      text_selected_color,        &sTextSelectedColor)
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LISTBOXITEM_H_ */
