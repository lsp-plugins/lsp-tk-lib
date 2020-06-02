/*
 * Button.cpp
 *
 *  Created on: 21 июн. 2017 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t Button::metadata        = { "Button", &Widget::metadata };

        Button::Button(Display *dpy):
            Widget(dpy),
            sColor(&sProperties),
            sLightColor(&sProperties),
            sTextColor(&sProperties),
            sLTextColor(&sProperties),
            sHoleColor(&sProperties),
            sFont(&sProperties),
            sText(&sProperties),
            sConstraints(&sProperties),
            sMode(&sProperties),
            sDown(&sProperties),
            sLed(&sProperties),
            sEditable(&sProperties),
            sHole(&sProperties)
        {
            nState          = 0;
            nBMask          = 0;
            nChanges        = 0;
            sButton.nLeft   = -1;
            sButton.nTop    = -1;
            sButton.nWidth  = 0;
            sButton.nHeight = 0;

            pClass          = &metadata;
        }
        
        Button::~Button()
        {
        }

        status_t Button::init()
        {
            status_t result = Widget::init();
            if (result != STATUS_OK)
                return result;

            sColor.bind("color", &sStyle);
            sLightColor.bind("light.color", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sLTextColor.bind("light.text.color", &sStyle);
            sHoleColor.bind("hole.color", &sStyle);
            sFont.bind("font", &sStyle);
            sText.bind(&sStyle, pDisplay->dictionary());
            sConstraints.bind("size.constraints", &sStyle);
            sMode.bind("mode", &sStyle);
            sDown.bind("down", &sStyle);
            sLed.bind("led", &sStyle);
            sEditable.bind("editable", &sStyle);
            sHole.bind("hole", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sColor.init(sclass, "#cccccc");
                sLightColor.init(sclass, "#00cc00");
                sTextColor.init(sclass, "#000000");
                sLTextColor.init(sclass, "#000000");
                sHoleColor.init(sclass, "#000000");
                sFont.init(sclass, 12.0f);
                sConstraints.init(sclass, 18, 18, -1, -1);
                sMode.init(sclass, BM_NORMAL);
                sDown.init(sclass, false);
                sLed.init(sclass, false);
                sEditable.init(sclass, true);
                sHole.init(sclass, true);
            }

            // Additional slots
            handler_id_t id = 0;
            id = sSlots.add(SLOT_CHANGE, slot_on_change, self());
            if (id >= 0) id = sSlots.add(SLOT_SUBMIT, slot_on_submit, self());

            return (id >= 0) ? STATUS_OK : -id;
        }

        status_t Button::slot_on_change(Widget *sender, void *ptr, void *data)
        {
            Button *_this = widget_ptrcast<Button>(ptr);
            return (_this != NULL) ? _this->on_change() : STATUS_BAD_ARGUMENTS;
        }

        status_t Button::slot_on_submit(Widget *sender, void *ptr, void *data)
        {
            Button *_this = widget_ptrcast<Button>(ptr);
            return (_this != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }

        status_t Button::on_change()
        {
            return STATUS_OK;
        }

        status_t Button::on_submit()
        {
            return STATUS_OK;
        }


        void Button::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (sColor.is(prop))
                query_draw();
            if (sLightColor.is(prop))
                query_draw();
            if (sTextColor.is(prop))
                query_draw();
            if (sLTextColor.is(prop))
                query_draw();
            if (sHoleColor.is(prop))
                query_draw();
            if (sFont.is(prop))
                query_resize();
            if (sText.is(prop))
                query_resize();
            if (sConstraints.is(prop))
                query_resize();

            if (sMode.is(prop))
                update_mode(sMode.get());

            if (sLed.is(prop))
            {
                size_t state = sLed.add_as_flag(nState, S_LED);
                if (state != nState)
                {
                    nState  = state;
                    query_resize();
                }
            }

            if (sHole.is(prop))
            {
                size_t state = sHole.add_as_flag(nState, S_HOLE);
                if (state != nState)
                {
                    nState  = state;
                    query_resize();
                }
            }

            if (sDown.is(prop))
            {
                size_t state    = nState & (~(S_DOWN | S_PRESSED | S_TOGGLED));
                if (sDown.get())
                    state          |= S_DOWN | ((nState & S_TRIGGER) ? S_PRESSED : S_TOGGLED);
                if (state != nState)
                {
                    nState  = state;
                    query_resize();
                }
            }

            if (sEditable.is(prop))
            {
                size_t state = sLed.add_as_flag(nState, S_EDITABLE);
                if (state != nState)
                {
                    nState  = state;
                    query_draw();
                }
            }
        }

        void Button::update_mode(button_mode_t mode)
        {
            size_t old = nState;
            switch (sMode.get())
            {
                case BM_TRIGGER:
                    nState      = (nState & (~S_TOGGLE)) | S_TRIGGER;
                    break;
                case BM_TOGGLE:
                    nState      = (nState & (~S_TRIGGER)) | S_TOGGLE;
                    break;
                case BM_NORMAL:
                default:
                    nState      = nState & (~(S_TRIGGER | S_TOGGLE));
                    break;
            }

            if (old != nState)
                query_draw();
        }

        bool Button::check_mouse_over(ssize_t x, ssize_t y)
        {
            x              -= sButton.nLeft;
            y              -= sButton.nTop;

            return (x >= 0) && (y >= 0) && (x < sButton.nWidth) && (x < sButton.nHeight);
        }

        void Button::draw(ws::ISurface *s)
        {
//            ws::IGradient *gr = NULL;
//            size_t pressed = nState;
//
//            // Prepare palette
//            Color bg_color(sBgColor);
//            Color color(sColor);
//            color.scale_lightness(brightness());
//
//            // Draw background
//            s->fill_rect(0, 0, sSize.nWidth, sSize.nHeight, bg_color);
//
//            // Calculate real boundaries
//            ssize_t c_x     = (sSize.nWidth >> 1);
//            ssize_t c_y     = (sSize.nHeight >> 1);
//
//            // Calculate parameters
//            Color hole(0.0f, 0.0f, 0.0f);
//            float b_rad  = sqrtf(nWidth*nWidth + nHeight*nHeight);
//            size_t bsize = (nWidth < nHeight) ? nWidth : nHeight;
//            ssize_t b_w  = nWidth >> 1;
//            ssize_t b_h  = nHeight >> 1;
//            ssize_t b_r  = bsize >> 1;          // Button radius
//            ssize_t b_rr = 2 + (bsize >> 4);    // Button rounding radius
//            ssize_t l_rr = (bsize >> 2);
//
//            // Draw hole
//            bool aa = s->set_antialiasing(true);
//            s->fill_round_rect(c_x - b_w - 1, c_y - b_h - 1, nWidth + 2, nHeight + 2, b_rr + 1, hole);
//
//            // Change size if pressed
//            ssize_t b_l = b_rr;
//            if (pressed & S_PRESSED)
//            {
//                b_l ++;
//                b_r --;
//                b_w --;
//                b_h --;
//                b_rr --;
//            }
//            else if (pressed & S_TOGGLED)
//            {
//                b_r --;
//                b_w --;
//                b_h --;
//            }
//            else
//                b_l ++;
//
//            float lightness = color.lightness();
//            if (pressed & S_LED)
//            {
//                // Draw light
////                size_t flag = (nState & S_TRIGGER) ? S_PRESSED : S_TOGGLED;
//
//                if (pressed & S_DOWN)
//                {
//                    ssize_t x_rr = l_rr - 1;
//
//                    gr  =  s->linear_gradient(c_x, c_y - b_h, c_x, c_y - b_h - x_rr);
//                    gr->add_color(0.0, color, 0.5f);
//                    gr->add_color(1.0, color, 1.0f);
//                    s->fill_triangle(c_x - b_w - l_rr, c_y - b_h - l_rr, c_x + b_w + l_rr, c_y - b_h - l_rr, c_x, c_y, gr);
//                    delete gr;
//
//                    gr  =  s->linear_gradient(c_x, c_y + b_h, c_x, c_y + b_h + x_rr);
//                    gr->add_color(0.0, color, 0.5f);
//                    gr->add_color(1.0, color, 1.0f);
//                    s->fill_triangle(c_x + b_w + l_rr, c_y + b_h + l_rr, c_x - b_w - l_rr, c_y + b_h + l_rr, c_x, c_y, gr);
//                    delete gr;
//
//                    gr  =  s->linear_gradient(c_x - b_w, c_y, c_x - b_w - x_rr, c_y);
//                    gr->add_color(0.0, color, 0.5f);
//                    gr->add_color(1.0, color, 1.0f);
//                    s->fill_triangle(c_x - b_w - l_rr, c_y - b_h - l_rr, c_x - b_w - l_rr, c_y + b_h + l_rr, c_x, c_y, gr);
//                    delete gr;
//
//                    gr  =  s->linear_gradient(c_x + b_w, c_y, c_x + b_w + x_rr, c_y);
//                    gr->add_color(0.0, color, 0.5f);
//                    gr->add_color(1.0, color, 1.0f);
//                    s->fill_triangle(c_x + b_w + l_rr, c_y + b_h + l_rr, c_x + b_w + l_rr, c_y - b_h - l_rr, c_x, c_y, gr);
//                    delete gr;
//                }
//                else
//                    lightness  *= 0.5f;
//            }
//
//            for (ssize_t i=0; (i++)<b_l; )
//            {
//                float bright = lightness * sqrtf(i * i) / b_l;
//
//                if (pressed & S_PRESSED)
//                    gr = s->radial_gradient(c_x - b_w, c_y + b_h, b_rad * 0.25f, c_x - b_w, c_y + b_h, b_rad * 3.0f);
//                else if (pressed & S_TOGGLED)
//                    gr = s->radial_gradient(c_x - b_w, c_y + b_h, b_rad * 0.25f, c_x - b_w, c_y + b_h, b_rad * 3.0f);
//                else
//                    gr = s->radial_gradient(c_x + b_w, c_y - b_h, b_rad * 0.25f, c_x + b_w, c_y - b_h, b_rad * 3.0f);
//
//                Color cl(color);
//                cl.lightness(bright);
//                gr->add_color(0.0f, cl);
//                cl.darken(0.9f);
//                gr->add_color(1.0f, cl);
//
//                s->fill_round_rect(c_x - b_w, c_y - b_h, b_w*2, b_h*2, b_rr, gr);
//                delete gr; // Delete gradient!
//
//                if ((--b_r) < 0)
//                    b_r = 0;
//                if ((--b_w) < 0)
//                    b_w = 0;
//                if ((--b_h) < 0)
//                    b_h = 0;
//            }
//
//            if (pressed & S_LED)
//            {
//                Color cl(color);
//                cl.lightness(lightness);
//
//                gr = s->radial_gradient(c_x - b_w, c_y + b_h, b_rad * 0.25f, c_x, c_y, b_rad * 0.8f);
//                gr->add_color(0.0, cl);
//                gr->add_color(1.0, 1.0f, 1.0f, 1.0f);
//                s->fill_round_rect(c_x - b_w, c_y - b_h, b_w * 2.0f, b_h * 2.0f, b_rr, gr);
//                delete gr;
//            }
//
//            // Output text
//            LSPString title;
//            sTitle.format(&title);
//            if (title.length() > 0)
//            {
//                text_parameters_t tp;
//                font_parameters_t fp;
//
//                Color font_color(sFont.raw_color());
//                font_color.scale_lightness(brightness());
//
//                sFont.get_parameters(s, &fp);
//                sFont.get_text_parameters(s, &tp, &title);
//
//                if (pressed & S_PRESSED)
//                {
//                    c_y++;
//                    c_x++;
//                }
//
//                sFont.draw(s, c_x - (tp.XAdvance * 0.5f), c_y - (fp.Height * 0.5f) + fp.Ascent, font_color, &title);
//            }
//
//            s->set_antialiasing(aa);
        }

        void Button::size_request(ws::size_limit_t *r)
        {
//            r->nMaxWidth    = -1;
//            r->nMaxHeight   = -1;
//            r->nMinWidth    = nMinWidth;
//            r->nMinHeight   = nMinHeight;
//
//            LSPString title;
//            sTitle.format(&title);
//
//            if (title.length() > 0)
//            {
//                text_parameters_t tp;
//                font_parameters_t fp;
//
//                ISurface *s = pDisplay->create_surface(1, 1);
//
//                if (s != NULL)
//                {
//                    sFont.get_parameters(s, &fp);
//                    sFont.get_text_parameters(s, &tp, &title);
//                    s->destroy();
//                    delete s;
//
//                    tp.Width       += 10;
//                    fp.Height      += 10;
//
//                    if (r->nMinWidth < tp.Width)
//                        r->nMinWidth    = tp.Width;
//                    if (r->nMinHeight < fp.Height)
//                        r->nMinHeight   = fp.Height;
//                }
//            }
//
//            size_t size     = (nWidth < nHeight) ? nWidth : nHeight;
//            size_t delta    = (nState & S_LED) ? 2 + (size >> 2) : 2;
//
//            r->nMinWidth   += delta;
//            r->nMinHeight  += delta;
        }

        void Button::realize(const ws::rectangle_t *r)
        {
            Widget::realize(r);

//            nWidth      = nMinWidth;
//            nHeight     = nMinHeight;
//
//            LSPString title;
//            sTitle.format(&title);
//            if (title.length() <= 0)
//                return;
//
//            text_parameters_t tp;
//            font_parameters_t fp;
//            ISurface *s = pDisplay->create_surface(1, 1);
//            if (s == NULL)
//                return;
//
//            sFont.get_parameters(s, &fp);
//            sFont.get_text_parameters(s, &tp, &title);
//            s->destroy();
//            delete s;
//
//            tp.Width       += 10;
//            fp.Height      += 10;
//
//            if (nWidth < tp.Width)
//                nWidth      = tp.Width;
//            if (nHeight < fp.Height)
//                nHeight     = fp.Height;
        }

        status_t Button::on_mouse_down(const ws::event_t *e)
        {
            if (!(nState & S_EDITABLE))
                return STATUS_OK;

            take_focus();

            bool m_over         = check_mouse_over(e->nLeft, e->nTop);
            size_t mask         = nBMask;
            nBMask             |= (1 << e->nCode);

            if (!mask)
            {
                if (!m_over)
                {
                    nState             |= S_OUT; // Mark that out of the button area
                    return STATUS_OK;
                }
                else
                    nChanges        = 0;
            }

            if (nState & S_OUT) // Mouse button was initially pressed out of the button area
                return STATUS_OK;

            // Update state according to mouse position and mouse button state
            size_t state        = nState;
            if ((nBMask == (1 << ws::MCB_LEFT)) && (m_over))
                nState     |= S_PRESSED;
            else
                nState     &= ~S_PRESSED;

            // Special case for trigger button
            if ((nState & S_TRIGGER) && (state != nState))
            {
                if ((nState & S_PRESSED) && (!(nState & S_DOWN)))
                {
                    nState      |= S_DOWN;
                    nChanges    ++;
                    sSlots.execute(SLOT_CHANGE, this);
                }
                else if ((!(nState & S_PRESSED)) && (nState & S_DOWN))
                {
                    nState      &= ~S_DOWN;
                    nChanges    ++;
                    sSlots.execute(SLOT_CHANGE, this);
                }
            }

            // Query draw if state changed
            if (state != nState)
                query_draw();

            return STATUS_OK;
        }

        status_t Button::on_mouse_up(const ws::event_t *e)
        {
            if (!(nState & S_EDITABLE))
                return STATUS_OK;

            size_t mask     = nBMask;
            nBMask         &= ~(1 << e->nCode);

            // Mouse button was initially pressed out of the button area, ignore this case
            if ((nBMask == 0) && (nState & S_OUT))
            {
                nState &= ~S_OUT;
                return STATUS_OK;
            }

            size_t state        = nState;
            bool m_over         = check_mouse_over(e->nLeft, e->nTop);

            if (nState & S_TRIGGER)
            {
                // Update state according to mouse position and mouse button state
                size_t state        = nState;
                if ((nBMask == (1 << ws::MCB_LEFT)) && (m_over))
                    nState     |= S_PRESSED;
                else
                    nState     &= ~S_PRESSED;

                if (state != nState)
                {
                    if ((nState & S_PRESSED) && (!(nState & S_DOWN)))
                    {
                        nState      |= S_DOWN;
                        nChanges    ++;
                        sSlots.execute(SLOT_CHANGE, this);
                    }
                    else if ((!(nState & S_PRESSED)) && (nState & S_DOWN))
                    {
                        nState      &= ~S_DOWN;
                        nChanges    ++;
                        sSlots.execute(SLOT_CHANGE, this);
                    }
                }
            }
            else if (nState & S_TOGGLE)
            {
                if ((mask == (1 << ws::MCB_LEFT)) && (e->nCode == ws::MCB_LEFT) && (m_over))
                    nState ^= S_TOGGLED;

                if (state != nState)
                {
                    if ((nState & S_TOGGLED) && (!(nState & S_DOWN)))
                    {
                        nState      |= S_DOWN;
                        nChanges    ++;
                        sSlots.execute(SLOT_CHANGE, this);
                    }
                    else if ((!(nState & S_TOGGLED)) && (nState & S_DOWN))
                    {
                        nState      &= ~S_DOWN;
                        nChanges    ++;
                        sSlots.execute(SLOT_CHANGE, this);
                    }
                }
            }
            else
            {
                // Released left mouse button over the button widget?
                if ((mask == (1 << ws::MCB_LEFT)) && (e->nCode == ws::MCB_LEFT))
                {
                    nState &= ~(S_PRESSED | S_TOGGLED | S_DOWN);
                    if (m_over)
                    {
                        nChanges    ++;
                        sSlots.execute(SLOT_CHANGE, this);
                    }
                }
            }

            if ((nBMask == (1 << ws::MCB_LEFT)) && (m_over))
                nState     |= S_PRESSED;
            else
                nState     &= ~S_PRESSED;

            if ((mask == size_t(1 << e->nCode)) && (nChanges > 0))
            {
                sSlots.execute(SLOT_SUBMIT, this);
                nChanges = 0;
            }

            // Query draw if state changed
            if (state != nState)
                query_draw();

            return STATUS_OK;
        }

        status_t Button::on_mouse_move(const ws::event_t *e)
        {
            if (!(nState & S_EDITABLE))
                return STATUS_OK;

            // Mouse button was initially pressed out of the button area, ignore this case
            if (nState & S_OUT)
                return STATUS_OK;

            // Update state according to mouse position and mouse button state
            size_t state        = nState;
            if ((nBMask == (1 << ws::MCB_LEFT)) && (check_mouse_over(e->nLeft, e->nTop)))
                nState     |= S_PRESSED;
            else
                nState     &= ~S_PRESSED;

            // Special case for trigger button
            if ((nState & S_TRIGGER) && (state != nState))
            {
                if ((nState & S_PRESSED) && (!(nState & S_DOWN)))
                {
                    nState      |= S_DOWN;
                    nChanges    ++;
                    sSlots.execute(SLOT_CHANGE, this);
                }
                else if ((!(nState & S_PRESSED)) && (nState & S_DOWN))
                {
                    nState      &= ~S_DOWN;
                    nChanges    ++;
                    sSlots.execute(SLOT_CHANGE, this);
                }
            }

            // Query draw if state changed
            if (state != nState)
                query_draw();

            return STATUS_OK;
        }

    } /* namespace tk */
} /* namespace lsp */
