/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 30 июл. 2020 г.
 *
 * lsp-tk-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-tk-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-tk-lib. If not, see <https://www.gnu.org/licenses/>.
 */

#include <lsp-plug.in/tk/tk.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(ListBoxItem, Widget)
                // Bind
                sTextAdjust.bind("text.adjust", this);
                sBgSelectedColor.bind("bg.selected.color", this);
                sTextColor.bind("text.color", this);
                sTextSelectedColor.bind("text.selected.color", this);
                // Configure
                sTextAdjust.set(TA_NONE);
                sBgSelectedColor.set("#00ccff");
                sTextColor.set("#000000");
                sTextSelectedColor.set("#ffffff");
                // Override
                sPadding.set(2, 2, 0, 0);
                sBgColor.set("#ffffff");
                // Commit
                sPadding.override();
                sBgColor.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(ListBoxItem, "ListBoxItem", "root");
        }

        const w_class_t ListBoxItem::metadata       = { "ListBoxItem", &Widget::metadata };

        ListBoxItem::ListBoxItem(Display *dpy):
            Widget(dpy),
            sText(&sProperties),
            sTextAdjust(&sProperties),
            sBgSelectedColor(&sProperties),
            sTextColor(&sProperties),
            sTextSelectedColor(&sProperties)
        {
            pClass = &metadata;
        }
        
        ListBoxItem::~ListBoxItem()
        {
            nFlags     |= FINALIZED;
        }
    
        status_t ListBoxItem::init()
        {
            status_t res = Widget::init();
            if (res != STATUS_OK)
                return res;

            sTextAdjust.bind("text.adjust", &sStyle);
            sText.bind(&sStyle, pDisplay->dictionary());
            sBgSelectedColor.bind("bg.selected.color", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sTextSelectedColor.bind("text.selected.color", &sStyle);

            return res;
        }

        void ListBoxItem::property_changed(Property *prop)
        {
            if (sText.is(prop))
                query_resize();
            if (sTextAdjust.is(prop))
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
