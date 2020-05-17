/*
 * types.h
 *
 *  Created on: 5 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_TYPES_H_
#define LSP_PLUG_IN_TK_TYPES_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /** Widget orientation
         *
         */
        enum orientation_t
        {
            O_HORIZONTAL,       //!< O_HORIZONTAL horizontal orientation
            O_VERTICAL          //!< O_VERTICAL vertical orientation
        };

        enum scrolling_t
        {
            SCROLL_NONE,        //!< SCROLL_NONE no scrolling permitted
            SCROLL_OPTIONAL,    //!< SCROLL_OPTIONAL scrolling is permitted but in optional state
            SCROLL_ALWAYS       //!< SCROLL_ALWAYS scrolling is permitted but in optional state
        };

        enum window_policy_t
        {
            WP_NORMAL,
            WP_GREEDY,
            WP_CHILD
        };

        enum allocation_t
        {
            WA_HFILL    = 1 << 0,
            WA_VFILL    = 1 << 1,
            WA_HALIGN   = 1 << 2,
            WA_VALIGN   = 1 << 3
        };

        typedef struct w_class_t
        {
            const char         *name;
            const w_class_t    *parent;
        } w_class_t;

        typedef struct padding_t
        {
            size_t              nLeft;
            size_t              nRight;
            size_t              nTop;
            size_t              nBottom;
        } padding_t;

        /**
         * Atom identifier
         */
        typedef ssize_t             atom_t;
    }
}

#endif /* LSP_PLUG_IN_TK_TYPES_H_ */
