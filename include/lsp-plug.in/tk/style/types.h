/*
 * types.h
 *
 *  Created on: 13 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_STYLE_TYPES_H_
#define LSP_PLUG_IN_TK_STYLE_TYPES_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Style;
        class Schema;

        /**
         * Style initialization function
         * @param style style to initialize
         * @param schema schema associated with style
         * @return status of operation
         */
        typedef status_t (* style_init_t)(Style *style, Schema *schema);
    }
}

#endif /* LSP_PLUG_IN_TK_STYLE_TYPES_H_ */
