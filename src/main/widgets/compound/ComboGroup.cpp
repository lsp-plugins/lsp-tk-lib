/*
 * ComboGroup.cpp
 *
 *  Created on: 14 авг. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace tk
    {
        //-----------------------------------------------------------------------------
        // ComboBox popup window implementation
        const w_class_t ComboBox::Window::metadata      = { "ComboGroup::Window", &PopupWindow::metadata };

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

        status_t ComboGroup::List::on_submit()
        {
            pCGroup->sOpened.set(false);
            return pCGroup->sSlots.execute(SLOT_SUBMIT, pCGroup, NULL);
        }

        status_t ComboGroup::List::on_change()
        {
            ListBoxItem *it  = vSelected.any();
            ListBoxItem *old = pCGroup->sSelected.set(it);

            if (old != it)
                pCGroup->sSlots.execute(SLOT_CHANGE, pCGroup, NULL);

            return STATUS_OK;
        }

        //-----------------------------------------------------------------------------
        // ComboGroup implementation
        const w_class_t Group::metadata         = { "ComboGroup", &WidgetContainer::metadata };

        ComboGroup::ComboGroup(Display *dpy):
            Align(dpy),
            sFont(&sProperties),
            sColor(&sProperties),
            sTextColor(&sProperties),
            sText(&sProperties),
            sOpened(&sProperties),
            sBorder(&sProperties),
            sTextBorder(&sProperties),
            sRadius(&sProperties),
            sTextRadius(&sProperties),
            sEmbedding(&sProperties),
            sLayout(&sProperties),
            sSizeConstraints(&sProperties)
        {
            sLabel.nLeft        = 0;
            sLabel.nTop         = 0;
            sLabel.nWidth       = 0;
            sLabel.nHeight      = 0;

            sArea.nLeft         = 0;
            sArea.nTop          = 0;
            sArea.nWidth        = 0;
            sArea.nHeight       = 0;

            pClass              = &metadata;
        }

        ComboGroup::~ComboGroup()
        {
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

            // Configure Window
            sWindow.add(&sLBox);
            sWindow.add_arrangement(A_BOTTOM, 0, true);
            sWindow.add_arrangement(A_TOP, 0, true);
            sWindow.layout()->set_scale(1.0f);

            sFont.bind("font", &sStyle);
            sColor.bind("color", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sText.bind(&sStyle, pDisplay->dictionary());
            sOpened.bind("opened", &sStyle);
            sBorder.bind("border.size", &sStyle);
            sTextBorder.bind("text.border", &sStyle);
            sRadius.bind("border.radius", &sStyle);
            sTextRadius.bind("text.radius", &sStyle);
            sEmbedding.bind("embed", &sStyle);
            sLayout.bind("layout", &sStyle);
            sSizeConstraints.bind("size.constraints", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sFont.init(sclass);
                sColor.init(sclass, "#000000");
                sTextColor.init(sclass, "#ffffff");
                sOpened.init(sclass, false);
                sBorder.init(sclass, 2);
                sTextBorder.init(sclass, 2);
                sRadius.init(sclass, 10);
                sTextRadius.init(sclass, 10);
                sEmbedding.init(sclass, false);
                sLayout.init(sclass, 0.0f, 0.0f, 1.0f, 1.0f);
                sSizeConstraints.init(sclass);
            }

            return STATUS_OK;
        }

        void ComboGroup::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);
            if (sFont.is(prop))
                query_resize();
            if (sColor.is(prop))
                query_draw();
            if (sTextColor.is(prop))
                query_draw();
            if (sText.is(prop))
                query_resize();
            if (sOpened.is(prop))
            {
                bool visible = sWindow.visibility()->get();
                if (sOpened.get() != visible)
                {
                    if (!visible)
                    {
                        ws::rectangle_t r;
                        this->get_screen_rectangle(&r);
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
            if (sBorder.is(prop))
                query_resize();
            if (sRadius.is(prop))
                query_resize();
            if (sTextRadius.is(prop))
                query_resize();
            if (sEmbedding.is(prop))
                query_resize();
            if (sLayout.is(prop))
                query_resize();
            if (sSizeConstraints.is(prop))
                query_resize();
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

        void ComboGroup::allocate(alloc_t *alloc)
        {
            float scaling   = lsp_max(0.0f, sScaling.get());
            ssize_t border  = (sBorder.get() > 0) ? lsp_max(1.0f, sBorder.get() * scaling) : 0;
            ssize_t radius  = lsp_max(0.0f, sRadius.get() * scaling);

            // Text allocation
            ws::rectangle_t xr;
            xr.nLeft        = 0;
            xr.nTop         = 0;

            // Text parameters
            LSPString s;
            ws::text_parameters_t tp;
            ws::font_parameters_t fp;

            ssize_t tborder     = lsp_max(0.0f, sTextBorder.get() * scaling);
            ssize_t tradius     = lsp_max(0.0f, sTextRadius.get() * scaling);
            sText.format(&s);

            sFont.get_parameters(pDisplay, scaling, &fp);
            sFont.get_text_parameters(pDisplay, &tp, scaling, &s);
            xr.nWidth           = tp.Width + tborder + tradius;
            xr.nHeight          = lsp_max(fp.Height, tp.Height) + tborder*2;
            alloc->text         = xr;

            xr.nWidth          += radius * 1.5f;
            alloc->rtext        = xr;

            // Compute padding
            ssize_t xborder = lsp_max(0.0f, (radius-border) * M_SQRT1_2);
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

            ssize_t hpad       = alloc.pad.nLeft + alloc.pad.nRight;
            ssize_t vpad       = alloc.pad.nTop  + alloc.pad.nBottom;

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
            lsp_trace("width=%d, height=%d", int(r->nWidth), int(r->nHeight));
            WidgetContainer::realize(r);

            // Compute text and widget area
            alloc_t alloc;
            allocate(&alloc);

            sLabel          = alloc.text;
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
            ListBoxItem *it = sSelected.get();
            ssize_t index   = (it != NULL) ? sLBox.items()->index_of(it) : NULL;
            return vWidgets.get(index);
        }

        void ComboGroup::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            if (nFlags & REDRAW_SURFACE)
                force = true;

            // Clear the space
            lsp::Color color;

            ws::rectangle_t xr;
            float scaling   = lsp_max(0.0f, sScaling.get());
            float bright    = lsp_max(0.0f, sBrightness.get());
            ssize_t border  = (sBorder.get() > 0) ? lsp_max(1.0f, sBorder.get() * scaling) : 0;
            ssize_t radius  = lsp_max(0.0f, sRadius.get() * scaling);
            bool bg         = false;

            bool aa         = s->set_antialiasing(false);

            // Draw background if current child is invisible or not present
            Widget *widget  = current_widget();

            if ((widget != NULL) && (widget->visibility()->get()))
            {
                color.copy(widget->bg_color()->color());
                if (force)
                {
                    widget->get_rectangle(&xr);

                    if (widget->redraw_pending())
                    {
                        // Draw the child only if it is visible in the area
                        if (Size::intersection(&xr, &sSize))
                            widget->render(s, &xr, force);

                        widget->commit_redraw();
                    }

                    // Render the child background
                    if (Size::overlap(area, &sSize))
                    {
                        s->clip_begin(area);
                        {
                            color.copy(widget->bg_color()->color());
                            s->fill_frame(color, &sSize, &xr);
                        }
                        s->clip_end();
                    }
                }
            }
            else
            {
                color.copy(sBgColor);
                s->fill_rect(color, &sSize);
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
                        color.copy(sBgColor);

                        xr          = sSize;
                        xg          = border * 2;
                        xr.nLeft   += border;
                        xr.nTop    += border;
                        xr.nWidth  -= xg;
                        xr.nHeight -= xg;

                        ssize_t ir  = lsp_max(0, radius - border);
                        s->fill_round_frame(color, ir, SURFMASK_ALL_CORNER ^ SURFMASK_LT_CORNER, &sSize, &xr);
                    }

                    // Draw frame
                    color.copy(sColor);
                    color.scale_lightness(bright);

                    xr          = sSize;
                    xg          = border >> 1;
                    ir          = lsp_max(0, radius - xg);
                    xr.nLeft   += xg;
                    xr.nTop    += xg;
                    xr.nWidth  -= (border*2 - xg);
                    xr.nHeight -= (border*2 - xg);

                    s->set_antialiasing(true);
                    s->wire_round_rect(color, SURFMASK_ALL_CORNER ^ SURFMASK_LT_CORNER, ir, &xr, border);
                }

                // Draw text
                if (Size::overlap(area, &sLabel))
                {
                    ir          = lsp_max(0.0f, sTextRadius.get() * scaling);

                    // Draw text background
                    color.copy(sColor);
                    color.scale_lightness(bright);

                    s->set_antialiasing(true);
                    s->fill_round_rect(color, SURFMASK_RB_CORNER, ir, &sLabel);

                    // Draw text
                    LSPString text;
                    ws::text_parameters_t tp;
                    ws::font_parameters_t fp;
                    color.copy(sTextColor);
                    color.scale_lightness(bright);

                    ssize_t tborder     = lsp_max(0.0f, sTextBorder.get() * scaling);
                    sText.format(&text);

                    sFont.get_parameters(pDisplay, scaling, &fp);
                    sFont.get_text_parameters(pDisplay, &tp, scaling, &text);

                    sFont.draw(s, color,
                            sLabel.nLeft + tborder - tp.XBearing, sLabel.nTop + tborder + fp.Ascent,
                            scaling, &text);
                }

                s->clip_end();
            }

            s->set_antialiasing(aa);
        }

    } /* namespace tk */
} /* namespace lsp */


