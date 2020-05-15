/*
 * Window.h
 *
 *  Created on: 16 июн. 2017 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_WINDOW_H_
#define LSP_PLUG_IN_TK_WIDGETS_WINDOW_H_

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
            protected:
                friend class WindowActions;
                friend class Display;

            public:
                static const w_class_t    metadata;

            protected:
                ws::IWindow            *pWindow;
                void                   *pNativeHandle;
                Widget                 *pChild;
                ssize_t                 nScreen;
                Widget                 *pFocus;
                Widget                 *pPointed;
                bool                    bHasFocus;
                bool                    bOverridePointer;
                bool                    bMapFlag;
                float                   nVertPos;
                float                   nHorPos;
                float                   nVertScale;
                float                   nHorScale;
                window_poilicy_t        enPolicy;

                Timer                   sRedraw;

                prop::String            sTitle;
                prop::String            sRole;
                prop::Color             sBorderColor;
                prop::BorderStyle       sBorderStyle;
                prop::Float             sBorderSize;
                prop::Float             sBorderRadius;
                prop::WindowActions     sActions;
                prop::Position          sPosition;
                prop::Size              sSize;
                prop::SizeConstraints   sSizeConstraints;

            //---------------------------------------------------------------------------------
            // Slot handlers
            protected:
                static status_t     tmr_redraw_request(ws::timestamp_t ts, void *args);
                static status_t     slot_window_close(Widget *sender, void *ptr, void *data);

                virtual Widget     *find_widget(ssize_t x, ssize_t y);
                status_t            do_render();
                void                do_destroy();
                status_t            sync_size();
                status_t            update_pointer();

                virtual void        property_changed(Property *prop);

            //---------------------------------------------------------------------------------
            // Construction and destruction
            public:
                explicit Window(Display *dpy, void *handle = NULL, ssize_t screen = -1);
                virtual ~Window();

                /** Init window
                 *
                 * @return status of operation
                 */
                virtual status_t init();

                /** Destroy window
                 *
                 */
                virtual void destroy();

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

                status_t                        get_absolute_geometry(ws::rectangle_t *r);

                inline Widget                  *focused_child() const       { return const_cast<Window *>(this)->pFocus; }

                inline Widget                  *pointed_child() const       { return const_cast<Window *>(this)->pPointed; }

                inline bool                     override_pointer() const    { return bOverridePointer; }

                inline window_poilicy_t         policy() const              { return enPolicy; }

                inline float                    vpos() const                { return nVertPos; }
                inline float                    hpos() const                { return nHorPos; }
                inline float                    vscale() const              { return nVertScale; }
                inline float                    hscale() const              { return nHorScale; }

                inline String                  *title()                     { return &sTitle; }
                inline const String            *title() const               { return &sTitle; }
                inline String                  *role()                      { return &sRole; }
                inline const String            *role() const                { return &sRole; }
                inline Color                   *border_color()              { return &sBorderColor; }
                inline const Color             *border_color() const        { return &sBorderColor; }
                inline BorderStyle             *border_style()              { return &sBorderStyle; }
                inline const BorderStyle       *border_style() const        { return &sBorderStyle; }
                inline WindowActions           *actions()                   { return &sActions; }
                inline const WindowActions     *actions() const             { return &sActions; }
                inline Size                    *size()                      { return &sSize; }
                inline const Size              *size() const                { return &sSize; }
                inline SizeConstraints         *size_constraints()          { return &sSizeConstraints; }
                inline const SizeConstraints   *size_constraints() const    { return &sSizeConstraints; }

            //---------------------------------------------------------------------------------
            // Manipulation
            public:
                virtual void            render(ws::ISurface *s, bool force);

                virtual status_t        set_cursor(ws::mouse_pointer_t mp);

                virtual status_t        override_pointer(bool override = true);

                /** Hide window
                 *
                 */
                virtual bool            hide();

                /** Show window
                 *
                 */
                virtual bool            show();

                /** Show window over window of actor
                 *
                 * @param actor actor
                 * @return status of operation
                 */
                virtual bool            show(Widget *actor);

                virtual status_t        add(Widget *widget);

                virtual status_t        remove(Widget *widget);

                virtual status_t        handle_event(const ws::event_t *e);

                virtual status_t        set_focus(bool focus = true);

                virtual status_t        toggle_focus();

                virtual bool            has_focus() const;

            public:
                status_t                focus_child(Widget *focus);
                status_t                unfocus_child(Widget *focus);

                status_t                toggle_child_focus(Widget *focus);

                status_t                point_child(Widget *focus);

                status_t                grab_events(ws::grab_t grab);
                status_t                ungrab_events();

                void                    set_policy(window_poilicy_t policy);

                void                    set_vpos(float value);
                void                    set_hpos(float value);
                void                    set_vscale(float value);
                void                    set_hscale(float value);

                status_t                set_class(const char *instance, const char *wclass);
                status_t                set_class(const LSPString *instance, const LSPString *wclass);

                status_t                set_role(const char *role);
                status_t                set_role(const LSPString *role);

            //---------------------------------------------------------------------------------
            // Event handling
            public:
                /** Close event
                 *
                 * @param e close event
                 * @return status of operation
                 */
                virtual status_t        on_close(const ws::event_t *e);

                /** Widget has taken focus
                 *
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t        on_focus_in(const ws::event_t *e);

                /** Widget has lost focus
                 *
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t        on_focus_out(const ws::event_t *e);

                /** Realize window
                 *
                 * @param r realize parameters
                 */
                virtual void            realize(const ws::rectangle_t *r);

                /** Request size of the window
                 *
                 * @param r pointer to structure to store data
                 */
                virtual void            size_request(ws::size_limit_t *r);

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

#endif /* LSP_PLUG_IN_TK_WIDGETS_WINDOW_H_ */
