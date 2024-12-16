/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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
                sBgHoverColor.bind("bg.hover.color", this);
                sTextColor.bind("text.color", this);
                sTextSelectedColor.bind("text.selected.color", this);
                sTextHoverColor.bind("text.hover.color", this);
                // Configure
                sTextAdjust.set(TA_NONE);
                sBgSelectedColor.set("#00ccff");
                sBgHoverColor.set("#00aaee");
                sTextColor.set("#000000");
                sTextSelectedColor.set("#ffffff");
                sTextHoverColor.set("#eeeeee");
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
            sBgHoverColor.bind("bg.hover.color", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sTextSelectedColor.bind("text.selected.color", &sStyle);
            sTextHoverColor.bind("text.hover.color", &sStyle);

            return res;
        }

        void ListBoxItem::property_changed(Property *prop)
        {
            if (prop->one_of(sText, sTextAdjust))
                query_resize();
            if (prop->one_of(sBgSelectedColor, sBgHoverColor, sTextColor, sTextSelectedColor, sTextHoverColor))
                query_draw();
        }
    } /* namespace tk */
} /* namespace lsp */
