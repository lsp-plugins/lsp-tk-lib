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
        STYLE_INITIALIZER_BEGIN(ListBoxItem, Widget);

            prop::String::init("text", style);
            prop::Color::init("bg.selected.color", style, "#00ccff");
            prop::Color::init("text.color", style, "#000000");
            prop::Color::init("text.selected.color", style, "#ffffff");
            // Overrides
            prop::Padding::override("padding", style, 2, 2, 0, 0);
            prop::Color::override("bg.color", style, "#ffffff");

        STYLE_INITIALIZER_END(ListBoxItem, "ListBoxItem");
        LSP_BUILTIN_STYLE(ListBoxItem);

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
            nFlags     |= FINALIZED;
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
