/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 17 авг. 2020 г.
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
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            static const char *prop_opened[] =
            {
                "num.opened",
                "den.opened"
            };

            // Fraction style
            LSP_TK_STYLE_IMPL_BEGIN(Fraction, Widget)
                // Bind
                style::FractionColors *c = &vColors[style::FRACTION_NORMAL];
                c->sColor.bind("color", this);
                c->sNumColor.bind("num.color", this);
                c->sDenColor.bind("den.color", this);

                c = &vColors[style::FRACTION_INACTIVE];
                c->sColor.bind("inactive.color", this);
                c->sNumColor.bind("inactive.num.color", this);
                c->sDenColor.bind("inactive.den.color", this);

                sFont.bind("font", this);
                sAngle.bind("angle", this);
                sTextPad.bind("text.pad", this);
                sThick.bind("thick", this);
                sActive.bind("active", this);

                // Configure
                c = &vColors[style::FRACTION_NORMAL];
                c->sColor.set("#000000");
                c->sNumColor.set("#000000");
                c->sDenColor.set("#000000");

                c = &vColors[style::FRACTION_INACTIVE];
                c->sColor.set("#444444");
                c->sNumColor.set("#444444");
                c->sDenColor.set("#444444");

                sFont.set_size(14.0f);
                sAngle.set(60.0f);
                sTextPad.set(6);
                sThick.set(1);
                sActive.set(true);

                // Override
                sFont.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(Fraction, "Fraction", "root");

            // ComboBox::Window style
            LSP_TK_BUILTIN_STYLE(PopupWindow, "Fraction::Window", "PopupWindow");

            // Fraction::List style
            LSP_TK_BUILTIN_STYLE(ListBox, "Fraction::List", "ListBox");

            void FractionColors::listener(tk::prop::Listener *listener)
            {
                sColor.listener(listener);
                sNumColor.listener(listener);
                sDenColor.listener(listener);
            }

            bool FractionColors::property_changed(Property *prop)
            {
                return prop->one_of(sColor, sNumColor, sDenColor);
            }
        }

        //-----------------------------------------------------------------------------
        // Fraction popup window implementation
        const w_class_t Fraction::Window::metadata      = { "Fraction::Window", &PopupWindow::metadata };

        Fraction::Window::Window(Display *dpy, Fraction *frac, Combo *combo):
            PopupWindow(dpy)
        {
            pClass          = &metadata;

            pFrac           = frac;
            pCombo          = combo;

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
        const tether_t Fraction::tether_list[] =
        {
            { TF_LEFT  | TF_TOP | TF_HORIZONTAL | TF_HSTRETCH,      1.0f,  1.0f   },
            { TF_LEFT  | TF_BOTTOM | TF_HORIZONTAL | TF_HSTRETCH,   1.0f,  -1.0f  },
            { TF_LEFT  | TF_TOP | TF_HORIZONTAL | TF_HSTRETCH,      1.0f,  -1.0f  },
            { TF_LEFT  | TF_BOTTOM | TF_HORIZONTAL | TF_HSTRETCH,   1.0f,  1.0f   },
        };

        Fraction::Combo::Combo(Display *dpy, Fraction *frac):
            sList(dpy, frac, this),
            sWindow(dpy, frac, this),
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

        status_t Fraction::Combo::init(size_t idx)
        {
            Style *style    = &pFrac->sStyle;

            sText.bind(style, pFrac->display()->dictionary());
            sOpened.bind(style::prop_opened[idx], style);

            // Initialize widgets
            status_t result = sWindow.init();
            if (result == STATUS_OK)
                result  = sList.init();
            if (result != STATUS_OK)
                return result;

            // Configure Window
            sWindow.add(&sList);
            sWindow.set_tether(tether_list, sizeof(tether_list)/sizeof(tether_t));
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
            sFont(&sProperties),
            sAngle(&sProperties),
            sTextPad(&sProperties),
            sThick(&sProperties),
            sActive(&sProperties)
        {
            pClass          = &metadata;

            nMBState        = 0;
            enTrgState      = NONE_CLICK;

            for (size_t i=0; i<style::FRACTION_TOTAL; ++i)
                vColors[i].listener(&sProperties);
        }

        Fraction::~Fraction()
        {
            nFlags     |= FINALIZED;
            do_destroy();
        }

        void Fraction::do_destroy()
        {
        }

        void Fraction::destroy()
        {
            nFlags     |= FINALIZED;
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
            r.nTop     += cb->sArea.nTop  - (cb->sArea.nHeight >> 1);
            r.nWidth    = cb->sArea.nWidth;
            r.nHeight   = cb->sArea.nHeight;

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
                res     = sNum.init(0);
            if (res == STATUS_OK)
                res     = sDen.init(1);

            style::FractionColors *c = &vColors[style::FRACTION_NORMAL];
            c->sColor.bind("color", &sStyle);
            c->sNumColor.bind("num.color", &sStyle);
            c->sDenColor.bind("den.color", &sStyle);

            c = &vColors[style::FRACTION_INACTIVE];
            c->sColor.bind("inactive.color", &sStyle);
            c->sNumColor.bind("inactive.num.color", &sStyle);
            c->sDenColor.bind("inactive.den.color", &sStyle);

            sFont.bind("font", &sStyle);
            sAngle.bind("angle", &sStyle);
            sTextPad.bind("text.pad", &sStyle);
            sThick.bind("thick", &sStyle);
            sActive.bind("active", &sStyle);

            // Bind slots
            handler_id_t id;
            id = sSlots.add(SLOT_CHANGE, slot_on_change, self());
            if (id >= 0) id = sSlots.add(SLOT_SUBMIT, slot_on_change, self());
            if (id < 0)
                return -id;

            return STATUS_OK;
        }

        style::FractionColors *Fraction::select_colors()
        {
            size_t flags = (sActive.get()) ? style::FRACTION_NORMAL : style::FRACTION_INACTIVE;
            return &vColors[flags];
        }

        void Fraction::property_changed(Property *prop)
        {
            Widget::property_changed(prop);
            sNum.property_changed(prop);
            sDen.property_changed(prop);

            // Self properties
            style::FractionColors *colors = select_colors();
            if (colors->property_changed(prop))
                query_draw();

            if (sActive.is(prop))
                query_draw();

            if (prop->one_of(sFont, sAngle, sTextPad, sThick))
                query_resize();
        }

        ssize_t Fraction::estimate_size(Combo *cb, ws::rectangle_t *r)
        {
            // Get font parameters
            ws::text_parameters_t tp;
            float scaling   = lsp_max(0.0f, sScaling.get());
            float fscaling  = lsp_max(0.0f, scaling * sFontScaling.get());

            // Estimate empty text size
            LSPString s;
            cb->sText.format(&s);
            sFont.get_text_parameters(pDisplay, &tp, fscaling, &s);
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
                sFont.get_text_parameters(pDisplay, &tp, fscaling, &s);
                r->nWidth       = lsp_max(r->nWidth, tp.Width);
                r->nHeight      = lsp_max(r->nHeight, tp.Width);
            }

            return width;
        }

        void Fraction::allocate(alloc_t *a)
        {
            ws::font_parameters_t fp;
            float scaling   = lsp_max(0.0f, sScaling.get());
            float fscaling  = lsp_max(0.0f, scaling * sFontScaling.get());
            float angle     = sAngle.get() * M_PI / 180.0f;
            float tpad      = (sTextPad.get() + sThick.get()) * scaling;

            sFont.get_parameters(pDisplay, fscaling, &fp);
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

        void Fraction::draw(ws::ISurface *s, bool force)
        {
            LSPString num, den;

            ws::font_parameters_t fp;
            ws::text_parameters_t tp, bp;

            float scaling   = lsp_max(0.0f, sScaling.get());
            float fscaling  = lsp_max(0.0f, scaling * sFontScaling.get());
            float bright    = sBrightness.get();
            float angle     = sAngle.get() * M_PI / 180.0f;
            float lw        = lsp_max(1.0f, sThick.get() * scaling * ((sFont.bold()) ? 2.0f : 1.0f));
            const style::FractionColors *colors = select_colors();

            // Prepare palette
            lsp::Color bg_color;
            lsp::Color color(colors->sColor);
            lsp::Color tc(colors->sNumColor);
            lsp::Color bc(colors->sDenColor);

            get_actual_bg_color(bg_color);
            color.scale_lch_luminance(bright);
            tc.scale_lch_luminance(bright);
            bc.scale_lch_luminance(bright);

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
            sFont.get_parameters(s, fscaling, &fp);
            sFont.get_text_parameters(s, &tp, fscaling, &num);
            sFont.get_text_parameters(s, &bp, fscaling, &den);

            // Draw line
            float dx        = fp.Height * cosf(angle);
            float dy        = fp.Height * sinf(angle);
            ssize_t cx      = sSize.nWidth >> 1;
            ssize_t cy      = sSize.nHeight >> 1;
            bool aa         = s->set_antialiasing(true);
            s->line(color, cx + dx, cy - dy, cx - dx, cy + dy, lw);

            // Output numerator and denominator
            sFont.draw(
                s, tc,
                truncf(sNum.sArea.nLeft - (tp.Width * 0.5f) - tp.XBearing),
                truncf(sNum.sArea.nTop  + fp.Ascent  - fp.Height*0.5f),
                fscaling, &num
            );
            sFont.draw(
                s, bc,
                truncf(sDen.sArea.nLeft - (bp.Width * 0.5f) - bp.XBearing),
                truncf(sDen.sArea.nTop  + fp.Ascent  - fp.Height*0.5f),
                fscaling, &den
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
                ssize_t x, y;
                pDisplay->get_pointer_location(0, &x, &y);

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

    } /* namespace tk */
} /* namespace lsp */
