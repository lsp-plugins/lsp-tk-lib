/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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
                style::ListBoxItemColors *c = &vColors[style::LISTBOXITEM_NORMAL];
                c->sBgColor.bind("bg.color", this);
                c->sTextColor.bind("text.color", this);

                c = &vColors[style::LISTBOXITEM_SELECTED];
                c->sBgColor.bind("bg.selected.color", this);
                c->sTextColor.bind("text.selected.color", this);

                c = &vColors[style::LISTBOXITEM_HOVER];
                c->sBgColor.bind("bg.hover.color", this);
                c->sTextColor.bind("text.hover.color", this);

                c = &vColors[style::LISTBOXITEM_SELECTED | style::LISTBOXITEM_HOVER];
                c->sBgColor.bind("bg.selected.hover.color", this);
                c->sTextColor.bind("text.selected.hover.color", this);

                c = &vColors[style::LISTBOXITEM_INACTIVE];
                c->sBgColor.bind("inactive.bg.color", this);
                c->sTextColor.bind("inactive.text.color", this);

                c = &vColors[style::LISTBOXITEM_SELECTED | style::LISTBOXITEM_INACTIVE];
                c->sBgColor.bind("inactive.bg.selected.color", this);
                c->sTextColor.bind("inactive.text.selected.color", this);

                c = &vColors[style::LISTBOXITEM_HOVER | style::LISTBOXITEM_INACTIVE];
                c->sBgColor.bind("inactive.bg.hover.color", this);
                c->sTextColor.bind("inactive.text.hover.color", this);

                c = &vColors[style::LISTBOXITEM_SELECTED | style::LISTBOXITEM_HOVER | style::LISTBOXITEM_INACTIVE];
                c->sBgColor.bind("inactive.bg.selected.hover.color", this);
                c->sTextColor.bind("inactive.text.selected.hover.color", this);

                sTextAdjust.bind("text.adjust", this);
                sActive.bind("active", this);

                // Configure
                c = &vColors[style::LISTBOXITEM_NORMAL];
                c->sTextColor.set("#000000");

                c = &vColors[style::LISTBOXITEM_SELECTED];
                c->sBgColor.set("#00ccff");
                c->sTextColor.set("#ffffff");

                c = &vColors[style::LISTBOXITEM_HOVER];
                c->sBgColor.set("#00aaee");
                c->sTextColor.set("#eeeeee");

                c = &vColors[style::LISTBOXITEM_SELECTED | style::LISTBOXITEM_HOVER];
                c->sBgColor.set("#00aaee");
                c->sTextColor.set("#eeeeee");

                c = &vColors[style::LISTBOXITEM_INACTIVE];
                c->sBgColor.set("#cccccc");
                c->sTextColor.set("#444444");

                c = &vColors[style::LISTBOXITEM_SELECTED | style::LISTBOXITEM_INACTIVE];
                c->sBgColor.set("#cccccc");
                c->sTextColor.set("#444444");

                c = &vColors[style::LISTBOXITEM_HOVER | style::LISTBOXITEM_INACTIVE];
                c->sBgColor.set("#cccccc");
                c->sTextColor.set("#444444");

                c = &vColors[style::LISTBOXITEM_SELECTED | style::LISTBOXITEM_HOVER | style::LISTBOXITEM_INACTIVE];
                c->sBgColor.set("#cccccc");
                c->sTextColor.set("#444444");

                sTextAdjust.set(TA_NONE);
                sActive.set(true);

                // Override
                sPadding.set(2, 2, 0, 0);
                sBgColor.set("#ffffff");

                // Commit
                sPadding.override();
                sBgColor.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(ListBoxItem, "ListBoxItem", "root");

            void ListBoxItemColors::listener(tk::prop::Listener *listener)
            {
                sBgColor.listener(listener);
                sTextColor.listener(listener);
            }

            bool ListBoxItemColors::property_changed(Property *prop)
            {
                return prop->one_of(sBgColor, sTextColor);
            }
        }

        const w_class_t ListBoxItem::metadata       = { "ListBoxItem", &Widget::metadata };

        ListBoxItem::ListBoxItem(Display *dpy):
            Widget(dpy),
            sText(&sProperties),
            sTextAdjust(&sProperties),
            sActive(&sProperties)
        {
            pClass = &metadata;

            for (size_t i=0; i<style::LISTBOXITEM_TOTAL; ++i)
                vColors[i].listener(&sProperties);
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

            style::ListBoxItemColors *c = &vColors[style::LISTBOXITEM_NORMAL];
            c->sBgColor.bind("bg.color", &sStyle);
            c->sTextColor.bind("text.color", &sStyle);

            c = &vColors[style::LISTBOXITEM_SELECTED];
            c->sBgColor.bind("bg.selected.color", &sStyle);
            c->sTextColor.bind("text.selected.color", &sStyle);

            c = &vColors[style::LISTBOXITEM_HOVER];
            c->sBgColor.bind("bg.hover.color", &sStyle);
            c->sTextColor.bind("text.hover.color", &sStyle);

            c = &vColors[style::LISTBOXITEM_SELECTED | style::LISTBOXITEM_HOVER];
            c->sBgColor.bind("bg.selected.hover.color", &sStyle);
            c->sTextColor.bind("text.selected.hover.color", &sStyle);

            c = &vColors[style::LISTBOXITEM_INACTIVE];
            c->sBgColor.bind("inactive.bg.color", &sStyle);
            c->sTextColor.bind("inactive.text.color", &sStyle);

            c = &vColors[style::LISTBOXITEM_SELECTED | style::LISTBOXITEM_INACTIVE];
            c->sBgColor.bind("inactive.bg.selected.color", &sStyle);
            c->sTextColor.bind("inactive.text.selected.color", &sStyle);

            c = &vColors[style::LISTBOXITEM_HOVER | style::LISTBOXITEM_INACTIVE];
            c->sBgColor.bind("inactive.bg.hover.color", &sStyle);
            c->sTextColor.bind("inactive.text.hover.color", &sStyle);

            c = &vColors[style::LISTBOXITEM_SELECTED | style::LISTBOXITEM_HOVER | style::LISTBOXITEM_INACTIVE];
            c->sBgColor.bind("inactive.bg.selected.hover.color", &sStyle);
            c->sTextColor.bind("inactive.text.selected.hover.color", &sStyle);

            sTextAdjust.bind("text.adjust", &sStyle);
            sText.bind(&sStyle, pDisplay->dictionary());
            sActive.bind("active", &sStyle);

            return res;
        }

        style::ListBoxItemColors *ListBoxItem::select_colors(bool selected, bool hover)
        {
            size_t flags = (sActive.get()) ? style::TABITEM_NORMAL : style::TABITEM_INACTIVE;
            if (selected)
                flags          |= style::TABITEM_SELECTED;
            if (hover)
                flags          |= style::TABITEM_HOVER;

            return &vColors[flags];
        }

        void ListBoxItem::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            // Self properties
            for (size_t i=0; i<style::LISTBOXITEM_TOTAL; ++i)
                if (vColors[i].property_changed(prop))
                {
                    query_draw();
                    ListBox *list = widget_cast<ListBox>(parent());
                    if (list != NULL)
                        list->query_draw(REDRAW_CHILD | REDRAW_SURFACE);

                    break;
                }

            if (sActive.is(prop))
            {
                query_draw();
                ListBox *list = widget_cast<ListBox>(parent());
                if (list != NULL)
                    list->query_draw(REDRAW_CHILD | REDRAW_SURFACE);
            }

            if (prop->one_of(sText, sTextAdjust))
                query_resize();
        }
    } /* namespace tk */
} /* namespace lsp */
