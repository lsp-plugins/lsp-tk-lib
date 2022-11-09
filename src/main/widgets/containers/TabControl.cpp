/*
 * Copyright (C) 2022 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2022 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 8 нояб. 2022 г.
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
            LSP_TK_STYLE_IMPL_BEGIN(TabControl, WidgetContainer)
                // Bind
                sActiveTabFont.bind("font", this);
                sInactiveTabFont.bind("font.inactive", this);
                sActiveTabTextAdjust.bind("text.adjust", this);
                sInactiveTabTextAdjust.bind("text.adjust.inactive", this);
                sBorderColor.bind("border.color", this);
                sActiveTabColor.bind("tab.color", this);
                sInactiveTabColor.bind("tab.color.inactive", this);
                sActiveTabBorderColor.bind("tab.border.color", this);
                sInactiveTabBorderColor.bind("tab.border.color.inactive", this);
                sActiveTabTextColor.bind("tab.text.color", this);
                sInactiveTabTextColor.bind("tab.text.color.inactive", this);
                sActiveTabTextPadding.bind("tab.text.padding", this);
                sInactiveTabTextColor.bind("tab.text.padding.inactive", this);
                sBorderSize.bind("border.size", this);
                sActiveTabBorderSize.bind("tab.border.size", this);
                sInactiveTabBorderSize.bind("tab.border.size.inactive", this);
                sBorderRadius.bind("border.radius", this);
                sActiveTabRadius.bind("tab.radius", this);
                sInactiveTabRadius.bind("tab.radius.inactive", this);
                sEmbedding.bind("embed", this);
                sLayout.bind("layout", this);
                sHeading.bind("heading", this);
                sSizeConstraints.bind("size.constraints", this);

                // Configure
                sActiveTabFont.set_size(12.0f);
                sInactiveTabFont.set_size(12.0f);
                sActiveTabTextAdjust.set(TA_NONE);
                sInactiveTabTextAdjust.set(TA_NONE);
                sBorderColor.set("#888888");
                sActiveTabColor.set("#888888");
                sInactiveTabColor.set("#ffffff");
                sActiveTabBorderColor.set("#888888");
                sInactiveTabBorderColor.set("#000000");
                sActiveTabTextColor.set("#ffffff");
                sInactiveTabTextColor.set("#000000");
                sActiveTabTextPadding.set_all(0);
                sInactiveTabTextPadding.set_all(0);
                sBorderSize.set(2);
                sActiveTabBorderSize.set(1);
                sInactiveTabBorderSize.set(1);
                sBorderRadius.set(10);
                sActiveTabRadius.set(6);
                sInactiveTabRadius.set(6);
                sEmbedding.set(false);
                sLayout.set(0.0f, 0.0f, 1.0f, 1.0f);
                sHeading.set(-1.0f, -1.0f, 0.0f, 0.0f);
                sSizeConstraints.set_all(-1);
            LSP_TK_STYLE_IMPL_END

            LSP_TK_BUILTIN_STYLE(TabControl, "TabControl", "root");
        } /* namepsace style */

        //-----------------------------------------------------------------------------
        // TabControl implementation
        const w_class_t TabControl::metadata        = { "TabControl", &WidgetContainer::metadata };

        TabControl::TabControl(Display *dpy):
            WidgetContainer(dpy),
            sActiveTabFont(&sProperties),
            sInactiveTabFont(&sProperties),
            sActiveTabTextAdjust(&sProperties),
            sInactiveTabTextAdjust(&sProperties),
            sBorderColor(&sProperties),
            sActiveTabColor(&sProperties),
            sInactiveTabColor(&sProperties),
            sActiveTabBorderColor(&sProperties),
            sInactiveTabBorderColor(&sProperties),
            sActiveTabTextColor(&sProperties),
            sInactiveTabTextColor(&sProperties),
            sActiveTabTextPadding(&sProperties),
            sInactiveTabTextPadding(&sProperties),
            sBorderSize(&sProperties),
            sActiveTabBorderSize(&sProperties),
            sInactiveTabBorderSize(&sProperties),
            sBorderRadius(&sProperties),
            sActiveTabRadius(&sProperties),
            sInactiveTabRadius(&sProperties),
            sEmbedding(&sProperties),
            sLayout(&sProperties),
            sHeading(&sProperties),
            sSizeConstraints(&sProperties),
            vWidgets(&sProperties, &sIListener),
            sSelected(&sProperties)
        {
            sArea.nLeft         = 0;
            sArea.nTop          = 0;
            sArea.nWidth        = 0;
            sArea.nHeight       = 0;

            nMBState            = 0;
            pEventTab           = NULL;

            pClass              = &metadata;
        }

        TabControl::~TabControl()
        {
            nFlags     |= FINALIZED;
        }

        status_t TabControl::init()
        {
            // Initialize widgets
            status_t result = WidgetContainer::init();
            if (result != STATUS_OK)
                return result;

            sIListener.bind_all(this, on_add_widget, on_remove_widget);

            // Configure Window
            sActiveTabFont.bind("font", &sStyle);
            sInactiveTabFont.bind("font.inactive", &sStyle);
            sActiveTabTextAdjust.bind("text.adjust", &sStyle);
            sInactiveTabTextAdjust.bind("text.adjust.inactive", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sActiveTabColor.bind("tab.color", &sStyle);
            sInactiveTabColor.bind("tab.color.inactive", &sStyle);
            sActiveTabBorderColor.bind("tab.border.color", &sStyle);
            sInactiveTabBorderColor.bind("tab.border.color.inactive", &sStyle);
            sActiveTabTextColor.bind("tab.text.color", &sStyle);
            sInactiveTabTextColor.bind("tab.text.color.inactive", &sStyle);
            sActiveTabTextPadding.bind("tab.text.padding", &sStyle);
            sInactiveTabTextColor.bind("tab.text.padding.inactive", &sStyle);
            sBorderSize.bind("border.size", &sStyle);
            sActiveTabBorderSize.bind("tab.border.size", &sStyle);
            sInactiveTabBorderSize.bind("tab.border.size.inactive", &sStyle);
            sBorderRadius.bind("border.radius", &sStyle);
            sActiveTabRadius.bind("tab.radius", &sStyle);
            sInactiveTabRadius.bind("tab.radius.inactive", &sStyle);
            sEmbedding.bind("embed", &sStyle);
            sLayout.bind("layout", &sStyle);
            sHeading.bind("heading", &sStyle);
            sSizeConstraints.bind("size.constraints", &sStyle);

            // Bind slots
            handler_id_t id;
            id = sSlots.add(SLOT_CHANGE, slot_on_change, self());
            if (id >= 0) id = sSlots.add(SLOT_SUBMIT, slot_on_change, self());
            if (id < 0)
                return -id;

            return STATUS_OK;
        }

        void TabControl::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);

            if (prop->one_of(sActiveTabFont, sInactiveTabFont,
                sActiveTabTextAdjust, sInactiveTabTextAdjust))
                query_resize();
            if (prop->one_of(sBorderColor, sActiveTabColor, sInactiveTabColor,
                sActiveTabBorderColor, sInactiveTabBorderColor,
                sActiveTabTextColor, sInactiveTabTextColor))
                query_draw();
            if (prop->one_of(sActiveTabTextPadding, sInactiveTabTextPadding))
                query_resize();
            if (prop->one_of(sBorderSize, sActiveTabBorderSize, sInactiveTabBorderSize,
                sBorderRadius, sActiveTabRadius, sInactiveTabRadius))
                query_resize();
            if (prop->one_of(sEmbedding, sLayout, sHeading, sSizeConstraints))
                query_resize();
            if (vWidgets.is(prop))
                query_resize();
            if (sSelected.is(prop))
                query_resize();
        }

        void TabControl::allocate(alloc_t *alloc)
        {
            // TODO
        }

        void TabControl::size_request(ws::size_limit_t *r)
        {
            // TODO
        }

        void TabControl::realize(const ws::rectangle_t *r)
        {
            // TODO
        }

        void TabControl::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
        }

        status_t TabControl::add(Widget *child)
        {
            Tab *tab = widget_cast<Tab>(child);
            if (tab == NULL)
                return STATUS_BAD_TYPE;

            return vWidgets.add(tab);
        }

        status_t TabControl::remove(Widget *child)
        {
            Tab *tab = widget_cast<Tab>(child);
            if (tab == NULL)
                return STATUS_BAD_TYPE;

            return vWidgets.premove(tab);
        }

        status_t TabControl::remove_all()
        {
            vWidgets.clear();
            return STATUS_OK;
        }

        status_t TabControl::on_change()
        {
            return STATUS_OK;
        }

        status_t TabControl::on_submit()
        {
            return STATUS_OK;
        }

        Widget *TabControl::current_widget()
        {
            Widget *it      = sSelected.get();
            if (it == NULL)
                return  vWidgets.get(0);
            return (vWidgets.contains(it)) ? it : NULL;
        }

        tk::Tab *TabControl::find_tab(ssize_t x, ssize_t y)
        {
            float scaling           = lsp_max(0.0f, sScaling.get());

            // Check that mouse pointer is inside of the tab control
            ssize_t border_radius   = lsp_max(0.0f, sBorderRadius.get() * scaling);
            if (!Position::rinside(&sArea, x, y, border_radius))
                return NULL;

            size_t mask                 = 0;
            ssize_t active_tab_radius   = lsp_max(0.0f, sActiveTabRadius.get() * scaling);
            ssize_t inactive_tab_radius = lsp_max(0.0f, sInactiveTabRadius.get() * scaling);
            if ((active_tab_radius > 0) || (inactive_tab_radius > 0))
            {
                if (sHeading.halign() <= 0)
                    mask                =  (sHeading.valign() <= 0) ? SURFMASK_RB_CORNER : SURFMASK_RT_CORNER;
                else
                    mask                =  (sHeading.valign() <= 0) ? SURFMASK_LB_CORNER : SURFMASK_LT_CORNER;
            }

            for (size_t i=0, n=vTabs.size(); i<n; ++i)
            {
                ws::rectangle_t *r  = vTabs.uget(i);
                tk::Tab *w          = vWidgets.get(i);
                if ((w == NULL) || (!w->visibility()->get()))
                    continue;
                // TODO
            }
            return NULL;
        }

        bool TabControl::scroll_item(ssize_t increment)
        {
            // TODO
            return false;
        }

        status_t TabControl::on_mouse_down(const ws::event_t *e)
        {
            if (nMBState == 0)
                pEventTab   = find_tab(e->nLeft, e->nTop);
            nMBState       |= 1 << e->nCode;

            return STATUS_OK;
        }

        Widget *TabControl::find_widget(ssize_t x, ssize_t y)
        {
            Widget *widget  = current_widget();
            if (widget == NULL)
                return NULL;
            if ((widget->is_visible_child_of(this)) && (widget->inside(x, y)))
                return widget;

            return NULL;
        }

        status_t TabControl::on_mouse_up(const ws::event_t *e)
        {
            size_t mask     = 1 << e->nCode;
            size_t prev     = nMBState;
            nMBState       &= (~mask);

            if (prev == mask)
            {
                if ((e->nCode == ws::MCB_LEFT) && (pEventTab != NULL))
                {
                    tk::Tab *found = find_tab(e->nLeft, e->nTop);
                    if (found == pEventTab)
                        sSelected.set(found);
                }
            }

            if (nMBState == 0)
                pEventTab       = NULL;

            return STATUS_OK;
        }

        status_t TabControl::on_mouse_move(const ws::event_t *e)
        {
            return STATUS_OK;
        }

        status_t TabControl::on_mouse_scroll(const ws::event_t *e)
        {
            if (nMBState != 0)
                return STATUS_OK;

            tk::Widget *found = find_tab(e->nLeft, e->nTop);
            if (found != NULL)
            {
                if (e->nCode == ws::MCD_UP)
                {
                    if (scroll_item(-1))
                        sSlots.execute(SLOT_SUBMIT, this, NULL);
                }
                else if (e->nCode == ws::MCD_DOWN)
                {
                    if (scroll_item(1))
                        sSlots.execute(SLOT_SUBMIT, this, NULL);
                }
            }

            return STATUS_OK;
        }

        status_t TabControl::on_key_down(const ws::event_t *e)
        {
            switch (e->nCode)
            {
                case ws::WSK_LEFT:
                case ws::WSK_KEYPAD_LEFT:
                    if (scroll_item(-1))
                        sSlots.execute(SLOT_SUBMIT, this, NULL);
                    break;

                case ws::WSK_RIGHT:
                case ws::WSK_KEYPAD_RIGHT:
                    if (scroll_item(1))
                        sSlots.execute(SLOT_SUBMIT, this, NULL);
                    break;

                default:
                    break;
            }

            return STATUS_OK;
        }

        void TabControl::on_add_widget(void *obj, Property *prop, void *w)
        {
            Widget *item = widget_ptrcast<Widget>(w);
            if (item == NULL)
                return;

            TabControl *_this = widget_ptrcast<TabControl>(obj);
            if (_this == NULL)
                return;

            item->set_parent(_this);
            _this->query_resize();
        }

        void TabControl::on_remove_widget(void *obj, Property *prop, void *w)
        {
            Widget *item = widget_ptrcast<Widget>(w);
            if (item == NULL)
                return;

            TabControl *_this = widget_ptrcast<TabControl>(obj);
            if (_this == NULL)
                return;

            // Reset active widget if present
            if (_this->sSelected.get() == item)
                _this->sSelected.set(NULL);
            if (_this->pEventTab == item)
                _this->pEventTab       = NULL;

            _this->unlink_widget(item);
            _this->query_resize();
        }

        status_t TabControl::slot_on_change(Widget *sender, void *ptr, void *data)
        {
            TabControl *_this = widget_ptrcast<TabControl>(ptr);
            return (_this != NULL) ? _this->on_change() : STATUS_BAD_ARGUMENTS;
        }

        status_t TabControl::slot_on_submit(Widget *sender, void *ptr, void *data)
        {
            TabControl *_this = widget_ptrcast<TabControl>(ptr);
            return (_this != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }

    } /* namespace tk */
} /* namespace lsp */
