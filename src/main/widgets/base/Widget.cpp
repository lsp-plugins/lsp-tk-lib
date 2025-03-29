/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 15 июн. 2017 г.
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

#include <lsp-plug.in/common/debug.h>
#include <lsp-plug.in/tk/helpers/draw.h>

#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(Widget, Style)
                // Bind
                sAllocation.bind("allocation", this);
                sScaling.bind("size.scaling", this);
                sFontScaling.bind("font.scaling", this);
                sBrightness.bind("brightness", this);
                sBgBrightness.bind("bg.brightness", this);
                sPadding.bind("padding", this);
                sBgColor.bind("bg.color", this);
                sBgInherit.bind("bg.inherit", this);
                sVisibility.bind("visible", this);
                sPointer.bind("pointer", this);
                sDrawMode.bind("draw.mode", this);
                // Configure
                sAllocation.set(true, false);
                sScaling.set(1.0f);
                sFontScaling.set(1.0f);
                sBrightness.set(1.0f);
                sBgBrightness.set(1.0f);
                sPadding.set_all(0);
                sBgColor.set("#cccccc");
                sBgInherit.set(false);
                sVisibility.set(true);
                sPointer.set(ws::MP_DEFAULT);
                sDrawMode.set(DM_CLASSIC);
                // Commit
                sVisibility.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(Widget, "Widget", "root");
        }

        //---------------------------------------------------------------------
        void Widget::PropListener::notify(Property *prop)
        {
            if (pWidget->valid())
                pWidget->property_changed(prop);
        }

        //---------------------------------------------------------------------
        const w_class_t Widget::metadata = { "Widget", NULL };

        Widget::Widget(Display *dpy):
            sStyle(dpy->schema(), NULL, NULL),
            sProperties(this),
            sAllocation(&sProperties),
            sScaling(&sProperties),
            sFontScaling(&sProperties),
            sBrightness(&sProperties),
            sBgBrightness(&sProperties),
            sPadding(&sProperties),
            sBgColor(&sProperties),
            sVisibility(&sProperties),
            sPointer(&sProperties),
            sTag(&sProperties),
            sDrawMode(&sProperties)
        {
            nFlags                  = REDRAW_SURFACE | SIZE_INVALID | RESIZE_PENDING;
            pClass                  = &metadata;
            pDisplay                = dpy;
            pParent                 = NULL;

            sLimit.nMinWidth        = -1;
            sLimit.nMinHeight       = -1;
            sLimit.nMaxWidth        = -1;
            sLimit.nMaxHeight       = -1;
            sLimit.nPreWidth        = -1;
            sLimit.nPreHeight       = -1;

            sSize.nLeft             = 0;
            sSize.nTop              = 0;
            sSize.nWidth            = 0;
            sSize.nHeight           = 0;
            pSurface                = NULL;
        }

        Widget::~Widget()
        {
            nFlags     |= FINALIZED;
            do_destroy();
        }

        void Widget::destroy()
        {
            nFlags     |= FINALIZED;
            do_destroy();
        }

        const char *Widget::style_class() const
        {
            return pClass->name;
        }

        bool Widget::instance_of(const w_class_t *wclass) const
        {
            const w_class_t *wc = pClass;
            while (wc != NULL)
            {
                if (wc == wclass)
                    return true;
                wc = wc->parent;
            }

            return false;
        }

        status_t Widget::init()
        {
            // Mark as initialized
            nFlags     |= INITIALIZED;

            // Initialize style
            status_t res = sStyle.init();
            if (res == STATUS_OK)
            {
                sAllocation.bind("allocation", &sStyle);
                sScaling.bind("size.scaling", &sStyle);
                sFontScaling.bind("font.scaling", &sStyle);
                sBrightness.bind("brightness", &sStyle);
                sBgBrightness.bind("bg.brightness", &sStyle);
                sPadding.bind("padding", &sStyle);
                sBgColor.bind("bg.color", &sStyle);
                sBgInherit.bind("bg.inherit", &sStyle);
                sVisibility.bind("visible", &sStyle);
                sPointer.bind("pointer", &sStyle);
                sDrawMode.bind("draw.mode", &sStyle);
            }

            // Configure the style class
            const char *ws_class = style_class();
            Style *sclass = pDisplay->schema()->get(ws_class);
            if (sclass != NULL)
            {
                LSP_STATUS_ASSERT(sStyle.set_default_parents(ws_class));
                LSP_STATUS_ASSERT(sStyle.add_parent(sclass));
            }

            // Declare slots
            handler_id_t id = 0;

            id = sSlots.add(SLOT_FOCUS_IN, slot_focus_in, self());
            if (id >= 0) id = sSlots.add(SLOT_FOCUS_OUT, slot_focus_out, self());
            if (id >= 0) id = sSlots.add(SLOT_KEY_DOWN, slot_key_down, self());
            if (id >= 0) id = sSlots.add(SLOT_KEY_UP, slot_key_up, self());
            if (id >= 0) id = sSlots.add(SLOT_MOUSE_DOWN, slot_mouse_down, self());
            if (id >= 0) id = sSlots.add(SLOT_MOUSE_UP, slot_mouse_up, self());
            if (id >= 0) id = sSlots.add(SLOT_MOUSE_MOVE, slot_mouse_move, self());
            if (id >= 0) id = sSlots.add(SLOT_MOUSE_SCROLL, slot_mouse_scroll, self());
            if (id >= 0) id = sSlots.add(SLOT_MOUSE_CLICK, slot_mouse_click, self());
            if (id >= 0) id = sSlots.add(SLOT_MOUSE_DBL_CLICK, slot_mouse_dbl_click, self());
            if (id >= 0) id = sSlots.add(SLOT_MOUSE_TRI_CLICK, slot_mouse_tri_click, self());
            if (id >= 0) id = sSlots.add(SLOT_MOUSE_IN, slot_mouse_in, self());
            if (id >= 0) id = sSlots.add(SLOT_MOUSE_OUT, slot_mouse_out, self());
            if (id >= 0) id = sSlots.add(SLOT_HIDE, slot_hide, self());
            if (id >= 0) id = sSlots.add(SLOT_SHOW, slot_show, self());
            if (id >= 0) id = sSlots.add(SLOT_DESTROY, slot_destroy, self());
            if (id >= 0) id = sSlots.add(SLOT_RESIZE, slot_resize, self());
            if (id >= 0) id = sSlots.add(SLOT_RESIZE_PARENT, slot_resize_parent, self());
            if (id >= 0) id = sSlots.add(SLOT_DRAG_REQUEST, slot_drag_request, self());
            if (id >= 0) id = sSlots.add(SLOT_REALIZED, slot_realized, self());
            if (id >= 0) id = sSlots.add(SLOT_MOUSE_POINTER, slot_mouse_pointer, self());

            return (id >= 0) ? STATUS_OK : -id;
        }

        void Widget::do_destroy()
        {
            // Remove from parent window
            Window *wnd             = widget_cast<Window>(toplevel());
            if (wnd != NULL)
                wnd->discard_widget(this);

            // Set parent widget to NULL
            set_parent(NULL);
            sStyle.destroy();

            // Destroy surface
            if (pSurface != NULL)
            {
                pSurface->destroy();
                delete pSurface;
                pSurface = NULL;
            }

            // Execute slots and unbind all to prevent duplicate on_destroy calls
            sSlots.execute(SLOT_DESTROY, this);
            sSlots.destroy();
        }

        void Widget::property_changed(Property *prop)
        {
            if (prop->one_of(sScaling, sFontScaling, sPadding, sAllocation))
                query_resize();
            if (sBrightness.is(prop))
                query_draw();
            if (prop->one_of(sBgBrightness, sBgColor, sBgInherit))
                query_draw(REDRAW_CHILD | REDRAW_SURFACE);

            if (sVisibility.is(prop))
            {
                if (sVisibility.get())
                    show_widget();
                else
                    hide_widget();
            }
        }

        bool Widget::is_visible_child_of(const Widget *parent) const
        {
            if (pParent != parent)
                return false;
            if (!valid())
                return false;

            return sVisibility.get();
        }

        void Widget::unlink_widget(Widget *w)
        {
            if (w == NULL)
                return;

            Window *wnd = widget_cast<Window>(w->toplevel());
            if (w->pParent == this)
            {
                w->pParent  = NULL;         // First remove parent
                if (wnd != NULL)
                    wnd->discard_widget(w);     // Then discard widget
            }
        }

        status_t Widget::slot_mouse_move(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = widget_ptrcast<Widget>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return _this->on_mouse_move(ev);
        }

        status_t Widget::slot_mouse_down(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = widget_ptrcast<Widget>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return _this->on_mouse_down(ev);
        }

        status_t Widget::slot_mouse_up(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = widget_ptrcast<Widget>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return _this->on_mouse_up(ev);
        }

        status_t Widget::slot_mouse_click(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = widget_ptrcast<Widget>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return _this->on_mouse_click(ev);
        }

        status_t Widget::slot_mouse_dbl_click(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = widget_ptrcast<Widget>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return _this->on_mouse_dbl_click(ev);
        }

        status_t Widget::slot_mouse_tri_click(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = widget_ptrcast<Widget>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return _this->on_mouse_tri_click(ev);
        }

        status_t Widget::slot_mouse_scroll(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = widget_ptrcast<Widget>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return _this->on_mouse_scroll(ev);
        }

        status_t Widget::slot_mouse_in(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = widget_ptrcast<Widget>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return _this->on_mouse_in(ev);
        }

        status_t Widget::slot_mouse_out(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = widget_ptrcast<Widget>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return _this->on_mouse_out(ev);
        }

        status_t Widget::slot_key_down(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = widget_ptrcast<Widget>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return _this->on_key_down(ev);
        }

        status_t Widget::slot_key_up(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = widget_ptrcast<Widget>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return _this->on_key_up(ev);
        }

        status_t Widget::slot_hide(Widget *sender, void *ptr, void *data)
        {
            if (ptr == NULL)
                return STATUS_BAD_ARGUMENTS;

            Widget *_this  = widget_ptrcast<Widget>(ptr);
            return _this->on_hide();
        }

        status_t Widget::slot_show(Widget *sender, void *ptr, void *data)
        {
            if (ptr == NULL)
                return STATUS_BAD_ARGUMENTS;

            Widget *_this  = widget_ptrcast<Widget>(ptr);
            return _this->on_show();
        }

        status_t Widget::slot_destroy(Widget *sender, void *ptr, void *data)
        {
            if (ptr == NULL)
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = widget_ptrcast<Widget>(ptr);
            return _this->on_destroy();
        }

        status_t Widget::slot_resize(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this       = widget_ptrcast<Widget>(ptr);
            ws::rectangle_t *ev = static_cast<ws::rectangle_t *>(data);
            return _this->on_resize(ev);
        }

        status_t Widget::slot_resize_parent(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this       = widget_ptrcast<Widget>(ptr);
            ws::rectangle_t *ev = static_cast<ws::rectangle_t *>(data);
            return _this->on_resize_parent(ev);
        }

        status_t Widget::slot_focus_in(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = widget_ptrcast<Widget>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return _this->on_focus_in(ev);
        }

        status_t Widget::slot_focus_out(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = widget_ptrcast<Widget>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return _this->on_focus_out(ev);
        }

        status_t Widget::slot_drag_request(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = widget_ptrcast<Widget>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            const char * const *ctype = _this->pDisplay->get_drag_mime_types();

            return _this->on_drag_request(ev, ctype);
        }

        status_t Widget::slot_realized(Widget *sender, void *ptr, void *data)
        {
            if (ptr == NULL)
                return STATUS_BAD_ARGUMENTS;

            Widget *_this  = widget_ptrcast<Widget>(ptr);
            ws::rectangle_t *r = static_cast<ws::rectangle_t *>(data);
            return _this->on_realized(r);
        }

        status_t Widget::slot_mouse_pointer(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *self = widget_ptrcast<Widget>(ptr);
            tk::pointer_event_t *pointer = static_cast<tk::pointer_event_t *>(data);
            return self->on_mouse_pointer(pointer);
        }

        bool Widget::inside(ssize_t left, ssize_t top)
        {
            if (!sVisibility.get())
                return false;

            return Position::inside(&sSize, left, top);
        }

        Widget *Widget::parent(const w_class_t *meta)
        {
            for (Widget *w=pParent; w != NULL; w = w->pParent)
            {
                if (w->instance_of(meta))
                    return w;
            }
            return NULL;
        }

        void Widget::hide_widget()
        {
            // Remove from parent window
            Window *wnd             = widget_cast<Window>(toplevel());
            if (wnd != NULL)
                wnd->discard_widget(this);

            // Drop surface to not to eat memory
            if (pSurface != NULL)
            {
                pSurface->destroy();
                delete pSurface;
                pSurface = NULL;
            }

            // Execute slot
            sSlots.execute(SLOT_HIDE, this);

            // Query draw for parent widget
            if (pParent != NULL)
                pParent->query_resize();
        }

        void Widget::show_widget()
        {
            query_resize();
            query_draw(REDRAW_CHILD | REDRAW_SURFACE);
            sSlots.execute(SLOT_SHOW, this);
        }

        Widget *Widget::find_widget(ssize_t x, ssize_t y)
        {
            return NULL;
        }

        void Widget::set_parent(Widget *parent)
        {
            if (pParent == parent)
                return;

            WidgetContainer *wc     = widget_cast<WidgetContainer>(pParent);
            Window *wnd             = widget_cast<Window>(toplevel());
            pParent                 = NULL; // Important!

            if (wc != NULL)
                wc->remove(this);
            if (wnd != NULL)
                wnd->discard_widget(this);

            // Update current parent
            pParent                 = parent;
        }

        Widget *Widget::toplevel()
        {
            Widget *p = this;
            while (p->pParent != NULL)
                p = p->pParent;

            return p;
        }

        bool Widget::has_parent(const tk::Widget *w) const
        {
            for (const Widget *p = this; p->pParent != NULL; p = p->pParent)
                if (p->pParent == w)
                    return true;

            return false;
        }

        void Widget::query_draw(size_t flags)
        {
            if (!sVisibility.get())
                return;

            // Check that flags have been changed
            flags       = nFlags | (flags & (REDRAW_CHILD | REDRAW_SURFACE));
            if (flags == nFlags)
                return;

            // Update flags and call parent
            nFlags      = flags;
            if (pParent != NULL)
                pParent->query_draw(REDRAW_CHILD);
        }

        void Widget::commit_redraw()
        {
            nFlags &= ~(REDRAW_SURFACE | REDRAW_CHILD);
        }

        void Widget::show()
        {
            sVisibility.set(true);
        }

        void Widget::hide()
        {
            sVisibility.set(false);
        }

        status_t Widget::queue_destroy()
        {
            if (pDisplay == NULL)
                return STATUS_BAD_STATE;
            return pDisplay->queue_destroy(this);
        }

        void Widget::query_resize()
        {
            if (nFlags & REALIZE_ACTIVE)
                return;

            // Update flags
            nFlags     |= (RESIZE_PENDING | SIZE_INVALID);

            if (!sVisibility.get())
                return;

            if (pParent != NULL)
                pParent->query_resize();
        }

        void Widget::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            // Get surface of widget
            ws::ISurface *src  = get_surface(s);
            if (src == NULL)
                return;

            // Render to the main surface
            s->clip_begin(area);
            lsp_finally { s->clip_end(); };
            s->draw(src, sSize.nLeft, sSize.nTop, 1.0f, 1.0f, 0.0f);
        }

        ws::ISurface *Widget::get_surface(ws::ISurface *s)
        {
            return get_surface(s, sSize.nWidth, sSize.nHeight);
        }

        ws::ISurface *Widget::get_surface(ws::ISurface *s, ssize_t width, ssize_t height)
        {
            // Create new surface if needed
            const bool redraw = create_cached_surface(&pSurface, s, width, height);
            if (pSurface == NULL)
                return s;

            // Redraw surface if required
            if ((redraw) || (nFlags & REDRAW_SURFACE))
            {
                pSurface->begin();
                    draw(pSurface, redraw);
                pSurface->end();
                nFlags         &= ~REDRAW_SURFACE;
            }

            return pSurface;
        }

        void Widget::draw(ws::ISurface *s, bool force)
        {
        }

        void Widget::realize(const ws::rectangle_t *r)
        {
            // Do not report size request on size change
            if ((sSize.nLeft == r->nLeft) &&
                (sSize.nTop  == r->nTop) &&
                (sSize.nWidth == r->nWidth) &&
                (sSize.nHeight == r->nHeight))
                return;

            // Execute slot and commit size
            ws::rectangle_t xr = *r;
            sSlots.execute(SLOT_RESIZE, this, &xr);
            sSize        = *r;
        }

        void Widget::realize_widget(const ws::rectangle_t *r)
        {
            nFlags     |= REALIZE_ACTIVE;

            // Call for realize
            realize(r);

            // Reset size pending flags
            nFlags     &= ~(SIZE_INVALID | RESIZE_PENDING | REALIZE_ACTIVE);
            query_draw();   // Always query redraw after realize()

            // Send Realized() event
            ws::rectangle_t rm = *r;
            sSlots.execute(SLOT_REALIZED, this, &rm);
        }

        void Widget::get_size_limits(ws::size_limit_t *l)
        {
            if (nFlags & SIZE_INVALID)
            {
                // Perform size request
                size_request(l);

                // Store size limit and update flags
                sLimit  = *l;
                nFlags &= ~SIZE_INVALID;
            }
            else
                *l  = sLimit;
        }

        void Widget::get_padded_size_limits(ws::size_limit_t *l)
        {
            // Get size limits and apply padding
            get_size_limits(l);
            sPadding.add(l, sScaling.get());
        }

        void Widget::get_padding(padding_t *p)
        {
            float scale     = lsp_max(0.0f, sScaling.get());
            sPadding.compute(p, scale);
        }

        void Widget::size_request(ws::size_limit_t *r)
        {
            r->nMinWidth    = -1;
            r->nMinHeight   = -1;
            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;
            r->nPreWidth    = -1;
            r->nPreHeight   = -1;
        }

        bool Widget::has_focus() const
        {
            Widget *_this = const_cast<Widget *>(this);
            Window *wnd = widget_cast<Window>(_this->toplevel());
            return (wnd != NULL) ? wnd->check_focus(_this) : false;
        }

        bool Widget::take_focus()
        {
            Window *wnd = widget_cast<Window>(toplevel());
            return (wnd != NULL) ? wnd->take_focus(this) : false;
        }

        bool Widget::kill_focus()
        {
            Window *wnd = widget_cast<Window>(toplevel());
            return (wnd != NULL) ? wnd->do_kill_focus(this) : false;
        }

        ws::mouse_pointer_t Widget::current_pointer(ssize_t x, ssize_t y)
        {
            ws::mouse_pointer_t mp = sPointer.get();
            pointer_event_t ev;

            ev.nLeft        = x;
            ev.nTop         = y;
            ev.enPointer    = mp;

            status_t res = sSlots.execute(SLOT_MOUSE_POINTER, this, &ev);
            if (res == STATUS_OK)
                mp              = ev.enPointer;

            return mp;
        }

        status_t Widget::handle_event(const ws::event_t *e)
        {
            #define FWD_EVENT(ev, slot_id) \
                case ev: \
                { \
                    ws::event_t tmp = *e; \
                    sSlots.execute(slot_id, this, &tmp); \
                    break; \
                }

            switch (e->nType)
            {
                FWD_EVENT(ws::UIE_KEY_DOWN, SLOT_KEY_DOWN )
                FWD_EVENT(ws::UIE_KEY_UP, SLOT_KEY_UP )
                FWD_EVENT(ws::UIE_MOUSE_DOWN, SLOT_MOUSE_DOWN )
                FWD_EVENT(ws::UIE_MOUSE_UP, SLOT_MOUSE_UP )
                FWD_EVENT(ws::UIE_MOUSE_IN, SLOT_MOUSE_IN )
                FWD_EVENT(ws::UIE_MOUSE_OUT, SLOT_MOUSE_OUT )
                FWD_EVENT(ws::UIE_MOUSE_MOVE, SLOT_MOUSE_MOVE )
                FWD_EVENT(ws::UIE_MOUSE_SCROLL, SLOT_MOUSE_SCROLL )
                FWD_EVENT(ws::UIE_MOUSE_CLICK, SLOT_MOUSE_CLICK )
                FWD_EVENT(ws::UIE_MOUSE_DBL_CLICK, SLOT_MOUSE_DBL_CLICK )
                FWD_EVENT(ws::UIE_MOUSE_TRI_CLICK, SLOT_MOUSE_TRI_CLICK )
                FWD_EVENT(ws::UIE_FOCUS_IN, SLOT_FOCUS_IN )
                FWD_EVENT(ws::UIE_FOCUS_OUT, SLOT_FOCUS_OUT )

                case ws::UIE_DRAG_REQUEST:
                {
                    // Call nested handler first
                    Widget *handler = find_widget(e->nLeft, e->nTop);
                    if ((handler != NULL) && (handler != this))
                        handler->handle_event(e);

                    // Handle the event if drag request is still pending
                    if (pDisplay->drag_pending())
                    {
                        ws::event_t tmp = *e;
                        sSlots.execute(SLOT_DRAG_REQUEST, this, &tmp);
                    }
                    break;
                }

                default:
                    break;
            }
            #undef FWD_EVENT

            return STATUS_OK;
        }

        status_t Widget::get_screen_rectangle(ws::rectangle_t *r, const ws::rectangle_t *sr)
        {
            *r = *sr;
            Window *wnd = widget_cast<Window>(toplevel());

            ws::rectangle_t ar;
            if ((wnd != NULL) && (wnd->get_screen_rectangle(&ar) == STATUS_OK))
            {
                r->nLeft   += ar.nLeft;
                r->nTop    += ar.nTop;
            }

            return STATUS_OK;
        }

        status_t Widget::get_screen_rectangle(ws::rectangle_t *r)
        {
            return get_screen_rectangle(r, &sSize);
        }

        status_t Widget::get_padded_screen_rectangle(ws::rectangle_t *r, const ws::rectangle_t *sr)
        {
            sPadding.leave(r, sr, sScaling.get());
            Window *wnd = widget_cast<Window>(toplevel());

            ws::rectangle_t ar;
            if ((wnd != NULL) && (wnd->get_screen_rectangle(&ar) == STATUS_OK))
            {
                r->nLeft   += ar.nLeft;
                r->nTop    += ar.nTop;
            }

            return STATUS_OK;
        }

        void Widget::get_actual_bg_color(lsp::Color *color, float brightness) const
        {
            if (brightness < 0.0f)
                brightness = sBgBrightness.get();

            if ((!sBgInherit.get()) || (pParent == NULL))
            {
                color->copy(sBgColor.color());
                color->scale_lch_luminance(brightness);
                return;
            }

            WidgetContainer *pw = widget_cast<WidgetContainer>(pParent);
            if (pw == NULL)
            {
                color->copy(sBgColor.color());
                color->scale_lch_luminance(brightness);
                return;
            }

            pw->get_child_bg_color(color);
            color->scale_lch_luminance(brightness);
        }

        void Widget::get_actual_bg_color(lsp::Color &color, float brightness) const
        {
            get_actual_bg_color(&color, brightness);
        }

        status_t Widget::get_padded_screen_rectangle(ws::rectangle_t *r)
        {
            return get_screen_rectangle(r, &sSize);
        }

        status_t Widget::on_key_down(const ws::event_t *e)
        {
            return STATUS_OK;
        }

        status_t Widget::on_key_up(const ws::event_t *e)
        {
            return STATUS_OK;
        }

        status_t Widget::on_mouse_down(const ws::event_t *e)
        {
            return STATUS_OK;
        }

        status_t Widget::on_mouse_up(const ws::event_t *e)
        {
            return STATUS_OK;
        }

        status_t Widget::on_mouse_move(const ws::event_t *e)
        {
            return STATUS_OK;
        }

        status_t Widget::on_mouse_in(const ws::event_t *e)
        {
            return STATUS_OK;
        }

        status_t Widget::on_mouse_out(const ws::event_t *e)
        {
            return STATUS_OK;
        }

        status_t Widget::on_mouse_scroll(const ws::event_t *e)
        {
            return STATUS_OK;
        }

        status_t Widget::on_mouse_click(const ws::event_t *e)
        {
            return STATUS_OK;
        }

        status_t Widget::on_mouse_dbl_click(const ws::event_t *e)
        {
            return STATUS_OK;
        }

        status_t Widget::on_mouse_tri_click(const ws::event_t *e)
        {
            return STATUS_OK;
        }

        status_t Widget::on_resize(const ws::rectangle_t *r)
        {
            return STATUS_OK;
        }

        status_t Widget::on_resize_parent(const ws::rectangle_t *r)
        {
            return STATUS_OK;
        }

        status_t Widget::on_hide()
        {
            return STATUS_OK;
        }

        status_t Widget::on_show()
        {
            return STATUS_OK;
        }

        status_t Widget::on_destroy()
        {
            return STATUS_OK;
        }

        status_t Widget::on_focus_in(const ws::event_t *e)
        {
            return STATUS_OK;
        }

        status_t Widget::on_focus_out(const ws::event_t *e)
        {
            return STATUS_OK;
        }

        status_t Widget::on_drag_request(const ws::event_t *e, const char * const *ctype)
        {
            return STATUS_OK;
        }

        status_t Widget::on_realized(const ws::rectangle_t *r)
        {
            return STATUS_OK;
        }

        status_t Widget::on_mouse_pointer(tk::pointer_event_t *ev)
        {
            return STATUS_OK;
        }

    } /* namespace tk */
} /* namespace lsp */
