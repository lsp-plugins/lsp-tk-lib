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
// TODO
//                Menu                       *pMenu;

                prop::String                sText;
                prop::MenuItemType          sType;
                prop::Boolean               sChecked;

            protected:
                static status_t             slot_on_submit(Widget *sender, void *ptr, void *data);

            protected:
                virtual void                property_changed(Property *prop);

            public:
                explicit MenuItem(Display *dpy);
                virtual ~MenuItem();

                virtual status_t            init();

            public:
//                Menu                       *menu()                   { return pMenu; }
//                const Menu                 *menu() const             { return pMenu; }

                inline String              *text()                  { return &sText;        }
                inline const String        *text() const            { return &sText;        }

                inline MenuItemType        *type()                  { return &sType;        }
                inline const MenuItemType  *type() const            { return &sType;        }

                inline Boolean             *checked()               { return &sChecked;     }
                inline const Boolean       *checked() const         { return &sChecked;     }

            public:
//                status_t            set_submenu(LSPMenu *submenu);

            public:
                virtual status_t            on_submit();
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_MENUITEM_H_ */
