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

#ifndef LSP_PLUG_IN_TK_WIDGETS_COMPOUND_MENU_H_
#define LSP_PLUG_IN_TK_WIDGETS_COMPOUND_MENU_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class MenuItem;

        class Menu: public WidgetContainer
        {
            public:
                static const w_class_t    metadata;

            protected:
                friend class MenuItem;

            protected:
                typedef struct item_t
                {
                    MenuItem           *item;       // Menu item
                    padding_t           pad;        // Padding
                    ws::rectangle_t     area;       // Overall area allocated to menu item
                    ws::rectangle_t     check;      // Check box/radio area
                    ws::rectangle_t     text;       // Menu text area
                    ws::rectangle_t     scut;       // Shortcut text area
                    ws::rectangle_t     ref;        // Reference to submenu area
                } item_t;

                typedef struct istats_t
                {
                    ssize_t             full_w;     // Full width
                    ssize_t             full_h;     // Full height
                    ssize_t             item_w;     // Maximum item width
                    ssize_t             item_h;     // Maximum item height
                    ssize_t             check_w;    // Check box width
                    ssize_t             check_h;    // Check box height
                    ssize_t             scut_w;     // Shortcut width
                    ssize_t             scut_h;     // Shortcut height
                    ssize_t             link_w;     // Link width
                    ssize_t             link_h;     // Link height
                    size_t              items;      // Number of items
                    size_t              separators; // Number of separators
                    ssize_t             max_scroll; // Maximum scroll
                    bool                ckbox;      // at least one check box is present
                    bool                shortcut;   // at least one shortcut is present
                    bool                submenu;    // at least one submenu is present
                } istats_t;

                enum selection_t
                {
                    SEL_NONE            = -3,
                    SEL_TOP_SCROLL      = -2,
                    SEL_BOTTOM_SCROLL   = -1
                };

                class Window: public PopupWindow
                {
                    public:
                        static const w_class_t    metadata;

                    private:
                        Menu   *pMenu;

                    public:
                        explicit Window(Display *dpy, Menu *menu);

                    protected:
                        virtual Widget     *sync_mouse_handler(const ws::event_t *e);
                        virtual Widget     *acquire_mouse_handler(const ws::event_t *e);
                        virtual Widget     *release_mouse_handler(const ws::event_t *e);

                    public:
                        virtual status_t    handle_event(const ws::event_t *e);

                        virtual bool        take_focus();
                };

                class MenuScroll: public Widget
                {
                    public:
                        static const w_class_t    metadata;

                    private:
                        Menu       *pMenu;
                        ssize_t     nDirection;
                        bool        bActive;

                    public:
                        explicit MenuScroll(Display *dpy, Menu *menu, ssize_t dir);

                    public:
                        virtual status_t        on_mouse_in(const ws::event_t *e);
                        virtual status_t        on_mouse_out(const ws::event_t *e);
                        virtual status_t        on_focus_out(const ws::event_t *e);

                        bool                    active() const;
                };

            protected:
                lltl::parray<MenuItem>  vItems;
                lltl::darray<item_t>    vVisible;       // List of visible items

                ssize_t                 nSelected;      // Selected menu item
                ssize_t                 nKeyScroll;     // Key scroll direction
                ssize_t                 nMouseScroll;   // Mouse scroll direction
                Menu                   *pParentMenu;    // Parent menu
                Menu                   *pChildMenu;     // Child menu
                Menu                   *pKeyboardMenu;  // Keyboard menu handler
                istats_t                sIStats;        // Realized statistics
                Window                  sWindow;        // Associated popup window
                MenuScroll              sUp;            // Up-scroll button
                MenuScroll              sDown;          // Down-scroll button

                Timer                   sKeyTimer;      // Key scroll timer
                Timer                   sMouseTimer;    // Mouse scroll timer

                prop::Font              sFont;
                prop::Float             sScrolling;
                prop::Integer           sBorderSize;
                prop::Color             sBorderColor;
                prop::Color             sScrollColor;
                prop::Color             sScrollSelectedColor;
                prop::Color             sScrollTextColor;
                prop::Color             sScrollTextSelectedColor;
                prop::Integer           sCheckSize;
                prop::Integer           sCheckBorder;
                prop::Integer           sCheckBorderGap;
                prop::Integer           sCheckBorderRadius;
                prop::Integer           sSeparatorWidth;
                prop::Integer           sSpacing;
                prop::WidgetPtr<Menu>   sSubmenu;       // Sub-menu

            protected:
                static status_t             key_scroll_handler(ws::timestamp_t sched, ws::timestamp_t time, void *arg);
                static status_t             mouse_scroll_handler(ws::timestamp_t sched, ws::timestamp_t time, void *arg);

            protected:
                void                        allocate_items(lltl::darray<item_t> *out, istats_t *stats);

                status_t                    start_mouse_scroll(ssize_t dir);
                status_t                    end_mouse_scroll();

                void                        do_destroy();
                void                        hide_nested_menus(Menu *parent);
                void                        show_submenu(Menu *parent, Widget *w);
                Menu                       *root_menu();
                Menu                       *find_menu(const ws::event_t *ev, ws::rectangle_t *xr);

            protected:
                virtual void                property_changed(Property *prop);

                virtual void                size_request(ws::size_limit_t *r);

                virtual void                realize(const ws::rectangle_t *r);

                virtual void                show_widget();

                virtual void                hide_widget();

                virtual void                select_menu_item(MenuItem *item, bool popup);

                virtual void                select_menu_item(ssize_t index, bool popup);

                virtual void                select_first_item(bool popup);

                virtual void                submit_menu_item(MenuItem *item, bool focus);

                virtual void                sync_scroll(MenuItem *item);

                virtual status_t            on_key_scroll(ssize_t dir);

                virtual status_t            on_mouse_scroll(ssize_t dir);

            public:
                explicit Menu(Display *dpy);
                virtual ~Menu();

                virtual status_t            init();
                virtual void                destroy();

            public:
                LSP_TK_PROPERTY(Font,               font,                       &sFont)
                LSP_TK_PROPERTY(Integer,            spacing,                    &sSpacing)
                LSP_TK_PROPERTY(Float,              scrolling,                  &sScrolling)
                LSP_TK_PROPERTY(Integer,            border_size,                &sBorderSize)
                LSP_TK_PROPERTY(Color,              border_color,               &sBorderColor)
                LSP_TK_PROPERTY(Color,              scroll_selected_color,      &sScrollSelectedColor)
                LSP_TK_PROPERTY(Color,              scroll_text_color,          &sScrollTextColor)
                LSP_TK_PROPERTY(Color,              scroll_text_selected_color, &sScrollTextSelectedColor)
                LSP_TK_PROPERTY(Integer,            check_size,                 &sCheckSize)
                LSP_TK_PROPERTY(Integer,            check_border,               &sCheckBorder)
                LSP_TK_PROPERTY(Integer,            check_border_gap,           &sCheckBorderGap)
                LSP_TK_PROPERTY(Integer,            check_border_radius,        &sCheckBorderRadius)
                LSP_TK_PROPERTY(Integer,            separator_width,            &sSeparatorWidth)

                LSP_TK_PROPERTY(Rectangle,          trigger_area,               sWindow.trigger_area())
                LSP_TK_PROPERTY(Integer,            trigger_screen,             sWindow.trigger_screen())
                LSP_TK_PROPERTY(WidgetPtr<Widget>,  trigger_widget,             sWindow.trigger_widget())

            public:
                virtual Widget             *find_widget(ssize_t x, ssize_t y);

                virtual status_t            add(Widget *child);

                virtual status_t            insert(Widget *child, size_t index);

                virtual status_t            remove(Widget *child);

                virtual Widget             *get(size_t index);

                virtual void                show();
                virtual void                show(Widget *w);
                virtual void                showxy(Widget *w);
                virtual void                show(Widget *w, ssize_t x, ssize_t y);
                virtual void                show(Widget *w, ssize_t x, ssize_t y, ssize_t xw, ssize_t xh);
                virtual void                show(Widget *w, const ws::rectangle_t *r);

                virtual void                draw(ws::ISurface *s);

                virtual status_t            on_key_down(const ws::event_t *e);

                virtual status_t            on_key_up(const ws::event_t *e);
        };
    
        STYLE_INITIALIZER_DEF(Menu, WidgetContainer);
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_COMPOUND_MENU_H_ */
