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
            sSize(&sProperties),
            sSizeConstraints(&sProperties)
        {
            lsp_trace("native_handle = %p", handle);

            pWindow         = NULL;
            pChild          = NULL;
            pNativeHandle   = handle;
            nScreen         = screen;
            pFocus          = NULL;
            pPointed        = NULL;
            bHasFocus       = false;
            bOverridePointer= false;
            bMapFlag        = false;
            bSizeRequest    = true;
            nVertPos        = 0.5f;
            nHorPos         = 0.5f;
            nVertScale      = 0.0f;
            nHorScale       = 0.0f;

            enPolicy        = WP_NORMAL;

            nFlags         &= ~F_VISIBLE;
            pClass          = &metadata;
        }

        Window::~Window()
        {
            do_destroy();
        }

        status_t Window::init()
        {
            // Initialize redraw timer
            ws::IDisplay *dpy   = pDisplay->display();
            if (dpy == NULL)
                return STATUS_BAD_STATE;

            // Create and initialize window
            pWindow     = (pNativeHandle != NULL) ? dpy->create_window(pNativeHandle) :
                          (nScreen >= 0) ? dpy->create_window(nScreen) :
                          dpy->create_window();

            if (pWindow == NULL)
                return STATUS_UNKNOWN_ERR;

            // Initialize
            status_t result = pWindow->init();
            if (result != STATUS_SUCCESS)
            {
                destroy();
                return result;
            }
            pWindow->set_handler(this);

            // Initialize parent class
            if ((result = WidgetContainer::init()) != STATUS_OK)
                return result;

            // Bind properties
            sTitle.bind(&sStyle, pDisplay->dictionary());
            sRole.bind(&sStyle, pDisplay->dictionary());
            sBorderColor.bind("border.color", &sStyle);
            sBorderStyle.bind("border.style", &sStyle);
            sBorderSize.bind("border.size", &sStyle);
            sBorderRadius.bind("border.radius", &sStyle);
            sActions.bind("actions", &sStyle);
            sPosition.bind("position", &sStyle);
            sSize.bind("size", &sStyle);
            sSizeConstraints.bind("size.constraints", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sBorderColor.init(sclass, "#000000");
                sBorderStyle.init(sclass, ws::BS_SIZEABLE);
                sBorderSize.init(sclass, 0);
                sBorderRadius.init(sclass, 2);
                sActions.init(sclass, ws::WA_ALL);
                sPosition.init(sclass, 0, 0);
                sSize.init(sclass, 160, 100);
                sSizeConstraints.init(sclass, -1, -1, -1, -1);
            }

            // Add slot(s)
            handler_id_t id = 0;
            id = sSlots.add(SLOT_CLOSE, slot_window_close, self());
            if (id < 0)
                return - id;

            // Bind redraw handler
            sRedraw.bind(dpy);
            sRedraw.set_handler(tmr_redraw_request, self());

            lsp_trace("Window has been initialized");

            return STATUS_OK;
        }

        status_t Window::sync_size()
        {
            // Request size
            ws::size_limit_t sr;
            sr.nMinWidth    = -1;
            sr.nMinHeight   = -1;
            sr.nMaxWidth    = -1;
            sr.nMaxHeight   = -1;
            size_request(&sr);

            // Set window's size constraints and update geometry
            pWindow->set_size_constraints(&sr);
            ws::rectangle_t r = sRectangle;
            if (enPolicy == WP_GREEDY)
            {
                if (sr.nMinWidth > 0)
                    r.nWidth        = sr.nMinWidth;
                if (sr.nMinHeight > 0)
                    r.nHeight       = sr.nMinHeight;
            }
            else
            {
                // Check whether window matches constraints
                if ((sr.nMaxWidth > 0) && (r.nWidth > sr.nMaxWidth))
                    r.nWidth        = sr.nMaxWidth;
                if ((sr.nMaxHeight > 0) && (r.nHeight > sr.nMaxHeight))
                    r.nHeight       = sr.nMaxHeight;

                if ((sr.nMinWidth > 0) && (r.nWidth < sr.nMinWidth))
                    r.nWidth        = sr.nMinWidth;
                if ((sr.nMinHeight > 0) && (r.nHeight < sr.nMinHeight))
                    r.nHeight       = sr.nMinHeight;
            }

            if ((sRectangle.nWidth != r.nWidth) && (sRectangle.nHeight != r.nHeight))
                pWindow->resize(r.nWidth, r.nHeight);

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
            if (pWindow == NULL)
                return STATUS_OK;

            if (bSizeRequest)
            {
                lsp_trace("Synchronizing size");
                sync_size();
                bSizeRequest    = false;
                query_draw(REDRAW_CHILD | REDRAW_SURFACE);
                realize(&sRectangle);
            }

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

        void Window::query_resize()
        {
            bSizeRequest = true;
        }

        status_t Window::get_absolute_geometry(ws::rectangle_t *r)
        {
            if (pWindow == NULL)
                return STATUS_BAD_STATE;
            return pWindow->get_absolute_geometry(r);
        }

        void Window::render(ws::ISurface *s, bool force)
        {
            lsp::Color bg_color(sBgColor);

            if (pChild == NULL)
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
                    0, 0, sRectangle.nWidth, sRectangle.nHeight,
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
                        bw + 0.5, bw + 0.5, sRectangle.nWidth - border-1, sRectangle.nHeight - border-1,
                        radius, SURFMASK_ALL_CORNER, border,
                        bc
                    );
                    s->set_antialiasing(aa);
                }
            }
        }

        status_t Window::set_cursor(ws::mouse_pointer_t mp)
        {
            WidgetContainer::set_cursor(mp);
            return update_pointer();
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

            ws::mouse_pointer_t mp  = enCursor;
            if ((!bOverridePointer) && (pPointed != NULL))
                mp      = pPointed->active_cursor();

            if (mp == pWindow->get_mouse_pointer())
                return STATUS_OK;

            return pWindow->set_mouse_pointer(mp);
        }

        void Window::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);

            if (sSize.is(prop))
                query_resize();

            if (pWindow != NULL)
            {
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
                if (sSize.is(prop))
                    query_resize();
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
            }
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

        void Window::set_policy(window_poilicy_t policy)
        {
            window_poilicy_t old = enPolicy;
            enPolicy = policy;
            if ((old == policy) || (!(nFlags & F_VISIBLE)))
                return;

            query_resize();
        }

        bool Window::hide()
        {
            sRedraw.cancel();
            if (pWindow != NULL)
                pWindow->hide();

            if (!(nFlags & F_VISIBLE))
                return false;
            nFlags &= ~F_VISIBLE;

            // Drop surface to not to eat memory
            if (pSurface != NULL)
            {
                pSurface->destroy();
                delete pSurface;
                pSurface = NULL;
            }

            // Query draw for parent widget
            if (pParent != NULL)
                pParent->query_resize();

            return true;
        }

        bool Window::show()
        {
            return show(NULL);
        }

        bool Window::show(Widget *actor)
        {
            if (nFlags & F_VISIBLE)
                return false;

            nFlags |= F_VISIBLE;
            if (pParent != NULL)
                pParent->query_resize();

            if (pWindow == NULL)
            {
                sSlots.execute(SLOT_SHOW, this);
                return true;
            }

            // Evaluate layering
            Window *wnd = (actor != NULL) ? widget_cast<Window>(actor->toplevel()) : NULL;

            // Update window parameters
            sync_size();
            update_pointer();

            // Launch redraw timer
            sRedraw.launch(-1, 40);
            query_draw();

            // Show window
            if (wnd != NULL)
            {
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

                pWindow->show(wnd->pWindow);
            }
            else
                pWindow->show();

            return true;
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
                    sRedraw.launch(-1, 40);
                    query_draw();
                    if (bMapFlag != bool(nFlags & F_VISIBLE))
                    {
                        lsp_trace("SHOW ptr=%p", this);
                        result      = sSlots.execute(SLOT_SHOW, this, &ev);
                        bMapFlag    = nFlags & F_VISIBLE;
                    }
                    break;

                case ws::UIE_HIDE:
                    sRedraw.cancel();
                    if (bMapFlag != bool(nFlags & F_VISIBLE))
                    {
                        lsp_trace("HIDE ptr=%p", this);
                        result      = sSlots.execute(SLOT_HIDE, this, &ev);
                        bMapFlag    = nFlags & F_VISIBLE;
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
                {
                    ws::rectangle_t r;
//                    result = pWindow->get_geometry(&r);
                    lsp_trace("resize to: %d, %d, %d, %d", int(e->nLeft), int(e->nTop), int(e->nWidth), int(e->nHeight));
                    r.nLeft     = e->nLeft;
                    r.nTop      = e->nTop;
                    r.nWidth    = e->nWidth;
                    r.nHeight   = e->nHeight;
                    if (result == STATUS_OK)
                        this->realize(&r);
                    break;
                }

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

            ws::rectangle_t r;
            pChild->get_rectangle(&r);

            if (x < r.nLeft)
                return NULL;
            if (y < r.nTop)
                return NULL;
            if (x >= (r.nLeft + r.nWidth))
                return NULL;
            if (y >= (r.nTop + r.nHeight))
                return NULL;

            return pChild;
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
            if (!visible())
                return STATUS_OK;
            return (pWindow != NULL) ? pWindow->set_focus(focus) : STATUS_BAD_STATE;
        }

        status_t Window::toggle_focus()
        {
            if (!visible())
                return STATUS_OK;
            return (pWindow != NULL) ? pWindow->toggle_focus() : STATUS_BAD_STATE;
        }

        bool Window::has_focus() const
        {
            return (visible()) ? bHasFocus : false;
        }

        void Window::realize(const ws::rectangle_t *r)
        {
            lsp_trace("width=%d, height=%d", int(r->nWidth), int(r->nHeight));
            WidgetContainer::realize(r);
            bSizeRequest        = false;

            if (pChild == NULL)
                return;

            // Query for size
            ws::size_limit_t sr;
            pChild->get_size_limit(&sr);

            // Calculate realize parameters
            ws::rectangle_t rc;
            float scaling       = sScaling.get();
            size_t border       = sBorderSize.get() * scaling;

            // Dimensions
            ssize_t xs          = r->nWidth  - sPadding.horizontal() - border * 2;
            ssize_t ys          = r->nHeight - sPadding.vertical() - border * 2;

            if ((sr.nMinWidth >= 0) && (sr.nMinWidth > xs))
            {
                rc.nLeft            = border + sPadding.left();
                rc.nWidth           = sr.nMinWidth;
            }
            else if (sr.nMaxWidth < 0)
            {
                rc.nLeft            = border + sPadding.left();
                rc.nWidth           = xs;
            }
            else
            {
                rc.nWidth           = (sr.nMinWidth >= 0)   ? sr.nMinWidth  + (xs - sr.nMinWidth)   * nHorScale     : xs * nHorScale;
                if (rc.nWidth > xs)
                    rc.nWidth           = xs;
                xs                 -= rc.nWidth;
                rc.nLeft            = border + sPadding.left() + xs * nHorPos;
            }

            if ((sr.nMinHeight >= 0) && (sr.nMinHeight > ys))
            {
                rc.nTop             = border + sPadding.top();
                rc.nHeight          = sr.nMinHeight;
            }
            else if (sr.nMaxHeight < 0)
            {
                rc.nTop             = border + sPadding.top();
                rc.nHeight          = ys;
            }
            else
            {
                rc.nHeight          = (sr.nMinHeight >= 0)  ? sr.nMinHeight + (ys - sr.nMinHeight)  * nVertScale    : ys * nVertScale;
                if (rc.nHeight > ys)
                    rc.nHeight          = ys;
                ys                 -= rc.nHeight;
                rc.nTop             = border + sPadding.top() + ys * nVertPos;
            }

            // Call for realize
            pChild->realize(&rc);
            pChild->query_draw();
        }

        void Window::size_request(ws::size_limit_t *r)
        {
            padding_t pad;
            float scaling       = sScaling.get();
            sPadding.compute(&pad, scaling);
            size_t border       = sBorderSize.get();

            r->nMinWidth        = pad.nLeft + pad.nRight + border * 2;
            r->nMinHeight       = pad.nTop + pad.nBottom + border * 2;
            r->nMaxWidth        = -1;
            r->nMaxHeight       = -1;

            if (pChild != NULL)
            {
                ws::size_limit_t cr;
                pChild->get_size_limit(&cr);

                r->nMinWidth       += lsp_max(cr.nMinWidth, 0);
                r->nMinHeight      += lsp_max(cr.nMinHeight, 0);
            }

            // Apply constraints to the window
            sSizeConstraints.apply(r, scaling);
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

        status_t Window::set_role(const char *role)
        {
            if (pWindow == NULL)
                return STATUS_BAD_STATE;
            return pWindow->set_role(role);
        }

        status_t Window::set_role(const LSPString *role)
        {
            return set_role(role->get_utf8());
        }

    } /* namespace tk */
} /* namespace lsp */
