/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 6 мая 2020 г.
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

#ifndef LSP_PLUG_IN_TK_SLOTS_TYPES_H_
#define LSP_PLUG_IN_TK_SLOTS_TYPES_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Widget;

        /** List of available slots for event processing
         *
         */
        enum slot_t
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
            SLOT_CANCEL,            //!< SLOT_CANCEL Triggered when the 'Cancel' action has been performed (for example, in dialogs)
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

        /** Event handler identifier
         *
         */
        typedef ssize_t             handler_id_t;

        /** Event handler type
         *
         * @param sender the widget that initiated an event
         * @param ptr additional pointer passed as an argument to the bind()
         * @param data data structure to process (handle)
         * @return status of operation
         */
        typedef status_t (* event_handler_t)(Widget *sender, void *ptr, void *data);
    }
}

#endif /* LSP_PLUG_IN_TK_SLOTS_TYPES_H_ */
