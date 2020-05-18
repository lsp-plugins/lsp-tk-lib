/*
 * Window.cpp
 *
 *  Created on: 16 июн. 2017 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/common/debug.h>
#include <lsp-plug.in/common/status.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t Window::metadata = { "Window", &WidgetContainer::metadata };

        Window::Window(Display *dpy, void *handle, ssize_t screen):
            WidgetContainer(dpy),
            sTitle(&sProperties),
            sRole(&sProperties),
            sBorderColor(&sProperties),
            sBorderStyle(&sProperties),
            sBorderSize(&sProperties),
            sBorderRadius(&sProperties),
            sActions(&sProperties),
            sPosition(&sProperties),
            sWindowSize(&sProperties),
            sSizeConstraints(&sProperties),
            sLayout(&sProperties),
            sPolicy(&sProperties)
        {
            lsp_trace("native_handle = %p", handle);

            pWindow         = NULL;
            pChild          = NULL;
            pNativeHandle   = handle;
            pFocus          = NULL;
            pPointed        = NULL;
            bHasFocus       = false;
            bMapped         = false;
            bOverridePointer= false;
            fScaling        = 1.0f;
            pActor          = NULL;

            pClass          = &metadata;
        }

        Window::~Window()
        {
            do_destroy();
        }

        status_t Window::init()
        {
            status_t result;

            // Initialize redraw timer
            ws::IDisplay *dpy   = pDisplay->display();
            if (dpy == NULL)
                return STATUS_BAD_STATE;

            // Initialize parent class
            if ((result = WidgetContainer::init()) != STATUS_OK)
                return result;

            // Override some properties
            sVisibility.set(false);

            // Bind properties
            sTitle.bind(&sStyle, pDisplay->dictionary());
            sRole.bind(&sStyle, pDisplay->dictionary());
            sBorderColor.bind("border.color", &sStyle);
            sBorderStyle.bind("border.style", &sStyle);
            sBorderSize.bind("border.size", &sStyle);
            sBorderRadius.bind("border.radius", &sStyle);
            sActions.bind("actions", &sStyle);
            sPosition.bind("position", &sStyle);
            sWindowSize.bind("size", &sStyle);
            sSizeConstraints.bind("size.constraints", &sStyle);
            sLayout.bind("layout", &sStyle);
            sPolicy.bind("policy", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sBorderColor.init(sclass, "#000000");
                sBorderStyle.init(sclass, ws::BS_SIZEABLE);
                sBorderSize.init(sclass, 0);
                sBorderRadius.init(sclass, 2);
                sActions.init(sclass, ws::WA_ALL);
                sPosition.init(sclass, 0, 0);
                sWindowSize.init(sclass, 160, 100);
                sSizeConstraints.init(sclass, -1, -1, -1, -1);
                sLayout.init(sclass, 0.0f, 0.0f, 0.0f, 0.0f);
                sPolicy.init(sclass, WP_NORMAL);
            }

            // Cache the actual scaling factor
            fScaling    = sScaling.get();

            // Create and initialize window
            pWindow     = (pNativeHandle != NULL) ? dpy->create_window(pNativeHandle) : dpy->create_window();

            if (pWindow == NULL)
                return STATUS_UNKNOWN_ERR;

            // Initialize
            if ((result = pWindow->init()) != STATUS_SUCCESS)
                return result;
            pWindow->set_handler(this);

            // Add slot(s)
            handler_id_t id = 0;
            id = sSlots.add(SLOT_CLOSE, slot_window_close, self());
            if (id < 0)
                return - id;

            // Bind redraw handler
            sRedraw.bind(dpy);
            sRedraw.set_handler(tmr_redraw_request, self());

            lsp_trace("Window has been initialized");

            if (sVisibility.get())
                show_widget();

            return STATUS_OK;
        }

        status_t Window::sync_size()
        {
            lsp_trace("Synchronizing size");

            // Request size limits of the window
            ws::size_limit_t sr;
            ws::rectangle_t r;

            get_size_limits(&sr);
            float scaling       = lsp_max(0.0f, sScaling.get());
            float border        = sBorderSize.get() * scaling;
            sPosition.get(&r);
            sWindowSize.compute(&r, scaling);

            // Set window's size constraints and update geometry
            if (sPolicy.get() == WP_GREEDY)
            {
                // Minimize size of window as possible
                r.nWidth            = lsp_max(0, sr.nMinWidth + border*2);
                r.nHeight           = lsp_max(0, sr.nMinHeight + border*2);
                sPadding.add(&r, scaling);
            }
            else if (sPolicy.get() == WP_CHILD)
            {
                // Apply size constraints of the child to the window
                sPadding.sub(&r, scaling);
                r.nWidth            = lsp_max(1, ssize_t(r.nWidth) - border*2);
                r.nHeight           = lsp_max(1, ssize_t(r.nHeight) - border*2);

                // Apply size constraints of the widget
                SizeConstraints::apply(&r, &sr);

                // Add border and padding
                r.nWidth           += border*2;
                r.nHeight          += border*2;
                sPadding.add(&r, scaling);
            }
            else
            {
                // Add border and padding
                ws::rectangle_t     xr;
                xr.nWidth           = lsp_max(0, sr.nMinWidth  + border*2);
                xr.nHeight          = lsp_max(0, sr.nMinHeight + border*2);
                sPadding.add(&r, scaling);

                // Maximize the width
                r.nWidth            = lsp_max(xr.nWidth,  r.nWidth);
                r.nHeight           = lsp_max(xr.nHeight, r.nHeight);
            }

            // Window size should be at least 1x1 pixel
            r.nWidth            = lsp_max(r.nWidth,  1);
            r.nHeight           = lsp_max(r.nHeight, 1);

            // Check if we need to resize window
            pWindow->set_size_constraints(&sr);
            if ((sSize.nWidth != r.nWidth) && (sSize.nHeight != r.nHeight))
                pWindow->resize(r.nWidth, r.nHeight);

            // Copy actual rectangle to output
            realize(&r);

            return STATUS_OK;
        }

        void Window::do_destroy()
        {
            if (pChild != NULL)
            {
                unlink_widget(pChild);
                pChild = NULL;
            }

            if (pWindow != NULL)
            {
                pWindow->destroy();
                delete pWindow;
                pWindow = NULL;
            }
        }

        void Window::destroy()
        {
            do_destroy();

            WidgetContainer::destroy();
        }

        status_t Window::tmr_redraw_request(ws::timestamp_t ts, void *args)
        {
            if (args == NULL)
                return STATUS_BAD_ARGUMENTS;

            Window *_this   = widget_ptrcast<Window>(args);
            return (_this != NULL) ? _this->do_render() : STATUS_BAD_ARGUMENTS;
        }

        status_t Window::slot_window_close(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Window *_this   = widget_ptrcast<Window>(ptr);
            return (_this != NULL) ? _this->on_close(static_cast<ws::event_t *>(data)) : STATUS_BAD_ARGUMENTS;
        }

        status_t Window::do_render()
        {
            if ((pWindow == NULL) || (!bMapped))
                return STATUS_OK;

            if (resize_pending())
                sync_size();

            if (!redraw_pending())
                return STATUS_OK;

            // call rendering
            ws::ISurface *s = pWindow->get_surface();
            if (s == NULL)
                return STATUS_OK;

            s->begin();
                render(s, nFlags & REDRAW_SURFACE);
                commit_redraw();
            s->end();

            // And also update pointer
            update_pointer();

            return STATUS_OK;
        }

        status_t Window::get_absolute_geometry(ws::rectangle_t *r)
        {
            if (pWindow == NULL)
                return STATUS_BAD_STATE;
            return pWindow->get_absolute_geometry(r);
        }

        void Window::render(ws::ISurface *s, bool force)
        {
            if (!bMapped)
                return;

            lsp::Color bg_color(sBgColor);

            if ((pChild == NULL) || (!pChild->visibility()->get()))
            {
                s->clear(bg_color);
                return;
            }

            if ((force) || (pChild->redraw_pending()))
            {
                pChild->render(s, force);
                pChild->commit_redraw();
            }

            if (force)
            {
                ws::rectangle_t cr;
                pChild->get_rectangle(&cr);

                s->fill_frame(
                    0, 0, sSize.nWidth, sSize.nHeight,
                    cr.nLeft, cr.nTop, cr.nWidth, cr.nHeight,
                    bg_color);

                float scaling   = sScaling.get();
                float border    = sBorderSize.get() * scaling;

                if (border > 0)
                {
                    float radius = sBorderRadius.get() * scaling;
                    bool aa = s->set_antialiasing(true);
                    float bw = border * 0.5f;

                    lsp::Color bc(sBorderColor);
                    bc.scale_lightness(sBrightness.get());

                    s->wire_round_rect(
                        bw + 0.5, bw + 0.5, sSize.nWidth - border-1, sSize.nHeight - border-1,
                        radius, SURFMASK_ALL_CORNER, border,
                        bc
                    );
                    s->set_antialiasing(aa);
                }
            }
        }

        status_t Window::override_pointer(bool override)
        {
            if (bOverridePointer == override)
                return STATUS_OK;
            bOverridePointer = override;
            return update_pointer();
        }

        status_t Window::update_pointer()
        {
            if (pWindow == NULL)
                return STATUS_OK;

            ws::mouse_pointer_t mp  = sPointer.get();
            if ((!bOverridePointer) && (pPointed != NULL))
                mp      = pPointed->actual_pointer();

            return (mp == pWindow->get_mouse_pointer()) ? STATUS_OK : pWindow->set_mouse_pointer(mp);
        }

        void Window::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);

            if (pWindow == NULL)
                return;

            if (sPointer.is(prop))
                update_pointer();
            if (sTitle.is(prop))
            {
                // Make formatted title of the window
                LSPString text;
                status_t res = sTitle.format(&text);
                if (res != STATUS_OK)
                    return;

                // Perform ASCII formatting
                char *ascii = text.clone_ascii();
                const char *caption = text.get_utf8();
                if (caption == NULL)
                    caption = "";

                pWindow->set_caption((ascii != NULL) ? ascii : "", caption);
                if (ascii != NULL)
                    ::free(ascii);
            }
            if (sRole.is(prop))
            {
                // Make formatted title of the window
                LSPString text;
                status_t res = sRole.format(&text);
                if (res != STATUS_OK)
                    return;
                pWindow->set_role(text.get_utf8());
            }
            if (sBorderColor.is(prop))
                query_draw();
            if (sBorderSize.is(prop) || sScaling.is(prop))
                query_resize();
            if (sBorderRadius.is(prop) || sScaling.is(prop))
                query_resize();
            if (sBorderStyle.is(prop))
                pWindow->set_border_style(sBorderStyle.get());
            if (sActions.is(prop))
                pWindow->set_window_actions(sActions.actions());
            if (sPosition.is(prop))
                pWindow->move(sPosition.left(), sPosition.top());
            if (sSizeConstraints.is(prop) || sScaling.is(prop) || (sActions.is(prop)))
            {
                ws::size_limit_t l;
                sSizeConstraints.compute(&l, sScaling.get());
                lsp_trace("Setting size constraints: w={%d, %d}, h={%d, %d}",
                        int(l.nMinWidth), int(l.nMaxWidth),
                        int(l.nMinHeight), int(l.nMaxHeight)
                    );
                pWindow->set_size_constraints(&l);
            }
            if (sWindowSize.is(prop) || sScaling.is(prop))
            {
                float scaling = lsp_max(0.0f, sScaling.get());
                if ((scaling != fScaling) && (bMapped))
                {
                    ws::rectangle_t rect;
                    ws::size_limit_t limit;
                    sWindowSize.compute(&rect, scaling);
                    sSizeConstraints.compute(&limit, scaling);

                    fScaling    = scaling;
                    pWindow->set_size_constraints(-1, -1, -1, -1);
                    pWindow->resize(rect.nWidth, rect.nHeight);
                    pWindow->set_size_constraints(&limit);
                }
                else
                    query_resize();
            }
            if (sLayout.is(prop))
            {
                if (pChild != NULL)
                    pChild->query_resize();
            }
            if (sPolicy.is(prop) || sScaling.is(prop))
                query_resize();
        }

        status_t Window::point_child(Widget *focus)
        {
            if (pPointed == focus)
                return STATUS_OK;
            pPointed    = (focus != this) ? focus : this;
            return update_pointer();
        }

        status_t Window::grab_events(ws::grab_t grab)
        {
            return (pWindow != NULL) ? pWindow->grab_events(grab) : STATUS_BAD_STATE;
        }

        status_t Window::ungrab_events()
        {
            return (pWindow != NULL) ? pWindow->ungrab_events() : STATUS_BAD_STATE;
        }

        void Window::hide_widget()
        {
            if (pWindow != NULL)
                pWindow->hide();

            WidgetContainer::hide_widget();
        }

        void Window::show_widget()
        {
            // Remember last actor and reset
            Window *wnd = pActor;
            pActor      = NULL;

            // Call parent widget to show
            WidgetContainer::show_widget();
            if (pWindow == NULL)
                return;

            // Update window parameters
            sync_size();
            update_pointer();

            // There is no actor - simple show
            if (wnd == NULL)
            {
                pWindow->show();
                return;
            }

            // Correct window location
            switch (sBorderStyle.get())
            {
                case ws::BS_DIALOG:
                {
                    ws::rectangle_t r, rw;
                    r.nLeft         = 0;
                    r.nTop          = 0;
                    r.nWidth        = 0;
                    r.nHeight       = 0;

                    rw.nLeft        = 0;
                    rw.nTop         = 0;
                    rw.nWidth       = 0;
                    rw.nHeight      = 0;

                    wnd->get_rectangle(&r);
                    pWindow->get_geometry(&rw);

                    ssize_t left    = r.nLeft + ((r.nWidth - rw.nWidth) >> 1);
                    ssize_t top     = r.nTop  + ((r.nHeight - rw.nHeight) >> 1);

                    sPosition.set(left, top);
                    break;
                }
                default:
                    break;
            }

            // Show over the actor window
            pWindow->show(wnd->pWindow);
        }

        void Window::show()
        {
            pActor      = NULL;
            return WidgetContainer::show();
        }

        void Window::show(Widget *actor)
        {
            // Already visible?
            if (sVisibility.get())
                return;

            pActor = (actor != NULL) ? widget_cast<Window>(actor->toplevel()) : NULL;
            return WidgetContainer::show();
        }

        status_t Window::add(Widget *widget)
        {
            if (pChild != NULL)
                return STATUS_ALREADY_EXISTS;

            widget->set_parent(this);
            pChild = widget;

            query_resize();

            return STATUS_OK;
        }

        status_t Window::remove(Widget *widget)
        {
            if (pChild != widget)
                return STATUS_NOT_FOUND;

            unlink_widget(pChild);
            pChild  = NULL;

            return STATUS_OK;
        }

        status_t Window::remove_all()
        {
            return (pChild != NULL) ? remove(pChild) : STATUS_OK;
        }

        status_t Window::handle_event(const ws::event_t *e)
        {
            status_t result = STATUS_OK;
            ws::event_t ev = *e;

            switch (e->nType)
            {
                case ws::UIE_FOCUS_IN:
                    result = sSlots.execute(SLOT_FOCUS_IN, this, &ev);
                    break;

                case ws::UIE_FOCUS_OUT:
                    result = sSlots.execute(SLOT_FOCUS_OUT, this, &ev);
                    break;

                case ws::UIE_SHOW:
                    lsp_trace("show event received");
                    if (!bMapped)
                    {
                        bMapped     = true;
                        sRedraw.launch(-1, 40);
                        query_draw();
                    }
                    break;

                case ws::UIE_HIDE:
                    lsp_trace("hide event received");
                    if (bMapped)
                    {
                        bMapped     = false;
                        if (pSurface != NULL)
                        {
                            pSurface->destroy();
                            delete pSurface;
                        }
                        sRedraw.cancel();
                    }
                    break;

                case ws::UIE_REDRAW:
                    query_draw(REDRAW_SURFACE);
                    break;

                case ws::UIE_CLOSE:
                    result = sSlots.execute(SLOT_CLOSE, this, &ev);
                    break;

                case ws::UIE_KEY_DOWN:
                case ws::UIE_KEY_UP:
                    lsp_trace("key event received, focus = %p", pFocus);
                    result = (pFocus != NULL) ?
                        pFocus->handle_event(e) :
                        Widget::handle_event(e);
                    break;

                case ws::UIE_RESIZE:
                    if (bMapped)
                    {
                        ws::rectangle_t r;

                        lsp_trace("resize to: %d, %d, %d, %d", int(e->nLeft), int(e->nTop), int(e->nWidth), int(e->nHeight));

                        sPosition.commit(e->nLeft, e->nTop);
                        sWindowSize.commit(e->nWidth, e->nHeight, sScaling.get());

                        r.nLeft     = e->nLeft;
                        r.nTop      = e->nTop;
                        r.nWidth    = e->nWidth;
                        r.nHeight   = e->nHeight;

                        realize(&r);
                    }
                    break;

                default:
                    result      = WidgetContainer::handle_event(e);
                    break;
            }

            // Update pointer
            update_pointer();

            return result;
        }

        Widget *Window::find_widget(ssize_t x, ssize_t y)
        {
            if (pChild == NULL)
                return NULL;

            return (pChild->inside(x, y)) ? pChild : NULL;
        }

        status_t Window::on_close(const ws::event_t *e)
        {
            return STATUS_OK;
        }

        status_t Window::on_focus_in(const ws::event_t *e)
        {
            bHasFocus = true;
            return WidgetContainer::on_focus_in(e);
        }

        status_t Window::on_focus_out(const ws::event_t *e)
        {
            bHasFocus = false;
            return WidgetContainer::on_focus_out(e);
        }

        status_t Window::focus_child(Widget *focus)
        {
            if (pFocus == focus)
                return STATUS_OK;
            else if ((focus != NULL) && (focus->toplevel() != this))
                return STATUS_BAD_HIERARCHY;

            ws::event_t ev;
            ev.nLeft        = 0;
            ev.nTop         = 0;
            ev.nWidth       = 0;
            ev.nHeight      = 0;
            ev.nCode        = 0;
            ev.nState       = 0;
            ev.nTime        = 0;

            if (pFocus != NULL)
            {
                ev.nType        = ws::UIE_FOCUS_OUT;
                Widget *f    = pFocus;
                pFocus          = NULL;
                status_t status = f->handle_event(&ev);
                if (status != STATUS_OK)
                    return status;
            }

            if (focus != NULL)
            {
                ev.nType        = ws::UIE_FOCUS_IN;
                pFocus          = focus;
                status_t status = focus->handle_event(&ev);
                if (status != STATUS_OK)
                    return status;
            }

            return STATUS_OK;
        }

        status_t Window::unfocus_child(Widget *focus)
        {
            if (pPointed == focus)
                pPointed = NULL;
            if (focus != pFocus)
                return STATUS_OK;

            ws::event_t ev;
            ev.nType        = ws::UIE_FOCUS_OUT;
            ev.nLeft        = 0;
            ev.nTop         = 0;
            ev.nWidth       = 0;
            ev.nHeight      = 0;
            ev.nCode        = 0;
            ev.nState       = 0;
            ev.nTime        = 0;

            status_t status = pFocus->handle_event(&ev);
            pFocus          = NULL;
            return status;
        }

        status_t Window::toggle_child_focus(Widget *focus)
        {
            return (focus == pFocus) ? unfocus_child(focus) : focus_child(focus);
        }

        status_t Window::set_focus(bool focus)
        {
            if (!sVisibility.get())
                return STATUS_OK;
            return (pWindow != NULL) ? pWindow->set_focus(focus) : STATUS_BAD_STATE;
        }

        status_t Window::toggle_focus()
        {
            if (!sVisibility.get())
                return STATUS_OK;
            return (pWindow != NULL) ? pWindow->toggle_focus() : STATUS_BAD_STATE;
        }

        bool Window::has_focus() const
        {
            return (sVisibility.get()) ? bHasFocus : false;
        }

        void Window::realize_widget(const ws::rectangle_t *r)
        {
            lsp_trace("width=%d, height=%d", int(r->nWidth), int(r->nHeight));
            sPosition.commit(r->nLeft, r->nTop);
            sWindowSize.commit(r->nWidth, r->nHeight, sScaling.get());

            WidgetContainer::realize_widget(r);
            if ((pChild == NULL) || (!pChild->visibility()->get()))
                return;

            // Query for size
            ws::size_limit_t sr;
            pChild->get_size_limits(&sr);

            // Calculate realize parameters
            ws::rectangle_t rc;
            float scaling       = lsp_max(sScaling.get(), 0.0f);
            float border        = sBorderSize.get() * scaling;

            rc.nLeft            = border;
            rc.nTop             = border;
            rc.nWidth           = lsp_max(0, r->nWidth  - border * 2);
            rc.nHeight          = lsp_max(0, r->nHeight - border * 2);
            sPadding.sub(&rc, scaling);
            sLayout.apply(&rc, &sr);

            // Call for realize
            pChild->realize(&rc);
        }

        void Window::size_request(ws::size_limit_t *r)
        {
            float scaling       = sScaling.get();
            float border        = sBorderSize.get() * scaling;

            r->nMinWidth        = border * 2;
            r->nMinHeight       = border * 2;
            r->nMaxWidth        = -1;
            r->nMaxHeight       = -1;

            sPadding.add(r, scaling);

            if (pChild != NULL)
            {
                ws::size_limit_t cr;
                pChild->get_size_limits(&cr);

                r->nMinWidth       += lsp_max(cr.nMinWidth, 0);
                r->nMinHeight      += lsp_max(cr.nMinHeight, 0);
            }

            // Window should be at least of 1x1 pixel size
            r->nMinWidth        = lsp_max(r->nMinWidth, 1);
            r->nMinHeight       = lsp_max(r->nMinHeight, 1);

            // Apply constraints to the window
            sSizeConstraints.apply(r, scaling);

            lsp_trace("w={%d, %d}, h={%d, %d}", int(r->nMinWidth), int(r->nMaxWidth), int(r->nMinHeight), int(r->nMaxHeight));
        }

        status_t Window::set_icon(const void *bgra, size_t width, size_t height)
        {
            if (pWindow == NULL)
                return STATUS_BAD_STATE;

            return pWindow->set_icon(bgra, width, height);
        }

        status_t Window::set_class(const char *instance, const char *wclass)
        {
            if (pWindow == NULL)
                return STATUS_BAD_STATE;
            return pWindow->set_class(instance, wclass);
        }

        status_t Window::set_class(const LSPString *instance, const LSPString *wclass)
        {
            if ((instance == NULL) || (wclass == NULL))
                return STATUS_BAD_ARGUMENTS;
            if (pWindow == NULL)
                return STATUS_BAD_STATE;
            char *i = instance->clone_ascii();
            if (i == NULL)
                return STATUS_NO_MEM;
            const char *c = instance->get_utf8();

            status_t res = (c != NULL) ? set_class(i, c) : STATUS_NO_MEM;
            ::free(i);
            return res;
        }

    } /* namespace tk */
} /* namespace lsp */
