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

#include <lsp-plug.in/stdlib/math.h>
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
                sBorderColor.bind("border.color", this);
                sBorderSize.bind("border.size", this);
                sBorderRadius.bind("border.radius", this);
                sTabSpacing.bind("tab.spacing", this);
                sEmbedding.bind("embed", this);
                sHeading.bind("heading", this);
                sSizeConstraints.bind("size.constraints", this);

                // Configure
                sBorderColor.set("#888888");
                sBorderSize.set(2);
                sBorderRadius.set(10);
                sTabSpacing.set(1);
                sEmbedding.set(false);
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
            sBorderColor(&sProperties),
            sBorderSize(&sProperties),
            sBorderRadius(&sProperties),
            sTabSpacing(&sProperties),
            sEmbedding(&sProperties),
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
            sBorderColor.bind("border.color", &sStyle);
            sBorderSize.bind("border.size", &sStyle);
            sBorderRadius.bind("border.radius", &sStyle);
            sTabSpacing.bind("tab.spacing", &sStyle);
            sEmbedding.bind("embed", &sStyle);
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

            if (sBorderColor.is(prop))
                query_draw();
            if (prop->one_of(sBorderSize, sBorderRadius, sTabSpacing))
                query_resize();
            if (prop->one_of(sEmbedding, sHeading, sSizeConstraints))
                query_resize();
            if (vWidgets.is(prop))
                query_resize();
            if (sSelected.is(prop))
                query_resize();
        }

        void TabControl::allocate_tabs(ws::rectangle_t *area, lltl::darray<tab_t> *tabs)
        {
            float scaling           = lsp_max(0.0f, sScaling.get());
            float fscaling          = lsp_max(0.0f, scaling * sFontScaling.get());
            size_t tab_spacing      = lsp_max(0.0f, sTabSpacing.get() * scaling);
            bool top_align          = sHeading.valign() <= 0.0f;

            area->nLeft             = 0;
            area->nTop              = 0;
            area->nWidth            = 0;
            area->nHeight           = 0;

            LSPString caption;

            size_t x                = 0;
            ssize_t max_h           = 0;

            // Step 1: allocate tabs
            for (size_t i=0, n=vWidgets.size(); i<n; ++i)
            {
                tk::Tab *w = vWidgets.get(i);
                if ((w == NULL) || (!w->is_visible_child_of(this)))
                    continue;

                // Create new record
                tab_t *tab              = tabs->append();
                if (tab == NULL)
                    return;

                // Obtain font properties and padding
                padding_t padding;
                ws::text_parameters_t tp;
                size_t border_rgap      = lsp_max(0.0f, w->border_radius()->get() * scaling * M_SQRT1_2);
                w->text()->format(&caption);
                w->font()->get_multitext_parameters(pDisplay, &tp, fscaling, &caption);
                w->text_padding()->compute(&padding, scaling);

                // Write parameters
                tab->widget             = w;
                tab->bounds.nLeft       = x;
                tab->bounds.nTop        = 0;
                tab->text.nWidth        = tp.Width;
                tab->text.nHeight       = tp.Height;
                tab->bounds.nWidth      = 2 * border_rgap + tab->text.nWidth + padding.nLeft + padding.nRight;
                tab->bounds.nHeight     = border_rgap + tab->text.nHeight + padding.nTop + padding.nBottom;
                tab->text.nLeft         = tab->bounds.nLeft + border_rgap + padding.nLeft;
                tab->text.nTop          = tab->bounds.nTop + padding.nTop + ((top_align) ? border_rgap : 0);

                // Update coordinates of the next tab
                max_h                   = lsp_max(max_h, tab->bounds.nHeight);
                x                      += tab->bounds.nWidth + tab_spacing;
            }

            area->nHeight           = max_h;

            // Step 2: make tabs of the same height
            for (size_t i=0, n=tabs->size(); i<n; ++i)
            {
                tab_t *tab              = tabs->uget(i);
                ssize_t dy              = max_h - tab->bounds.nHeight;
                tab->bounds.nHeight    += dy;
                tab->text.nHeight      += dy;
                area->nWidth            = tab->bounds.nLeft + tab->bounds.nWidth;
            }
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
