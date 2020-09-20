/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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

namespace lsp
{
    namespace tk
    {
        const w_class_t MenuItem::metadata      = { "MenuItem", &Widget::metadata };

        MenuItem::MenuItem(Display *dpy):
            Widget(dpy),
            sMenu(&sProperties),
            sText(&sProperties),
            sType(&sProperties),
            sChecked(&sProperties),
            sBgSelectedColor(&sProperties),
            sTextColor(&sProperties),
            sTextSelectedColor(&sProperties),
            sCheckColor(&sProperties),
            sCheckBgColor(&sProperties),
            sCheckBorderColor(&sProperties),
            sShortcut(&sProperties)
        {
            pClass      = &metadata;
        }
        
        MenuItem::~MenuItem()
        {
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

            sText.bind(&sStyle, pDisplay->dictionary());
            sType.bind("type", &sStyle);
            sChecked.bind("checked", &sStyle);
            sBgSelectedColor.bind("bg.selected.color", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sTextSelectedColor.bind("text.selected.color", &sStyle);
            sCheckColor.bind("check.color", &sStyle);
            sCheckBgColor.bind("check.bg.color", &sStyle);
            sCheckBorderColor.bind("check.border.color", &sStyle);
            sShortcut.bind("shortcut", &sStyle);
            sMenu.bind(NULL);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sType.init(sclass, MI_NORMAL);
                sChecked.init(sclass, false);

                sBgSelectedColor.init(sclass, "#000088");
                sTextColor.init(sclass, "#000000");
                sTextSelectedColor.init(sclass, "#ffffff");
                sCheckColor.init(sclass, "#00ccff");
                sCheckBgColor.init(sclass, "#ffffff");
                sCheckBorderColor.init(sclass, "#000000");
                sShortcut.init(sclass);

                // Override
                sPadding.override(sclass, 16, 16, 2, 2);
            }

            handler_id_t id = sSlots.add(SLOT_SUBMIT, slot_on_submit, self());

            return (id >= 0) ? STATUS_OK : -id;
        }

        void MenuItem::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (sText.is(prop))
                query_resize();
            if (sType.is(prop))
                query_resize();
            if (sChecked.is(prop))
                query_draw();
        }

        status_t MenuItem::on_submit()
        {
            return STATUS_OK;
        }

        status_t MenuItem::on_focus_in(const ws::event_t *e)
        {
            Menu *m = widget_cast<Menu>(parent());
            lsp_trace("this = %p", this);
            if (m != NULL)
                m->select_menu_item(this, false);
            return STATUS_OK;
        }

        status_t MenuItem::on_mouse_in(const ws::event_t *e)
        {
            Menu *m = widget_cast<Menu>(parent());
            lsp_trace("this = %p", this);
            if (m != NULL)
                m->select_menu_item(this, true);
            return STATUS_OK;
        }

        status_t MenuItem::on_mouse_up(const ws::event_t *e)
        {
            // Allow only left button click
            lsp_trace("this=%p", this);
            if ((e->nCode != ws::MCB_LEFT) || ((e->nState & ws::MCF_BTN_MASK) != ws::MCF_LEFT))
                return STATUS_OK;

            Menu *m = widget_cast<Menu>(parent());
            lsp_trace("parent=%p", m);

            if (m != NULL)
                m->submit_menu_item(this, false);

            sSlots.execute(SLOT_SUBMIT, this);

            return STATUS_OK;
        }
    
    } /* namespace tk */
} /* namespace lsp */
