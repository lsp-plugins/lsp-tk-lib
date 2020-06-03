/*
 * Widget.cpp
 *
 *  Created on: 15 июн. 2017 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/common/debug.h>

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
            sStyle(dpy->schema()),
            sProperties(this),
            sAllocation(&sProperties),
            sScaling(&sProperties),
            sBrightness(&sProperties),
            sPadding(&sProperties),
            sBgColor(&sProperties),
            sVisibility(&sProperties),
            sPointer(&sProperties)
        {
            nFlags                  = REDRAW_SURFACE | SIZE_INVALID;
            pClass                  = &metadata;
            pDisplay                = dpy;
            pParent                 = NULL;

            sLimit.nMinWidth        = -1;
            sLimit.nMinHeight       = -1;
            sLimit.nMaxWidth        = -1;
            sLimit.nMaxHeight       = -1;
            sSize.nLeft             = 0;
            sSize.nTop              = 0;
            sSize.nWidth            = 0;
            sSize.nHeight           = 0;
            pSurface                = NULL;
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
                sAllocation.bind("allocation", &sStyle);
                sScaling.bind("scaling", &sStyle);
                sBrightness.bind("brightness", &sStyle);
                sPadding.bind("padding", &sStyle);
                sBgColor.bind("bg.color", &sStyle);
                sPointer.bind("pointer", &sStyle);

                Style *sclass = style_class();
                if (sclass != NULL)
                {
                    sStyle.add_parent(sclass);
                    sAllocation.init(sclass, true, true, false, false);
                    sPadding.init(sclass, 0, 0, 0, 0);
                    sScaling.init(sclass, 1.0f);
                    sBrightness.init(sclass, 1.0f);
                    sBgColor.init(sclass, "#cccccc");
                    sPointer.init(sclass, ws::MP_DEFAULT);
                }

                // Override settings for hfill and vfill
                sAllocation.set(true, false);
            }

            // Override some properties
            sVisibility.set(true);

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
            if (sAllocation.is(prop))
                query_resize();
            if (sVisibility.is(prop))
            {
                if (sVisibility.get())
                    show_widget();
                else
                    hide_widget();
            }
        }

        Style *Widget::style_class() const
        {
            Schema *s = const_cast<Widget *>(this)->pDisplay->schema();
            if (s == NULL)
                return NULL;
            return s->get(pClass->name);
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

        bool Widget::inside(ssize_t left, ssize_t top)
        {
            if (!sVisibility.get())
                return false;

            return Position::inside(&sSize, left, top);
        }

        void Widget::hide_widget()
        {
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

            if (pParent != NULL)
            {
                Window *wnd = widget_cast<Window>(toplevel());
//                sStyle.remove_parent(pParent->style()); // Unlink style

                WidgetContainer *wc = widget_cast<WidgetContainer>(pParent);
                if (wc != NULL)
                    wc->remove(this);
                wnd->discard_widget(wc);
            }

            pParent = parent;
//            if (parent != NULL) // Inherit the style of parent widget
//                sStyle.add_parent(parent->style());
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
            if (!sVisibility.get())
                return;

            // Update flags
            nFlags     |= (RESIZE_PENDING | SIZE_INVALID);
            if (pParent != NULL)
                pParent->query_resize();
        }

        void Widget::render(ws::ISurface *s, bool force)
        {
            // Get surface of widget
            ws::ISurface *src  = get_surface(s);
            if (src == NULL)
                return;

            // Render to the main surface
            s->draw(src, sSize.nLeft, sSize.nTop);
        }

        ws::ISurface *Widget::get_surface(ws::ISurface *s)
        {
            return get_surface(s, sSize.nWidth, sSize.nHeight);
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
            // Call for realize
            realize(r);

            // Reset size pending flags
            nFlags     &= ~(SIZE_INVALID | RESIZE_PENDING);
            query_draw();   // Always query redraw after realize()
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
            return (wnd != NULL) ? wnd->kill_focus(this) : false;
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
    }

} /* namespace lsp */
