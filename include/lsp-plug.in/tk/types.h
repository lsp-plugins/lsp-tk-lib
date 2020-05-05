/*
 * types.h
 *
 *  Created on: 5 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_TYPES_H_
#define LSP_PLUG_IN_TK_TYPES_H_

#include <lsp-plug.in/tk/version.h>
#include <lsp-plug.in/common/types.h>

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

        /** Widget classes
         *
         */
        enum wclass_t
        {
            W_UNKNOWN = -1,//!< W_UNKNOWN
            W_ALIGN,
            W_AXIS,
            W_BASIS,
            W_BODY,
            W_BOX,
            W_BUTTON,
            W_CELL,
            W_CENTER,
            W_COMBO,
            W_DOT,
            W_EDIT,
            W_FILE,
            W_GRAPH,
            W_GRID,
            W_GROUP,
            W_INDICATOR,
            W_KNOB,
            W_LABEL,
            W_LED,
            W_LISTBOX,
            W_MARKER,
            W_MENU,
            W_MENUITEM,
            W_MESH,
            W_METER,
            W_MSGBOX,
            W_MSTUD,
            W_PARAM,
            W_PLUGIN,
            W_PORT,
            W_SBAR,
            W_SEP,
            W_SWITCH,
            W_TEXT,
            W_VALUE,
            W_WINDOW
        };

        /** List of available slots for event processing
         *
         */
        enum ui_slot_t
        {
            LSPSLOT_FOCUS_IN,           //!< LSPSLOT_FOCUS_IN Triggered when element takes focus
            LSPSLOT_FOCUS_OUT,          //!< LSPSLOT_FOCUS_IN Triggered when element loses focus
            LSPSLOT_KEY_DOWN,           //!< LSPSLOT_KEY_DOWN Triggered on keyboard key press
            LSPSLOT_KEY_UP,             //!< LSPSLOT_KEY_UP Triggered on keyboard key release
            LSPSLOT_MOUSE_DOWN,         //!< LSPSLOT_MOUSE_DOWN Triggered on mouse button press
            LSPSLOT_MOUSE_UP,           //!< LSPSLOT_MOUSE_UP Triggered on mouse button release
            LSPSLOT_MOUSE_MOVE,         //!< LSPSLOT_MOUSE_MOVE Triggered on mouse pointer motion
            LSPSLOT_MOUSE_SCROLL,       //!< LSPSLOT_MOUSE_SCROLL Triggered on mouse scroll event
            LSPSLOT_MOUSE_CLICK,        //!< LSPSLOT_MOUSE_DBL_CLICK Triggered on mouse click
            LSPSLOT_MOUSE_DBL_CLICK,    //!< LSPSLOT_MOUSE_DBL_CLICK Triggered on mouse double click
            LSPSLOT_MOUSE_TRI_CLICK,    //!< LSPSLOT_MOUSE_TRI_CLICK Triggered on mouse triple click
            LSPSLOT_MOUSE_IN,           //!< LSPSLOT_MOUSE_IN Triggered when mouse first entered widget's area
            LSPSLOT_MOUSE_OUT,          //!< LSPSLOT_MOUSE_OUT Triggered when mouse left widget's area
            LSPSLOT_SHOW,               //!< LSPSLOT_SHOW Triggered when widget becomes visible
            LSPSLOT_HIDE,               //!< LSPSLOT_HIDE Triggered when widget becomes invisible
            LSPSLOT_SUBMIT,             //!< LSPSLOT_SUBMIT Triggered when value(s) stored by the widget is submitted (but can be not changed)
            LSPSLOT_CHANGE,             //!< LSPSLOT_CHANGE Triggered only when value(s) stored by the widget is(are) changed
            LSPSLOT_DESTROY,            //!< LSPSLOT_DESTROY Triggered when widget associated with slot is destroyed
            LSPSLOT_RESIZE,             //!< LSPSLOT_RESIZE Triggered when the geometry of widget has been changed
            LSPSLOT_RESIZE_PARENT,      //!< LSPSLOT_RESIZE_PARENT Triggered when the parent widget has been resized
            LSPSLOT_CLOSE,              //!< LSPSLOT_CLOSE Triggered when the window is requested to close
            LSPSLOT_HSCROLL,            //!< LSPSLOT_HSCROLL Triggered when the horizontal scrolling is applied
            LSPSLOT_VSCROLL,            //!< LSPSLOT_VSCROLL Triggered when the vertical scrolling is applied
            LSPSLOT_ACTIVATE,           //!< LSPSLOT_ACTIVATE Triggered some user-interaction
            LSPSLOT_DRAW3D,             //!< LSPSLOT_DRAW3D Triggered when a 3D-rendering is required
            LSPSLOT_DRAG_REQUEST,       //!< LSPSLOT_DRAG_REQUEST Triggered when a drag request is pending on the widget
            LSPSLOT_BEFORE_POPUP,       //!< LSPSLOT_BEFORE_POPUP Triggered before pop-up element is going to be shown
            LSPSLOT_POPUP,              //!< LSPSLOT_POPUP Triggered after pop-up element has been shown
        };

        typedef struct w_class_t
        {
            const char         *name;
            const w_class_t    *parent;
        } w_class_t;

        enum ui_property_type_t
        {
            PT_INT,         // Integer property
            PT_FLOAT,       // Floating-point property
            PT_BOOL,        // Boolean property
            PT_STRING,      // String (text) property

            PT_UNKNOWN  = -1
        };

        /**
         * Atom identifier
         */
        typedef ssize_t             ui_atom_t;
    }
}

#endif /* LSP_PLUG_IN_TK_TYPES_H_ */
