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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_MENUITEM_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_MENUITEM_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Menu;

        // Style definition
        namespace style
        {
            typedef struct MenuItemColors
            {
                prop::Color                 sBgSelectedColor;
                prop::Color                 sTextColor;
                prop::Color                 sTextSelectedColor;
                prop::Color                 sCheckColor;
                prop::Color                 sCheckBgColor;
                prop::Color                 sCheckBorderColor;

                void listener(tk::prop::Listener *listener);
                bool property_changed(Property *prop);
            } MenuItemColors;

            enum MenuItemColorState
            {
                MENUITEM_NORMAL         = 0,
                MENUITEM_INACTIVE       = 1 << 0,

                MENUITEM_TOTAL          = 1 << 1
            };

            LSP_TK_STYLE_DEF_BEGIN(MenuItem, Widget)
                MenuItemColors              vColors[MENUITEM_TOTAL];

                prop::String                sText;
                prop::TextAdjust            sTextAdjust;
                prop::MenuItemType          sType;
                prop::Boolean               sChecked;
                prop::Shortcut              sShortcut;
            LSP_TK_STYLE_DEF_END
        }

        class MenuItem: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                friend class Menu;

                enum mitm_flags_t
                {
                    MITM_0          = style::MENUITEM_NORMAL,
                    MITM_1          = style::MENUITEM_INACTIVE,
                    MITM_TOTAL      = style::MENUITEM_TOTAL
                };

            protected:
                style::MenuItemColors       vColors[style::MENUITEM_TOTAL];
                prop::WidgetPtr<Menu>       sMenu;
                prop::String                sText;
                prop::TextAdjust            sTextAdjust;
                prop::MenuItemType          sType;
                prop::Boolean               sChecked;
                prop::Shortcut              sShortcut;

            protected:
                static status_t             slot_on_submit(Widget *sender, void *ptr, void *data);

            protected:
                style::MenuItemColors      *select_colors();

            protected:
                virtual void                property_changed(Property *prop) override;

            public:
                explicit MenuItem(Display *dpy);
                MenuItem(const MenuItem &) = delete;
                MenuItem(MenuItem &&) = delete;
                virtual ~MenuItem() override;
                MenuItem & operator = (const MenuItem &) = delete;
                MenuItem & operator = (MenuItem &&) = delete;

                virtual status_t            init() override;
                virtual void                destroy() override;

            public:
                LSP_TK_PROPERTY(Color,                  bg_selected_color,              &vColors[MITM_0].sBgSelectedColor)
                LSP_TK_PROPERTY(Color,                  text_color,                     &vColors[MITM_0].sTextColor)
                LSP_TK_PROPERTY(Color,                  text_selected_color,            &vColors[MITM_0].sTextSelectedColor)
                LSP_TK_PROPERTY(Color,                  check_color,                    &vColors[MITM_0].sCheckColor)
                LSP_TK_PROPERTY(Color,                  check_bg_color,                 &vColors[MITM_0].sCheckBgColor)
                LSP_TK_PROPERTY(Color,                  check_border_color,             &vColors[MITM_0].sCheckBorderColor)
                LSP_TK_PROPERTY(Color,                  inactive_bg_selected_color,     &vColors[MITM_1].sBgSelectedColor)
                LSP_TK_PROPERTY(Color,                  inactive_text_color,            &vColors[MITM_1].sTextColor)
                LSP_TK_PROPERTY(Color,                  inactive_text_selected_color,   &vColors[MITM_1].sTextSelectedColor)
                LSP_TK_PROPERTY(Color,                  inactive_check_color,           &vColors[MITM_1].sCheckColor)
                LSP_TK_PROPERTY(Color,                  inactive_check_bg_color,        &vColors[MITM_1].sCheckBgColor)
                LSP_TK_PROPERTY(Color,                  inactive_check_border_color,    &vColors[MITM_1].sCheckBorderColor)

                LSP_TK_PROPERTY(WidgetPtr<Menu>,        menu,                           &sMenu)
                LSP_TK_PROPERTY(String,                 text,                           &sText)
                LSP_TK_PROPERTY(TextAdjust,             text_adjust,                    &sTextAdjust)
                LSP_TK_PROPERTY(MenuItemType,           type,                           &sType)
                LSP_TK_PROPERTY(Boolean,                checked,                        &sChecked)
                LSP_TK_PROPERTY(Shortcut,               shortcut,                       &sShortcut)

            public:
                virtual status_t            on_focus_in(const ws::event_t *e) override;
                virtual status_t            on_mouse_in(const ws::event_t *e) override;
                virtual status_t            on_mouse_up(const ws::event_t *e) override;

            public:
                virtual status_t            on_submit();
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_MENUITEM_H_ */
