/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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
                // Bind colors
                ButtonColors *c = &vColors[BUTTON_NORMAL];
                c->sColor.bind("color", this);
                c->sTextColor.bind("text.color", this);
                c->sBorderColor.bind("border.color", this);

                c = &vColors[BUTTON_DOWN];
                c->sColor.bind("down.color", this);
                c->sTextColor.bind("text.down.color", this);
                c->sBorderColor.bind("border.down.color", this);

                c = &vColors[BUTTON_HOVER];
                c->sColor.bind("hover.color", this);
                c->sTextColor.bind("text.hover.color", this);
                c->sBorderColor.bind("border.hover.color", this);

                c = &vColors[BUTTON_DOWN | BUTTON_HOVER];
                c->sColor.bind("down.hover.color", this);
                c->sTextColor.bind("text.down.hover.color", this);
                c->sBorderColor.bind("border.down.hover.color", this);

                c = &vColors[BUTTON_INACTIVE];
                c->sColor.bind("inactive.color", this);
                c->sTextColor.bind("inactive.text.color", this);
                c->sBorderColor.bind("inactive.border.color", this);

                c = &vColors[BUTTON_DOWN | BUTTON_INACTIVE];
                c->sColor.bind("inactive.down.color", this);
                c->sTextColor.bind("inactive.text.down.color", this);
                c->sBorderColor.bind("inactive.border.down.color", this);

                c = &vColors[BUTTON_HOVER | BUTTON_INACTIVE];
                c->sColor.bind("inactive.hover.color", this);
                c->sTextColor.bind("inactive.text.hover.color", this);
                c->sBorderColor.bind("inactive.border.hover.color", this);

                c = &vColors[BUTTON_DOWN | BUTTON_HOVER | BUTTON_INACTIVE];
                c->sColor.bind("inactive.down.hover.color", this);
                c->sTextColor.bind("inactive.text.down.hover.color", this);
                c->sBorderColor.bind("inactive.border.down.hover.color", this);

                sHoleColor.bind("hole.color", this);

                sFont.bind("font", this);
                sTextAdjust.bind("text.adjust", this);
                sConstraints.bind("size.constraints", this);
                sTextLayout.bind("text.layout", this);
                sMode.bind("mode", this);
                sDown.bind("down", this);
                sDownColors.bind("down.colors", this);
                sLed.bind("led", this);
                sBorderSize.bind("border.size", this);
                sBorderPressedSize.bind("border.pressed.size", this);
                sBorderDownSize.bind("border.down.size", this);
                sEditable.bind("editable", this);
                sActive.bind("active", this);
                sHole.bind("hole", this);
                sFlat.bind("flat", this);
                sTextClip.bind("text.clip", this);
                sTextPadding.bind("text.padding", this);
                sHover.bind("hover", this);
                sGradient.bind("gradient", this);
                sTextShift.bind("text.shift", this);
                sTextDownShift.bind("text.down.shift", this);
                sTextPressedShift.bind("text.pressed.shift", this);
                // Configure
                c = &vColors[BUTTON_NORMAL];
                c->sColor.set("#cccccc");
                c->sTextColor.set("#000000");
                c->sBorderColor.set("#888888");

                c = &vColors[BUTTON_DOWN];
                c->sColor.set("#ffffff");
                c->sTextColor.set("#000000");
                c->sBorderColor.set("#cccccc");

                c = &vColors[BUTTON_HOVER];
                c->sColor.set("#00cc00");
                c->sTextColor.set("#000000");
                c->sBorderColor.set("#888888");

                c = &vColors[BUTTON_DOWN | BUTTON_HOVER];
                c->sColor.set("#00ff00");
                c->sTextColor.set("#444444");
                c->sBorderColor.set("#888888");

                c = &vColors[BUTTON_INACTIVE];
                c->sColor.set("#cccccc");
                c->sTextColor.set("#444444");
                c->sBorderColor.set("#444444");

                c = &vColors[BUTTON_DOWN | BUTTON_INACTIVE];
                c->sColor.set("#cccccc");
                c->sTextColor.set("#444444");
                c->sBorderColor.set("#cccccc");

                c = &vColors[BUTTON_HOVER | BUTTON_INACTIVE];
                c->sColor.set("#cccccc");
                c->sTextColor.set("#000000");
                c->sBorderColor.set("#888888");

                c = &vColors[BUTTON_DOWN | BUTTON_HOVER | BUTTON_INACTIVE];
                c->sColor.set("#cccccc");
                c->sTextColor.set("#444444");
                c->sBorderColor.set("#888888");

                sHoleColor.set("#000000");

                sFont.set_size(12.0f);
                sTextAdjust.set(tk::TA_NONE);
                sConstraints.set(18, 18, -1, -1);
                sTextLayout.set(0.0f, 0.0f);
                sMode.set(BM_NORMAL);
                sDown.set(false);
                sDownColors.set(false);
                sLed.set(0);
                sBorderSize.set(3);
                sBorderPressedSize.set(3);
                sBorderDownSize.set(2);
                sEditable.set(true);
                sActive.set(true);
                sHole.set(true);
                sFlat.set(false);
                sTextClip.set(false);
                sTextPadding.set(2, 2, 2, 2);
                sGradient.set(true);
                sTextShift.set(-1, -1);
                sTextDownShift.set(0, 0);
                sTextPressedShift.set(1, 1);
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(Button, "Button", "root");

            void ButtonColors::listener(tk::prop::Listener *listener)
            {
                sColor.listener(listener);
                sTextColor.listener(listener);
                sBorderColor.listener(listener);
            }

            bool ButtonColors::property_changed(Property *prop)
            {
                return prop->one_of(sColor, sTextColor, sBorderColor);
            }
        }

        const w_class_t Button::metadata        = { "Button", &Widget::metadata };

        Button::Button(Display *dpy):
            Widget(dpy),
            sHoleColor(&sProperties),
            sFont(&sProperties),
            sText(&sProperties),
            sTextAdjust(&sProperties),
            sConstraints(&sProperties),
            sTextLayout(&sProperties),
            sMode(&sProperties),
            sDown(&sProperties),
            sDownColors(&sProperties),
            sLed(&sProperties),
            sBorderSize(&sProperties),
            sBorderPressedSize(&sProperties),
            sBorderDownSize(&sProperties),
            sEditable(&sProperties),
            sActive(&sProperties),
            sHole(&sProperties),
            sFlat(&sProperties),
            sTextClip(&sProperties),
            sTextPadding(&sProperties),
            sHover(&sProperties),
            sGradient(&sProperties),
            sTextShift(&sProperties),
            sTextDownShift(&sProperties),
            sTextPressedShift(&sProperties)
        {
            pClass          = &metadata;

            for (size_t i=0; i<BTN_TOTAL; ++i)
                vColors[i].listener(&sProperties);

            nState          = 0;
            nBMask          = 0;
            nChanges        = 0;
            sButton.nLeft   = -1;
            sButton.nTop    = -1;
            sButton.nWidth  = 0;
            sButton.nHeight = 0;

        }
        
        Button::~Button()
        {
            nFlags     |= FINALIZED;
            destroy_text_estimations();
        }

        status_t Button::init()
        {
            status_t result = Widget::init();
            if (result != STATUS_OK)
                return result;

            style::ButtonColors *c = &vColors[style::BUTTON_NORMAL];
            c->sColor.bind("color", &sStyle);
            c->sTextColor.bind("text.color", &sStyle);
            c->sBorderColor.bind("border.color", &sStyle);

            c = &vColors[style::BUTTON_DOWN];
            c->sColor.bind("down.color", &sStyle);
            c->sTextColor.bind("text.down.color", &sStyle);
            c->sBorderColor.bind("border.down.color", &sStyle);

            c = &vColors[style::BUTTON_HOVER];
            c->sColor.bind("hover.color", &sStyle);
            c->sTextColor.bind("text.hover.color", &sStyle);
            c->sBorderColor.bind("border.hover.color", &sStyle);

            c = &vColors[style::BUTTON_DOWN | style::BUTTON_HOVER];
            c->sColor.bind("down.hover.color", &sStyle);
            c->sTextColor.bind("text.down.hover.color", &sStyle);
            c->sBorderColor.bind("border.down.hover.color", &sStyle);

            c = &vColors[style::BUTTON_INACTIVE];
            c->sColor.bind("inactive.color", &sStyle);
            c->sTextColor.bind("inactive.text.color", &sStyle);
            c->sBorderColor.bind("inactive.border.color", &sStyle);

            c = &vColors[style::BUTTON_DOWN | style::BUTTON_INACTIVE];
            c->sColor.bind("inactive.down.color", &sStyle);
            c->sTextColor.bind("inactive.text.down.color", &sStyle);
            c->sBorderColor.bind("inactive.border.down.color", &sStyle);

            c = &vColors[style::BUTTON_HOVER | style::BUTTON_INACTIVE];
            c->sColor.bind("inactive.hover.color", &sStyle);
            c->sTextColor.bind("inactive.text.hover.color", &sStyle);
            c->sBorderColor.bind("inactive.border.hover.color", &sStyle);

            c = &vColors[style::BUTTON_DOWN | style::BUTTON_HOVER | style::BUTTON_INACTIVE];
            c->sColor.bind("inactive.down.hover.color", &sStyle);
            c->sTextColor.bind("inactive.text.down.hover.color", &sStyle);
            c->sBorderColor.bind("inactive.border.down.hover.color", &sStyle);

            sHoleColor.bind("hole.color", &sStyle);

            sFont.bind("font", &sStyle);
            sText.bind(&sStyle, pDisplay->dictionary());
            sConstraints.bind("size.constraints", &sStyle);
            sTextLayout.bind("text.layout", &sStyle);
            sTextAdjust.bind("text.adjust", &sStyle);
            sMode.bind("mode", &sStyle);
            sDown.bind("down", &sStyle);
            sDownColors.bind("down.colors", &sStyle);
            sLed.bind("led", &sStyle);
            sBorderSize.bind("border.size", &sStyle);
            sBorderPressedSize.bind("border.pressed.size", &sStyle);
            sBorderDownSize.bind("border.down.size", &sStyle);
            sEditable.bind("editable", &sStyle);
            sActive.bind("active", &sStyle);
            sHole.bind("hole", &sStyle);
            sFlat.bind("flat", &sStyle);
            sTextClip.bind("text.clip", &sStyle);
            sTextPadding.bind("text.padding", &sStyle);
            sHover.bind("hover", &sStyle);
            sGradient.bind("gradient", &sStyle);
            sTextShift.bind("text.shift", &sStyle);
            sTextDownShift.bind("text.down.shift", &sStyle);
            sTextPressedShift.bind("text.pressed.shift", &sStyle);

            // Additional slots
            handler_id_t id = 0;
            id = sSlots.add(SLOT_CHANGE, slot_on_change, self());
            if (id >= 0) id = sSlots.add(SLOT_SUBMIT, slot_on_submit, self());

            return (id >= 0) ? STATUS_OK : -id;
        }

        void Button::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            style::ButtonColors *cols = select_colors();
            if (cols->property_changed(prop))
                query_draw();

            if (sActive.is(prop))
                set_active(sActive.get());

            if (prop->one_of(sHoleColor, sHover, sGradient))
                query_draw();

            if (prop->one_of(sFont, sText, sTextAdjust, sConstraints, sBorderSize, sBorderPressedSize, sBorderDownSize))
                query_resize();

            if (sTextLayout.is(prop))
                query_draw();
            if (sTextClip.is(prop))
                query_draw();

            if (sMode.is(prop))
                update_mode(sMode.get());

            if (sLed.is(prop))
            {
                size_t state = lsp_setflag(nState, S_LED, sLed.get() > 0);
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

        style::ButtonColors *Button::select_colors()
        {
            size_t flags = (sActive.get()) ? style::BUTTON_NORMAL : style::BUTTON_INACTIVE;
            if ((nState & S_DOWN) && ((nState & S_LED) || (sLed.get() > 0) || (sDownColors.get())))
                flags      |= style::BUTTON_DOWN;

            if ((sHover.get()) && (nState & S_HOVER))
                flags      |= style::BUTTON_HOVER;

            return &vColors[flags];
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

        ws::IGradient *Button::create_gradient(ws::ISurface *s, ws::rectangle_t &r, size_t pressed, float radius)
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
                radius *= 1.25f;

            return s->radial_gradient(
                r.nLeft + xoff, r.nTop + yoff,
                r.nLeft + xoff, r.nTop + yoff,
                radius);
        }

        size_t Button::destroy_text_estimations()
        {
            size_t removed = 0;
            for (lltl::iterator<prop::String> it = vEstimations.values(); it; ++it)
            {
                prop::String *s = it.get();
                if (s != NULL)
                {
                    ++removed;
                    delete s;
                }
            }
            vEstimations.clear();

            return removed;
        }

        void Button::clear_text_estimations()
        {
            const size_t removed = destroy_text_estimations();
            if (removed > 0)
                query_resize();
        }

        tk::String *Button::add_text_estimation()
        {
            prop::String *s = new prop::String(&sProperties);
            if (s == NULL)
                return NULL;
            s->bind(&sStyle, pDisplay->dictionary());

            if (vEstimations.add(s))
                return s;

            delete s;
            return NULL;
        }

        void Button::estimate_string_size(estimation_t *e, tk::String *s)
        {
            if (s == NULL)
                return;

            // Form the text string
            s->format(&e->text);
            sTextAdjust.apply(&e->text);

            // Estimate sizes
            sFont.get_multitext_parameters(pDisplay, &e->tp, e->fscaling, &e->text);

            e->min_width     = lsp_max(e->min_width, ceilf(e->tp.Width));
            e->min_height    = lsp_max(e->min_height, ceilf(lsp_max(e->tp.Height, e->fp.Height)));
        }

        void Button::draw(ws::ISurface *s, bool force)
        {
            ws::IGradient *g    = NULL;
            size_t pressed      = nState;
            float brightness    = select_brightness();
            float scaling       = lsp_max(0.0f, sScaling.get());
            float fscaling      = lsp_max(0.0f, scaling * sFontScaling.get());
            ws::rectangle_t r   = sButton;
            r.nLeft            -= sSize.nLeft;
            r.nTop             -= sSize.nTop;

            // Prepare palette
            const style::ButtonColors *colors = select_colors();

            lsp::Color bg_color;
            lsp::Color color(colors->sColor);
            lsp::Color tcolor(colors->sTextColor);
            lsp::Color border_color(colors->sBorderColor);
            lsp::Color xc;

            get_actual_bg_color(bg_color);

            color.scale_lch_luminance(brightness);
            tcolor.scale_lch_luminance(brightness);
            border_color.scale_lch_luminance(brightness);

            // Draw background
            bool aa     = s->set_antialiasing(false);
            s->fill_rect(bg_color, SURFMASK_NONE, 0.0f, 0, 0, sSize.nWidth, sSize.nHeight);

            // Draw hole
            if (pressed & S_HOLE)
            {
                lsp::Color hcolor(sHoleColor);
                size_t hole         = lsp_max(1, scaling);
                s->fill_rect(hcolor,
                    SURFMASK_NONE, 0.0f,
                    r.nLeft - hole, r.nTop - hole,
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
                g->set_start(lc, 0.5f);
                g->set_stop(color, 1.0f);
                s->fill_triangle(g, 0, 0, c_x, c_y, 0, ye);
                delete g;

                // Right
                g   =  s->linear_gradient(xe - h_p, c_y, xe, c_y);
                g->set_start(lc, 0.5f);
                g->set_stop(color, 1.0f);
                s->fill_triangle(g, xe, ye, c_x, c_y, xe, 0);
                delete g;

                // Top
                g   =  s->linear_gradient(c_x, v_p, c_x, 0);
                g->set_start(lc, 0.5f);
                g->set_stop(color, 1.0f);
                s->fill_triangle(g, 0, 0, xe, 0, c_x, c_y);
                delete g;

                // Bottom
                g   =  s->linear_gradient(c_x, ye - v_p, c_x, ye);
                g->set_start(lc, 0.5f);
                g->set_stop(color, 1.0f);
                s->fill_triangle(g, xe, ye, 0, ye, c_x, c_y);
                delete g;

//                s->set_antialiasing(aa);
//                return;
            }

            // Draw button
            {
                float delta         = sqrtf(r.nWidth * r.nWidth + r.nHeight * r.nHeight);
                float xb            = color.lightness();
                float max_border    = lsp_max(0.0f, sBorderSize.get() * scaling);
                max_border          = lsp_max(max_border, sBorderPressedSize.get() * scaling);
                max_border          = lsp_max(max_border, sBorderDownSize.get() * scaling);
                ssize_t max_chamfer = max_border;
                ssize_t chamfer     = (pressed & S_PRESSED) ? lsp_max(0.0f, sBorderDownSize.get() * scaling) :
                                      (pressed & S_DOWN) ? lsp_max(0.0f, sBorderPressedSize.get() * scaling) :
                                      lsp_max(0.0f, sBorderSize.get() * scaling);
                bool gradient       = sGradient.get();

                // Draw chamfer
                if ((!(pressed & S_FLAT)) || ((pressed & (S_PRESSED | S_DOWN)) != 0))
                {
                    if (gradient)
                    {
                        for (ssize_t i=0; i<chamfer; ++i)
                        {
                            // Compute color
                            float bright = float(i + 1.0f) / (chamfer + 1);

                            // Create gradient
                            g = create_gradient(s, r, pressed, delta);
                            xc.copy(color);
                            xc.scale_hsl_lightness(bright);
                            g->set_start(xc.red(), xc.green(), xc.blue());
                            xc.copy(color);
                            xc.scale_hsl_lightness(xb * bright);
                            g->set_stop(xc.red(), xc.green(), xc.blue());
                            s->fill_rect(g, SURFMASK_NONE, 0.0f, r.nLeft, r.nTop, r.nWidth, r.nHeight);
                            delete g;

                            // Update rect
                            r.nLeft        += 1;
                            r.nTop         += 1;
                            r.nWidth       -= 2;
                            r.nHeight      -= 2;
                        }
                    }
                    else
                    {
                        s->fill_rect(border_color, SURFMASK_NONE, 0.0f, &r);
                        r.nLeft        += chamfer;
                        r.nTop         += chamfer;
                        r.nWidth       -= chamfer * 2;
                        r.nHeight      -= chamfer * 2;
                    }
                }

                // Draw button face
                if (gradient)
                {
                    g = create_gradient(s, r, pressed, delta);
                    xc.copy(color);
                    xc.scale_hsl_lightness(1.0f);
                    g->set_start(xc.red(), xc.green(), xc.blue());
                    xc.copy(color);
                    xc.scale_hsl_lightness(xb);
                    g->set_stop(xc.red(), xc.green(), xc.blue());
                    s->fill_rect(g, SURFMASK_NONE, 0.0f, &r);
                    delete g;
                }
                else
                    s->fill_rect(color, SURFMASK_NONE, 0.0f, &r);

                // Do we have a text?
                LSPString text;
                sText.format(&text);
                sTextAdjust.apply(&text);
                if (text.length() > 0)
                {
                    chamfer         = max_chamfer - chamfer;
                    r.nLeft        += chamfer;
                    r.nTop         += chamfer;
                    r.nWidth       -= chamfer * 2;
                    r.nHeight      -= chamfer * 2;

                    sTextPadding.enter(&r, scaling);

                    if (pressed & S_PRESSED)
                    {
                        r.nLeft        += sTextPressedShift.left() * scaling;
                        r.nTop         += sTextPressedShift.top()  * scaling;
                    }
                    else if (pressed & S_TOGGLED)
                    {
                        r.nLeft        += sTextDownShift.left() * scaling;
                        r.nTop         += sTextDownShift.top()  * scaling;
                    }
                    else
                    {
                        r.nLeft        += sTextShift.left() * scaling;
                        r.nTop         += sTextShift.top()  * scaling;
                    }

                    s->clip_begin(r.nLeft, r.nTop, r.nWidth, r.nHeight);

                    // Estimate font parameters
                    ws::font_parameters_t fp;
                    ws::text_parameters_t tp;
                    sFont.get_parameters(s, fscaling, &fp);
                    sFont.get_multitext_parameters(s, &tp, fscaling, &text);

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
                        sFont.get_text_parameters(s, &tp, fscaling, &text, last, tail);
                        float dx    = (r.nWidth - tp.Width) * 0.5f;
                        ssize_t x   = r.nLeft   + dx * halign - tp.XBearing;
                        y          += fp.Height;

                        sFont.draw(s, tcolor, x, y, fscaling, &text, last, tail);
                        last        = curr + 1;
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
            sTextAdjust.apply(&text);

            // Form the estimation parameters
            estimation_t e;
            e.scaling       = scaling;
            e.fscaling      = lsp_max(0.0f, e.scaling * sFontScaling.get());
            e.min_width     = 0;
            e.min_height    = 0;
            sFont.get_parameters(pDisplay, e.fscaling, &e.fp);

            // Estimate the size of the label
            for (lltl::iterator<prop::String> it = vEstimations.values(); it; ++it)
                estimate_string_size(&e, it.get());

            if ((text.length() > 0) && (!sTextClip.get()))
                estimate_string_size(&e, &sText);

            xr.nWidth          = lsp_max(xr.nWidth, e.min_width);
            xr.nHeight         = lsp_max(xr.nHeight, e.min_height);

            sTextPadding.add(&xr, scaling);

            float border        = sBorderSize.get() * scaling;
            border              = lsp_max(border, sBorderPressedSize.get() * scaling);
            border              = lsp_max(border, sBorderDownSize.get() * scaling);

            ssize_t chamfer     = lsp_max(0.0f, border);
            ssize_t hole        = (nState & S_HOLE) ? lsp_max(1, scaling) : 0;
            ssize_t light       = (nState & S_LED)  ? lsp_max(1, scaling * (sLed.get() + 2)) : 0;
            ssize_t outer       = lsp_max(hole, light)*2;

            xr.nWidth          += chamfer * 2;
            xr.nHeight         += chamfer * 2;

            r->nMinWidth        = xr.nWidth;
            r->nMinHeight       = xr.nHeight;
            r->nMaxWidth        = -1;
            r->nMaxHeight       = -1;
            r->nPreWidth        = -1;
            r->nPreHeight       = -1;

            // Update constraints
            sConstraints.apply(r, scaling);
            SizeConstraints::add(r, outer, outer);
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
            nBMask             |= (size_t(1) << e->nCode);

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
            nState              = lsp_setflag(nState, S_HOVER, m_over);
            nState              = lsp_setflag(nState, S_PRESSED, (nBMask == (size_t(1) << ws::MCB_LEFT)) && (m_over));

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
            nBMask         &= ~(size_t(1) << e->nCode);

            // Mouse button was initially pressed out of the button area, ignore this case
            if ((nBMask == 0) && (nState & S_OUT))
            {
                nState &= ~S_OUT;
                return STATUS_OK;
            }

            size_t state        = nState;
            bool m_over         = Position::inside(&sButton, e->nLeft, e->nTop);
            nState              = lsp_setflag(nState, S_HOVER, m_over);

            if (nState & S_TRIGGER)
            {
                // Update state according to mouse position and mouse button state
                size_t state        = nState;
                nState              = lsp_setflag(nState, S_PRESSED, (nBMask == (size_t(1) << ws::MCB_LEFT)) && (m_over));

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
                if ((mask == (size_t(1) << ws::MCB_LEFT)) && (e->nCode == ws::MCB_LEFT) && (m_over))
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
                if ((mask == (size_t(1) << ws::MCB_LEFT)) && (e->nCode == ws::MCB_LEFT))
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

            nState              = lsp_setflag(nState, S_PRESSED, (nBMask == (size_t(1) << ws::MCB_LEFT)) && (m_over));
            if ((mask == (size_t(1) << e->nCode)) && (nChanges > 0))
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
            size_t state        = nState;

            // Update state according to mouse position and mouse button state
            bool m_over         = Position::inside(&sButton, e->nLeft, e->nTop);
            nState              = lsp_setflag(nState, S_HOVER, m_over);
            nState              = lsp_setflag(nState, S_PRESSED, (nBMask == (size_t(1) << ws::MCB_LEFT)) && (m_over));

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

        status_t Button::on_mouse_out(const ws::event_t *e)
        {
            if (nState & S_HOVER)
            {
                nState             &= ~S_HOVER;
                query_draw();
            }
            return STATUS_OK;
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
    } /* namespace tk */
} /* namespace lsp */
