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
            SCROLL_NONE,        //!< SCROLL_NONE no scrolling permitted, the nested widget fully fits the area
            SCROLL_CLIP,        //!< SCROLL_CLIP no scrolling permitted, nested widget is clipped
            SCROLL_OPTIONAL,    //!< SCROLL_OPTIONAL scrolling is permitted but only when necessary
            SCROLL_ALWAYS       //!< SCROLL_ALWAYS scrolling is always enabled
        };

        enum window_policy_t
        {
            WP_NORMAL,
            WP_GREEDY,
            WP_CHILD
        };

        enum button_mode_t
        {
            BM_NORMAL,          //!< Simple push button
            BM_TOGGLE,          //!< Push/Pull button
            BM_TRIGGER          //!< Push button that acts as a trigger
        };

        enum allocation_t
        {
            WA_HFILL    = 1 << 0,
            WA_VFILL    = 1 << 1,
            WA_HALIGN   = 1 << 2,
            WA_VALIGN   = 1 << 3
        };

        /**
         * Key modifiers for shortcuts
         */
        enum key_modifier_t
        {
            KM_LCTRL    = 1 << 0,
            KM_RCTRL    = 1 << 1,
            KM_LALT     = 1 << 2,
            KM_RALT     = 1 << 3,
            KM_LSHIFT   = 1 << 4,
            KM_RSHIFT   = 1 << 5,
            KM_LMETA    = 1 << 6,
            KM_RMETA    = 1 << 7,
            KM_LSUPER   = 1 << 8,
            KM_RSUPER   = 1 << 9,
            KM_LHYPER   = 1 << 10,
            KM_RHYPER   = 1 << 11,

            KM_CTRL     = KM_LCTRL | KM_RCTRL,
            KM_ALT      = KM_LALT | KM_RALT,
            KM_SHIFT    = KM_LSHIFT | KM_RSHIFT,
            KM_META     = KM_LMETA | KM_RMETA,
            KM_SUPER    = KM_LSUPER | KM_RSUPER,
            KM_HYPER    = KM_LHYPER | KM_RHYPER,

            KM_ALL      = KM_CTRL | KM_ALT | KM_SHIFT | KM_META | KM_SUPER | KM_HYPER
        };

        /**
         * Menu item type
         */
        enum menu_item_type_t
        {
            MI_NORMAL,      //!< MI_NORMAL normal menu, with text
            MI_SEPARATOR,   //!< MI_SEPARATOR separator
            MI_CHECK,       //!< MI_CHECK check box style
            MI_RADIO        //!< MI_RADIO radio button style
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
