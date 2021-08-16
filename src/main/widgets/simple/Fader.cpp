/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 19 нояб. 2017 г.
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
            LSP_TK_STYLE_IMPL_BEGIN(Fader, Widget)
                // Init
                sBtnColor.bind("button.color", this);
                sBtnBorderColor.bind("button.border.color", this);
                sScaleColor.bind("scale.color", this);
                sScaleBorderColor.bind("scale.border.color", this);
                sBalanceColor.bind("balance.color", this);
                sSizeRange.bind("size", this);
                sValue.bind("value", this);
                sStep.bind("step", this);
                sBtnWidth.bind("button.width", this);
                sBtnAspect.bind("button.aspect", this);
                sAngle.bind("angle", this);
                sScaleWidth.bind("scale.width", this);
                sScaleBorder.bind("scale.border.size", this);
                sScaleRadius.bind("scale.border.radius", this);
                sScaleGradient.bind("scale.border.gradient", this);
                sBtnPointer.bind("button.pointer", this);
                sBtnBorder.bind("button.border.size", this);
                sBtnRadius.bind("button.border.radius", this);
                sBtnGradient.bind("button.border.gradient", this);
                sBalance.bind("balance", this);
                sScaleBrightness.bind("scale.brightness", this);
                sBalanceColorCustom.bind("balance.color.custom", this);
                // Configure
                sBtnColor.set("#cccccc");
                sBtnBorderColor.set("#cccccc");
                sScaleColor.set("#000000");
                sScaleBorderColor.set("#ffffff");
                sBalanceColor.set("#000000");
                sSizeRange.set(64, -1);
                sValue.set(0.5f);
                sStep.set(0.01f);
                sBtnWidth.set(12, 12);
                sBtnAspect.set(1.41f);
                sAngle.set(0);
                sScaleWidth.set(4);
                sScaleBorder.set(3);
                sScaleRadius.set(5);
                sScaleGradient.set(true);
                sBtnBorder.set(3);
                sBtnRadius.set(3);
                sBtnGradient.set(true);
                sBtnPointer.set(ws::MP_DEFAULT);
                sBalance.set(0.0f);
                sScaleBrightness.set(0.75f);
                sBalanceColorCustom.set(false);
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(Fader, "Fader", "root");
        }

        const w_class_t Fader::metadata         = { "Fader", &Widget::metadata };
        
        Fader::Fader(Display *dpy):
            Widget(dpy),
            sBtnColor(&sProperties),
            sBtnBorderColor(&sProperties),
            sScaleColor(&sProperties),
            sScaleBorderColor(&sProperties),
            sBalanceColor(&sProperties),
            sSizeRange(&sProperties),
            sValue(&sProperties),
            sStep(&sProperties),
            sBtnWidth(&sProperties),
            sBtnAspect(&sProperties),
            sAngle(&sProperties),
            sScaleWidth(&sProperties),
            sScaleBorder(&sProperties),
            sScaleRadius(&sProperties),
            sScaleGradient(&sProperties),
            sBtnBorder(&sProperties),
            sBtnRadius(&sProperties),
            sBtnGradient(&sProperties),
            sBtnPointer(&sProperties),
            sBalance(&sProperties),
            sScaleBrightness(&sProperties),
            sBalanceColorCustom(&sProperties)
        {
            nLastV          = 0;
            nButtons        = 0;
            nXFlags         = 0;
            fCurrValue      = 0.0f;
            fLastValue      = 0.0f;

            sButton.nLeft   = -1;
            sButton.nTop    = -1;
            sButton.nWidth  = 0;
            sButton.nHeight = 0;

            sHole.nLeft     = -1;
            sHole.nTop      = -1;
            sHole.nWidth    = 0;
            sHole.nHeight   = 0;

            pClass          = &metadata;
        }
        
        Fader::~Fader()
        {
            nFlags     |= FINALIZED;
            do_destroy();
        }

        status_t Fader::init()
        {
            status_t result = Widget::init();
            if (result != STATUS_OK)
                return result;

            sBtnColor.bind("button.color", &sStyle);
            sBtnBorderColor.bind("button.border.color", &sStyle);
            sScaleColor.bind("scale.color", &sStyle);
            sScaleBorderColor.bind("scale.border.color", &sStyle);
            sBalanceColor.bind("balance.color", &sStyle);
            sSizeRange.bind("size", &sStyle);
            sValue.bind("value", &sStyle);
            sStep.bind("step", &sStyle);
            sBtnWidth.bind("button.width", &sStyle);
            sBtnAspect.bind("button.aspect", &sStyle);
            sAngle.bind("angle", &sStyle);
            sScaleWidth.bind("scale.width", &sStyle);
            sScaleBorder.bind("scale.border.size", &sStyle);
            sScaleRadius.bind("scale.border.radius", &sStyle);
            sScaleGradient.bind("scale.border.gradient", &sStyle);
            sBtnBorder.bind("button.border.size", &sStyle);
            sBtnRadius.bind("button.border.radius", &sStyle);
            sBtnGradient.bind("button.border.gradient", &sStyle);
            sBtnPointer.bind("button.pointer", &sStyle);
            sBalance.bind("balance", &sStyle);
            sScaleBrightness.bind("scale.brightness", &sStyle);
            sBalanceColorCustom.bind("balance.color.custom", &sStyle);

            handler_id_t id = 0;
            id = sSlots.add(SLOT_CHANGE, slot_on_change, self());
            if (id < 0)
                return -id;

            return STATUS_OK;
        }

        void Fader::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (sBtnColor.is(prop))
                query_draw();
            if (sBtnBorderColor.is(prop))
                query_draw();
            if (sScaleColor.is(prop))
                query_draw();
            if (sScaleBorderColor.is(prop))
                query_draw();
            if (sBalanceColor.is(prop))
                query_draw();
            if (sSizeRange.is(prop))
                query_resize();
            if (sValue.is(prop))
                sync_button_pos();
            if (sBtnWidth.is(prop))
                query_resize();
            if (sBtnAspect.is(prop))
                query_resize();
            if (sAngle.is(prop))
                query_resize();
            if (sScaleWidth.is(prop))
                query_resize();
            if (sScaleBorder.is(prop))
                query_resize();
            if (sScaleRadius.is(prop))
                query_resize();
            if (sScaleGradient.is(prop))
                query_draw();
            if (sBtnBorder.is(prop))
                query_resize();
            if (sBtnRadius.is(prop))
                query_resize();
            if (sBtnGradient.is(prop))
                query_draw();
            if (sBalance.is(prop))
                query_draw();
            if (sScaleBrightness.is(prop))
                query_draw();
            if (sBalanceColorCustom.is(prop))
                query_draw();
        }

        void Fader::sync_button_pos()
        {
            size_t angle        = sAngle.get();
            float v             = sValue.get_normalized();
            if (angle & 1)
            {
                sButton.nLeft       = sSize.nLeft;
                sButton.nTop        = sSize.nTop  + (1.0f - v) * (sSize.nHeight - sButton.nHeight);
            }
            else
            {
                sButton.nTop        = sSize.nTop;
                sButton.nLeft       = sSize.nLeft + v * (sSize.nWidth  - sButton.nWidth);
            }

            query_draw();
        }

        void Fader::size_request(ws::size_limit_t *r)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());
            float aspect        = lsp_max(0.0f, sBtnAspect.get());
            size_t angle        = sAngle.get();
            ssize_t hole_width  = lsp_max(0.0f, sScaleWidth.get() * scaling);

            ssize_t schamfer    = (sScaleBorder.get() > 0) ? lsp_max(1, scaling * sScaleBorder.get()) : 0.0f;
            ssize_t sradius     = (sScaleRadius.get() > 0) ? lsp_max(1, scaling * sScaleRadius.get()) : 0.0f;
            ssize_t range       = lsp_max(schamfer*2 + hole_width, sradius * 2);

            ssize_t bchamfer    = (sBtnBorder.get() > 0) ? lsp_max(1, scaling * sBtnBorder.get()) : 0.0f;
            ssize_t bradius     = (sBtnRadius.get() > 0) ? lsp_max(1, scaling * sBtnRadius.get()) : 0.0f;

            ssize_t smin, smax, bmin, bmax;
            sSizeRange.compute(&smin, &smax, scaling);
            sBtnWidth.compute(&bmin, &bmax, scaling);

            bmin                = lsp_max(bmin, bchamfer*2 + bradius*2);
            if (bmax >= 0)
                bmax                = lsp_max(bmax, bmin);

            if (angle & 1) // Vertical
            {
                // Compute parameters for vertical fader
                r->nMinWidth        = lsp_max(bmin, range);
                r->nMaxWidth        = (bmax >= 0) ? lsp_max(bmax, range) : -1;
                bmin                = lsp_max(bmin, ssize_t(r->nMinWidth * aspect), range);

                // Compute boundaries for slider
                r->nMinHeight       = lsp_max(smin, bmin * 3);
                r->nMaxHeight       = (smax >= 0) ? lsp_max(smax, bmin) : -1;
            }
            else // Horizontal
            {
                // Compute parameters for vertical fader
                r->nMinHeight       = lsp_max(bmin, range);
                r->nMaxHeight       = (bmax >= 0) ? lsp_max(bmax, range) : -1;
                bmin                = lsp_max(bmin, ssize_t(r->nMinHeight * aspect), range);

                // Compute boundaries for slider
                r->nMinWidth        = lsp_max(smin, bmin * 3);
                r->nMaxWidth        = (smax >= 0) ? lsp_max(smax, bmin) : -1;
            }

            r->nPreWidth        = -1;
            r->nPreHeight       = -1;
        }

        void Fader::realize(const ws::rectangle_t *r)
        {
            Widget::realize(r);

            float scaling       = lsp_max(0.0f, sScaling.get());
            float aspect        = lsp_max(0.0f, sBtnAspect.get());
            size_t angle        = sAngle.get();
            ssize_t hole_width  = lsp_max(0.0f, sScaleWidth.get() * scaling);

            ssize_t schamfer    = (sScaleBorder.get() > 0) ? lsp_max(1, scaling * sScaleBorder.get()) : 0.0f;
            ssize_t sradius     = (sScaleRadius.get() > 0) ? lsp_max(1, scaling * sScaleRadius.get()) : 0.0f;
            ssize_t range       = lsp_max(schamfer*2 + hole_width, sradius * 2);

            sButton.nLeft       = r->nLeft;
            sButton.nTop        = r->nTop;

            if (angle & 1)
            {
                sButton.nWidth      = r->nWidth;
                sButton.nHeight     = lsp_max(r->nWidth * aspect, range);

                sHole.nWidth        = range;
                sHole.nHeight       = r->nHeight + range - sButton.nHeight;
            }
            else
            {
                sButton.nHeight     = r->nHeight;
                sButton.nWidth      = lsp_max(r->nHeight * aspect, range);

                sHole.nHeight       = range;
                sHole.nWidth        = r->nWidth + range - sButton.nWidth;
            }

            // Locate hole at center
            sHole.nLeft         = r->nLeft + ((r->nWidth  - sHole.nWidth) >> 1);
            sHole.nTop          = r->nTop  + ((r->nHeight - sHole.nHeight) >> 1);

            // Sync position of the fader button
            sync_button_pos();
        }

        status_t Fader::slot_on_change(Widget *sender, void *ptr, void *data)
        {
            Fader *_this = widget_ptrcast<Fader>(ptr);
            return (_this != NULL) ? _this->on_change() : STATUS_BAD_ARGUMENTS;
        }

        status_t Fader::on_change()
        {
            return STATUS_OK;
        }

        float Fader::update_value(float value)
        {
            float old   = sValue.set(value);
            if (old != sValue.get())
                sSlots.execute(SLOT_CHANGE, this);
            return old;
        }

        status_t Fader::on_mouse_down(const ws::event_t *e)
        {
            if (nButtons == 0)
            {
                if (Position::inside(&sButton, e->nLeft, e->nTop))
                {
                    if (e->nCode == ws::MCB_RIGHT)
                        nXFlags        |= F_PRECISION | F_MOVER;
                    else if (e->nCode == ws::MCB_LEFT)
                        nXFlags        |= F_MOVER;
                    else
                        nXFlags        |= F_IGNORE;
                }
                else
                    nXFlags        |= F_IGNORE;

                if (!(nXFlags & F_IGNORE))
                {
                    nLastV      = (sAngle.get() & 1) ? e->nTop : e->nLeft;
                    fLastValue  = sValue.get();
                    fCurrValue  = fLastValue;
                }
            }

            nButtons       |= (1 << e->nCode);
            if (nXFlags & F_IGNORE)
                return STATUS_OK;

            size_t key      = (nXFlags & F_PRECISION) ? ws::MCB_RIGHT : ws::MCB_LEFT;

            // Update value
            float value     = (nButtons == size_t(1 << key)) ? fCurrValue : fLastValue;
            update_value(value);

            return STATUS_OK;
        }

        status_t Fader::on_mouse_up(const ws::event_t *e)
        {
            nButtons       &= ~(1 << e->nCode);
            if (nXFlags & F_IGNORE)
            {
                if (nButtons == 0)
                {
                    if (Position::inside(&sButton, e->nLeft, e->nTop))
                        nXFlags     = F_MOVER;
                    else
                        nXFlags     = 0;
                }
                return STATUS_OK;
            }

            size_t key      = (nXFlags & F_PRECISION) ? ws::MCB_RIGHT : ws::MCB_LEFT;
            float value;

            if (nButtons == 0) // All mouse buttons are released now
            {
                nXFlags     = 0;
                value       = (e->nCode == key) ? fCurrValue : fLastValue;
            }
            else if (nButtons == size_t(1 << key)) // Currently pressed initially selected button
                value       = fCurrValue;
            else
                value       = fLastValue;

            // Update value
            update_value(value);

            return STATUS_OK;
        }

        status_t Fader::on_mouse_move(const ws::event_t *e)
        {
            if (nXFlags & F_IGNORE)
                return STATUS_OK;

            size_t key = (nXFlags & F_PRECISION) ? ws::MCB_RIGHT : ws::MCB_LEFT;
            if (nButtons != size_t(1 << key))
            {
                if ((nButtons == 0) && (Position::inside(&sButton, e->nLeft, e->nTop)))
                    nXFlags    |= F_MOVER;
                else
                    nXFlags    &= ~F_MOVER;
                return STATUS_OK;
            }

            // Different behaviour for slider
            nXFlags        |= F_MOVER;
            ssize_t angle   = sAngle.get() & 3;

            ssize_t value   = (angle & 1) ? e->nTop : e->nLeft;
            float result    = fLastValue;
            if (value != nLastV)
            {
                ssize_t range   = (angle & 1) ? sSize.nHeight - sButton.nHeight : sSize.nWidth - sButton.nWidth;
                float delta     = sValue.range() * float(value - nLastV) / range; // normalized
                if (angle >= 2)
                    delta           = -delta;
                float accel     = 1.0f;

                if (nXFlags & F_PRECISION)
                {
                    accel = (e->nState & ws::MCF_SHIFT)   ? 1.0f :
                            (e->nState & ws::MCF_CONTROL) ? sStep.accel() :
                            sStep.decel();
                }
                else
                {
                    accel = (e->nState & ws::MCF_SHIFT)   ? sStep.decel() :
                            (e->nState & ws::MCF_CONTROL) ? sStep.accel() :
                            1.0f;
                }

                result        = ((angle == 1) || (angle == 2)) ? result - delta*accel : result + delta*accel;
            }

            // Update value
            fCurrValue      = result;
            update_value(result);

            return STATUS_OK;
        }

        status_t Fader::on_mouse_scroll(const ws::event_t *e)
        {
            ssize_t angle   = sAngle.get();
            float step = (e->nState & ws::MCF_SHIFT)   ? sStep.step_decel() :
                         (e->nState & ws::MCF_CONTROL) ? sStep.step_accel() :
                         sStep.step();

            if (((angle & 3) == 0) || ((angle & 3) == 3))
                step            = - step;
            float delta     = (e->nCode == ws::MCD_UP) ? step : -step;

            update_value(sValue.get() + delta);

            return STATUS_OK;
        }

        void Fader::draw(ws::ISurface *s)
        {
            ws::IGradient *g;
            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t schamfer    = (sScaleBorder.get() > 0) ? lsp_max(1, scaling * sScaleBorder.get()) : 0.0f;
            ssize_t sradius     = (sScaleRadius.get() > 0) ? lsp_max(1, scaling * sScaleRadius.get()) : 0.0f;
            ssize_t bchamfer    = (sBtnBorder.get() > 0) ? lsp_max(1, scaling * sBtnBorder.get()) : 0.0f;
            ssize_t bradius     = (sBtnRadius.get() > 0) ? lsp_max(1, scaling * sBtnRadius.get()) : 0.0f;
            size_t angle        = sAngle.get();
            float bright        = sBrightness.get();

            // Prepare palette
            lsp::Color bg_color;
            lsp::Color button(sBtnColor);
            lsp::Color scol, sdcol;
            lsp::Color bborder(sBtnBorderColor);
            lsp::Color sborder(sScaleBorderColor);

            if (sBalanceColorCustom.get())
            {
                scol.copy(sBalanceColor);
                sdcol.copy(sScaleColor);

                scol.scale_lightness(bright);
                sdcol.scale_lightness(bright);
            }
            else
            {
                scol.copy(sScaleColor);
                sdcol.copy(sScaleColor);

                scol.scale_lightness(bright);
                sdcol.scale_lightness(sScaleBrightness.get() * bright);
            }

            get_actual_bg_color(bg_color);
            button.scale_lightness(bright);
            scol.scale_lightness(bright);
            sdcol.scale_lightness(bright);

            // Clear surface
            s->clear(bg_color);
            bool aa             = s->set_antialiasing(true);

            // Draw the hole
            ws::rectangle_t h   = sHole;
            h.nLeft            -= sSize.nLeft;
            h.nTop             -= sSize.nTop;

            if (sScaleGradient.get())
            {
                float delta         = (angle & 1) ? sSize.nHeight : sSize.nWidth;

                for (ssize_t i=0; i<schamfer; ++i)
                {
                    // Compute button
                    float l = float(schamfer - i) / schamfer;
                    sborder.lightness(l);
                    sborder.scale_lightness(bright);

                    if (angle & 1) // vertical
                        g = s->radial_gradient(0, sSize.nHeight, scaling, 0, sSize.nHeight, delta);
                    else // horizontal
                        g = s->radial_gradient(0, sSize.nHeight, scaling, 0, sSize.nHeight, delta);

                    g->add_color(0.0, sborder);
                    g->add_color(1.0, 0.5 * sborder.red(), 0.5 *  sborder.green(), 0.5 * sborder.blue());
                    s->fill_round_rect(g, SURFMASK_ALL_CORNER, sradius, &h);
                    delete g;

                    h.nLeft        += 1;
                    h.nTop         += 1;
                    h.nWidth       -= 2;
                    h.nHeight      -= 2;
                    sradius         = lsp_max(0, sradius - 1);
                }
            }
            else
            {
                // Just draw simple border
                sborder.scale_lightness(bright);
                s->fill_round_rect(sborder, SURFMASK_ALL_CORNER, sradius, &h);
                sradius         = lsp_max(0, sradius - schamfer);
                h.nLeft        += schamfer;
                h.nTop         += schamfer;
                h.nWidth       -= (schamfer << 1);
                h.nHeight      -= (schamfer << 1);
            }

            // Draw the scale
            {
                s->fill_round_rect(sdcol, SURFMASK_ALL_CORNER, sradius, &h);
                float balance   = sValue.get_normalized(sBalance.get());
                float value     = sValue.get_normalized();

                ssize_t boff, bval;
                ws::rectangle_t c   = sSize;
                c.nLeft        -= sSize.nLeft;
                c.nTop         -= sSize.nTop;

                switch (angle & 3)
                {
                    case 0: // +
                        boff            = sHole.nLeft - sSize.nLeft + sHole.nWidth * balance;
                        bval            = sHole.nLeft - sSize.nLeft + sHole.nWidth * value;
                        c.nLeft         = lsp_min(boff, bval);
                        c.nWidth        = lsp_max(boff, bval) - c.nLeft;
                        break;
                    case 1:
                        boff            = sHole.nTop - sSize.nTop + sHole.nHeight * (1.0f - balance);
                        bval            = sHole.nTop - sSize.nTop + sHole.nHeight * (1.0f - value);
                        c.nTop          = lsp_min(boff, bval);
                        c.nHeight       = lsp_max(boff, bval) - c.nTop;
                        break;
                    case 2:
                        boff            = sHole.nLeft - sSize.nLeft + sHole.nWidth * balance;
                        bval            = sHole.nLeft - sSize.nLeft + sHole.nWidth * value;
                        c.nLeft         = lsp_min(boff, bval);
                        c.nWidth        = lsp_max(boff, bval) - c.nLeft;
                        break;
                    default:  // +
                        boff            = sHole.nTop - sSize.nTop + sHole.nHeight * (1.0f - balance);
                        bval            = sHole.nTop - sSize.nTop + sHole.nHeight * (1.0f - value);
                        c.nTop          = lsp_min(boff, bval);
                        c.nHeight       = lsp_max(boff, bval) - c.nTop;
                        break;
                }

                s->clip_begin(&c);
                s->fill_round_rect(scol, SURFMASK_ALL_CORNER, sradius, &h);
                s->clip_end();
            }


            // Draw button
            h               = sButton;
            h.nLeft        -= sSize.nLeft;
            h.nTop         -= sSize.nTop;

            if (sBtnGradient.get())
            {
                float delta     = sqrtf(sButton.nWidth * sButton.nWidth + sButton.nHeight * sButton.nHeight) * 0.5f;
                float xb        = button.lightness();

                for (ssize_t i=0; i<bchamfer; ++i)
                {
                    // Compute button
                    float l        = float(i + 1.0f) / (bchamfer + 1);

                    // Create gradient
                    g = s->radial_gradient(h.nLeft + h.nWidth + bchamfer, h.nTop - bchamfer, 0, h.nLeft + h.nWidth + bchamfer, h.nTop - bchamfer, delta);
                    bborder.lightness(1.0f);
                    g->add_color(0.0, bborder.red(), bborder.green(), bborder.blue());
                    bborder.lightness(xb * l);
                    g->add_color(1.0, bborder.red(), bborder.green(), bborder.blue());
                    s->fill_round_rect(g, SURFMASK_ALL_CORNER, bradius, &h);
                    delete g;

                    // Update rect
                    h.nLeft        += 1;
                    h.nTop         += 1;
                    h.nWidth       -= 2;
                    h.nHeight      -= 2;
                    bradius         = lsp_max(0, bradius - 1);
                }

                // Draw button face
                g = s->radial_gradient(h.nLeft + h.nWidth + bchamfer, h.nTop - bchamfer, 0, h.nLeft + h.nWidth + bchamfer, h.nTop - bchamfer, delta);
                button.lightness(1.0f);
                g->add_color(0.0, button.red(), button.green(), button.blue());
                button.lightness(xb);
                g->add_color(1.0, button.red(), button.green(), button.blue());
                s->fill_round_rect(g, SURFMASK_ALL_CORNER, bradius, &h);
                delete g;
            }
            else
            {
                // Just draw simple border
                bborder.scale_lightness(bright);
                s->fill_round_rect(bborder, SURFMASK_ALL_CORNER, bradius, &h);
                bradius         = lsp_max(0, bradius - bchamfer);
                h.nLeft        += bchamfer;
                h.nTop         += bchamfer;
                h.nWidth       -= (bchamfer << 1);
                h.nHeight      -= (bchamfer << 1);

                // Fill the button
                s->fill_round_rect(button, SURFMASK_ALL_CORNER, bradius, &h);
            }

            // Restore antialiasing
            s->set_antialiasing(aa);
        }

        ws::mouse_pointer_t Fader::current_pointer()
        {
            return ((nXFlags & (F_MOVER | F_IGNORE)) == F_MOVER) ?
                    sBtnPointer.get((sAngle.get() & 1) ? ws::MP_VSIZE : ws::MP_HSIZE) :
                    Widget::current_pointer();
        }
    
    } /* namespace tk */
} /* namespace lsp */
