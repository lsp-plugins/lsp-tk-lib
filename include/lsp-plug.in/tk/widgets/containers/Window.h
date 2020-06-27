/*
 * Window.h
 *
 *  Created on: 16 июн. 2017 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_WINDOW_H_
#define LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_WINDOW_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/ws/IWindow.h>

namespace lsp
{
    namespace tk
    {
        class Window: public WidgetContainer
        {
            private:
                Window & operator = (const Window &);

            protected:
                friend class Display;
                friend class Widget;

                typedef struct mouse_handler_t
                {
                    size_t              nState;             // State of mouse buttons
                    ssize_t             nLeft;              // Last X coordinate
                    ssize_t             nTop;               // Last Y coordinate
                    Widget             *pWidget;            // Current widget that handles events
                } mouse_handler_t;

                typedef struct key_handler_t
                {
                    size_t              nKeys;              // Number of keys pressed
                    Widget             *pWidget;            // Keyboard handler
                } key_handler_t;

            public:
                static const w_class_t    metadata;

            protected:
                ws::IWindow            *pWindow;            // Underlying window
                void                   *pNativeHandle;      // Native handle of the window
                Widget                 *pChild;             // Child widget
                Widget                 *pFocused;           // Focused widget
                bool                    bMapped;
                bool                    bOverridePointer;
                float                   fScaling;           // Cached scaling factor

                mouse_handler_t         hMouse;             // Mouse handler
                key_handler_t           hKeys;              // Key handler

                Window                 *pActor;
                Timer                   sRedraw;

                prop::String            sTitle;
                prop::String            sRole;
                prop::Color             sBorderColor;
                prop::BorderStyle       sBorderStyle;
                prop::Integer           sBorderSize;
                prop::Float             sBorderRadius;
                prop::WindowActions     sActions;
                prop::Position          sPosition;
                prop::Size              sWindowSize;
                prop::SizeConstraints   sSizeConstraints;
                prop::Layout            sLayout;
                prop::WindowPolicy      sPolicy;

            //---------------------------------------------------------------------------------
            // Slot handlers
            protected:
                static status_t     tmr_redraw_request(ws::timestamp_t ts, void *args);
                static status_t     slot_window_close(Widget *sender, void *ptr, void *data);

                status_t            do_render();
                void                do_destroy();
                virtual status_t    sync_size();
                status_t            update_pointer();

                // Mouse operations
                virtual Widget     *sync_mouse_handler(const ws::event_t *e);
                virtual Widget     *acquire_mouse_handler(const ws::event_t *e);
                virtual Widget     *release_mouse_handler(const ws::event_t *e);

                // Focus operations
                inline bool         check_focus(Widget *w) const    { return pFocused == w; }
                virtual bool        take_focus(Widget *w);
                bool                kill_focus(Widget *w);

            protected:
                virtual Widget     *find_widget(ssize_t x, ssize_t y);
                virtual void        property_changed(Property *prop);
                virtual void        hide_widget();
                virtual void        show_widget();
                virtual void        size_request(ws::size_limit_t *r);
                virtual void        realize(const ws::rectangle_t *r);
                virtual status_t    post_init();

                /**
                 * Discard widget: notify window that widget has been removed from the widget tree
                 * @param w widget
                 */
                void                discard_widget(Widget *w);

            //---------------------------------------------------------------------------------
            // Construction and destruction
            public:
                explicit Window(Display *dpy, void *handle = NULL, ssize_t screen = -1);
                virtual ~Window();

                virtual status_t                init();

                virtual void                    destroy();

            //---------------------------------------------------------------------------------
            // Properties
            public:
                /** Get native window handle
                 *
                 */
                inline void                    *handle()                    { return (pWindow != NULL) ? pWindow->handle() : NULL; };

                /**
                 * Get native window
                 * @return native window
                 */
                inline ws::IWindow             *native()                    { return pWindow; };

                /**
                 * Return true if window is a sub-window of another window
                 * @return true if window is a sub-window of another window
                 */
                inline bool                     nested()                    { return pNativeHandle != NULL; }

                inline ssize_t                  screen()                    { return (pWindow != NULL) ? pWindow->screen() : -1; };

                virtual status_t                get_screen_rectangle(ws::rectangle_t *r);
                virtual status_t                get_padded_screen_rectangle(ws::rectangle_t *r);

                inline bool                     override_pointer() const    { return bOverridePointer; }

            public:
                LSP_TK_PROPERTY(String,             title,              &sTitle)
                LSP_TK_PROPERTY(String,             role,               &sRole)
                LSP_TK_PROPERTY(Color,              border_color,       &sBorderColor)
                LSP_TK_PROPERTY(BorderStyle,        border_style,       &sBorderStyle)
                LSP_TK_PROPERTY(Integer,            border_size,        &sBorderSize)
                LSP_TK_PROPERTY(WindowActions,      actions,            &sActions)
                LSP_TK_PROPERTY(Size,               size,               &sWindowSize)
                LSP_TK_PROPERTY(SizeConstraints,    size_constraints,   &sSizeConstraints)
                LSP_TK_PROPERTY(Layout,             layout,             &sLayout)
                LSP_TK_PROPERTY(WindowPolicy,       policy,             &sPolicy)

            //---------------------------------------------------------------------------------
            // Manipulation
            public:
                virtual void            render(ws::ISurface *s, const ws::rectangle_t *area, bool force);

                virtual status_t        override_pointer(bool override = true);

                /** Show window
                 *
                 */
                virtual void            show();

                /** Show window over window of actor
                 *
                 * @param actor actor
                 * @return status of operation
                 */
                virtual void            show(Widget *actor);

                virtual status_t        add(Widget *widget);

                virtual status_t        remove(Widget *widget);

                virtual status_t        remove_all();

                virtual status_t        handle_event(const ws::event_t *e);

                virtual bool            take_focus();

                virtual bool            kill_focus();

            public:
                status_t                grab_events(ws::grab_t grab);
                status_t                ungrab_events();

                status_t                set_class(const char *instance, const char *wclass);
                status_t                set_class(const LSPString *instance, const LSPString *wclass);

            //---------------------------------------------------------------------------------
            // Event handling
            public:
                /** Close event
                 *
                 * @param e close event
                 * @return status of operation
                 */
                virtual status_t        on_close(const ws::event_t *e);

                /** Set window icon
                 *
                 * @param bgra color data
                 * @param width width of icon
                 * @param height height of icon
                 * @return status of operation
                 */
                virtual status_t        set_icon(const void *bgra, size_t width, size_t height);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_WINDOW_H_ */
