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
                inline WidgetPtr<Menu>         *menu()                  { return &sMenu;                }
                inline const WidgetPtr<Menu>   *menu() const            { return &sMenu;                }

                inline String              *text()                      { return &sText;                }
                inline const String        *text() const                { return &sText;                }

                inline MenuItemType        *type()                      { return &sType;                }
                inline const MenuItemType  *type() const                { return &sType;                }

                inline Boolean             *checked()                   { return &sChecked;             }
                inline const Boolean       *checked() const             { return &sChecked;             }

                inline Color               *bg_selected_color()         { return &sBgSelectedColor;     }
                inline const Color         *bg_selected_color() const   { return &sBgSelectedColor;     }

                inline Color               *text_color()                { return &sTextColor;           }
                inline const Color         *text_color() const          { return &sTextColor;           }

                inline Color               *text_selected_color()       { return &sTextSelectedColor;   }
                inline const Color         *text_selected_color() const { return &sTextSelectedColor;   }

                inline Color               *check_color()               { return &sCheckColor;          }
                inline const Color         *check_color() const         { return &sCheckColor;          }

                inline Color               *check_bg_color()            { return &sCheckBgColor;        }
                inline const Color         *check_bg_color() const      { return &sCheckBgColor;        }

                inline Color               *check_border_color()        { return &sCheckBorderColor;    }
                inline const Color         *check_border_color() const  { return &sCheckBorderColor;    }

                inline Shortcut            *shortcut()                  { return &sShortcut;            }
                inline const Shortcut      *shortcut() const            { return &sShortcut;            }

            public:
                virtual status_t            on_submit();

                virtual status_t            on_focus_in(const ws::event_t *e);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_MENUITEM_H_ */
