/*
 * Menu.h
 *
 *  Created on: 18 сент. 2017 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_MENU_H_
#define LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_MENU_H_

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

                typedef struct ibutton_t
                {
                    ws::rectangle_t     sPos;       // position
                    bool                bEnabled;   // button enabled
                } ibutton_t;

                enum selection_t
                {
                    SEL_NONE            = -3,
                    SEL_TOP_SCROLL      = -2,
                    SEL_BOTTOM_SCROLL   = -1
                };

                class MenuWindow: public PopupWindow
                {
                    private:
                        Menu *pMenu;

                    public:
                        explicit inline MenuWindow(Display *dpy, Menu *menu): PopupWindow(dpy) { pMenu = menu; }

                    protected:
                        virtual Widget     *sync_mouse_handler(const ws::event_t *e);
                        virtual Widget     *acquire_mouse_handler(const ws::event_t *e);
                        virtual Widget     *release_mouse_handler(const ws::event_t *e);

                    public:
                        virtual status_t    handle_event(const ws::event_t *e);
                };

            protected:
                lltl::parray<MenuItem>  vItems;
                lltl::darray<item_t>    vVisible;       // List of visible items

                ssize_t                 nSelected;      // Selected menu item
                ssize_t                 nKeyScroll;     // Key scroll direction
                MenuWindow              sWindow;        // Associated popup window
                istats_t                sIStats;        // Realized statistics
                ibutton_t               sUp;            // Up-scroll button
                ibutton_t               sDown;          // Down-scroll button

                Timer                   sKeyTimer;      // Key scroll timer

                prop::Font              sFont;
                prop::Float             sScrolling;
                prop::Integer           sBorderSize;
                prop::Color             sBorderColor;
                prop::Integer           sCheckSize;
                prop::Integer           sCheckBorder;
                prop::Integer           sCheckBorderGap;
                prop::Integer           sCheckBorderRadius;
                prop::Integer           sSeparatorWidth;
                prop::Integer           sSpacing;
                prop::WidgetPtr<Menu>   sSubmenu;           // Sub-menu

//                MenuWindow             *pWindow;
//                LSPMenu                *pParentMenu;
//                LSPMenu                *pActiveMenu;
//                LSPTimer                sScroll;
//                ssize_t                 nSelected;
//                ssize_t                 nScroll;
//                ssize_t                 nScrollMax;
//                size_t                  nMBState;
//                LSPColor                sSelColor;
//                LSPColor                sBorderColor;

            protected:
                static status_t             keyscroll_handler(ws::timestamp_t time, void *arg);

            protected:
//                void                        estimate_sizes(isizes_t *sz);

                void                        allocate_items(lltl::darray<item_t> *out, istats_t *stats);

//                ssize_t                     find_item(ssize_t x, ssize_t y, ssize_t *ry);
//                void                        update_scroll();
//                void                        selection_changed(ssize_t sel, ssize_t ry);
//                Menu                       *check_inside_submenu(ws::event_t *e);

                void                        do_destroy();

            protected:
                virtual void                property_changed(Property *prop);

                virtual void                size_request(ws::size_limit_t *r);

                virtual void                realize(const ws::rectangle_t *r);

                virtual void                show_widget();

                virtual void                hide_widget();

                virtual void                select_menu_item(MenuItem *item);

                virtual void                sync_scroll(MenuItem *item);

                virtual status_t            on_key_scroll(ssize_t dir);

            public:
                explicit Menu(Display *dpy);
                virtual ~Menu();

                virtual status_t            init();
                virtual void                destroy();

            public:
                inline Font                *font()                      { return &sFont;                    }
                inline const Font          *font() const                { return &sFont;                    }

                inline Integer             *spacing()                   { return &sSpacing;                 }
                inline const Integer       *spacing() const             { return &sSpacing;                 }

                inline Float               *scrolling()                 { return &sScrolling;               }
                inline const Float         *scrolling() const           { return &sScrolling;               }

                inline Integer             *border_size()               { return &sBorderSize;              }
                inline const Integer       *border_size() const         { return &sBorderSize;              }

                inline Color               *border_color()              { return &sBorderColor;             }
                inline const Color         *border_color() const        { return &sBorderColor;             }

                inline Integer             *check_size()                { return &sCheckSize;               }
                inline const Integer       *check_size() const          { return &sCheckSize;               }

                inline Integer             *check_border()              { return &sCheckBorder;             }
                inline const Integer       *check_border() const        { return &sCheckBorder;             }

                inline Integer             *check_border_gap()          { return &sCheckBorderGap;          }
                inline const Integer       *check_border_gap() const    { return &sCheckBorderGap;          }

                inline Integer             *check_border_radius()       { return &sCheckBorderRadius;       }
                inline const Integer       *check_border_radius() const { return &sCheckBorderRadius;       }

                inline Integer             *separator_width()           { return &sSeparatorWidth;          }
                inline const Integer       *separator_width() const     { return &sSeparatorWidth;          }

                Rectangle                  *trigger_area()              { return sWindow.trigger_area();    }
                const Rectangle            *trigger_area() const        { return sWindow.trigger_area();    }

                Integer                    *trigger_screen()            { return sWindow.trigger_screen();  }
                const Integer              *trigger_screen() const      { return sWindow.trigger_screen();  }

                WidgetPtr<Widget>          *trigger_widget()            { return sWindow.trigger_widget();  }
                const WidgetPtr<Widget>    *trigger_widget() const      { return sWindow.trigger_widget();  }

            public:
                virtual Widget             *find_widget(ssize_t x, ssize_t y);

                virtual status_t            add(Widget *child);

                virtual status_t            insert(Widget *child, size_t index);

                virtual status_t            remove(Widget *child);

                virtual Widget             *get(size_t index);

                virtual void                show();
                virtual void                show(Widget *w);
                virtual void                show(Widget *w, ssize_t x, ssize_t y);
                virtual void                show(Widget *w, ssize_t x, ssize_t y, ssize_t xw, ssize_t xh);
                virtual void                show(Widget *w, const ws::rectangle_t *r);

                virtual void                draw(ws::ISurface *s);

                virtual status_t            on_key_down(const ws::event_t *e);

                virtual status_t            on_key_up(const ws::event_t *e);
//
//                virtual status_t    on_mouse_down(const ws::event_t *e);
//
//                virtual status_t    on_mouse_up(const ws::event_t *e);
//
//                virtual status_t    on_mouse_move(const ws::event_t *e);
//
//                virtual status_t    on_mouse_scroll(const ws::event_t *e);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_MENU_H_ */
