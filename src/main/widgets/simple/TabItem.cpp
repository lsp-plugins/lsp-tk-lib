/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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
                style::TabItemColors *c = &vColors[style::TABITEM_NORMAL];
                c->sColor.bind("color", this);
                c->sBorderColor.bind("border.color", this);
                c->sTextColor.bind("text.color", this);

                c = &vColors[style::TABITEM_SELECTED];
                c->sColor.bind("selected.color", this);
                c->sBorderColor.bind("border.selected.color", this);
                c->sTextColor.bind("text.selected.color", this);

                c = &vColors[style::TABITEM_HOVER];
                c->sColor.bind("hover.color", this);
                c->sBorderColor.bind("border.hover.color", this);
                c->sTextColor.bind("text.hover.color", this);

                c = &vColors[style::TABITEM_SELECTED | style::TABITEM_HOVER];
                c->sColor.bind("selected.hover.color", this);
                c->sBorderColor.bind("border.selected.hover.color", this);
                c->sTextColor.bind("text.selected.hover.color", this);

                c = &vColors[style::TABITEM_INACTIVE];
                c->sColor.bind("inactive.color", this);
                c->sBorderColor.bind("inactive.border.color", this);
                c->sTextColor.bind("inactive.text.color", this);

                c = &vColors[style::TABITEM_SELECTED | style::TABITEM_INACTIVE];
                c->sColor.bind("inactive.selected.color", this);
                c->sBorderColor.bind("inactive.border.selected.color", this);
                c->sTextColor.bind("inactive.text.selected.color", this);

                c = &vColors[style::TABITEM_HOVER | style::TABITEM_INACTIVE];
                c->sColor.bind("inactive.hover.color", this);
                c->sBorderColor.bind("inactive.border.hover.color", this);
                c->sTextColor.bind("inactive.text.hover.color", this);

                c = &vColors[style::TABITEM_SELECTED | style::TABITEM_HOVER | style::TABITEM_INACTIVE];
                c->sColor.bind("inactive.selected.hover.color", this);
                c->sBorderColor.bind("inactive.border.selected.hover.color", this);
                c->sTextColor.bind("inactive.text.selected.hover.color", this);

                sTextAdjust.bind("text.adjust", this);
                sTextLayout.bind("text.layout", this);
                sTextPadding.bind("text.padding", this);
                sFont.bind("font", this);
                sBorderSize.bind("border.size", this);
                sBorderRadius.bind("border.radius", this);
                sActive.bind("active", this);

                // Configure
                c = &vColors[style::TABITEM_NORMAL];
                c->sColor.set("#cccccc");
                c->sBorderColor.set("#888888");
                c->sTextColor.set("#888888");

                c = &vColors[style::TABITEM_SELECTED];
                c->sColor.set("#ffffff");
                c->sBorderColor.set("#000000");
                c->sTextColor.set("#000000");

                c = &vColors[style::TABITEM_HOVER];
                c->sColor.set("#00ccff");
                c->sBorderColor.set("#eeeeee");
                c->sTextColor.set("#eeeeee");

                c = &vColors[style::TABITEM_SELECTED | style::TABITEM_HOVER];
                c->sColor.set("#ffffff");
                c->sBorderColor.set("#000000");
                c->sTextColor.set("#000000");

                c = &vColors[style::TABITEM_INACTIVE];
                c->sColor.set("#888888");
                c->sBorderColor.set("#444444");
                c->sTextColor.set("#cccccc");

                c = &vColors[style::TABITEM_SELECTED | style::TABITEM_INACTIVE];
                c->sColor.set("#888888");
                c->sBorderColor.set("#444444");
                c->sTextColor.set("#cccccc");

                c = &vColors[style::TABITEM_HOVER | style::TABITEM_INACTIVE];
                c->sColor.set("#888888");
                c->sBorderColor.set("#444444");
                c->sTextColor.set("#cccccc");

                c = &vColors[style::TABITEM_SELECTED | style::TABITEM_HOVER | style::TABITEM_INACTIVE];
                c->sColor.set("#888888");
                c->sBorderColor.set("#444444");
                c->sTextColor.set("#cccccc");

                sTextAdjust.set(TA_NONE);
                sTextLayout.set(-1.0f, 0.0f);
                sTextPadding.set_all(2);
                sFont.set_size(12.0f);
                sBorderSize.set(1);
                sBorderRadius.set(4);
                sActive.set(true);
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(TabItem, "TabItem", "Tab");

            void TabItemColors::listener(tk::prop::Listener *listener)
            {
                sColor.listener(listener);
                sBorderColor.listener(listener);
                sTextColor.listener(listener);
            }

            bool TabItemColors::property_changed(Property *prop)
            {
                return prop->one_of(sColor, sBorderColor, sTextColor);
            }
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
            sBorderSize(&sProperties),
            sBorderRadius(&sProperties),
            sActive(&sProperties)
        {
            pClass      = &metadata;

            for (size_t i=0; i<style::TABITEM_TOTAL; ++i)
                vColors[i].listener(&sProperties);

            pWidget     = NULL;
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

            style::TabItemColors *c = &vColors[style::TABITEM_NORMAL];
            c->sColor.bind("color", &sStyle);
            c->sBorderColor.bind("border.color", &sStyle);
            c->sTextColor.bind("text.color", &sStyle);

            c = &vColors[style::TABITEM_SELECTED];
            c->sColor.bind("selected.color", &sStyle);
            c->sBorderColor.bind("border.selected.color", &sStyle);
            c->sTextColor.bind("text.selected.color", &sStyle);

            c = &vColors[style::TABITEM_HOVER];
            c->sColor.bind("hover.color", &sStyle);
            c->sBorderColor.bind("border.hover.color", &sStyle);
            c->sTextColor.bind("text.hover.color", &sStyle);

            c = &vColors[style::TABITEM_SELECTED | style::TABITEM_HOVER];
            c->sColor.bind("selected.hover.color", &sStyle);
            c->sBorderColor.bind("border.selected.hover.color", &sStyle);
            c->sTextColor.bind("text.selected.hover.color", &sStyle);

            c = &vColors[style::TABITEM_INACTIVE];
            c->sColor.bind("inactive.color", &sStyle);
            c->sBorderColor.bind("inactive.border.color", &sStyle);
            c->sTextColor.bind("inactive.text.color", &sStyle);

            c = &vColors[style::TABITEM_SELECTED | style::TABITEM_INACTIVE];
            c->sColor.bind("inactive.selected.color", &sStyle);
            c->sBorderColor.bind("inactive.border.selected.color", &sStyle);
            c->sTextColor.bind("inactive.text.selected.color", &sStyle);

            c = &vColors[style::TABITEM_HOVER | style::TABITEM_INACTIVE];
            c->sColor.bind("inactive.hover.color", &sStyle);
            c->sBorderColor.bind("inactive.border.hover.color", &sStyle);
            c->sTextColor.bind("inactive.text.hover.color", &sStyle);

            c = &vColors[style::TABITEM_SELECTED | style::TABITEM_HOVER | style::TABITEM_INACTIVE];
            c->sColor.bind("inactive.selected.hover.color", &sStyle);
            c->sBorderColor.bind("inactive.border.selected.hover.color", &sStyle);
            c->sTextColor.bind("inactive.text.selected.hover.color", &sStyle);

            sText.bind(&sStyle, pDisplay->dictionary());
            sTextAdjust.bind("text.adjust", &sStyle);
            sTextLayout.bind("text.layout", &sStyle);
            sTextPadding.bind("text.padding", &sStyle);
            sFont.bind("font", &sStyle);
            sBorderSize.bind("border.size", &sStyle);
            sBorderRadius.bind("border.radius", &sStyle);
            sActive.bind("active", &sStyle);

            return STATUS_OK;
        }

        style::TabItemColors *TabItem::select_colors(bool selected, bool hover)
        {
            size_t flags = (sActive.get()) ? style::TABITEM_NORMAL : style::TABITEM_INACTIVE;
            if (selected)
                flags          |= style::TABITEM_SELECTED;
            if (hover)
                flags          |= style::TABITEM_HOVER;

            return &vColors[flags];
        }

        void TabItem::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            // Self properties
            for (size_t i=0; i<style::TABITEM_TOTAL; ++i)
                if (vColors[i].property_changed(prop))
                {
                    query_draw();
                    break;
                }

            if (sActive.is(prop))
                set_active(sActive.get());

            if (prop->one_of(sText, sTextAdjust, sTextLayout, sTextPadding, sFont, sBorderSize, sBorderRadius))
                query_resize();
        }

    } /* namespace tk */
} /* namespace lsp */



