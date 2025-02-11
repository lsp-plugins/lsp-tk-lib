/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 18 сент. 2017 г.
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
#include <lsp-plug.in/common/debug.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(MenuItem, Widget)
                // Bind
                style::MenuItemColors *c = &vColors[style::MENUITEM_NORMAL];
                c->sBgSelectedColor.bind("bg.selected.color", this);
                c->sTextColor.bind("text.color", this);
                c->sTextSelectedColor.bind("text.selected.color", this);
                c->sCheckColor.bind("check.color", this);
                c->sCheckBgColor.bind("check.bg.color", this);
                c->sCheckBorderColor.bind("check.border.color", this);

                c = &vColors[style::MENUITEM_INACTIVE];
                c->sBgSelectedColor.bind("inactive.bg.selected.color", this);
                c->sTextColor.bind("inactive.text.color", this);
                c->sTextSelectedColor.bind("inactive.text.selected.color", this);
                c->sCheckColor.bind("inactive.check.color", this);
                c->sCheckBgColor.bind("inactive.check.bg.color", this);
                c->sCheckBorderColor.bind("inactive.check.border.color", this);

                sTextAdjust.bind("text.adjust", this);
                sType.bind("type", this);
                sChecked.bind("checked", this);
                sActive.bind("active", this);
                sShortcut.bind("shortcut", this);

                // Configure
                c = &vColors[style::MENUITEM_NORMAL];
                c->sBgSelectedColor.set("#000088");
                c->sTextColor.set("#000000");
                c->sTextSelectedColor.set("#ffffff");
                c->sCheckColor.set("#00ccff");
                c->sCheckBgColor.set("#ffffff");
                c->sCheckBorderColor.set("#000000");

                c = &vColors[style::MENUITEM_INACTIVE];
                c->sBgSelectedColor.set("#888888");
                c->sTextColor.set("#444444");
                c->sTextSelectedColor.set("#cccccc");
                c->sCheckColor.set("#888888");
                c->sCheckBgColor.set("#cccccc");
                c->sCheckBorderColor.set("#000000");

                sTextAdjust.set(TA_NONE);
                sType.set(MI_NORMAL);
                sChecked.set(false);
                sActive.set(true);
                sShortcut.clear();

                // Override
                sPadding.set(16, 16, 2, 2);

                // Commit
                sPadding.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(MenuItem, "MenuItem", "root");

            void MenuItemColors::listener(tk::prop::Listener *listener)
            {
                sBgSelectedColor.listener(listener);
                sTextColor.listener(listener);
                sTextSelectedColor.listener(listener);
                sCheckColor.listener(listener);
                sCheckBgColor.listener(listener);
                sCheckBorderColor.listener(listener);
            }

            bool MenuItemColors::property_changed(Property *prop)
            {
                return prop->one_of(
                    sBgSelectedColor, sTextColor, sTextSelectedColor, sCheckColor,
                    sCheckBgColor, sCheckBorderColor);
            }
        }

        const w_class_t MenuItem::metadata      = { "MenuItem", &Widget::metadata };

        MenuItem::MenuItem(Display *dpy):
            Widget(dpy),
            sMenu(&sProperties),
            sText(&sProperties),
            sTextAdjust(&sProperties),
            sType(&sProperties),
            sChecked(&sProperties),
            sActive(&sProperties),
            sShortcut(&sProperties)
        {
            pClass      = &metadata;

            for (size_t i=0; i<style::MENUITEM_TOTAL; ++i)
                vColors[i].listener(&sProperties);
        }
        
        MenuItem::~MenuItem()
        {
            nFlags     |= FINALIZED;
        }

        void MenuItem::destroy()
        {
            nFlags     |= FINALIZED;
            Widget::destroy();
        }

        status_t MenuItem::slot_on_submit(Widget *sender, void *ptr, void *data)
        {
            MenuItem *_this = widget_ptrcast<MenuItem>(ptr);
            return (ptr != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }

        status_t MenuItem::init()
        {
            status_t res    = Widget::init();
            if (res != STATUS_OK)
                return res;

            style::MenuItemColors *c = &vColors[style::MENUITEM_NORMAL];
            c->sBgSelectedColor.bind("bg.selected.color", &sStyle);
            c->sTextColor.bind("text.color", &sStyle);
            c->sTextSelectedColor.bind("text.selected.color", &sStyle);
            c->sCheckColor.bind("check.color", &sStyle);
            c->sCheckBgColor.bind("check.bg.color", &sStyle);
            c->sCheckBorderColor.bind("check.border.color", &sStyle);

            c = &vColors[style::MENUITEM_INACTIVE];
            c->sBgSelectedColor.bind("inactive.bg.selected.color", &sStyle);
            c->sTextColor.bind("inactive.text.color", &sStyle);
            c->sTextSelectedColor.bind("inactive.text.selected.color", &sStyle);
            c->sCheckColor.bind("inactive.check.color", &sStyle);
            c->sCheckBgColor.bind("inactive.check.bg.color", &sStyle);
            c->sCheckBorderColor.bind("inactive.check.border.color", &sStyle);

            sTextAdjust.bind("text.adjust", &sStyle);
            sText.bind(&sStyle, pDisplay->dictionary());
            sType.bind("type", &sStyle);
            sChecked.bind("checked", &sStyle);
            sActive.bind("active", &sStyle);
            sShortcut.bind("shortcut", &sStyle);
            sMenu.bind(NULL);

            handler_id_t id = sSlots.add(SLOT_SUBMIT, slot_on_submit, self());

            return (id >= 0) ? STATUS_OK : -id;
        }

        style::MenuItemColors *MenuItem::select_colors()
        {
            size_t flags = (sActive.get()) ? style::MENUITEM_NORMAL : style::MENUITEM_INACTIVE;
            return &vColors[flags];
        }

        void MenuItem::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            // Self properties
            style::MenuItemColors *colors = select_colors();
            if (colors->property_changed(prop))
                query_draw();

            if (sActive.is(prop))
            {
                query_draw();

                // Call parent menu for redraw
                tk::Menu *parent = widget_cast<tk::Menu>(this->parent());
                if (parent != NULL)
                    parent->query_draw(REDRAW_CHILD | REDRAW_SURFACE);
            }

            if (prop->one_of(sChecked))
                query_draw();
            if (prop->one_of(sTextAdjust, sText, sType))
                query_resize();
        }

        status_t MenuItem::on_submit()
        {
            return STATUS_OK;
        }

        status_t MenuItem::on_focus_in(const ws::event_t *e)
        {
            Menu *m = widget_cast<Menu>(parent());
//            lsp_trace("this = %p", this);
            if (m != NULL)
                m->select_menu_item(this, false);
            return STATUS_OK;
        }

        status_t MenuItem::on_mouse_in(const ws::event_t *e)
        {
            Menu *m = widget_cast<Menu>(parent());
//            lsp_trace("this = %p", this);
            if (m != NULL)
                m->select_menu_item(this, true);
            return STATUS_OK;
        }

        status_t MenuItem::on_mouse_up(const ws::event_t *e)
        {
            // Allow only left button click
//            lsp_trace("this=%p", this);
            if ((e->nCode != ws::MCB_LEFT) || ((e->nState & ws::MCF_BTN_MASK) != ws::MCF_LEFT))
                return STATUS_OK;

            Menu *m = widget_cast<Menu>(parent());
//            lsp_trace("parent=%p", m);

            if (m != NULL)
                m->submit_menu_item(this, false);

            sSlots.execute(SLOT_SUBMIT, this);

            return STATUS_OK;
        }
    
    } /* namespace tk */
} /* namespace lsp */
