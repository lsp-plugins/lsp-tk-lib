/*
 * Fraction.cpp
 *
 *  Created on: 17 авг. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

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
                pFrac->query_resize();
            if (sOpened.is(prop))
                pFrac->set_visibility(this, sOpened.get());
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
            this->get_padded_screen_rectangle(&r, &cb->sArea);

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
                sFont.init(sclass);
                sAngle.init(sclass, 60.0f);
                sTextPad.init(sclass, 2);
                sThick.init(sclass, 2);
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

        void Fraction::size_request(ws::size_limit_t *r)
        {
            // TODO
        }

        void Fraction::realize(const ws::rectangle_t *r)
        {
            // TODO
        }

        void Fraction::draw(ws::ISurface *s)
        {
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

        status_t Fraction::on_mouse_down(const ws::event_t *e)
        {
            // TODO
            return STATUS_OK;
        }

        status_t Fraction::on_mouse_up(const ws::event_t *e)
        {
            // TODO
            return STATUS_OK;
        }

        status_t Fraction::on_mouse_scroll(const ws::event_t *e)
        {
            // TODO
            return STATUS_OK;
        }

        status_t Fraction::on_change()
        {
            // TODO
            return STATUS_OK;
        }

        status_t Fraction::on_submit()
        {
            return STATUS_OK;
        }

    }
}
