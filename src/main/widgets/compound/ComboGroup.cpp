/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 14 авг. 2020 г.
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
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/common/debug.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(ComboGroup, WidgetContainer)
                // Bind
                sFont.bind("font", this);
                sTextAdjust.bind("text.adjust", this);
                sColor.bind("color", this);
                sTextColor.bind("text.color", this);
                sSpinColor.bind("spin.color", this);
                sOpened.bind("opened", this);
                sBorder.bind("border.size", this);
                sTextPadding.bind("text.padding", this);
                sRadius.bind("border.radius", this);
                sTextRadius.bind("text.radius", this);
                sSpinSize.bind("spin.size", this);
                sSpinSpacing.bind("spin.spacing", this);
                sEmbedding.bind("embed", this);
                sLayout.bind("layout", this);
                sSizeConstraints.bind("size.constraints", this);
                sHeading.bind("heading", this);
                sInvertMouseVScroll.bind("mouse.vscroll.invert", this);
                // Configure
                sFont.set_size(12.0f);
                sTextAdjust.set(TA_NONE);
                sColor.set("#000000");
                sTextColor.set("#ffffff");
                sSpinColor.set("#ffffff");
                sOpened.set(false);
                sBorder.set(2);
                sTextPadding.set(2);
                sRadius.set(10);
                sTextRadius.set(10);
                sSpinSize.set(8);
                sSpinSpacing.set(0);
                sEmbedding.set(false);
                sLayout.set(0.0f, 0.0f, 1.0f, 1.0f);
                sSizeConstraints.set_all(-1);
                sHeading.set(-1.0f, 0.0f);
                sInvertMouseVScroll.set(false);
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(ComboGroup, "ComboGroup", "root");

            // ComboGroup::Window style
            LSP_TK_BUILTIN_STYLE(PopupWindow, "ComboGroup::Window", "Window");

            // ComboGroup::List style
            LSP_TK_BUILTIN_STYLE(ListBox, "ComboGroup::List", "ListBox");
        }

        //-----------------------------------------------------------------------------
        // ComboBox popup window implementation
        const w_class_t ComboGroup::Window::metadata    = { "ComboGroup::Window", &PopupWindow::metadata };

        ComboGroup::Window::Window(Display *dpy, ComboGroup *cgroup):
            PopupWindow(dpy)
        {
            pCGroup         = cgroup;
            pClass          = &metadata;
        }

        status_t ComboGroup::Window::on_hide()
        {
            pCGroup->sOpened.set(false);
            return STATUS_OK;
        }

        status_t ComboGroup::Window::on_show()
        {
            pCGroup->sOpened.set(true);
            return STATUS_OK;
        }

        //-----------------------------------------------------------------------------
        // ListBox popup implementation
        const w_class_t ComboGroup::List::metadata      = { "ComboGroup::List", &ListBox::metadata };

        ComboGroup::List::List(Display *dpy, ComboGroup *cgroup):
            ListBox(dpy)
        {
            pCGroup         = cgroup;
            pClass          = &metadata;
        }

        void ComboGroup::List::property_changed(Property *prop)
        {
            ListBox::property_changed(prop);
            if (vItems.is(prop))
                pCGroup->query_resize();
        }

        status_t ComboGroup::List::on_submit()
        {
            pCGroup->sOpened.set(false);
            pCGroup->query_resize();

            ListBoxItem *it  = vSelected.any();
            ListBoxItem *old = pCGroup->sSelected.set(it);

            if (old != it)
                pCGroup->sSlots.execute(SLOT_CHANGE, pCGroup, NULL);

            return pCGroup->sSlots.execute(SLOT_SUBMIT, pCGroup, NULL);
        }

        //-----------------------------------------------------------------------------
        // ComboGroup implementation
        const w_class_t ComboGroup::metadata        = { "ComboGroup", &WidgetContainer::metadata };

        const tether_t ComboGroup::tether_list[] =
        {
            { TF_LEFT | TF_BOTTOM | TF_HORIZONTAL | TF_HSTRETCH,     1.0f,  1.0f  },
            { TF_LEFT | TF_TOP | TF_HORIZONTAL | TF_HSTRETCH,        1.0f,  -1.0f },
        };

        ComboGroup::ComboGroup(Display *dpy):
            WidgetContainer(dpy),
            sLBox(dpy, this),
            sWindow(dpy, this),
            sFont(&sProperties),
            sTextAdjust(&sProperties),
            sColor(&sProperties),
            sTextColor(&sProperties),
            sSpinColor(&sProperties),
            sEmptyText(&sProperties),
            sOpened(&sProperties),
            sBorder(&sProperties),
            sTextPadding(&sProperties),
            sRadius(&sProperties),
            sTextRadius(&sProperties),
            sSpinSize(&sProperties),
            sSpinSpacing(&sProperties),
            sEmbedding(&sProperties),
            sLayout(&sProperties),
            sSizeConstraints(&sProperties),
            sHeading(&sProperties),
            sInvertMouseVScroll(&sProperties),
            vWidgets(&sProperties, &sIListener),
            sSelected(&sProperties)
        {
            sLabel.nLeft        = 0;
            sLabel.nTop         = 0;
            sLabel.nWidth       = 0;
            sLabel.nHeight      = 0;

            sText.nLeft         = 0;
            sText.nTop          = 0;
            sText.nWidth        = 0;
            sText.nHeight       = 0;

            sArea.nLeft         = 0;
            sArea.nTop          = 0;
            sArea.nWidth        = 0;
            sArea.nHeight       = 0;

            nMBState            = 0;
            bInside             = 0;

            pClass              = &metadata;
        }

        ComboGroup::~ComboGroup()
        {
            nFlags     |= FINALIZED;
        }

        status_t ComboGroup::init()
        {
            // Initialize widgets
            status_t result = WidgetContainer::init();
            if (result == STATUS_OK)
                result  = sWindow.init();
            if (result == STATUS_OK)
                result  = sLBox.init();
            if (result != STATUS_OK)
                return result;

            sIListener.bind_all(this, on_add_widget, on_remove_widget);

            // Configure Window
            sWindow.add(&sLBox);
            sWindow.set_tether(tether_list, sizeof(tether_list)/sizeof(tether_t));
            sWindow.add_tether(TF_LEFT | TF_TOP | TF_HORIZONTAL | TF_HSTRETCH, 1.0f, -1.0f);
            sWindow.layout()->set_scale(1.0f);

            sFont.bind("font", &sStyle);
            sTextAdjust.bind("text.adjust", &sStyle);
            sColor.bind("color", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sSpinColor.bind("spin.color", &sStyle);
            sEmptyText.bind(&sStyle, pDisplay->dictionary());
            sOpened.bind("opened", &sStyle);
            sBorder.bind("border.size", &sStyle);
            sTextPadding.bind("text.padding", &sStyle);
            sRadius.bind("border.radius", &sStyle);
            sTextRadius.bind("text.radius", &sStyle);
            sSpinSize.bind("spin.size", &sStyle);
            sSpinSpacing.bind("spin.spacing", &sStyle);
            sEmbedding.bind("embed", &sStyle);
            sLayout.bind("layout", &sStyle);
            sSizeConstraints.bind("size.constraints", &sStyle);
            sHeading.bind("heading", &sStyle);

            // Bind slots
            handler_id_t id;
            id = sSlots.add(SLOT_CHANGE, slot_on_change, self());
            if (id >= 0) id = sSlots.add(SLOT_SUBMIT, slot_on_change, self());
            if (id < 0)
                return -id;

            return STATUS_OK;
        }

        void ComboGroup::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);

            if (prop->one_of(sFont, sTextAdjust, sBorder, sPadding, sTextPadding, sRadius, sEmbedding, sHeading,
                    sEmptyText, sTextRadius, sSpinSize, sSpinSpacing, sLayout, sSizeConstraints, sActive, vWidgets))
                query_resize();

            if (prop->one_of(sColor, sTextColor))
                query_draw();

            if (sOpened.is(prop))
            {
                bool visible = sWindow.visibility()->get();
                if (sOpened.get() != visible)
                {
                    if (!visible)
                    {
                        ws::rectangle_t r;
                        this->get_padded_screen_rectangle(&r, &sText);
                        sWindow.trigger_area()->set(&r);
                        sWindow.trigger_widget()->set(this);
                        sWindow.show(this);

                        sWindow.grab_events(ws::GRAB_DROPDOWN);
                        sWindow.take_focus();
                        sLBox.take_focus();
                    }
                    else
                        sWindow.hide();
                }
            }

            if (sSelected.is(prop))
            {
                ListBoxItem *it = sSelected.get();
                if (sLBox.items()->contains(it))
                {
                    sLBox.selected()->clear();
                    sLBox.selected()->add(it);
                }
                else
                    sSelected.set(NULL);
                query_resize();
            }
        }

        void ComboGroup::on_add_widget(void *obj, Property *prop, void *w)
        {
            Widget *item = widget_ptrcast<Widget>(w);
            if (item == NULL)
                return;

            ComboGroup *_this = widget_ptrcast<ComboGroup>(obj);
            if (_this == NULL)
                return;

            item->set_parent(_this);
            _this->query_resize();
        }

        void ComboGroup::on_remove_widget(void *obj, Property *prop, void *w)
        {
            Widget *item = widget_ptrcast<Widget>(w);
            if (item == NULL)
                return;

            ComboGroup *_this = widget_ptrcast<ComboGroup>(obj);
            if (_this == NULL)
                return;

            // Reset active widget if present
            if (_this->sActive.get() == item)
                _this->sActive.set(NULL);

            _this->unlink_widget(item);
            _this->query_resize();
        }

        void ComboGroup::allocate(alloc_t *alloc)
        {
            bool has_list   = sLBox.items()->size() > 1;
            float scaling   = lsp_max(0.0f, sScaling.get());
            float fscaling  = lsp_max(0.0f, scaling * sFontScaling.get());
            ssize_t border  = (sBorder.get() > 0) ? lsp_max(1.0f, sBorder.get() * scaling) : 0;
            ssize_t radius  = lsp_max(0.0f, sRadius.get() * scaling);
            ssize_t spin    = (has_list) ? lsp_max(0.0f, sSpinSize.get() * scaling) : 0;
            ssize_t spin_sp = (has_list) ? lsp_max(0.0f, sSpinSpacing.get() * scaling) : 0;

            // Text allocation
            ws::rectangle_t xr;
            xr.nLeft        = 0;
            xr.nTop         = 0;

            // Text parameters
            LSPString s;
            ws::text_parameters_t tp;
            ws::font_parameters_t fp;
            ListBoxItem *it     = current_item();

            ssize_t tradius     = lsp_max(0.0f, sTextRadius.get() * scaling);
            if (it != NULL)
                it->text()->format(&s);
            else
                sEmptyText.format(&s);
            sTextAdjust.apply(&s);

            sFont.get_parameters(pDisplay, fscaling, &fp);
            sFont.get_text_parameters(pDisplay, &tp, fscaling, &s);
            xr.nWidth           = tp.Width + tradius + spin + spin_sp;
            xr.nHeight          = lsp_max(fp.Height, tp.Height);
            sTextPadding.add(&xr, scaling);
            alloc->text         = xr;

            xr.nWidth          += radius * 1.5f;
            alloc->rtext        = xr;

            // Compute padding
            ssize_t xborder = lsp_max(float(border), (radius-border) * M_SQRT1_2);
            padding_t pad;

            pad.nLeft       = (sEmbedding.left())   ? border : xborder;
            pad.nRight      = (sEmbedding.right())  ? border : xborder;
            pad.nTop        = (sEmbedding.top())    ? border : xborder;
            pad.nBottom     = (sEmbedding.bottom()) ? border : xborder;
            if (!sEmbedding.top())
                pad.nTop        = lsp_max(xr.nHeight, ssize_t(pad.nTop));

            alloc->pad      = pad;

            pad.nLeft       = lsp_max(pad.nLeft,   size_t(radius));
            pad.nRight      = lsp_max(pad.nRight,  size_t(radius));
            pad.nTop        = lsp_max(pad.nTop,    size_t(radius));
            pad.nBottom     = lsp_max(pad.nBottom, size_t(radius));

            alloc->xpad     = pad;
        }

        void ComboGroup::size_request(ws::size_limit_t *r)
        {
            float scaling   = lsp_max(0.0f, sScaling.get());
            alloc_t alloc;

            allocate(&alloc);

            ssize_t hpad        = alloc.pad.nLeft + alloc.pad.nRight;
            ssize_t vpad        = alloc.pad.nTop  + alloc.pad.nBottom;

            Widget *widget      = current_widget();

            if (widget == NULL)
            {
                r->nMinWidth        = 0;
                r->nMinHeight       = 0;
                r->nMaxWidth        = -1;
                r->nMaxHeight       = -1;
                r->nPreWidth        = -1;
                r->nPreHeight       = -1;
            }
            else
            {
                widget->get_padded_size_limits(r);
                r->nMinWidth        = (r->nMinWidth  >= 0) ? r->nMinWidth  + hpad : hpad;
                r->nMinHeight       = (r->nMinHeight >= 0) ? r->nMinHeight + vpad : vpad;
                r->nMaxWidth        = -1;
                r->nMaxHeight       = -1;
                r->nPreWidth        = -1;
                r->nPreHeight       = -1;
            }

            r->nMinWidth        = lsp_max(alloc.rtext.nWidth, r->nMinWidth);
            r->nMinHeight       = lsp_max(alloc.rtext.nHeight, r->nMinHeight);

            hpad                = alloc.xpad.nLeft + alloc.xpad.nRight;
            vpad                = alloc.xpad.nTop  + alloc.xpad.nBottom;
            r->nMinWidth        = lsp_max(r->nMinWidth,  hpad);
            r->nMinHeight       = lsp_max(r->nMinHeight, vpad);

            // Apply size constraints
            sSizeConstraints.apply(r, scaling);
        }

        void ComboGroup::realize(const ws::rectangle_t *r)
        {
            WidgetContainer::realize(r);

            // Compute text and widget area
            alloc_t alloc;
            allocate(&alloc);

            sText           = alloc.text;
            sText.nLeft    += r->nLeft;
            sText.nTop     += r->nTop;

            sHeading.happly(&sLabel, &alloc.text, r->nWidth);
            sLabel.nLeft   += r->nLeft;
            sLabel.nTop    += r->nTop;

            Padding::enter(&sArea, r, &alloc.pad);

            // Realize current child widget
            Widget *widget  = current_widget();
            if ((widget != NULL) && (widget->visibility()->get()))
            {
                ws::rectangle_t xr;
                ws::size_limit_t sr;

                widget->get_padded_size_limits(&sr);
                sLayout.apply(&xr, &sArea, &sr);
                widget->padding()->enter(&xr, widget->scaling()->get());
                widget->realize_widget(&xr);
            }
        }

        Widget *ComboGroup::current_widget()
        {
            Widget *active  = sActive.get();
            if ((active != NULL) && (vWidgets.contains(active)))
                return active;

            ListBoxItem *it = sSelected.get();
            ssize_t index   = ((it != NULL) && (it->visibility()->get())) ? sLBox.items()->index_of(it) : 0;
            return vWidgets.get(index);
        }

        ListBoxItem *ComboGroup::current_item()
        {
            ListBoxItem *it = sSelected.get();
            ssize_t index   = ((it != NULL) && (it->visibility()->get())) ? sLBox.items()->index_of(it) : -1;
            return (index >= 0) ? it : NULL;
        }

        void ComboGroup::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            if (nFlags & REDRAW_SURFACE)
                force = true;

            // Clear the space
            lsp::Color color;

            ws::rectangle_t xr;
            bool has_list   = sLBox.items()->size() > 1;
            float scaling   = lsp_max(0.0f, sScaling.get());
            float fscaling  = lsp_max(0.0f, scaling * sFontScaling.get());
            float bright    = lsp_max(0.0f, select_brightness());
            ssize_t border  = (sBorder.get() > 0) ? lsp_max(1.0f, sBorder.get() * scaling) : 0;
            ssize_t radius  = lsp_max(0.0f, sRadius.get() * scaling);
            bool bg         = false;

            bool aa         = s->set_antialiasing(false);

            // Draw background if current child is invisible or not present
            Widget *widget  = current_widget();

            if ((widget != NULL) && (widget->visibility()->get()))
            {
                widget->get_rectangle(&xr);

                if ((force) || (widget->redraw_pending()))
                {
                    // Draw the child only if it is visible in the area
                    if (Size::intersection(&xr, &sSize))
                        widget->render(s, &xr, force);

                    widget->commit_redraw();
                }

                if (force)
                {
                    // Render the child background
                    if (Size::overlap(area, &sSize))
                    {
                        s->clip_begin(area);
                        {
                            widget->get_actual_bg_color(color);
                            s->fill_frame(color, SURFMASK_NONE, 0.0f, &sSize, &xr);
                        }
                        s->clip_end();
                    }
                }
            }
            else
            {
                get_actual_bg_color(color);
                s->fill_rect(color, SURFMASK_NONE, 0.0f, &sSize);
                bg   = true;
            }

            // Render frame
            if (force)
            {
                ssize_t ir, xg;

                s->clip_begin(area);

                if (Size::overlap(area, &sSize))
                {
                    if (!bg)
                    {
                        get_actual_bg_color(color);

                        xr          = sSize;
                        xg          = border * 2;
                        xr.nLeft   += border;
                        xr.nTop    += border;
                        xr.nWidth  -= xg;
                        xr.nHeight -= xg;

                        ssize_t ir  = lsp_max(0, radius - border);
                        s->fill_frame(color, SURFMASK_ALL_CORNER ^ SURFMASK_LT_CORNER, ir, &sSize, &xr);
                    }

                    // Draw frame
                    color.copy(sColor);
                    color.scale_lch_luminance(bright);

                    s->set_antialiasing(true);
                    s->wire_rect(color, SURFMASK_ALL_CORNER ^ SURFMASK_LT_CORNER, radius, &sSize, border);
                }

                // Draw text (and image)
                if (Size::overlap(area, &sLabel))
                {
                    ssize_t spin        = (has_list) ? lsp_max(0.0f, sSpinSize.get() * scaling) : 0;
                    ssize_t spin_sp     = (has_list) ? lsp_max(0.0f, sSpinSpacing.get() * scaling) : 0;
                    ListBoxItem *it     = current_item();
                    ir                  = lsp_max(0.0f, sTextRadius.get() * scaling);

                    // Draw text background
                    color.copy(sColor);
                    color.scale_lch_luminance(bright);

                    s->set_antialiasing(true);
                    s->fill_rect(color, SURFMASK_RB_CORNER, ir, &sLabel);

                    // Draw text
                    LSPString text;
                    ws::text_parameters_t tp;
                    ws::font_parameters_t fp;
                    color.copy(sTextColor);
                    color.scale_lch_luminance(bright);

                    if (it != NULL)
                        it->text()->format(&text);
                    else
                        sEmptyText.format(&text);
                    sTextAdjust.apply(&text);

                    sFont.get_parameters(pDisplay, fscaling, &fp);
                    sFont.get_text_parameters(pDisplay, &tp, fscaling, &text);
                    ws::rectangle_t tloc;
                    sTextPadding.enter(&tloc, &sLabel, scaling);

                    sFont.draw(s, color,
                            tloc.nLeft + spin + spin_sp - tp.XBearing, tloc.nTop + fp.Ascent,
                            fscaling, &text);

                    // Draw arrows
                    if (spin > 0)
                    {
                        color.copy(sSpinColor);
                        color.scale_lch_luminance(bright);

                        s->fill_triangle(
                            color,
                            tloc.nLeft, tloc.nTop + (fp.Height*3.0f)/7.0f,
                            tloc.nLeft + spin*0.4f, tloc.nTop + fp.Height/7.0f,
                            tloc.nLeft + spin*0.8f, tloc.nTop + (fp.Height*3.0f)/7.0f);
                        s->fill_triangle(
                            color,
                            tloc.nLeft, tloc.nTop + (fp.Height*4.0f)/7.0f,
                            tloc.nLeft + spin*0.8f, tloc.nTop + (fp.Height*4.0f)/7.0f,
                            tloc.nLeft + spin*0.4f, tloc.nTop + (fp.Height*6.0f)/7.0f);
                    }
                }

                s->clip_end();
            }

            s->set_antialiasing(aa);
        }

        status_t ComboGroup::add(Widget *child)
        {
            return vWidgets.add(child);
        }

        status_t ComboGroup::remove(Widget *child)
        {
            return vWidgets.premove(child);
        }

        status_t ComboGroup::remove_all()
        {
            vWidgets.clear();
            return STATUS_OK;
        }

        status_t ComboGroup::add_item(ListBoxItem *child)
        {
            return sLBox.add(child);
        }

        status_t ComboGroup::remove_item(ListBoxItem *child)
        {
            return sLBox.remove(child);
        }

        status_t ComboGroup::remove_all_items()
        {
            return sLBox.remove_all();
        }

        Widget *ComboGroup::find_widget(ssize_t x, ssize_t y)
        {
            Widget *widget  = current_widget();
            if (widget == NULL)
                return NULL;
            if ((widget->is_visible_child_of(this)) && (widget->inside(x, y)))
                return widget;

            return NULL;
        }

        status_t ComboGroup::slot_on_change(Widget *sender, void *ptr, void *data)
        {
            ComboGroup *_this = widget_ptrcast<ComboGroup>(ptr);
            return (_this != NULL) ? _this->on_change() : STATUS_BAD_ARGUMENTS;
        }

        status_t ComboGroup::slot_on_submit(Widget *sender, void *ptr, void *data)
        {
            ComboGroup *_this = widget_ptrcast<ComboGroup>(ptr);
            return (_this != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }

        status_t ComboGroup::on_change()
        {
            return STATUS_OK;
        }

        status_t ComboGroup::on_submit()
        {
            return STATUS_OK;
        }

        status_t ComboGroup::on_mouse_down(const ws::event_t *e)
        {
            if (nMBState == 0)
                bInside = Position::inside(&sLabel, e->nLeft, e->nTop);
            nMBState       |= 1 << e->nCode;

            return STATUS_OK;
        }

        status_t ComboGroup::on_mouse_up(const ws::event_t *e)
        {
            size_t mask     = 1 << e->nCode;
            size_t prev     = nMBState;
            nMBState       &= (~mask);

            if (prev == mask)
            {
                if ((e->nCode == ws::MCB_LEFT) && (bInside))
                {
                    if (sLBox.items()->size() > 1)
                        sOpened.toggle();
                    else
                        sOpened.set(false);
                }
            }

            if (nMBState == 0)
                bInside         = false;

            return STATUS_OK;
        }

        status_t ComboGroup::on_mouse_move(const ws::event_t *e)
        {
            return STATUS_OK;
        }

        bool ComboGroup::scroll_item(ssize_t direction, size_t count)
        {
            WidgetList<ListBoxItem> *wl = sLBox.items();
            ListBoxItem *ci  = sSelected.get();
            ListBoxItem *xci = NULL;
            ssize_t curr = (ci != NULL) ? wl->index_of(ci) : -1;
            ssize_t last = wl->size() - 1;

            if (direction < 0)
            {
                while (curr > 0)
                {
                    xci = wl->get(--curr);
                    if ((xci == NULL) || (!xci->visibility()->get()))
                        continue;
                    if ((--count) <= 0)
                        break;
                }
            }
            else
            {
                while (curr < last)
                {
                    xci = wl->get(++curr);
                    if ((xci == NULL) || (!xci->visibility()->get()))
                        continue;
                    if ((--count) <= 0)
                        break;
                }
            }

            if ((xci != NULL) && (xci != ci))
            {
                sSelected.set(xci);
                sSlots.execute(SLOT_CHANGE, this, NULL);
                return true;
            }

            return false;
        }

        status_t ComboGroup::on_mouse_scroll(const ws::event_t *e)
        {
            ssize_t step = (sInvertMouseVScroll.get()) ? 1 : -1;

            if (Position::inside(&sLabel, e->nLeft, e->nTop))
            {
                if (e->nCode == ws::MCD_UP)
                {
                    if (scroll_item(step, 1))
                        sSlots.execute(SLOT_SUBMIT, this, NULL);
                }
                else if (e->nCode == ws::MCD_DOWN)
                {
                    if (scroll_item(-step, 1))
                        sSlots.execute(SLOT_SUBMIT, this, NULL);
                }
            }

            return STATUS_OK;
        }

        status_t ComboGroup::on_key_down(const ws::event_t *e)
        {
            switch (e->nCode)
            {
                case ws::WSK_UP:
                case ws::WSK_KEYPAD_UP:
                    if (scroll_item(-1, 1))
                        sSlots.execute(SLOT_SUBMIT, this, NULL);
                    break;

                case ws::WSK_DOWN:
                case ws::WSK_KEYPAD_DOWN:
                    if (scroll_item(1, 1))
                        sSlots.execute(SLOT_SUBMIT, this, NULL);
                    break;

                case ws::WSK_KEYPAD_ENTER:
                case ws::WSK_RETURN:
                case ws::WSK_KEYPAD_SPACE:
                case ' ':
                    sOpened.toggle();
                    break;

                default:
                    break;
            }

            return STATUS_OK;
        }
    } /* namespace tk */
} /* namespace lsp */


