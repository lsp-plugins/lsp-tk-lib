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
//                class MenuWindow: public LSPWindow
//                {
//                    protected:
//                        LSPMenu *pMenu;
//
//                    protected:
//                        virtual LSPWidget    *find_widget(ssize_t x, ssize_t y);
//
//                        LSPMenu             *get_handler(ws_event_t *e);
//
//                    public:
//                        MenuWindow(LSPDisplay *dpy, LSPMenu *menu, size_t screen);
//                        virtual ~MenuWindow();
//
//                    public:
//                        virtual void render(ISurface *s, bool force);
//
//                        virtual void size_request(size_request_t *r);
//
//                        virtual status_t on_mouse_down(const ws_event_t *e);
//
//                        virtual status_t on_mouse_up(const ws_event_t *e);
//
//                        virtual status_t on_mouse_scroll(const ws_event_t *e);
//
//                        virtual status_t on_mouse_move(const ws_event_t *e);
//                };

                enum selection_t
                {
                    SEL_NONE            = -3,
                    SEL_TOP_SCROLL      = -2,
                    SEL_BOTTOM_SCROLL   = -1
                };

            protected:
                lltl::parray<MenuItem>  vItems;

                prop::Font              sFont;
                prop::Position          sPosition;
                prop::Integer           sSpacing;
                prop::Integer           sScrolling;
                prop::Integer           sBorder;
                prop::Color             sBorderColor;
                prop::Integer           sCheckSize;
                prop::Integer           sCheckBorder;
                prop::Integer           sCheckBorderGap;
                prop::Integer           sCheckBorderRadius;


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
                ssize_t                     find_item(ssize_t x, ssize_t y, ssize_t *ry);
                void                        update_scroll();
                void                        selection_changed(ssize_t sel, ssize_t ry);
                Menu                       *check_inside_submenu(ws::event_t *e);

                void                        do_destroy();

            protected:
                virtual void                property_changed(Property *prop);

                virtual void                size_request(ws::size_limit_t *r);

                virtual void                realize(const ws::rectangle_t *r);

            public:
                explicit Menu(Display *dpy);
                virtual ~Menu();

                virtual status_t            init();
                virtual void                destroy();

            public:
                inline Font                *font()                      { return &sFont;                    }
                inline const Font          *font() const                { return &sFont;                    }

                inline Position            *position()                  { return &sPosition;                }
                inline const Position      *position() const            { return &sPosition;                }

                inline Integer             *spacing()                   { return &sSpacing;                 }
                inline const Integer       *spacing() const             { return &sSpacing;                 }

                inline Integer             *scrolling()                 { return &sScrolling;               }
                inline const Integer       *scrolling() const           { return &sScrolling;               }

                inline Integer             *border()                    { return &sBorder;                  }
                inline const Integer       *border() const              { return &sBorder;                  }

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

            public:
                virtual status_t    add(Widget *child);

                virtual status_t    insert(Widget *child, size_t index);

                virtual status_t    remove(Widget *child);

                virtual Widget     *get(size_t index);

                virtual void        draw(ws::ISurface *s);

                virtual void        query_resize();

                virtual bool        hide();

                virtual bool        show();

                virtual bool        show(size_t screen);

                virtual bool        show(size_t screen, ssize_t left, ssize_t top);

                virtual bool        show(Widget *w, size_t screen, ssize_t left, ssize_t top);

                virtual bool        show(Widget *w);

                virtual bool        show(Widget *w, ssize_t left, ssize_t top);

                virtual bool        show(Widget *w, const ws::event_t *ev);

                virtual status_t    on_mouse_down(const ws::event_t *e);

                virtual status_t    on_mouse_up(const ws::event_t *e);

                virtual status_t    on_mouse_move(const ws::event_t *e);

                virtual status_t    on_mouse_scroll(const ws::event_t *e);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_MENU_H_ */
