/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 14 июн. 2020 г.
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
            protected:
                static const prop::enum_t ENUM[];

            protected:
                explicit MenuItemType(prop::Listener *listener = NULL): Enum(ENUM, MI_NORMAL, listener) {};
                MenuItemType(const MenuItemType &) = delete;
                MenuItemType(MenuItemType &&) = delete;
                MenuItemType & operator = (const MenuItemType &) = delete;
                MenuItemType & operator = (MenuItemType &&) = delete;

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
                public:
                    explicit MenuItemType(prop::Listener *listener = NULL): tk::MenuItemType(listener) {};
                    MenuItemType(const MenuItemType &) = delete;
                    MenuItemType(MenuItemType &&) = delete;
                    MenuItemType & operator = (const MenuItemType &) = delete;
                    MenuItemType & operator = (MenuItemType &&) = delete;

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

                    inline void         listener(prop::Listener *listener)              { pListener = listener;                     }
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_PROP_ENUM_MENUITEMTYPE_H_ */
