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
                typedef struct item_t
                {
                    MenuItem           *item;       // Menu item
                    ws::rectangle_t     area;       // Overall area allocated to menu item
                    ws::rectangle_t     check;      // Check box/radio area
                    ws::rectangle_t     ref;        // Reference to submenu area
                    ws::rectangle_t     scut;       // Shortcut text area
                    ws::rectangle_t     text;       // Menu text area
                } item_t;

                typedef struct isizes_t
                {
                    ssize_t             mc_width;   // menu caption width
                    ssize_t             mc_height;  // menu caption height
                    ssize_t             cb_width;   // check box / radio button width
                    ssize_t             cb_height;  // check box / radio button height
                    ssize_t             mr_width;   // menu reference width
                    ssize_t             mr_height;  // menu reference height
                    ssize_t             sc_width;   // shortcut width
                    ssize_t             sc_height;  // shortcut height
                    ssize_t             sp_width;   // separator width
                    ssize_t             sp_height;  // separator height
                    ssize_t             sp_thick;   // separator thickness

                    size_t              items;      // number of visible items
                    size_t              separators; // number of visible separators
                    ssize_t             width;      // overall width
                    ssize_t             s_height;   // separator height
                    ssize_t             m_height;   // menu item height
                    ssize_t             m_lpad;
                    ssize_t             m_rpad;
                    ssize_t             hspacing;   // horizontal spacing
                    ssize_t             vspacing;   // vertical spacing
                    ssize_t             max_scroll; // maximum scrolling (in pixels), available only after realize()
                    bool                ckbox;      // at least one check box is present
                    bool                shortcut;   // at least one shortcut is present
                    bool                submenu;    // at least one submenu is present
                } isizes_t;

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

            protected:
                lltl::parray<MenuItem>  vItems;
                lltl::darray<item_t>    vVisible;       // List of visible items

                ssize_t                 nSelected;      // Selected menu item
                PopupWindow             sWindow;        // Associated popup window
                isizes_t                sISizes;        // Realized sizes
                ibutton_t               sUp;            // Up-scroll button
                ibutton_t               sDown;          // Down-scroll button

                prop::Font              sFont;
                prop::Integer           sHSpacing;
                prop::Integer           sVSpacing;
                prop::Float             sScrolling;
                prop::Integer           sBorderSize;
                prop::Color             sBorderColor;
                prop::Integer           sCheckSize;
                prop::Integer           sCheckBorder;
                prop::Integer           sCheckBorderGap;
                prop::Integer           sCheckBorderRadius;
                prop::Integer           sSeparatorWidth;
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
                static status_t             timer_handler(ws::timestamp_t time, void *arg);

            protected:
                void                        estimate_sizes(isizes_t *sz);

                void                        allocate_items(lltl::darray<item_t> *out);

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

            public:
                explicit Menu(Display *dpy);
                virtual ~Menu();

                virtual status_t            init();
                virtual void                destroy();

            public:
                inline Font                *font()                      { return &sFont;                    }
                inline const Font          *font() const                { return &sFont;                    }

                inline Integer             *hspacing()                  { return &sHSpacing;                }
                inline const Integer       *hspacing() const            { return &sHSpacing;                }

                inline Integer             *vspacing()                  { return &sVSpacing;                }
                inline const Integer       *vspacing() const            { return &sVSpacing;                }

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
