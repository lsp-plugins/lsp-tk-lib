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
            Widget(dpy),
            sText(&sProperties),
            sBgSelectedColor(&sProperties),
            sTextColor(&sProperties),
            sTextSelectedColor(&sProperties)
        {
            pClass = &metadata;
        }
        
        ListBoxItem::~ListBoxItem()
        {
        }
    
        status_t ListBoxItem::init()
        {
            status_t res = Widget::init();
            if (res != STATUS_OK)
                return res;

            sText.bind(&sStyle, pDisplay->dictionary());
            sBgSelectedColor.bind("bg.selected.color", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sTextSelectedColor.bind("text.selected.color", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sBgSelectedColor.init(sclass, "#00ccff");
                sTextColor.init(sclass, "#000000");
                sTextSelectedColor.init(sclass, "#ffffff");

                // Override
                sPadding.override(sclass, 2, 2, 0, 0);
                sBgColor.override(sclass, "#ffffff");
            }

            return res;
        }

        void ListBoxItem::property_changed(Property *prop)
        {
            if (sText.is(prop))
                query_resize();
            if (sBgSelectedColor.is(prop))
                query_draw();
            if (sTextColor.is(prop))
                query_draw();
            if (sTextSelectedColor.is(prop))
                query_draw();
        }
    } /* namespace tk */
} /* namespace lsp */
