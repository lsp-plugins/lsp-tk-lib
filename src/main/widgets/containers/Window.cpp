/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 16 июн. 2017 г.
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

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/stdlib/string.h>
#include <lsp-plug.in/common/debug.h>
#include <lsp-plug.in/common/status.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(Window, WidgetContainer)
                // Bind
                sBorderColor.bind("border.color", this);
                sBorderStyle.bind("border.style", this);
                sBorderSize.bind("border.size", this);
                sBorderRadius.bind("border.radius", this);
                sActions.bind("actions", this);
                sPosition.bind("position", this);
                sWindowSize.bind("size", this);
                sConstraints.bind("size.constraints", this);
                sLayout.bind("layout", this);
                sPolicy.bind("policy", this);
                // Configure
                sBorderColor.set("#000000");
                sBorderStyle.set(ws::BS_SIZEABLE);
                sBorderSize.set(0);
                sBorderRadius.set(2);
                sActions.set_actions(ws::WA_ALL);
                sPosition.set(0, 0);
                sWindowSize.set(160, 100);
                sConstraints.set(-1, -1, -1, -1);
                sLayout.set(0.0f, 0.0f, 0.0f, 0.0f);
                sPolicy.set(WP_NORMAL);
                // Override
                sVisibility.set(false);
                // Commit
                sVisibility.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(Window, "Window", "root");
        }

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
            pFocused        = NULL;
            pNativeHandle   = handle;
            bMapped         = false;
            bOverridePointer= false;
            fScaling        = 1.0f;
            pActor          = NULL;

            hMouse.nState   = 0;
            hMouse.nLeft    = 0;
            hMouse.nTop     = 0;
            hMouse.pWidget  = NULL;

            hKeys.pWidget   = NULL;

            pClass          = &metadata;
        }

        Window::~Window()
        {
            nFlags     |= FINALIZED;
            do_destroy();
        }

        status_t Window::init()
        {
            status_t result;

            // Initialize parent class
            if ((result = WidgetContainer::init()) != STATUS_OK)
                return result;

            // Initialize display
            ws::IDisplay *dpy   = pDisplay->display();
            if (dpy == NULL)
                return STATUS_BAD_STATE;

            // Create and initialize window
            pWindow     = (pNativeHandle != NULL) ? dpy->create_window(pNativeHandle) : dpy->create_window();

            if (pWindow == NULL)
                return STATUS_UNKNOWN_ERR;

            // Initialize
            if ((result = pWindow->init()) != STATUS_SUCCESS)
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
            sWindowSize.bind("size", &sStyle);
            sSizeConstraints.bind("size.constraints", &sStyle);
            sLayout.bind("layout", &sStyle);
            sPolicy.bind("policy", &sStyle);

            // Cache the actual scaling factor
            fScaling    = sScaling.get();

            // Add slot(s)
            handler_id_t id = 0;
            id = sSlots.add(SLOT_CLOSE, slot_window_close, self());
            if (id < 0)
                return - id;

            // Set self event handler
            pWindow->set_handler(this);

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
            // Request size limits of the window
            ws::size_limit_t sr;
            ws::rectangle_t r;

            get_padded_size_limits(&sr);
            float scaling       = lsp_max(0.0f, sScaling.get());
            size_t border       = lsp_max(0, sBorderSize.get()) * scaling;
            sPosition.get(&r);
            sWindowSize.compute(&r, scaling);

            // Set window's size constraints and update geometry
            if (sPolicy.get() == WP_GREEDY)
            {
                // Minimize size of window as possible
                r.nWidth            = lsp_max(0, sr.nMinWidth)  + border*2;
                r.nHeight           = lsp_max(0, sr.nMinHeight) + border*2;
            }
            else if (sPolicy.get() == WP_CHILD)
            {
                // Apply size constraints of the child to the window
                sPadding.sub(&r, scaling);
                r.nWidth            = lsp_max(1, ssize_t(r.nWidth  - border*2));
                r.nHeight           = lsp_max(1, ssize_t(r.nHeight - border*2));

                // Apply size constraints of the widget
                SizeConstraints::apply(&r, &sr);

                // Add border
                r.nWidth           += border*2;
                r.nHeight          += border*2;
            }
            else
            {
                // Add border
                ws::rectangle_t     xr;
                xr.nWidth           = lsp_max(0, sr.nMinWidth)  + border*2;
                xr.nHeight          = lsp_max(0, sr.nMinHeight) + border*2;

                // Maximize the width
                r.nWidth            = lsp_max(xr.nWidth,  r.nWidth);
                r.nHeight           = lsp_max(xr.nHeight, r.nHeight);
            }

            // Window size should be at least 1x1 pixel
            r.nWidth            = lsp_max(r.nWidth,  1);
            r.nHeight           = lsp_max(r.nHeight, 1);

            // Check if we need to resize window
//            lsp_trace("size constraints: w={%d, %d}, h={%d, %d}",
//                    int(sr.nMinWidth), int(sr.nMinHeight), int(sr.nMaxWidth), int(sr.nMaxHeight)
//            );
            pWindow->set_size_constraints(&sr);
            if ((sSize.nWidth != r.nWidth) || (sSize.nHeight != r.nHeight))
            {
                pWindow->resize(r.nWidth, r.nHeight);
                sWindowSize.commit_value(r.nWidth, r.nHeight, scaling);
            }

            // Realize widget container
            WidgetContainer::realize_widget(&r);

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
            nFlags     |= FINALIZED;
            do_destroy();
            WidgetContainer::destroy();
        }

        status_t Window::tmr_redraw_request(ws::timestamp_t sched, ws::timestamp_t ts, void *args)
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

            size_t flags = nFlags;

            ws::ISurface *bs = get_surface(s);
            bs->begin();
            {
                ws::rectangle_t xr;
                xr.nLeft    = 0;
                xr.nTop     = 0;
                xr.nWidth   = sSize.nWidth;
                xr.nHeight  = sSize.nHeight;
                render(bs, &xr, flags);
            }
            bs->end();

            s->begin();
                s->draw(bs, 0, 0);
            s->end();
            commit_redraw();

            // And also update pointer
            update_pointer();

            return STATUS_OK;
        }

        status_t Window::get_screen_rectangle(ws::rectangle_t *r)
        {
            if (pWindow == NULL)
                return STATUS_BAD_STATE;
            return pWindow->get_absolute_geometry(r);
        }

        status_t Window::get_screen_rectangle(ws::rectangle_t *r, const ws::rectangle_t *sr)
        {
            *r = *sr;
            if (pWindow == NULL)
                return STATUS_OK;

            ws::rectangle_t wr;
            status_t res = pWindow->get_absolute_geometry(&wr);
            if (res == STATUS_OK)
            {
                r->nLeft       += wr.nLeft;
                r->nTop        += wr.nTop;
            }

            return res;
        }

        status_t Window::get_padded_screen_rectangle(ws::rectangle_t *r)
        {
            if (pWindow == NULL)
                return STATUS_BAD_STATE;
            status_t res = pWindow->get_absolute_geometry(r);
            if (res == STATUS_OK)
                sPadding.enter(r, sScaling.get());
            return res;
        }

        status_t Window::get_padded_screen_rectangle(ws::rectangle_t *r, const ws::rectangle_t *sr)
        {
            *r = *sr;
            if (pWindow == NULL)
                return STATUS_OK;

            ws::rectangle_t wr;
            status_t res = pWindow->get_absolute_geometry(&wr);
            if (res == STATUS_OK)
            {
                sPadding.enter(r, sScaling.get());
                r->nLeft       += wr.nLeft;
                r->nTop        += wr.nTop;
            }
            return res;
        }

        void Window::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            if (!bMapped)
                return;

            lsp::Color bg_color;
            get_actual_bg_color(bg_color);

            if ((pChild == NULL) || (!pChild->visibility()->get()))
            {
                s->clear(bg_color);
                return;
            }

            if ((force) || (pChild->redraw_pending()))
            {
                // Draw the child only if it is visible in the area
                ws::rectangle_t xr;
                pChild->get_padded_rectangle(&xr);
                if (Size::intersection(&xr, area))
                    pChild->render(s, &xr, force);

                pChild->commit_redraw();
            }

            if (force)
            {
                ws::rectangle_t pr, cr;
                pChild->get_padded_rectangle(&pr);
                pChild->get_rectangle(&cr);

                s->fill_frame(bg_color,
                    0, 0, sSize.nWidth, sSize.nHeight,
                    pr.nLeft, pr.nTop, pr.nWidth, pr.nHeight
                );

                pChild->get_actual_bg_color(bg_color);
                s->fill_frame(bg_color, &pr, &cr);

                float scaling   = sScaling.get();
                float border    = sBorderSize.get() * scaling;

                if (border > 0)
                {
                    float radius = sBorderRadius.get() * scaling;
                    bool aa = s->set_antialiasing(true);
                    float bw = border * 0.5f;

                    lsp::Color bc(sBorderColor);
                    bc.scale_lch_luminance(sBrightness.get());

                    s->wire_round_rect_inside(
                        bc, SURFMASK_ALL_CORNER, radius,
                        bw, bw, sSize.nWidth, sSize.nHeight,
                        border
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
            if ((!bOverridePointer) && (hMouse.pWidget != NULL))
                mp      = hMouse.pWidget->current_pointer();

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
            if (sPadding.is(prop))
                query_resize();
            if (sBorderColor.is(prop))
                query_draw();
            if (sBorderSize.is(prop))
                query_resize();
            if (sBorderRadius.is(prop))
                query_resize();


            if (sBorderStyle.is(prop))
                pWindow->set_border_style(sBorderStyle.get());
            if (sActions.is(prop))
                pWindow->set_window_actions(sActions.actions());
            if (sPosition.is(prop))
                pWindow->move(sPosition.left(), sPosition.top());
            if (sSizeConstraints.is(prop) || sScaling.is(prop) || sActions.is(prop) || sFontScaling.is(prop) || sWindowSize.is(prop))
            {
//                float scaling = lsp_max(0.0f, sScaling.get());
//
//                ws::size_limit_t l;
//                sSizeConstraints.compute(&l, scaling);
//                pWindow->set_size_constraints(&l);
//
//                if ((scaling != fScaling) && (bMapped))
//                {
//                    ws::rectangle_t rect;
//                    ws::size_limit_t l;
//                    sWindowSize.compute(&rect, scaling);
//                    sSizeConstraints.compute(&l, scaling);
//
//                    fScaling    = scaling;
//                    pWindow->set_size_constraints(-1, -1, -1, -1);
//                    pWindow->resize(rect.nWidth, rect.nHeight);
//                    pWindow->set_size_constraints(&l);
////                    lsp_trace("Setting size constraints: w={%d, %d}, h={%d, %d}",
////                            int(l.nMinWidth), int(l.nMaxWidth),
////                            int(l.nMinHeight), int(l.nMaxHeight)
////                        );
//                }

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

        size_t Window::make_key_pressed(ws::code_t code)
        {
            for (size_t i=0, n=hKeys.vKeys.size(); i<n; ++i)
            {
                ws::code_t *xc = hKeys.vKeys.uget(i);
                if ((xc != NULL) && (code == *xc))
                    return hKeys.vKeys.size();
            }

            hKeys.vKeys.add(code);
            return hKeys.vKeys.size();
        }

        size_t Window::make_key_released(ws::code_t code)
        {
            for (size_t i=0; i < hKeys.vKeys.size(); )
            {
                ws::code_t *xc = hKeys.vKeys.uget(i);
                if ((xc != NULL) && (code == *xc))
                    hKeys.vKeys.remove(i);
                else
                    ++i;
            }

            return hKeys.vKeys.size();
        }

        status_t Window::handle_event(const ws::event_t *e)
        {
            status_t result = STATUS_OK;
            ws::event_t ev = *e;

            switch (e->nType)
            {
                //-------------------------------------------------------------
                // Window state and geometry handling
                case ws::UIE_SHOW:
//                    lsp_trace("show event received");
                    if (!bMapped)
                    {
                        bMapped     = true;
                        sRedraw.launch(-1, 40);
                        query_draw(REDRAW_SURFACE);
                    }
                    break;

                case ws::UIE_HIDE:
//                    lsp_trace("hide event received");
                    if (bMapped)
                    {
                        bMapped     = false;
                        if (pSurface != NULL)
                        {
                            pSurface->destroy();
                            delete pSurface;
                            pSurface = NULL;
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

                case ws::UIE_RESIZE:
                    if (bMapped)
                    {
                        ws::rectangle_t r;

                        if (!(nFlags & RESIZE_PENDING))
                        {
                            lsp_trace("resize to: l=%d, t=%d, w=%d, h=%d", int(e->nLeft), int(e->nTop), int(e->nWidth), int(e->nHeight));

                            r.nLeft     = e->nLeft;
                            r.nTop      = e->nTop;
                            r.nWidth    = e->nWidth;
                            r.nHeight   = e->nHeight;

                            sPosition.commit_value(r.nLeft, r.nTop);
                            sWindowSize.commit_value(r.nWidth, r.nHeight, sScaling.get());

                            realize_widget(&r);
                        }
                    }
                    break;

                //-------------------------------------------------------------
                // Mouse handling
                case ws::UIE_MOUSE_UP:
                {
//                    lsp_trace("e->nCode = %d, e->nState=0x%x state = 0x%x",
//                            int(e->nCode), int(e->nState), int(hMouse.nState));
                    Widget *h       = acquire_mouse_handler(e);
                    hMouse.nState  &= ~(1 << e->nCode);
                    hMouse.nLeft    = e->nLeft;
                    hMouse.nTop     = e->nTop;

                    if (h == this)
                        result          = WidgetContainer::handle_event(e);
                    else if (h != NULL)
                        result          = h->handle_event(e);

                    release_mouse_handler(e, true);
                    break;
                }

                case ws::UIE_MOUSE_DOWN:
                {
//                    lsp_trace("e->nCode = %d, e->nState=0x%x state = 0x%x",
//                            int(e->nCode), int(e->nState), int(hMouse.nState));
                    Widget *h       = acquire_mouse_handler(e);
                    hMouse.nState  |= (1 << e->nCode);
                    hMouse.nLeft    = e->nLeft;
                    hMouse.nTop     = e->nTop;

                    // Take focus first
                    take_focus(h);

                    // Bypass event to handler
                    if (h == this)
                        result          = WidgetContainer::handle_event(e);
                    else if (h != NULL)
                        result          = h->handle_event(e);

                    break;
                }

                case ws::UIE_MOUSE_MOVE:
                {
//                    lsp_trace("e->nCode = %d, e->nState=0x%x state = 0x%x",
//                            int(e->nCode), int(e->nState), int(hMouse.nState));
                    Widget *h       = acquire_mouse_handler(e);
                    hMouse.nState   = e->nState;
                    hMouse.nLeft    = e->nLeft;
                    hMouse.nTop     = e->nTop;

                    if (h == this)
                        result          = WidgetContainer::handle_event(e);
                    else if (h != NULL)
                        result          = h->handle_event(e);
                    break;
                };

                case ws::UIE_MOUSE_CLICK:
                case ws::UIE_MOUSE_DBL_CLICK:
                case ws::UIE_MOUSE_TRI_CLICK:
                case ws::UIE_MOUSE_SCROLL:
                {
                    Widget *h = acquire_mouse_handler(e);
                    if (h == this)
                        result          = WidgetContainer::handle_event(e);
                    else if (h != NULL)
                        result          = h->handle_event(e);

                    release_mouse_handler(e, true);
                    break;
                }

                //-------------------------------------------------------------
                // Keyboard handling
                case ws::UIE_KEY_DOWN:
                {
                    // Find the keyboard event handler
                    lsp_trace("key down: keys.pWidget = %p, pFocused = %p, nkeys=%d",
                        hKeys.pWidget, pFocused, int(hKeys.vKeys.size()));
                    Widget *h       = pFocused; //(hKeys.pWidget != NULL) ? hKeys.pWidget : pFocused;
                    if (h == NULL)
                        h               = find_widget(e->nLeft, e->nTop);

                    // Take focus first and acquire keyboard lock
                    make_key_pressed(e->nCode);
                    hKeys.pWidget       = h;

                    // Handle key press event
                    if (h == this)
                        result          = WidgetContainer::handle_event(e);
                    else if (h != NULL)
                        result          = h->handle_event(e);

                    break;
                }

                case ws::UIE_KEY_UP:
                {
                    // Find the keyboard event handler
                    Widget *h       = hKeys.pWidget;

                    // Release key lock state
                    if (make_key_released(e->nCode) <= 0)
                        hKeys.pWidget       = NULL;

                    lsp_trace("key up  : keys.pWidget = %p, pFocused = %p, nkeys=%d",
                            hKeys.pWidget, pFocused, int(hKeys.vKeys.size()));

                    // Handle key press event
                    if (h == this)
                        result          = WidgetContainer::handle_event(e);
                    else if (h != NULL)
                        result          = h->handle_event(e);

                    break;
                }

                //-------------------------------------------------------------
                // Drag & Drop
                case ws::UIE_DRAG_REQUEST:
                {
                    Widget *h   = find_widget(e->nLeft, e->nTop);
                    if (h == this)
                        WidgetContainer::handle_event(e);
                    else if (h != NULL)
                        h->handle_event(e);
                    break;
                }

                //-------------------------------------------------------------
                // Focus events
                case ws::UIE_FOCUS_OUT:
                {
                    Widget *h = hMouse.pWidget;
                    if (h != NULL)
                    {
                        // Create UIE_MOUSE_OUT event
                        ws::event_t ev;
                        ws::init_event(&ev);
                        ev.nType        = ws::UIE_MOUSE_OUT;
                        ev.nLeft        = hMouse.nLeft;
                        ev.nTop         = hMouse.nLeft;

                        // Reset state of mouse handler
                        hMouse.nState   = 0;
                        hMouse.nLeft    = 0;
                        hMouse.nTop     = 0;
                        hMouse.pWidget  = NULL;

                        // Send event to the handler
                        h->handle_event(&ev);
                    }

                    kill_focus(pFocused);
                    break;
                }

                case ws::UIE_MOUSE_IN:
                {
                    Widget *w   = hMouse.pWidget;
                    if (!(hMouse.nState & ws::MCF_BTN_MASK))
                    {
                        w = acquire_mouse_handler(e);
                        hMouse.nState   = e->nState;
                        hMouse.nLeft    = e->nLeft;
                        hMouse.nTop     = e->nTop;
                    }

                    if (w != this)
                        WidgetContainer::handle_event(e);

                    break;
                }

                case ws::UIE_MOUSE_OUT:
                {
                    Widget *w   = hMouse.pWidget;
                    if (!(hMouse.nState & ws::MCF_BTN_MASK))
                        release_mouse_handler(e, false);
                    if (w != this)
                        WidgetContainer::handle_event(e);
                    break;
                }

                //-------------------------------------------------------------
                // Other events
                default:
                    result      = WidgetContainer::handle_event(e);
                    break;
            }

            // Update pointer
            update_pointer();

            return result;
        }

        status_t Window::resize_window(const ws::rectangle_t *size)
        {
            sPosition.set(size->nLeft, size->nTop);
            sWindowSize.set(size->nWidth, size->nHeight, sScaling.get());

            return STATUS_OK;
        }

        status_t Window::resize_window(ssize_t width, ssize_t height)
        {
            sWindowSize.set(width, height, sScaling.get());
            return STATUS_OK;
        }

        Widget *Window::sync_mouse_handler(const ws::event_t *e, bool lookup)
        {
            // Update current widget
            Widget *old     = hMouse.pWidget;
            Widget *child   = (lookup) ? find_widget(e->nLeft, e->nTop) : NULL;
            if (child == old)
                return old;

            hMouse.pWidget  = child;

            // Send UIE_MOUSE_OUT to previous handler
            if (old != NULL)
            {
                ws::event_t ev;
                init_event(&ev);
                ev.nType    = ws::UIE_MOUSE_OUT;
                ev.nLeft    = e->nLeft;
                ev.nTop     = e->nTop;

                if (old != this)
                    old->handle_event(&ev);
                else
                    WidgetContainer::handle_event(&ev);
            }

            // Send UIE_MOUSE_IN to current handler
            if (child != NULL)
            {
                ws::event_t ev;
                init_event(&ev);
                ev.nType    = ws::UIE_MOUSE_IN;
                ev.nLeft    = e->nLeft;
                ev.nTop     = e->nTop;

                if (child != this)
                    child->handle_event(&ev);
                else
                    WidgetContainer::handle_event(&ev);
            }

            return child;
        }

        Widget *Window::acquire_mouse_handler(const ws::event_t *e)
        {
            // Check that we work in exclusive mode
            Widget *old = hMouse.pWidget;
            if ((hMouse.nState & ws::MCF_BTN_MASK) && (old))
                return old;

            return sync_mouse_handler(e, true);
        }

        Widget *Window::release_mouse_handler(const ws::event_t *e, bool lookup)
        {
            Widget *old = hMouse.pWidget;

            if (old == NULL)
            {
                hMouse.nState   = 0;
                return old;
            }

            if (hMouse.nState & ws::MCF_BTN_MASK)
                return old;

            return sync_mouse_handler(e, lookup);
        }

        bool Window::take_focus(Widget *w)
        {
            // Change focus
            Widget *old = pFocused;
            if (w == old)
                return false;
            pFocused    = w;
            lsp_trace("take_focus: %p", pFocused);

            // Notify previous focus holder about focus change
            if (old != NULL)
            {
                ws::event_t ev;
                ws::init_event(&ev);
                ev.nType        = ws::UIE_FOCUS_OUT;
                old->handle_event(&ev);
            }

            // Notify new focus holder about focus change
            if (w != NULL)
            {
                ws::event_t ev;
                ws::init_event(&ev);
                ev.nType        = ws::UIE_FOCUS_IN;
                w->handle_event(&ev);
            }
            return true;
        }

        bool Window::kill_focus(Widget *w)
        {
            // Check that widget owns focus
            if (w != pFocused)
                return false;
            pFocused    = NULL;

            // Notify previous focus holder about focus change
            if (w != NULL)
            {
                ws::event_t ev;
                ws::init_event(&ev);
                ev.nType        = ws::UIE_FOCUS_OUT;
                w->handle_event(&ev);
            }
            return true;
        }

        Widget *Window::find_widget(ssize_t x, ssize_t y)
        {
            if ((pChild == NULL) || (!pChild->valid()) || (!pChild->inside(x, y)))
                return this;

            Widget *curr = pChild;
            while (true)
            {
                Widget *next = curr->find_widget(x, y);
                if (next == NULL)
                    return curr;
                curr    = next;
            }
        }

        status_t Window::on_close(const ws::event_t *e)
        {
            return STATUS_OK;
        }

        void Window::realize(const ws::rectangle_t *r)
        {
//            lsp_trace("width=%d, height=%d", int(r->nWidth), int(r->nHeight));
            sPosition.commit_value(r->nLeft, r->nTop);
            sWindowSize.commit_value(r->nWidth, r->nHeight, sScaling.get());

            WidgetContainer::realize(r);
            if ((pChild == NULL) || (!pChild->visibility()->get()))
                return;

            // Query for size
            ws::size_limit_t sr;
            float scaling       = lsp_max(sScaling.get(), 0.0f);
            size_t border       = lsp_max(0, sBorderSize.get()) * scaling;

            pChild->get_padded_size_limits(&sr);

            // Compute size of window without border
            ws::rectangle_t rc  = *r;
            rc.nLeft            = border;
            rc.nTop             = border;

            rc.nWidth           = lsp_max(0, ssize_t(r->nWidth  - border*2));
            rc.nHeight          = lsp_max(0, ssize_t(r->nHeight - border*2));

            // Exclude padding
            sPadding.enter(&rc, scaling);
            sLayout.apply(&rc, &sr); // Apply layout

            // Call for realize
            pChild->padding()->enter(&rc, pChild->scaling()->get());
            pChild->realize_widget(&rc);
        }

        void Window::discard_widget(Widget *w)
        {
            if (w == NULL)
                return;

            // Kill focus on the widget
            kill_focus(w);

            // Send UIE_MOUSE_OUT and discard mouse handler
            Widget *old = hMouse.pWidget;
            if (old == w)
            {
                ws::event_t ev;
                ws::init_event(&ev);
                ev.nType            = ws::UIE_MOUSE_OUT;
                ev.nLeft            = hMouse.nLeft;
                ev.nTop             = hMouse.nTop;
                hMouse.pWidget      = NULL;

                old->handle_event(&ev);

                if ((valid()) && (sVisibility.get()))
                    sync_mouse_handler(&ev, true);
            }
        }

        void Window::size_request(ws::size_limit_t *r)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());
            size_t border       = lsp_max(0, sBorderSize.get()) * scaling;

            r->nMinWidth        = border * 2;
            r->nMinHeight       = border * 2;
            r->nMaxWidth        = -1;
            r->nMaxHeight       = -1;
            r->nPreWidth        = -1;
            r->nPreHeight       = -1;

            if ((pChild != NULL) && (pChild->visibility()->get()))
            {
                ws::size_limit_t cr;
                pChild->get_padded_size_limits(&cr);

                r->nMinWidth       += lsp_max(cr.nMinWidth, 0);
                r->nMinHeight      += lsp_max(cr.nMinHeight, 0);
            }

            // Window should be at least of 1x1 pixel size
            r->nMinWidth        = lsp_max(r->nMinWidth, 0);
            r->nMinHeight       = lsp_max(r->nMinHeight, 0);

            // Apply constraints to the window
            sSizeConstraints.apply(r, scaling);

//            lsp_trace("this=%p, w={%d, %d}, h={%d, %d}", this,
//                    int(r->nMinWidth), int(r->nMaxWidth), int(r->nMinHeight), int(r->nMaxHeight));
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

        bool Window::take_focus()
        {
            if (pWindow == NULL)
                return false;
            return pWindow->set_focus(true) == STATUS_OK;
        }

        bool Window::kill_focus()
        {
            if (pWindow == NULL)
                return false;
            return pWindow->set_focus(false) == STATUS_OK;
        }

        bool Window::has_parent() const
        {
            return (pWindow != NULL) ? pWindow->has_parent() : false;
        }

    } /* namespace tk */
} /* namespace lsp */
