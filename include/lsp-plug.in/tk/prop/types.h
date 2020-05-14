/*
 * types.h
 *
 *  Created on: 7 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_TYPES_H_
#define LSP_PLUG_IN_TK_PROP_TYPES_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

// Property definitions
#define LSP_TK_PROP_LANGUAGE        "language"

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
    }
}

#endif /* LSP_PLUG_IN_TK_PROP_TYPES_H_ */
