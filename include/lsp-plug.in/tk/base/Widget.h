/*
 * Widget.h
 *
 *  Created on: 15 июн. 2017 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_BASE_WIDGET_H_
#define LSP_PLUG_IN_TK_BASE_WIDGET_H_

#include <lsp-plug.in/tk/version.h>
#include <lsp-plug.in/tk/types.h>

#include <lsp-plug.in/ws/IEventHandler.h>
#include <lsp-plug.in/ws/ISurface.h>

#include <lsp-plug.in/tk/slots.h>
#include <lsp-plug.in/tk/style.h>
#include <lsp-plug.in/tk/prop.h>

namespace lsp
{
    namespace tk
    {
        class ComplexWidget;
        class Display;

        /** Basic widget class for any widget in the toolkit
         *
         */
        class Widget: public ws::IEventHandler
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum flags_t
                {
                    REDRAW_SURFACE  = 1 << 0,       // Need to redraw surface
                    REDRAW_CHILD    = 1 << 1,       // Need to redraw child only
                    RESIZE_VALID    = 1 << 2,       // Size limit structure is valid
                    RESIZE_PENDING  = 1 << 3,       // The resize request is pending

                    F_VISIBLE       = 1 << 8,       // Widget is visible
                    F_REALIZED      = 1 << 9,       // Widget is realized
                    F_HFILL         = 1 << 10,      // Widget should desirable fill all the provided area horizontally
                    F_VFILL         = 1 << 11,      // Widget should desirable fill all the provided area vertically
                    F_HEXPAND       = 1 << 12,      // Horizontal area for the widget should be expanded
                    F_VEXPAND       = 1 << 13,      // Vertical area for the widget should be expanded
                };

            protected:
                class PropListener: public prop::Listener
                {
                    protected:
                        Widget     *pWidget;

                    public:
                        inline PropListener(Widget *w)  { pWidget = w; }
                        virtual void notify(Property *prop);
                };

            protected:
                size_t              nFlags;         // Flags
                const w_class_t    *pClass;         // Widget class descriptor
                Display            *pDisplay;
                ComplexWidget      *pParent;

                ws::size_limit_t    sSizeLimit;     // Cached size limit
                ws::rectangle_t     sRectangle;     // Real geometry
                ws::mouse_pointer_t enCursor;
                ws::ISurface       *pSurface;

                SlotSet             sSlots;         // Slots
                Style               sStyle;         // Style
                PropListener        sProperties;    // Properties listener

                prop::Float         sScaling;       // UI scaling factor
                prop::Float         sBrightness;    // Brightness
                prop::Padding       sPadding;
                prop::Color         sBgColor;       // Widget color


            //---------------------------------------------------------------------------------
            // Slot handlers
            protected:
                static status_t     slot_mouse_move(Widget *sender, void *ptr, void *data);
                static status_t     slot_mouse_down(Widget *sender, void *ptr, void *data);
                static status_t     slot_mouse_up(Widget *sender, void *ptr, void *data);
                static status_t     slot_mouse_dbl_click(Widget *sender, void *ptr, void *data);
                static status_t     slot_mouse_tri_click(Widget *sender, void *ptr, void *data);
                static status_t     slot_mouse_scroll(Widget *sender, void *ptr, void *data);
                static status_t     slot_mouse_in(Widget *sender, void *ptr, void *data);
                static status_t     slot_mouse_out(Widget *sender, void *ptr, void *data);
                static status_t     slot_key_down(Widget *sender, void *ptr, void *data);
                static status_t     slot_key_up(Widget *sender, void *ptr, void *data);
                static status_t     slot_hide(Widget *sender, void *ptr, void *data);
                static status_t     slot_show(Widget *sender, void *ptr, void *data);
                static status_t     slot_destroy(Widget *sender, void *ptr, void *data);
                static status_t     slot_resize(Widget *sender, void *ptr, void *data);
                static status_t     slot_resize_parent(Widget *sender, void *ptr, void *data);
                static status_t     slot_focus_in(Widget *sender, void *ptr, void *data);
                static status_t     slot_focus_out(Widget *sender, void *ptr, void *data);
                static status_t     slot_drag_request(Widget *sender, void *ptr, void *data);

            //---------------------------------------------------------------------------------
            // Interface for nested classes
            protected:
                void                do_destroy();

                void                unlink_widget(Widget *widget);

                virtual void        property_changed(Property *prop);

                /** Request widget for size
                 *
                 * @param limit the widget size constraints to fill
                 */
                virtual void        size_request(ws::size_limit_t *r);

            //---------------------------------------------------------------------------------
            // Construction and destruction
            public:
                explicit Widget(Display *dpy);
                virtual ~Widget();

                /** Initialize wiget
                 *
                 */
                virtual status_t init();

                /** Destroy widget
                 *
                 */
                virtual void destroy();

            //---------------------------------------------------------------------------------
            // Metadata, casting and type information
            public:
                /** Get widget class
                 *
                 * @return actual widget class metadata
                 */
                inline const w_class_t *get_class() const { return pClass; }

                /** Check wheter the widget is instance of some class
                 *
                 * @param wclass widget class
                 * @return true if widget is instance of some class
                 */
                bool instance_of(const w_class_t *wclass) const;

                inline bool instance_of(const w_class_t &wclass) const { return instance_of(&wclass); }

                /** Another way to check if widget is instance of some class
                 *
                 * @return true if widget is instance of some class
                 */
                template <class LSPTarget>
                    inline bool instance_of() const { return instance_of(&LSPTarget::metadata); };

                /** Cast widget to another type
                 *
                 * @return pointer to widget or NULL if cast failed
                 */
                template <class LSPTarget>
                    inline LSPTarget *cast() { return instance_of(&LSPTarget::metadata) ? static_cast<LSPTarget *>(this) : NULL; }

                /** Cast widget to another type
                 *
                 * @return pointer to widget or NULL if cast failed
                 */
                template <class LSPTarget>
                    inline const LSPTarget *cast() const { return instance_of(&LSPTarget::metadata) ? static_cast<const LSPTarget *>(this) : NULL; }

                /** Get pointer to self as pointer to Widget class
                 *
                 * @return pointer to self
                 */
                inline Widget *self()              { return this;  }

            //---------------------------------------------------------------------------------
            // Properties
            public:
                /** Get display
                 *
                 * @return display
                 */
                inline Display *display()           { return pDisplay; };

                /** Get widget dimensions
                 *
                 * @param r real widget dimensions
                 */
                inline void get_rectangle(ws::rectangle_t *r)   { *r = sRectangle; }

                /**
                 * Get size limit
                 * @param l size limit to calculate
                 */
                void get_size_limit(ws::size_limit_t *l);

                /** Check if there is redraw request pending
                 *
                 * @return true if there is redraw request pending
                 */
                inline bool redraw_pending() const  { return nFlags & (REDRAW_SURFACE | REDRAW_CHILD); };

                /** Check that widget is visible
                 *
                 * @return true if widget is visible
                 */
                inline bool visible() const         { return nFlags & F_VISIBLE; };

                /** Check that widget is hidden
                 *
                 * @return true if widget is visible
                 */
                inline bool hidden() const          { return !(nFlags & F_VISIBLE); };
                inline bool invisible() const       { return !(nFlags & F_VISIBLE); };

                /** Get expanding flag: true if container should desirable expand area provided for the widget
                 *
                 * @return expanding flag
                 */
                inline bool expand() const          { return (nFlags & (F_HEXPAND | F_VEXPAND)) == (F_HEXPAND | F_VEXPAND); }

                /** Get fill flag: true if container should desirable fill all provided area with widget
                 *
                 * @return fill flag
                 */
                inline bool fill() const            { return (nFlags & (F_HFILL | F_VFILL)) == (F_HFILL | F_VFILL); }

                /** Get horizontal fill flag: true if container should horizontally fill all provided area with widget
                 *
                 * @return horizontal fill flag
                 */
                inline bool hfill() const           { return nFlags & F_HFILL; }

                /** Get vertical fill flag: true if container should vertically fill all provided area with widget
                 *
                 * @return vertical fill flag
                 */
                inline bool vfill() const           { return nFlags & F_VFILL; }

                /** Check that widget has focus
                 *
                 * @return widget focus
                 */
                virtual bool has_focus() const;

                /** Check that widget has focus
                 *
                 * @return widget focus
                 */
                inline bool focused() const             { return has_focus(); };

                /** Check that widget is visible
                 *
                 * @return true if widget is visible
                 */
                inline bool is_visible() const          { return nFlags & F_VISIBLE; };

                /** Check that widget is realized
                 *
                 * @return true if widget is realized
                 */
                inline bool is_realized() const         { return nFlags & F_REALIZED; };

                /** Get horizontal coordinate of the left top corner relative to the parent widget
                 *
                 * @return the value
                 */
                ssize_t relative_left() const;

                /** Get horizontal coordinate of the right bottom corner relative to the parent widget
                 *
                 * @return the value
                 */
                ssize_t relative_right() const;

                /** Get vertical coordinate of the left top corner relative to the parent widget
                 *
                 * @return the value
                 */
                ssize_t relative_top() const;

                /** Get vertical coordinate of the right bottom corner relative to the parent widget
                 *
                 * @return the value
                 */
                ssize_t relative_bottom() const;

                /** Check that specified window coordinate lies within widget's bounds
                 * Always returns false for invisible widgets
                 *
                 * @param x x coordinate
                 * @param y y coordinate
                 * @return true on success
                 */
                virtual bool inside(ssize_t x, ssize_t y);

                /** Get parent widget
                 *
                 * @return parent widget
                 */
                inline ComplexWidget *parent()              { return pParent; }

                /** Get slots
                 *
                 * @return slots
                 */
                inline SlotSet *slots()                     { return &sSlots; }

                /** Get slot
                 *
                 * @param id slot identifier
                 * @return pointer to slot or NULL
                 */
                inline Slot *slot(slot_t id)                { return sSlots.slot(id); }

                /** Get mouse pointer
                 *
                 * @return mouse pointer
                 */
                inline ws::mouse_pointer_t cursor() const   { return enCursor; }

                /** Get active curstor
                 *
                 * @return active cursor
                 */
                virtual ws::mouse_pointer_t active_cursor() const;

                /**
                 * Return widget's style
                 * @return widget's style
                 */
                inline Style       *style()                 { return &sStyle;       }

                /** Get widget padding
                 *
                 * @return widget padding
                 */
                inline Padding     *padding()               { return &sPadding;     };

                /**
                 * Get background color of the widget
                 * @return background color of the widget
                 */
                inline Color       *bg_color()              { return &sBgColor;     }

                /**
                 * Get brightness property
                 * @return brightness property
                 */
                inline Float       *brightness()            { return &sBrightness;  }
                inline const Float *brightness() const      { return &sBrightness;  }

                /**
                 * Get brightness property
                 * @return brightness property
                 */
                inline Float       *scaling()               { return &sScaling;     }
                inline const Float *scaling() const         { return &sScaling;     }

            //---------------------------------------------------------------------------------
            // Manipulation
            public:
                /** Query widget for redraw
                 *
                 * @param flags redraw flags
                 */
                virtual void        query_draw(size_t flags = REDRAW_SURFACE);

                /**
                 * Put the widget to the destroy queue of the main loop
                 * @return status of operation
                 */
                virtual status_t    queue_destroy();

                /** Query widget for resize
                 *
                 */
                virtual void        query_resize();

                /** Set expanding flag
                 *
                 * @param value expanding flag value
                 */
                virtual void        set_expand(bool value = true);

                /** Set fill flag
                 *
                 * @param value filling flag value
                 */
                virtual void        set_fill(bool value = true);

                /** Set horizontal fill flag
                 *
                 * @param value horizontal filling flag value
                 */
                virtual void        set_hfill(bool value = true);

                /** Set vertical fill flag
                 *
                 * @param value vertical filling flag value
                 */
                virtual void        set_vfill(bool value = true);

                /** Set mouse pointer
                 *
                 * @param mp mouse pointer
                 * @return mouse pointer
                 */
                virtual status_t    set_cursor(ws::mouse_pointer_t mp);

                /** Get widget surface
                 *
                 * @param s base surface
                 * @return widget surface or NULL
                 */
                ws::ISurface       *get_surface(ws::ISurface *s);

                /** Get widget surface
                 *
                 * @param s base surface
                 * @param width requested width
                 * @param height requested height
                 * @return widget surface or NULL
                 */
                ws::ISurface       *get_surface(ws::ISurface *s, ssize_t width, ssize_t height);

                /** Render widget to the external surface
                 *
                 * @param surface surface to perform rendering
                 * @param force force child rendering
                 */
                virtual void        render(ws::ISurface *s, bool force);

                /** Draw widget on the internal surface
                 *
                 * @param surface surface to perform drawing
                 */
                virtual void        draw(ws::ISurface *s);

                /** Realize widget
                 *
                 * @param r widget realization parameters
                 */
                virtual void        realize(const ws::rectangle_t *r);

                /** Hide widget
                 *
                 */
                virtual bool        hide();

                /** Show widget
                 *
                 */
                virtual bool        show();

                /** Set widget visibility
                 *
                 * @param visible widget visibility
                 */
                virtual void        set_visible(bool visible=true);

                /** Set focus on widget
                 *
                 * @param focus focusing parameter
                 * @return status of operation
                 */
                virtual status_t    set_focus(bool focus = true);

                /** Kill focus on widget
                 *
                 * @return status of operation
                 */
                inline status_t     kill_focus() { return set_focus(false); };

                /** Kill focus on widget
                 *
                 * @return status of operation
                 */
                inline status_t     take_focus() { return set_focus(true); };

                /** Mark this widget to be currently pointed by mouse
                 *
                 * @return status of operation
                 */
                status_t            mark_pointed();

                /** Invert focus
                 *
                 * @return status of operation
                 */
                virtual status_t    toggle_focus();

                /** Set widget invisibility
                 *
                 * @param invisible widget invisibility
                 */
                inline void         set_invisible(bool invisible=true) { set_visible(!invisible); }

                /** Handle UI event from the display
                 *
                 * @param e UI event
                 * @return status of operation
                 */
                virtual status_t    handle_event(const ws::event_t *e);

                /** Set parent widget of this widget
                 *
                 * @param parent parent widget
                 */
                void                set_parent(ComplexWidget *parent);

                /** Commit widet redraw
                 *
                 */
                virtual void        commit_redraw();

                /** Get most top-level widget
                 *
                 * @return most top-level widget
                 */
                Widget             *toplevel();

            //---------------------------------------------------------------------------------
            // Event handling
            public:
                /** Widget has taken focus
                 *
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t on_focus_in(const ws::event_t *e);

                /** Widget has lost focus
                 *
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t on_focus_out(const ws::event_t *e);

                /** Handle key press event
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t on_key_down(const ws::event_t *e);

                /** Handle key release event
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t on_key_up(const ws::event_t *e);

                /** Handle mouse button press event
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t on_mouse_down(const ws::event_t *e);

                /** Handle mouse button release event
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t on_mouse_up(const ws::event_t *e);

                /** Handle mouse motion event
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t on_mouse_move(const ws::event_t *e);

                /** Handle mouse cursor moved into the zone of widget
                 *
                 * @param e mouse event that triggered MouseIn
                 * @return status of operation
                 */
                virtual status_t on_mouse_in(const ws::event_t *e);

                /** Handle mouse cursor moved outside the zone of widget
                 *
                 * @param e mouse event that triggered MouseIn
                 * @return status of operation
                 */
                virtual status_t on_mouse_out(const ws::event_t *e);

                /** Handle mouse scroll event
                 *
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t on_mouse_scroll(const ws::event_t *e);

                /** Handle mouse double click event
                 *
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t on_mouse_dbl_click(const ws::event_t *e);

                /** Handle mouse triple click
                 *
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t on_mouse_tri_click(const ws::event_t *e);

                /** Geometry has changed: size or position
                 *
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t on_resize(const ws::rectangle_t *r);

                /** Geometry of parent widget has changed: size or position
                 *
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t on_resize_parent(const ws::rectangle_t *r);

                /** The widget becomes hidden
                 *
                 * @return status of operation
                 */
                virtual status_t on_hide();

                /** The widget becomes visible
                 *
                 * @return status of operation
                 */
                virtual status_t on_show();

                /** The widget becomes destroyed
                 *
                 * @return status of operation
                 */
                virtual status_t on_destroy();

                /**
                 * Process the drag request event
                 * @param e drag request event
                 * @param ctype NULL-terminated list of provided content types
                 * @return status of operation
                 */
                virtual status_t on_drag_request(const ws::event_t *e, const char * const *ctype);
        };
    }

} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_BASE_WIDGET_H_ */
