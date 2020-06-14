/*
 * MenuItemType.h
 *
 *  Created on: 14 июн. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_ENUM_MENUITEMTYPE_H_
#define LSP_PLUG_IN_TK_PROP_ENUM_MENUITEMTYPE_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class MenuItemType: public Enum
        {
            private:
                MenuItemType & operator = (const MenuItemType &);

            protected:
                static const prop::enum_t ENUM[];

            protected:
                explicit MenuItemType(prop::Listener *listener = NULL): Enum(ENUM, listener) {};

            public:
                inline menu_item_type_t     get() const                 { return menu_item_type_t(nValue);          }
                inline menu_item_type_t     set(menu_item_type_t v)     { return menu_item_type_t(Enum::set(v));    }

                inline bool                 normal() const              { return nValue == MI_NORMAL;               }
                inline bool                 check() const               { return nValue == MI_CHECK;                }
                inline bool                 radio() const               { return nValue == MI_RADIO;                }
                inline bool                 separator() const           { return nValue == MI_SEPARATOR;            }

                inline menu_item_type_t     set_normal()                { return set(MI_NORMAL);                    }
                inline menu_item_type_t     set_check()                 { return set(MI_CHECK);                     }
                inline menu_item_type_t     set_radio()                 { return set(MI_RADIO);                     }
                inline menu_item_type_t     set_separator()             { return set(MI_SEPARATOR);                 }
        };

        namespace prop
        {
            class MenuItemType: public tk::MenuItemType
            {
                private:
                    MenuItemType & operator = (const MenuItemType &);

                public:
                    explicit MenuItemType(prop::Listener *listener = NULL): tk::MenuItemType(listener) {};

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::MenuItemType::bind(property, style); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::MenuItemType::bind(property, style); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::MenuItemType::bind(property, style); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::MenuItemType::unbind(); };

                    /**
                     * Initialize default value
                     * @param style style
                     * @param o default orientation
                     * @return status of operation
                     */
                    inline status_t     init(Style *style, menu_item_type_t t)          { return tk::MenuItemType::init(style, t);      };
            };
        }
    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_PROP_ENUM_MENUITEMTYPE_H_ */
