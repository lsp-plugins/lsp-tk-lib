/*
 * types.h
 *
 *  Created on: 5 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_TYPES_H_
#define LSP_PLUG_IN_TK_TYPES_H_

#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/tk/version.h>

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

        /** List of available slots for event processing
         *
         */
        enum ui_slot_t
        {
            SLOT_FOCUS_IN,          //!< SLOT_FOCUS_IN Triggered when element takes focus
            SLOT_FOCUS_OUT,         //!< SLOT_FOCUS_IN Triggered when element loses focus
            SLOT_KEY_DOWN,          //!< SLOT_KEY_DOWN Triggered on keyboard key press
            SLOT_KEY_UP,            //!< SLOT_KEY_UP Triggered on keyboard key release
            SLOT_MOUSE_DOWN,        //!< SLOT_MOUSE_DOWN Triggered on mouse button press
            SLOT_MOUSE_UP,          //!< SLOT_MOUSE_UP Triggered on mouse button release
            SLOT_MOUSE_MOVE,        //!< SLOT_MOUSE_MOVE Triggered on mouse pointer motion
            SLOT_MOUSE_SCROLL,      //!< SLOT_MOUSE_SCROLL Triggered on mouse scroll event
            SLOT_MOUSE_CLICK,       //!< SLOT_MOUSE_DBL_CLICK Triggered on mouse click
            SLOT_MOUSE_DBL_CLICK,   //!< SLOT_MOUSE_DBL_CLICK Triggered on mouse double click
            SLOT_MOUSE_TRI_CLICK,   //!< SLOT_MOUSE_TRI_CLICK Triggered on mouse triple click
            SLOT_MOUSE_IN,          //!< SLOT_MOUSE_IN Triggered when mouse first entered widget's area
            SLOT_MOUSE_OUT,         //!< SLOT_MOUSE_OUT Triggered when mouse left widget's area
            SLOT_SHOW,              //!< SLOT_SHOW Triggered when widget becomes visible
            SLOT_HIDE,              //!< SLOT_HIDE Triggered when widget becomes invisible
            SLOT_SUBMIT,            //!< SLOT_SUBMIT Triggered when value(s) stored by the widget is submitted (but can be not changed)
            SLOT_CHANGE,            //!< SLOT_CHANGE Triggered only when value(s) stored by the widget is(are) changed
            SLOT_DESTROY,           //!< SLOT_DESTROY Triggered when widget associated with slot is destroyed
            SLOT_RESIZE,            //!< SLOT_RESIZE Triggered when the geometry of widget has been changed
            SLOT_RESIZE_PARENT,     //!< SLOT_RESIZE_PARENT Triggered when the parent widget has been resized
            SLOT_CLOSE,             //!< SLOT_CLOSE Triggered when the window is requested to close
            SLOT_HSCROLL,           //!< SLOT_HSCROLL Triggered when the horizontal scrolling is applied
            SLOT_VSCROLL,           //!< SLOT_VSCROLL Triggered when the vertical scrolling is applied
            SLOT_ACTIVATE,          //!< SLOT_ACTIVATE Triggered some user-interaction
            SLOT_DRAW3D,            //!< SLOT_DRAW3D Triggered when a 3D-rendering is required
            SLOT_DRAG_REQUEST,      //!< SLOT_DRAG_REQUEST Triggered when a drag request is pending on the widget
            SLOT_BEFORE_POPUP,      //!< SLOT_BEFORE_POPUP Triggered before pop-up element is going to be shown
            SLOT_POPUP,             //!< SLOT_POPUP Triggered after pop-up element has been shown
        };

        typedef struct w_class_t
        {
            const char         *name;
            const w_class_t    *parent;
        } w_class_t;

        enum property_type_t
        {
            PT_INT,         // Integer property
            PT_FLOAT,       // Floating-point property
            PT_BOOL,        // Boolean property
            PT_STRING,      // String (text) property

            PT_UNKNOWN  = -1
        };

        typedef struct size_request_t
        {
            ssize_t             nMinWidth;
            ssize_t             nMinHeight;
            ssize_t             nMaxWidth;
            ssize_t             nMaxHeight;
        } size_request_t;

        typedef struct realize_t
        {
            ssize_t             nLeft;
            ssize_t             nTop;
            ssize_t             nWidth;
            ssize_t             nHeight;
        } realize_t;

        typedef struct padding_t
        {
            size_t              nLeft;
            size_t              nRight;
            size_t              nTop;
            size_t              nBottom;
        } padding_t;

        /** Event handler identifier
         *
         */
        typedef ssize_t             handler_id_t;

        /**
         * Atom identifier
         */
        typedef ssize_t             atom_t;

    }
}

#endif /* LSP_PLUG_IN_TK_TYPES_H_ */
