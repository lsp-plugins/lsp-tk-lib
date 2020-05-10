/*
 * Widget.cpp
 *
 *  Created on: 15 июн. 2017 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/base.h>

namespace lsp
{
    namespace tk
    {
        void Widget::PropListener::notify(Property *prop)
        {
            pWidget->property_changed(prop);
        }

        const w_class_t Widget::metadata = { "Widget", NULL };

        Widget::Widget(Display *dpy):
            sProperties(this),
            sScaling(&sProperties),
            sBrightness(&sProperties),
            sPadding(&sProperties),
            sBgColor(&sProperties)
        {
            pDisplay            = dpy;
            pSurface            = NULL;
            pParent             = NULL;

            pUID                = NULL;
            enCursor            = ws::MP_DEFAULT;
            sRealize.nLeft      = 0;
            sRealize.nTop       = 0;
            sRealize.nWidth     = 0;
            sRealize.nHeight    = 0;
            nFlags              = REDRAW_SURFACE | F_VISIBLE | F_HFILL | F_VFILL;
            pClass              = &metadata;
        }

        Widget::~Widget()
        {
            do_destroy();
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
            status_t res = sStyle.init();
            if (res == STATUS_OK)
            {
                sScaling.bind("scaling", this);
                sBrightness.bind("brightness", this);
                sPadding.bind("padding", this);
                sBgColor.bind("bg_color", this);
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

            return (id >= 0) ? STATUS_OK : -id;
        }

        void Widget::do_destroy()
        {
            // Set parent widget to NULL
            set_parent(NULL);

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

            // Destroy widget identifier
            if (pUID != NULL)
                ::free(pUID);
            pUID = NULL;
        }

        void Widget::property_changed(Property *prop)
        {
            if (sScaling.is(prop))
                query_resize();
            if (sBrightness.is(prop))
                query_draw();
            if (sPadding.is(prop))
                query_resize();
            if (sBgColor.is(prop))
                query_draw();
        }

        void Widget::unlink_widget(Widget *w)
        {
            if (w == NULL)
                return;
            if (w->pParent == this)
                w->pParent  = NULL;
        }

        status_t Widget::slot_mouse_move(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = static_cast<Widget *>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return _this->on_mouse_move(ev);
        }

        status_t Widget::slot_mouse_down(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = static_cast<Widget *>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return _this->on_mouse_down(ev);
        }

        status_t Widget::slot_mouse_up(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = static_cast<Widget *>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return _this->on_mouse_up(ev);
        }

        status_t Widget::slot_mouse_dbl_click(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = static_cast<Widget *>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return _this->on_mouse_dbl_click(ev);
        }

        status_t Widget::slot_mouse_tri_click(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = static_cast<Widget *>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return _this->on_mouse_tri_click(ev);
        }

        status_t Widget::slot_mouse_scroll(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = static_cast<Widget *>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return _this->on_mouse_scroll(ev);
        }

        status_t Widget::slot_mouse_in(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = static_cast<Widget *>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return _this->on_mouse_in(ev);
        }

        status_t Widget::slot_mouse_out(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = static_cast<Widget *>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return _this->on_mouse_out(ev);
        }

        status_t Widget::slot_key_down(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = static_cast<Widget *>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return _this->on_key_down(ev);
        }

        status_t Widget::slot_key_up(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = static_cast<Widget *>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return _this->on_key_up(ev);
        }

        status_t Widget::slot_hide(Widget *sender, void *ptr, void *data)
        {
            if (ptr == NULL)
                return STATUS_BAD_ARGUMENTS;

            Widget *_this  = static_cast<Widget *>(ptr);
            return _this->on_hide();
        }

        status_t Widget::slot_show(Widget *sender, void *ptr, void *data)
        {
            if (ptr == NULL)
                return STATUS_BAD_ARGUMENTS;

            Widget *_this  = static_cast<Widget *>(ptr);
            return _this->on_show();
        }

        status_t Widget::slot_destroy(Widget *sender, void *ptr, void *data)
        {
            if (ptr == NULL)
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = static_cast<Widget *>(ptr);
            return _this->on_destroy();
        }

        status_t Widget::slot_resize(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = static_cast<Widget *>(ptr);
            realize_t *ev   = static_cast<realize_t *>(data);
            return _this->on_resize(ev);
        }

        status_t Widget::slot_resize_parent(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = static_cast<Widget *>(ptr);
            realize_t *ev   = static_cast<realize_t *>(data);
            return _this->on_resize_parent(ev);
        }

        status_t Widget::slot_focus_in(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = static_cast<Widget *>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return _this->on_focus_in(ev);
        }

        status_t Widget::slot_focus_out(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = static_cast<Widget *>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return _this->on_focus_out(ev);
        }

        status_t Widget::slot_drag_request(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Widget *_this   = static_cast<Widget *>(ptr);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            const char * const *ctype = _this->pDisplay->get_drag_mime_types();

            return _this->on_drag_request(ev, ctype);
        }

        ssize_t Widget::relative_left() const
        {
            return sRealize.nLeft - ((pParent != NULL) ? pParent->left() : 0);
        }

        ssize_t Widget::relative_right() const
        {
            return sRealize.nLeft - ((pParent != NULL) ? pParent->left() : 0) + sRealize.nWidth;
        }

        ssize_t Widget::relative_top() const
        {
            return sRealize.nTop - ((pParent != NULL) ? pParent->top() : 0);
        }

        ssize_t Widget::relative_bottom() const
        {
            return sRealize.nTop - ((pParent != NULL) ? pParent->top() : 0) + sRealize.nHeight;
        }

        bool Widget::inside(ssize_t x, ssize_t y)
        {
            if (!(nFlags & F_VISIBLE))
                return false;
            else if (x < sRealize.nLeft)
                return false;
            else if (x >= (sRealize.nLeft + sRealize.nWidth))
                return false;
            else if (y < sRealize.nTop)
                return false;
            else if (y >= (sRealize.nTop + sRealize.nHeight))
                return false;

            return true;
        }

        ws::mouse_pointer_t Widget::active_cursor() const
        {
            return enCursor;
        }

        bool Widget::hide()
        {
            if (!(nFlags & F_VISIBLE))
                return false;
            nFlags &= ~F_VISIBLE;
//            lsp_trace("class = %s, this=%p", get_class()->name, this);

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

            return true;
        }

        bool Widget::show()
        {
            if (nFlags & F_VISIBLE)
                return false;
//            lsp_trace("class = %s, this=%p", get_class()->name, this);

            nFlags |= F_VISIBLE;
            if (pParent != NULL)
                pParent->query_resize();
            query_draw(REDRAW_CHILD | REDRAW_SURFACE);
            sSlots.execute(SLOT_SHOW, this);

            return true;
        }

        void Widget::set_parent(ComplexWidget *parent)
        {
            if (pParent == parent)
                return;

            if (pParent != NULL)
            {
                Window *wnd = widget_cast<Window>(toplevel());
                if (wnd != NULL)
                    wnd->unfocus_child(this);
                sStyle.remove_parent(pParent->style()); // Unlink style

                WidgetContainer *wc = widget_cast<WidgetContainer>(pParent);
                if (wc != NULL)
                    wc->remove(this);
            }

            pParent = parent;
            if (parent != NULL) // Inherit the style of parent widget
                sStyle.add_parent(parent->style());
        }

        Widget *Widget::toplevel()
        {
            Widget *p = this;
            while (p->pParent != NULL)
                p = p->pParent;

            return p;
        }

        void Widget::query_draw(size_t flags)
        {
            if (!(nFlags & F_VISIBLE))
                return;
            nFlags     |= (flags & (REDRAW_CHILD | REDRAW_SURFACE));
            if (pParent != NULL)
                pParent->query_draw(REDRAW_CHILD);
        }

        void Widget::commit_redraw()
        {
            nFlags &= ~(REDRAW_SURFACE | REDRAW_CHILD);
        }

        status_t Widget::queue_destroy()
        {
            if (pDisplay == NULL)
                return STATUS_BAD_STATE;
            return pDisplay->queue_destroy(this);
        }

        void Widget::query_resize()
        {
            Widget *w = toplevel();
            if ((w != NULL) && (w != this))
                w->query_resize();
        }

        void Widget::set_expand(bool value)
        {
            size_t flags = nFlags;
            if (value)
                nFlags  |= F_EXPAND;
            else
                nFlags  &= ~F_EXPAND;

            if (flags != nFlags)
                query_resize();
        }

        void Widget::set_fill(bool value)
        {
            size_t flags = nFlags;
            if (value)
                nFlags  |= F_HFILL | F_VFILL;
            else
                nFlags  &= ~(F_HFILL | F_VFILL);

            if (flags != nFlags)
                query_resize();
        }

        void Widget::set_hfill(bool value)
        {
            size_t flags = nFlags;
            if (value)
                nFlags  |= F_HFILL;
            else
                nFlags  &= ~F_HFILL;

            if (flags != nFlags)
                query_resize();
        }

        void Widget::set_vfill(bool value)
        {
            size_t flags = nFlags;
            if (value)
                nFlags  |= F_VFILL;
            else
                nFlags  &= ~F_VFILL;

            if (flags != nFlags)
                query_resize();
        }

        void Widget::set_visible(bool visible)
        {
            if (visible)
                show();
            else
                hide();
        }

        /** Set mouse pointer
         *
         * @param mp mouse pointer
         * @return mouse pointer
         */
        status_t Widget::set_cursor(ws::mouse_pointer_t mp)
        {
            enCursor       = mp;
            return STATUS_OK;
        }

        void Widget::render(ws::ISurface *s, bool force)
        {
            // Get surface of widget
            ws::ISurface *src  = get_surface(s);
            if (src == NULL)
                return;

            // Render to the main surface
            s->draw(src, sRealize.nLeft, sRealize.nTop);
        }

        status_t Widget::set_unique_id(const char *uid)
        {
            char *rep = NULL;
            if (uid != NULL)
            {
                if ((rep = strdup(uid)) == NULL)
                    return STATUS_NO_MEM;
            }

            if (pUID != NULL)
                free(pUID);
            pUID = rep;
            return STATUS_OK;
        }

        ws::ISurface *Widget::get_surface(ws::ISurface *s)
        {
            return get_surface(s, sRealize.nWidth, sRealize.nHeight);
        }

        ws::ISurface *Widget::get_surface(ws::ISurface *s, ssize_t width, ssize_t height)
        {
            // Check surface
            if (pSurface != NULL)
            {
                if ((width != ssize_t(pSurface->width())) || (height != ssize_t(pSurface->height())))
                {
                    pSurface->destroy();
                    delete pSurface;
                    pSurface    = NULL;
                }
            }

            // Create new surface if needed
            if (pSurface == NULL)
            {
                if (s == NULL)
                    return NULL;

                // Do not return surface if size is negative
                if ((width <= 0) || (height <= 0))
                    return NULL;

                pSurface        = s->create(width, height);
                if (pSurface == NULL)
                    return NULL;
                nFlags         |= REDRAW_SURFACE;
            }

            // Redraw surface if required
            if (nFlags & REDRAW_SURFACE)
            {
                draw(pSurface);
                nFlags         &= ~REDRAW_SURFACE;
            }

            return pSurface;
        }

        void Widget::draw(ws::ISurface *s)
        {
        }

        void Widget::realize(const realize_t *r)
        {
            // Do not report size request on size change
            if ((sRealize.nLeft == r->nLeft) &&
                (sRealize.nTop  == r->nTop) &&
                (sRealize.nWidth == r->nWidth) &&
                (sRealize.nHeight == r->nHeight))
                return;

            // Update size and execute slot
            sRealize        = *r;
            sSlots.execute(SLOT_RESIZE, this, &sRealize);
        }

        void Widget::size_request(ws::size_limit_t *r)
        {
            r->nMinWidth    = -1;
            r->nMinHeight   = -1;
            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;
        }

        bool Widget::has_focus() const
        {
            if (!(nFlags & F_VISIBLE))
                return false;

            Widget *_this   = const_cast<Widget *>(this);
            Window *wnd     = widget_cast<Window>(_this->toplevel());
            return (wnd != NULL) ? (wnd->focused_child() == this) : false;
        }

        status_t Widget::set_focus(bool focus)
        {
            if (!(nFlags & F_VISIBLE))
                return STATUS_OK;

            Window *wnd     = widget_cast<Window>(toplevel());
            if (wnd == NULL)
                return STATUS_BAD_HIERARCHY;
            return (focus) ? wnd->focus_child(this) : wnd->unfocus_child(this);
        }

        status_t Widget::toggle_focus()
        {
            if (!(nFlags & F_VISIBLE))
                return STATUS_OK;

            Window *wnd     = widget_cast<Window>(toplevel());
            return (wnd != NULL) ? wnd->toggle_child_focus(this) : STATUS_BAD_HIERARCHY;
        }

        status_t Widget::mark_pointed()
        {
            Window *wnd     = widget_cast<Window>(toplevel());
            if (wnd == NULL)
                return STATUS_SUCCESS;
            return wnd->point_child(this);
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
                FWD_EVENT(ws::UIE_MOUSE_DBL_CLICK, SLOT_MOUSE_DBL_CLICK )
                FWD_EVENT(ws::UIE_MOUSE_TRI_CLICK, SLOT_MOUSE_TRI_CLICK )
                FWD_EVENT(ws::UIE_FOCUS_IN, SLOT_FOCUS_IN )
                FWD_EVENT(ws::UIE_FOCUS_OUT, SLOT_FOCUS_OUT )
                FWD_EVENT(ws::UIE_DRAG_REQUEST, SLOT_DRAG_REQUEST )

                default:
                    break;
            }
            #undef FWD_EVENT

            return STATUS_OK;
        }

        void Widget::destroy()
        {
            do_destroy();
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
            // Always mark widget pointed
            return mark_pointed();
        }

        status_t Widget::on_mouse_out(const ws::event_t *e)
        {
            return STATUS_OK;
        }

        status_t Widget::on_mouse_scroll(const ws::event_t *e)
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

        status_t Widget::on_resize(const realize_t *r)
        {
            return STATUS_OK;
        }

        status_t Widget::on_resize_parent(const realize_t *r)
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
    }

} /* namespace lsp */
