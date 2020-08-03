/*
 * MenuItem.h
 *
 *  Created on: 18 сент. 2017 г.
 *      Author: sadko
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

        class MenuItem: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                friend class Menu;

            protected:
                prop::WidgetPtr<Menu>       sMenu;
                prop::String                sText;
                prop::MenuItemType          sType;
                prop::Boolean               sChecked;
                prop::Color                 sBgSelectedColor;
                prop::Color                 sTextColor;
                prop::Color                 sTextSelectedColor;
                prop::Color                 sCheckColor;
                prop::Color                 sCheckBgColor;
                prop::Color                 sCheckBorderColor;
                prop::Color                 sCheckBorderGapColor;
                prop::Shortcut              sShortcut;

            protected:
                static status_t             slot_on_submit(Widget *sender, void *ptr, void *data);

            protected:
                virtual void                property_changed(Property *prop);

            public:
                explicit MenuItem(Display *dpy);
                virtual ~MenuItem();

                virtual status_t            init();

            public:
                LSP_TK_PROPERTY(WidgetPtr<Menu>,            menu,                       &sMenu)
                LSP_TK_PROPERTY(String,                     text,                       &sText)
                LSP_TK_PROPERTY(MenuItemType,               type,                       &sType)
                LSP_TK_PROPERTY(Boolean,                    checked,                    &sChecked)
                LSP_TK_PROPERTY(Color,                      bg_selected_color,          &sBgSelectedColor)
                LSP_TK_PROPERTY(Color,                      text_color,                 &sTextColor)
                LSP_TK_PROPERTY(Color,                      text_selected_color,        &sTextSelectedColor)
                LSP_TK_PROPERTY(Color,                      check_color,                &sCheckColor)
                LSP_TK_PROPERTY(Color,                      check_bg_color,             &sCheckBgColor)
                LSP_TK_PROPERTY(Color,                      check_border_color,         &sCheckBorderColor)
                LSP_TK_PROPERTY(Shortcut,                   shortcut,                   &sShortcut)

            public:
                virtual status_t            on_submit();

                virtual status_t            on_focus_in(const ws::event_t *e);

                virtual status_t            on_mouse_in(const ws::event_t *e);

                virtual status_t            on_mouse_up(const ws::event_t *e);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_MENUITEM_H_ */
