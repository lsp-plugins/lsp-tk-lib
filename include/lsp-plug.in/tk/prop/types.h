/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 7 мая 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_TYPES_H_
#define LSP_PLUG_IN_TK_PROP_TYPES_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

// Property definitions
#define LSP_TK_PROP_LANGUAGE            "language"
#define LSP_TK_PROP_DEFAULT_LANGUAGE    "default"

#define LSP_TK_PROPERTY(type, name, expr)    \
    inline type         *name()         { return (expr); } \
    inline const type   *name() const   { return (expr); }

#define LSP_TK_IPROPERTY(type, name, expr, count)    \
    inline type         *name(size_t index)         { return (index < count) ? (expr) : NULL; } \
    inline const type   *name(size_t index) const   { return (index < count) ? (expr) : NULL; }

namespace lsp
{
    namespace tk
    {
        enum property_type_t
        {
            PT_INT,         // Integer property
            PT_FLOAT,       // Floating-point property
            PT_BOOL,        // Boolean property
            PT_STRING,      // String (text) property

            PT_UNKNOWN  = -1
        };

        namespace prop
        {
            typedef struct desc_t
            {
                const char         *postfix;
                property_type_t     type;
            } desc_t;

            typedef struct enum_t
            {
                const char         *name;
                ssize_t             value;
            } enum_t;
        }

        // Forward class declaration
        class Atoms;
        class Widget;
        class Style;
        class IStyleListener;
        class Widget;
    }
}

#endif /* LSP_PLUG_IN_TK_PROP_TYPES_H_ */
