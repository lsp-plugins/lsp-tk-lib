/*
 * Fraction.cpp
 *
 *  Created on: 17 авг. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/stdlib/math.h>

namespace lsp
{
    namespace tk
    {
        //-----------------------------------------------------------------------------
        // Fraction popup window implementation
        const w_class_t Fraction::Window::metadata      = { "Fraction::Window", &PopupWindow::metadata };

        Fraction::Window::Window(Display *dpy, Fraction *frac, Combo *combo):
            PopupWindow(dpy)
        {
            pFrac           = frac;
            pCombo          = combo;

            pClass          = &metadata;
        }

        status_t Fraction::Window::on_hide()
        {
            pCombo->sOpened.set(false);
            return STATUS_OK;
        }

        status_t Fraction::Window::on_show()
        {
            pCombo->sOpened.set(true);
            return STATUS_OK;
        }

        //-----------------------------------------------------------------------------
        // Fraction list implementation
        const w_class_t Fraction::List::metadata        = { "Fraction::List", &ListBox::metadata };

        Fraction::List::List(Display *dpy, Fraction *frac, Combo *combo):
            ListBox(dpy)
        {
            pFrac           = frac;
            pCombo          = combo;

            pClass          = &metadata;
        }

        void Fraction::List::property_changed(Property *prop)
        {
            ListBox::property_changed(prop);
            if (vItems.is(prop))
                pFrac->query_resize();
        }

        status_t Fraction::List::on_submit()
        {
            pCombo->sOpened.set(false);
            return pFrac->sSlots.execute(SLOT_SUBMIT, pFrac, NULL);
        }

        status_t Fraction::List::on_change()
        {
            ListBoxItem *it  = vSelected.any();
            ListBoxItem *old = pCombo->sSelected.set(it);

            if (old != it)
                pFrac->sSlots.execute(SLOT_CHANGE, pFrac, NULL);

            return STATUS_OK;
        }

        //-----------------------------------------------------------------------------
        // Combo item implementation
        Fraction::Combo::Combo(Display *dpy, Fraction *frac):
            sList(dpy, frac, this),
            sWindow(dpy, frac, this),
            sColor(&frac->sProperties),
            sText(&frac->sProperties),
            sSelected(&frac->sProperties),
            sOpened(&frac->sProperties)
        {
            pFrac           = frac;

            sArea.nLeft     = 0;
            sArea.nTop      = 0;
            sArea.nWidth    = 0;
            sArea.nHeight   = 0;
        }

        status_t Fraction::Combo::init(const char *prefix)
        {
            char buf[0x40], *end; // should be enough
            size_t len      = strlen(prefix);
            end             = &buf[len];
            Style *style    = &pFrac->sStyle;

            memcpy(buf, prefix, len);
            strcpy(end, ".color");
            sColor.bind(buf, style);
            sText.bind(style, pFrac->display()->dictionary());
            strcpy(end, ".opened");
            sOpened.bind(buf, style);

            // Initialize widgets
            status_t result = sWindow.init();
            if (result == STATUS_OK)
                result  = sList.init();
            if (result != STATUS_OK)
                return result;

            // Configure Window
            sWindow.add(&sList);
            sWindow.add_arrangement(A_BOTTOM, 0, true);
            sWindow.add_arrangement(A_TOP, 0, true);
            sWindow.layout()->set_scale(1.0f);

            return STATUS_OK;
        }

        void Fraction::Combo::destroy()
        {
            sList.set_parent(NULL);
            sList.destroy();
            sWindow.destroy();
        }

        void Fraction::Combo::property_changed(Property *prop)
        {
            if (sColor.is(prop))
                pFrac->query_draw();
            if (sText.is(prop))
                pFrac->query_resize();
            if (sSelected.is(prop))
            {
                ListBoxItem *it = sSelected.get();
                if (sList.items()->contains(it))
                {
                    sList.selected()->clear();
                    sList.selected()->add(it);
                }
                else
                    sSelected.set(NULL);

                pFrac->query_resize();
            }
            if (sOpened.is(prop))
                pFrac->set_visibility(this, sOpened.get());
        }

        bool Fraction::Combo::scroll_item(ssize_t direction, size_t count)
        {
            WidgetList<ListBoxItem> *wl = sList.items();
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
                pFrac->sSlots.execute(SLOT_CHANGE, pFrac, NULL);
                return true;
            }

            return false;
        }

        //-----------------------------------------------------------------------------
        // Fraction implementation
        const w_class_t Fraction::metadata          = { "Fraction", &Widget::metadata };

        Fraction::Fraction(Display *dpy):
            Widget(dpy),
            sNum(dpy, this),
            sDen(dpy, this),
            sColor(&sProperties),
            sFont(&sProperties),
            sAngle(&sProperties),
            sTextPad(&sProperties),
            sThick(&sProperties)
        {
            nMBState        = 0;
            enTrgState      = NONE_CLICK;

            pClass          = &metadata;
        }

        Fraction::~Fraction()
        {
            do_destroy();
        }

        void Fraction::do_destroy()
        {
        }

        void Fraction::destroy()
        {
            Widget::destroy();
            do_destroy();
        }

        void Fraction::set_visibility(Combo *cb, bool visible)
        {
            bool open = cb->sWindow.visibility()->get();
            if (open == visible)
                return;

            if (open)
            {
                cb->sWindow.hide();
                return;
            }

            // Make exclusive lists
            if (cb == &sNum) // Triggered numerator?
                sDen.sOpened.set(false);
            else if (cb == &sDen) // Triggered denominator?
                sNum.sOpened.set(false);

            ws::rectangle_t r;
            this->get_padded_screen_rectangle(&r);
            r.nLeft    += cb->sArea.nLeft - (cb->sArea.nWidth  >> 1);
            r.nTop     += cb->sArea.nTop  - (cb->sArea.nHeight >> 1) - r.nHeight;

            cb->sWindow.trigger_area()->set(&r);
            cb->sWindow.trigger_widget()->set(this);
            cb->sWindow.show(this);

            cb->sWindow.grab_events(ws::GRAB_DROPDOWN);
            cb->sWindow.take_focus();
            cb->sList.take_focus();
        }

        status_t Fraction::init()
        {
            // Initialize widget
            status_t res = Widget::init();
            if (res == STATUS_OK)
                res     = sNum.init("num");
            if (res == STATUS_OK)
                res     = sDen.init("den");

            sColor.bind("color", &sStyle);
            sFont.bind("font", &sStyle);
            sAngle.bind("angle", &sStyle);
            sTextPad.bind("text.pad", &sStyle);
            sThick.bind("thick", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sColor.init(sclass, "#000000");
                sFont.init(sclass, 14.0f);
                sAngle.init(sclass, 60.0f);
                sTextPad.init(sclass, 6);
                sThick.init(sclass, 1);
            }

            // Bind slots
            handler_id_t id;
            id = sSlots.add(SLOT_CHANGE, slot_on_change, self());
            if (id >= 0) id = sSlots.add(SLOT_SUBMIT, slot_on_change, self());
            if (id < 0)
                return -id;

            return STATUS_OK;
        }

        void Fraction::property_changed(Property *prop)
        {
            Widget::property_changed(prop);
            sNum.property_changed(prop);
            sDen.property_changed(prop);

            if (sColor.is(prop))
                query_draw();
            if (sFont.is(prop))
                query_resize();
            if (sAngle.is(prop))
                query_resize();
            if (sTextPad.is(prop))
                query_resize();
            if (sThick.is(prop))
                query_resize();
        }

        ssize_t Fraction::estimate_size(Combo *cb, ws::rectangle_t *r)
        {
            // Get font parameters
            ws::text_parameters_t tp;
            float scaling = lsp_max(0.0f, sScaling.get());

            // Estimate empty text size
            LSPString s;
            cb->sText.format(&s);
            sFont.get_text_parameters(pDisplay, &tp, scaling, &s);
            r->nWidth  = tp.Width;
            r->nHeight = tp.Height;

            // Estimate all visible items size
            WidgetList<ListBoxItem> *wl = cb->sList.items();
            ssize_t width       = 0;

            // Estimate the maximum width of the list box
            for (size_t i=0, n=wl->size(); i<n; ++i)
            {
                ListBoxItem *it = wl->get(i);
                if ((it == NULL) || (!it->visibility()->get()))
                    continue;
                it->text()->format(&s);
                if (s.is_empty())
                    continue;

                // Get text parameters
                sFont.get_text_parameters(pDisplay, &tp, scaling, &s);
                r->nWidth       = lsp_max(r->nWidth, tp.Width);
                r->nHeight      = lsp_max(r->nHeight, tp.Width);
            }

            return width;
        }

        void Fraction::allocate(alloc_t *a)
        {
            ws::font_parameters_t fp;
            float scaling   = lsp_max(0.0f, sScaling.get());
            float angle     = sAngle.get() * M_PI / 180.0f;
            float tpad      = (sTextPad.get() + sThick.get()) * scaling;

            sFont.get_parameters(pDisplay, scaling, &fp);
            estimate_size(&sNum, &a->sNum);
            estimate_size(&sDen, &a->sDen);

            a->sNum.nHeight = lsp_max(fp.Height, a->sNum.nHeight);
            a->sDen.nHeight = lsp_max(fp.Height, a->sDen.nHeight);

            // Fraction rotation
            float dx        = cosf(angle);
            float dy        = sinf(angle);
            a->dx           = dx;
            a->dy           = dy;

            a->sNum.nLeft   = -dy * (a->sNum.nHeight + tpad) * 0.5f;
            a->sNum.nTop    = -dx * (a->sNum.nHeight + tpad) * 0.5f;
            a->sDen.nLeft   = dy * (a->sDen.nHeight + tpad) * 0.5f;
            a->sDen.nTop    = dx * (a->sDen.nHeight + tpad) * 0.5f;

            ssize_t nwh     = a->sNum.nWidth  >> 1;
            ssize_t nhh     = a->sNum.nHeight >> 1;
            ssize_t dwh     = a->sDen.nWidth  >> 1;
            ssize_t dhh     = a->sDen.nHeight >> 1;

            ssize_t dx1     = (a->sNum.nLeft - nwh) - (a->sDen.nLeft + dwh);
            ssize_t dx2     = (a->sNum.nLeft + nwh) - (a->sDen.nLeft - dwh);
            ssize_t dy1     = (a->sNum.nTop  - nhh) - (a->sDen.nTop  + dhh);
            ssize_t dy2     = (a->sNum.nTop  + nhh) - (a->sDen.nTop  - dhh);

            a->sSize.nLeft  = 0;
            a->sSize.nTop   = 0;
            a->sSize.nWidth = lsp_max(lsp_abs(dx1), lsp_abs(dx2));
            a->sSize.nHeight= lsp_max(lsp_abs(dy1), lsp_abs(dy2));

            ssize_t cx      = a->sSize.nWidth  >> 1;
            ssize_t cy      = a->sSize.nHeight >> 1;

            a->sNum.nLeft  += cx;
            a->sNum.nTop   += cy;
            a->sDen.nLeft  += cx;
            a->sDen.nTop   += cy;
        }

        void Fraction::size_request(ws::size_limit_t *r)
        {
            alloc_t a;
            allocate(&a);

            r->nMinWidth    = a.sSize.nWidth;
            r->nMinHeight   = a.sSize.nHeight;
            r->nMaxWidth    = a.sSize.nWidth;
            r->nMaxHeight   = a.sSize.nHeight;
            r->nPreWidth    = r->nMinWidth;
            r->nPreHeight   = r->nMinHeight;
        }

        void Fraction::realize(const ws::rectangle_t *r)
        {
            alloc_t a;
            allocate(&a);

            sNum.sArea      = a.sNum;
            sDen.sArea      = a.sDen;

            Widget::realize(r);
        }

        void Fraction::draw(ws::ISurface *s)
        {
            LSPString num, den;

            ws::font_parameters_t fp;
            ws::text_parameters_t tp, bp;

            float scaling   = lsp_max(0.0f, sScaling.get());
            float bright    = sBrightness.get();
            float angle     = sAngle.get() * M_PI / 180.0f;
            float lw        = lsp_max(1.0f, sThick.get() * scaling * ((sFont.bold()) ? 2.0f : 1.0f));

            // Prepare palette
            lsp::Color bg_color(sBgColor);
            lsp::Color color(sColor);
            lsp::Color tc(sNum.sColor);
            lsp::Color bc(sDen.sColor);

            color.scale_lightness(bright);
            tc.scale_lightness(bright);
            bc.scale_lightness(bright);

            // Clear
            s->clear(bg_color);

            // Get numerator text
            ListBoxItem *sel = sNum.sSelected.get();
            if (sel != NULL)
            {
                if (!sNum.sList.items()->contains(sel))
                    sel     = NULL;
                else if (!sel->visibility()->get())
                    sel     = NULL;
            }

            if (sel != NULL)
                sel->text()->format(&num);
            else
                sNum.sText.format(&num);

            // Get denominator text
            sel = sDen.sSelected.get();
            if (sel != NULL)
            {
                if (!sDen.sList.items()->contains(sel))
                    sel     = NULL;
                else if (!sel->visibility()->get())
                    sel     = NULL;
            }

            if (sel != NULL)
                sel->text()->format(&den);
            else
                sDen.sText.format(&den);

            // Get font parameters
            sFont.get_parameters(s, scaling, &fp);
            sFont.get_text_parameters(s, &tp, scaling, &num);
            sFont.get_text_parameters(s, &bp, scaling, &den);

            // Draw line
            float dx        = fp.Height * cosf(angle);
            float dy        = fp.Height * sinf(angle);
            ssize_t cx      = sSize.nWidth >> 1;
            ssize_t cy      = sSize.nHeight >> 1;
            bool aa         = s->set_antialiasing(true);
            s->line(cx + dx, cy - dy, cx - dx, cy + dy, lw, color);

            // Output numerator and denominator
//            s->fill_rect(&tc, sNum.sArea.nLeft, sNum.sArea.nTop, 1, 1);
//            s->wire_rect(&tc,
//                    sNum.sArea.nLeft - sNum.sArea.nWidth*0.5f, sNum.sArea.nTop  - sNum.sArea.nHeight*0.5f,
//                    sNum.sArea.nWidth, sNum.sArea.nHeight, 1.0f
//                );
//            s->fill_rect(&bc, sDen.sArea.nLeft, sDen.sArea.nTop, 1, 1);
//            s->wire_rect(&bc,
//                    sDen.sArea.nLeft - sDen.sArea.nWidth*0.5f, sDen.sArea.nTop  - sDen.sArea.nHeight*0.5f,
//                    sDen.sArea.nWidth, sDen.sArea.nHeight, 1.0f
//                );

            sFont.draw(
                s, tc,
                sNum.sArea.nLeft - (tp.Width*0.5f) - tp.XBearing,
                sNum.sArea.nTop  + fp.Ascent  - fp.Height*0.5f,
                scaling, &num
            );
            sFont.draw(
                s, bc,
                sDen.sArea.nLeft - (bp.Width*0.5f) - bp.XBearing,
                sDen.sArea.nTop  + fp.Ascent  - fp.Height*0.5f,
                scaling, &den
            );

            s->set_antialiasing(aa);
        }

        status_t Fraction::slot_on_change(Widget *sender, void *ptr, void *data)
        {
            Fraction *_this = widget_ptrcast<Fraction>(ptr);
            return (_this != NULL) ? _this->on_change() : STATUS_BAD_ARGUMENTS;
        }

        status_t Fraction::slot_on_submit(Widget *sender, void *ptr, void *data)
        {
            Fraction *_this = widget_ptrcast<Fraction>(ptr);
            return (_this != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }

        bool Fraction::check_mouse_over(const ws::rectangle_t *area, const ws::event_t *ev)
        {
            ssize_t x = ev->nLeft - sSize.nLeft + (area->nWidth >> 1);
            ssize_t y = ev->nTop  - sSize.nTop  + (area->nHeight >> 1);

            return Position::inside(area, x, y);
        }

        status_t Fraction::on_mouse_down(const ws::event_t *e)
        {
            if (nMBState == 0)
            {
                if (check_mouse_over(&sNum.sArea, e))
                    enTrgState  = NUM_CLICK;
                else if (check_mouse_over(&sDen.sArea, e))
                    enTrgState  = DENOM_CLICK;
                else
                    enTrgState  = NONE_CLICK;
            }

            nMBState |= (1 << e->nCode);

            return STATUS_OK;
        }

        status_t Fraction::on_mouse_up(const ws::event_t *e)
        {
            size_t mask = 1 << e->nCode;

            if ((nMBState == mask) && (mask == ws::MCF_LEFT))
            {
                if ((enTrgState == NUM_CLICK) && (check_mouse_over(&sNum.sArea, e)))
                    sNum.sOpened.set(true);
                else if ((enTrgState == DENOM_CLICK) && (check_mouse_over(&sDen.sArea, e)))
                    sDen.sOpened.set(true);

                enTrgState  = NONE_CLICK;
            }

            nMBState &= ~mask;

            return STATUS_OK;
        }

        status_t Fraction::on_mouse_scroll(const ws::event_t *e)
        {
            ssize_t dir = (e->nCode == ws::MCD_UP) ? -1 :
                          (e->nCode == ws::MCD_DOWN) ? 1 : 0;
            if (dir == 0)
                return STATUS_OK;

            if (check_mouse_over(&sNum.sArea, e))
                return sNum.scroll_item(dir, 1);
            else if (check_mouse_over(&sDen.sArea, e))
                return sDen.scroll_item(dir, 1);

            return STATUS_OK;
        }

        status_t Fraction::on_change()
        {
            return STATUS_OK;
        }

        status_t Fraction::on_submit()
        {
            return STATUS_OK;
        }

    }
}
