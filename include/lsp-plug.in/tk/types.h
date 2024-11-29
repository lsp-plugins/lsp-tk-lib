/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 5 мая 2020 г.
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

#ifndef LSP_PLUG_IN_TK_TYPES_H_
#define LSP_PLUG_IN_TK_TYPES_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

// Basic dependencies
#include <lsp-plug.in/ws/ws.h>

// The default dictionary location
#define LSP_TK_ENV_DICT_PATH            "dictionary"
#define LSP_TK_ENV_DICT_PATH_DFL        "i18n"
// The default dictionary location
#define LSP_TK_ENV_SCHEMA_PATH          "schema"
// The default language selected at startup
#define LSP_TK_ENV_LANG                 "language"
#define LSP_TK_ENV_LANG_DFL             "en"
// The default configuration path
#define LSP_TK_ENV_CONFIG               "configuration"
#define LSP_TK_ENV_CONFIG_DFL           "lsp-tk"

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

        /**
         * Tether flags
         */
        enum tether_flags_t
        {
            // Horizontal arrangement tether
            TF_TOP          = 0 << 0,   // Top line of rectangle used as tether
            TF_BOTTOM       = 1 << 0,   // Bottom line of rectangle used as tether
            // Vertical arrangement tether
            TF_LEFT         = 0 << 1,   // Left line of rectangle used as tether
            TF_RIGHT        = 1 << 1,   // Right line of rectangle used as tether
            // Priority of tether
            TF_HORIZONTAL   = 0 << 2,   // Priority of horizontal tether is higher than vertical
            TF_VERTICAL     = 1 << 2,   // Priority of vertical tether is higher than horizontal
            // Stretching
            TF_HSTRETCH     = 1 << 3,   // Stretch horizontally
            TF_VSTRETCH     = 1 << 4,   // Stretch vertically
            // Maximizing
            TF_HMAXIMIZE    = 1 << 5,   // Maximize horizontally
            TF_VMAXIMIZE    = 1 << 6,   // Maximize vertically
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
         * Widget draw mode
         */
        enum draw_mode_t
        {
            DM_CLASSIC,         //!< Classic draw mode
            DM_MODERN,          //!< Modern draw mode
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

            KM_NONE     = 0,
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

        /**
         * Function for rendering framebuffer
         */
        enum graph_frame_function_t
        {
            GFF_RAINBOW,                //!< GFF_RAINBOW
            GFF_FOG,                    //!< GFF_FOG_COLOR
            GFF_COLOR,                  //!< GFF_COLOR
            GFF_LIGHTNESS,              //!< GFF_LIGHTNESS
            GFF_LIGHTNESS2,             //!< GFF_LIGHTNESS2

            GFF_DEFAULT = GFF_RAINBOW   //!< GFF_DEFAULT default function
        };

        enum text_adjust_t
        {
            TA_NONE,                    //!< No text adjust
            TA_TOUPPER,                 //!< Convert to upper case
            TA_TOLOWER                  //!< Convert to lower case
        };

        typedef struct w_class_t
        {
            const char         *name;
            const w_class_t    *parent;
        } w_class_t;

        typedef struct padding_t
        {
            size_t              nLeft;          // Padding from left
            size_t              nRight;         // Padding from right
            size_t              nTop;           // Padding from top
            size_t              nBottom;        // Padding from bottom
        } padding_t;

        typedef struct tether_t
        {
            size_t              nFlags;         // Tether flags, see tether_flags_t
            float               fHAlign;        // Horizontal alignment over tether line
            float               fVAlign;        // Vertical alignment over the tether line
        } tether_t;

        typedef struct pointer_event_t
        {
            ssize_t             nLeft;          // Mouse horizontal pointer position in window coordinates
            ssize_t             nTop;           // Mouse vertical pointer position in window coordinates
            ws::mouse_pointer_t enPointer;      // Mouse pointer value
        } pointer_event_t;

        /**
         * File dialog mode
         */
        enum file_dialog_mode_t
        {
            FDM_OPEN_FILE,
            FDM_SAVE_FILE
        };

        /**
         * Atom identifier
         */
        typedef ssize_t             atom_t;

    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_TYPES_H_ */
