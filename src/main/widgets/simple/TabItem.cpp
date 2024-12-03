/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 4 дек. 2024 г.
 *
 * lsp-tk-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-tk-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTabItemILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
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
            LSP_TK_STYLE_IMPL_BEGIN(TabItem, Widget)
                // Bind
                sTextAdjust.bind("text.adjust", this);
                sTextLayout.bind("text.layout", this);
                sTextPadding.bind("text.padding", this);
                sFont.bind("font", this);
                sColor.bind("color", this);
                sSelectedColor.bind("selected.color", this);
                sHoverColor.bind("hover.color", this);
                sBorderColor.bind("border.color", this);
                sBorderSelectedColor.bind("border.selected.color", this);
                sBorderHoverColor.bind("border.hover.color", this);
                sTextColor.bind("text.color", this);
                sTextSelectedColor.bind("text.selected.color", this);
                sTextHoverColor.bind("text.hover.color", this);
                sBorderSize.bind("border.size", this);
                sBorderRadius.bind("border.radius", this);

                // Configure
                sTextAdjust.set(TA_NONE);
                sTextLayout.set(-1.0f, 0.0f);
                sTextPadding.set_all(2);
                sFont.set_size(12.0f);
                sColor.set("#cccccc");
                sSelectedColor.set("#ffffff");
                sHoverColor.set("#00ccff");
                sBorderColor.set("#888888");
                sBorderSelectedColor.set("#000000");
                sBorderHoverColor.set("#eeeeee");
                sTextColor.set("#888888");
                sTextSelectedColor.set("#000000");
                sTextHoverColor.set("#eeeeee");
                sBorderSize.set(1);
                sBorderRadius.set(4);
            LSP_TK_STYLE_IMPL_END

            LSP_TK_BUILTIN_STYLE(TabItem, "TabItem", "Tab");
        } /* namespace style */

        //-----------------------------------------------------------------------------
        // TabItem implementation
        const w_class_t TabItem::metadata           = { "TabItem", &Widget::metadata };

        TabItem::TabItem(Display *dpy):
            Widget(dpy),
            sText(&sProperties),
            sTextAdjust(&sProperties),
            sTextLayout(&sProperties),
            sTextPadding(&sProperties),
            sFont(&sProperties),
            sColor(&sProperties),
            sSelectedColor(&sProperties),
            sHoverColor(&sProperties),
            sBorderColor(&sProperties),
            sBorderSelectedColor(&sProperties),
            sBorderHoverColor(&sProperties),
            sTextColor(&sProperties),
            sTextSelectedColor(&sProperties),
            sTextHoverColor(&sProperties),
            sBorderSize(&sProperties),
            sBorderRadius(&sProperties)
        {
            pWidget     = NULL;

            pClass      = &metadata;
        }

        TabItem::~TabItem()
        {
            nFlags     |= FINALIZED;
        }

        status_t TabItem::init()
        {
            status_t result = Widget::init();
            if (result != STATUS_OK)
                return result;

            sText.bind(&sStyle, pDisplay->dictionary());
            sTextAdjust.bind("text.adjust", &sStyle);
            sTextLayout.bind("text.layout", &sStyle);
            sTextPadding.bind("text.padding", &sStyle);
            sFont.bind("font", &sStyle);
            sColor.bind("color", &sStyle);
            sSelectedColor.bind("selected.color", &sStyle);
            sHoverColor.bind("hover.color", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sBorderSelectedColor.bind("border.selected.color", &sStyle);
            sBorderHoverColor.bind("border.hover.color", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sTextSelectedColor.bind("text.selected.color", &sStyle);
            sTextHoverColor.bind("text.hover.color", &sStyle);
            sBorderSize.bind("border.size", &sStyle);
            sBorderRadius.bind("border.radius", &sStyle);

            return STATUS_OK;
        }

        void TabItem::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (prop->one_of(sText, sTextAdjust, sTextLayout, sTextPadding, sFont))
                query_resize();
            if (prop->one_of(sColor, sSelectedColor, sHoverColor,
                sBorderColor, sBorderSelectedColor, sBorderHoverColor,
                sTextColor, sTextSelectedColor, sTextHoverColor))
                query_draw();
            if (prop->one_of(sBorderSize, sBorderRadius))
                query_resize();
        }

    } /* namespace tk */
} /* namespace lsp */



