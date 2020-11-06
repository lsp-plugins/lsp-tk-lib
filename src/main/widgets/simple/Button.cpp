/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 21 июн. 2017 г.
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
            LSP_TK_STYLE_IMPL_BEGIN(Button, Widget)
                // Bind
                sColor.bind("color", this);
                sLightColor.bind("led.color", this);
                sTextColor.bind("text.color", this);
                sLTextColor.bind("led.text.color", this);
                sHoleColor.bind("hole.color", this);
                sFont.bind("font", this);
                sConstraints.bind("size.constraints", this);
                sTextLayout.bind("text.layout", this);
                sMode.bind("mode", this);
                sDown.bind("down", this);
                sLed.bind("led", this);
                sEditable.bind("editable", this);
                sHole.bind("hole", this);
                sFlat.bind("flat", this);
                sTextClip.bind("text.clip", this);
                sTextPadding.bind("text.padding", this);
                // Configure
                sColor.set("#cccccc");
                sLightColor.set("#00cc00");
                sTextColor.set("#000000");
                sLTextColor.set("#000000");
                sHoleColor.set("#000000");
                sFont.set_size(12.0f);
                sConstraints.set(18, 18, -1, -1);
                sTextLayout.set(0.0f, 0.0f);
                sMode.set(BM_NORMAL);
                sDown.set(false);
                sLed.set(0);
                sEditable.set(true);
                sHole.set(true);
                sFlat.set(false);
                sTextClip.set(false);
                sTextPadding.set(2, 2, 2, 2);
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(Button, "Button");
        }

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
            sTextLayout(&sProperties),
            sMode(&sProperties),
            sDown(&sProperties),
            sLed(&sProperties),
            sEditable(&sProperties),
            sHole(&sProperties),
            sFlat(&sProperties),
            sTextClip(&sProperties),
            sTextPadding(&sProperties)
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
            nFlags     |= FINALIZED;
        }

        status_t Button::init()
        {
            status_t result = Widget::init();
            if (result != STATUS_OK)
                return result;

            sColor.bind("color", &sStyle);
            sLightColor.bind("led.color", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sLTextColor.bind("led.text.color", &sStyle);
            sHoleColor.bind("hole.color", &sStyle);
            sFont.bind("font", &sStyle);
            sText.bind(&sStyle, pDisplay->dictionary());
            sConstraints.bind("size.constraints", &sStyle);
            sTextLayout.bind("text.layout", &sStyle);
            sMode.bind("mode", &sStyle);
            sDown.bind("down", &sStyle);
            sLed.bind("led", &sStyle);
            sEditable.bind("editable", &sStyle);
            sHole.bind("hole", &sStyle);
            sFlat.bind("flat", &sStyle);
            sTextClip.bind("text.clip", &sStyle);
            sTextPadding.bind("text.padding", &sStyle);

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
            if (sTextLayout.is(prop))
                query_draw();
            if (sTextClip.is(prop))
                query_draw();

            if (sMode.is(prop))
                update_mode(sMode.get());

            if (sLed.is(prop))
            {
                size_t state = (sLed.get() > 0) ? nState | S_LED : nState & (~S_LED);
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

            if (sFlat.is(prop))
            {
                nState = sFlat.add_as_flag(nState, S_FLAT);
                query_draw();
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
                nState = sEditable.add_as_flag(nState, S_EDITABLE);
                query_draw();
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

        ws::IGradient *Button::create_gradient(ws::ISurface *s, ws::rectangle_t &r, size_t pressed, float r1, float r2)
        {
            // Create gradient
            float xoff  = 0.0f;
            float yoff  = 0.0f;

            if (pressed & S_DOWN)
            {
                xoff    = 0;
                yoff    = r.nHeight;
            }
            else
            {
                xoff    = r.nWidth;
                yoff    = 0;
            }

            if (pressed & S_PRESSED)
                r2 *= 1.25f;

            return s->radial_gradient(
                r.nLeft + xoff, r.nTop + yoff, r1,
                r.nLeft + xoff, r.nTop + yoff, r2
            );
        }

        void Button::draw(ws::ISurface *s)
        {
            ws::IGradient *g    = NULL;
            size_t pressed      = nState;
            float brightness    = sBrightness.get();
            float scaling       = lsp_max(0.0f, sScaling.get());
            ws::rectangle_t r   = sButton;
            r.nLeft            -= sSize.nLeft;
            r.nTop             -= sSize.nTop;

            // Prepare palette
            lsp::Color bg_color(sBgColor);
            lsp::Color color(sColor);
            lsp::Color tcolor(sTextColor);
            if (pressed & S_LED)
            {
                if (pressed & S_DOWN)
                {
                    color.copy(sLightColor);
                    tcolor.copy(sLTextColor);
                }
                else
                {
                    color.scale_lightness(0.5f);
                    tcolor.scale_lightness(0.5f);
                }
            }

            color.scale_lightness(brightness);
            tcolor.scale_lightness(brightness);

            // Draw background
            bool aa     = s->set_antialiasing(false);
            s->fill_rect(bg_color, 0, 0, sSize.nWidth, sSize.nHeight);

            // Draw hole
            if (pressed & S_HOLE)
            {
                lsp::Color hcolor(sHoleColor);
                size_t hole         = lsp_max(1, scaling);
                s->fill_rect(hcolor, r.nLeft - hole, r.nTop - hole,
                        r.nWidth + hole*2, r.nHeight + hole * 2);
            }

            // Draw light
            if ((pressed & (S_LED | S_DOWN)) == (S_LED | S_DOWN))
            {
                float c_x   = sSize.nWidth  >> 1;
                float c_y   = sSize.nHeight >> 1;
                float h_p   = sButton.nLeft - sSize.nLeft;
                float v_p   = sButton.nTop  - sSize.nTop;
                ssize_t xe  = sSize.nWidth  - 1;
                ssize_t ye  = sSize.nHeight - 1;

                lsp::Color lc(color);
//                lc.lightness(1.0f);

                // Left
                g   =  s->linear_gradient(h_p, c_y, 0, c_y);
                g->add_color(0.0, lc, 0.5f);
                g->add_color(1.0, color, 1.0f);
                s->fill_triangle(0, 0, c_x, c_y, 0, ye, g);
                delete g;

                // Right
                g   =  s->linear_gradient(xe - h_p, c_y, xe, c_y);
                g->add_color(0.0, lc, 0.5f);
                g->add_color(1.0, color, 1.0f);
                s->fill_triangle(xe, ye, c_x, c_y, xe, 0, g);
                delete g;

                // Top
                g   =  s->linear_gradient(c_x, v_p, c_x, 0);
                g->add_color(0.0, lc, 0.5f);
                g->add_color(1.0, color, 1.0f);
                s->fill_triangle(0, 0, xe, 0, c_x, c_y, g);
                delete g;

                // Bottom
                g   =  s->linear_gradient(c_x, ye - v_p, c_x, ye);
                g->add_color(0.0, lc, 0.5f);
                g->add_color(1.0, color, 1.0f);
                s->fill_triangle(xe, ye, 0, ye, c_x, c_y, g);
                delete g;

//                s->set_antialiasing(aa);
//                return;
            }

            // Draw button
            {
                float delta         = sqrtf(r.nWidth * r.nWidth + r.nHeight * r.nHeight);
                float xb            = color.lightness();
                size_t max_chamfer  = lsp_max(1, scaling * 3.0f);
                size_t chamfer      = (pressed & S_PRESSED) ? lsp_max(1, scaling * 2.0f) : max_chamfer;

                // Draw champfer
                if ((!(pressed & S_FLAT)) || ((pressed & (S_PRESSED | S_DOWN)) != 0))
                {
                    for (size_t i=0; i<chamfer; ++i)
                    {
                        // Compute color
                        float bright = float(i + 1.0f) / (chamfer + 1);

                        // Create gradient
                        g = create_gradient(s, r, pressed, 0, delta);
                        color.lightness(bright);
                        g->add_color(0.0, color.red(), color.green(), color.blue());
                        color.lightness(xb * bright);
                        g->add_color(1.0, color.red(), color.green(), color.blue());
                        s->fill_rect(g, r.nLeft, r.nTop, r.nWidth, r.nHeight);
                        delete g;

                        // Update rect
                        r.nLeft        += 1;
                        r.nTop         += 1;
                        r.nWidth       -= 2;
                        r.nHeight      -= 2;
                    }
                }

                // Draw button face
                g = create_gradient(s, r, pressed, 0, delta);
                color.lightness(1.0f);
                g->add_color(0.0, color.red(), color.green(), color.blue());
                color.lightness(xb);
                g->add_color(1.0, color.red(), color.green(), color.blue());
                s->fill_rect(g, r.nLeft, r.nTop, r.nWidth, r.nHeight);
                delete g;

                // Do we have a text?
                LSPString text;
                sText.format(&text);
                if (text.length() > 0)
                {
                    chamfer         = max_chamfer - chamfer;
                    r.nLeft        += chamfer;
                    r.nTop         += chamfer;
                    r.nWidth       -= chamfer * 2;
                    r.nHeight      -= chamfer * 2;

                    sTextPadding.enter(&r, scaling);
                    if (!(pressed & S_PRESSED))
                    {
                        r.nLeft    += (pressed & S_TOGGLED) ? scaling : -scaling;
                        r.nTop     += (pressed & S_TOGGLED) ? scaling : -scaling;
                    }

                    s->clip_begin(r.nLeft, r.nTop, r.nWidth, r.nHeight);

                    // Estimate font parameters
                    ws::font_parameters_t fp;
                    ws::text_parameters_t tp;
                    sFont.get_parameters(pDisplay, scaling, &fp);
                    sFont.get_multitext_parameters(pDisplay, &tp, scaling, &text);

                    // Prepare to draw
                    float halign    = lsp_limit(sTextLayout.halign() + 1.0f, 0.0f, 2.0f);
                    float valign    = lsp_limit(sTextLayout.valign() + 1.0f, 0.0f, 2.0f);
                    float dy        = (r.nHeight - tp.Height) * 0.5f;
                    ssize_t y       = r.nTop + dy * valign - fp.Descent;

                    // Estimate text size
                    ssize_t last = 0, curr = 0, tail = 0, len = text.length();

                    while (curr < len)
                    {
                        // Get next line indexes
                        curr    = text.index_of(last, '\n');
                        if (curr < 0)
                        {
                            curr        = len;
                            tail        = len;
                        }
                        else
                        {
                            tail        = curr;
                            if ((tail > last) && (text.at(tail-1) == '\r'))
                                --tail;
                        }

                        // Calculate text location
                        sFont.get_text_parameters(s, &tp, scaling, &text, last, tail);
                        float dx    = (r.nWidth - tp.Width) * 0.5f;
                        ssize_t x   = r.nLeft   + dx * halign - tp.XBearing;
                        y          += fp.Height;

                        sFont.draw(s, tcolor, x, y, scaling, &text, last, tail);
                        last    = curr + 1;
                    }

                    s->clip_end();
                }
            }

            s->set_antialiasing(aa);
        }

        void Button::size_request(ws::size_limit_t *r)
        {
            // Compute basic elements
            LSPString text;
            ws::rectangle_t xr;

            float scaling       = lsp_max(0.0f, sScaling.get());

            xr.nWidth       = 0;
            xr.nHeight      = 0;

            // Need to analyze text parameters?
            sText.format(&text);

            if ((text.length() > 0) && (!sTextClip.get()))
            {
                ws::font_parameters_t fp;
                ws::text_parameters_t tp;

                sFont.get_parameters(pDisplay, scaling, &fp);
                sFont.get_multitext_parameters(pDisplay, &tp, scaling, &text);

                ssize_t tminw   = ceil(tp.Width);
                ssize_t tminh   = ceil(lsp_max(tp.Height, fp.Height));

                xr.nWidth       = lsp_max(xr.nWidth, tminw);
                xr.nHeight      = lsp_max(xr.nHeight, tminh);

                sTextPadding.add(&xr, scaling);
            }

            size_t chamfer      = lsp_max(1, scaling * 3.0f);
            size_t hole         = (nState & S_HOLE) ? lsp_max(1, scaling) : 0;
            size_t light        = (nState & S_LED)  ? lsp_max(1, scaling * (sLed.get() + 2)) : 0;
            size_t outer        = lsp_max(hole, light);

            xr.nWidth          += (chamfer + outer) * 2;
            xr.nHeight         += (chamfer + outer) * 2;

            r->nMinWidth        = xr.nWidth;
            r->nMinHeight       = xr.nHeight;
            r->nMaxWidth        = -1;
            r->nMaxHeight       = -1;
            r->nPreWidth        = -1;
            r->nPreHeight       = -1;

            sConstraints.apply(r, scaling);
        }

        void Button::realize(const ws::rectangle_t *r)
        {
            Widget::realize(r);

            float scaling       = lsp_max(0.0f, sScaling.get());

            size_t hole         = (nState & S_HOLE) ? lsp_max(1, scaling) : 0;
            size_t light        = (nState & S_LED)  ? lsp_max(1, scaling * (sLed.get() + 2)) : 0;
            size_t outer        = lsp_max(hole, light);

            sButton.nLeft       = r->nLeft   + outer;
            sButton.nTop        = r->nTop    + outer;
            sButton.nWidth      = r->nWidth  - outer*2;
            sButton.nHeight     = r->nHeight - outer*2;
        }

        status_t Button::on_mouse_down(const ws::event_t *e)
        {
            if (!(nState & S_EDITABLE))
                return STATUS_OK;

            take_focus();

            bool m_over         = Position::inside(&sButton, e->nLeft, e->nTop);
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
                    sDown.commit_value(nState & S_DOWN);
                    nChanges    ++;
                    sSlots.execute(SLOT_CHANGE, this);
                }
                else if ((!(nState & S_PRESSED)) && (nState & S_DOWN))
                {
                    nState      &= ~S_DOWN;
                    sDown.commit_value(nState & S_DOWN);
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
            bool m_over         = Position::inside(&sButton, e->nLeft, e->nTop);

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
                        sDown.commit_value(nState & S_DOWN);
                        sSlots.execute(SLOT_CHANGE, this);
                    }
                    else if ((!(nState & S_PRESSED)) && (nState & S_DOWN))
                    {
                        nState      &= ~S_DOWN;
                        sDown.commit_value(nState & S_DOWN);
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
                        sDown.commit_value(nState & S_DOWN);
                        nChanges    ++;
                        sSlots.execute(SLOT_CHANGE, this);
                    }
                    else if ((!(nState & S_TOGGLED)) && (nState & S_DOWN))
                    {
                        nState      &= ~S_DOWN;
                        sDown.commit_value(nState & S_DOWN);
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
                    sDown.commit_value(nState & S_DOWN);
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
            if ((nBMask == (1 << ws::MCB_LEFT)) && (Position::inside(&sButton, e->nLeft, e->nTop)))
                nState     |= S_PRESSED;
            else
                nState     &= ~S_PRESSED;

            // Special case for trigger button
            if ((nState & S_TRIGGER) && (state != nState))
            {
                if ((nState & S_PRESSED) && (!(nState & S_DOWN)))
                {
                    nState      |= S_DOWN;
                    sDown.commit_value(nState & S_DOWN);
                    nChanges    ++;
                    sSlots.execute(SLOT_CHANGE, this);
                }
                else if ((!(nState & S_PRESSED)) && (nState & S_DOWN))
                {
                    nState      &= ~S_DOWN;
                    sDown.commit_value(nState & S_DOWN);
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
