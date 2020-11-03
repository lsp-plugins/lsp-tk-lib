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
        STYLE_INITIALIZER_BEGIN(Fader, Widget);

            prop::Color::init("color", style, "#cccccc");
            prop::Color::init("hole.color", style, "#000000");
            prop::SizeRange::init("size", style, 64, -1);
            prop::RangeFloat::init("value", style, 0.5f);
            prop::StepFloat::init("step", style, 0.01f);
            prop::SizeRange::init("button.width", style, 0);
            prop::Float::init("button.aspect", style, 1.41f);
            prop::Integer::init("angle", style, 0);
            prop::Pointer::init("button.pointer", style, ws::MP_DEFAULT);

        STYLE_INITIALIZER_END(Fader, "Fader");
        LSP_BUILTIN_STYLE(Fader);

        const w_class_t Fader::metadata         = { "Fader", &Widget::metadata };
        
        Fader::Fader(Display *dpy):
            Widget(dpy),
            sColor(&sProperties),
            sHoleColor(&sProperties),
            sSizeRange(&sProperties),
            sValue(&sProperties),
            sStep(&sProperties),
            sBtnWidth(&sProperties),
            sBtnAspect(&sProperties),
            sAngle(&sProperties),
            sBtnPointer(&sProperties)
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

            sColor.bind("color", &sStyle);
            sHoleColor.bind("hole.color", &sStyle);
            sSizeRange.bind("size", &sStyle);
            sValue.bind("value", &sStyle);
            sStep.bind("step", &sStyle);
            sBtnWidth.bind("button.width", &sStyle);
            sBtnAspect.bind("button.aspect", &sStyle);
            sAngle.bind("angle", &sStyle);
            sBtnPointer.bind("button.pointer", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sColor.init(sclass, "#cccccc");
                sHoleColor.init(sclass, "#000000");
                sSizeRange.init(sclass, 64, -1);
                sValue.init(sclass, 0.5f);
                sStep.init(sclass, 0.01f);
                sBtnWidth.init(sclass, 0);
                sBtnAspect.init(sclass, 1.41f);
                sAngle.init(sclass, 0);
                sBtnPointer.init(sclass, ws::MP_DEFAULT);
            }

            handler_id_t id = 0;
            id = sSlots.add(SLOT_CHANGE, slot_on_change, self());
            if (id < 0)
                return -id;

            return STATUS_OK;
        }

        void Fader::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (sColor.is(prop))
                query_draw();
            if (sHoleColor.is(prop))
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

            ssize_t chamfer     = lsp_max(1, scaling * 3.0f);
            size_t ir           = lsp_max(2, scaling * 2);
            ssize_t range       = (chamfer + ir)*2;

            ssize_t smin, smax, bmin, bmax;
            sSizeRange.compute(&smin, &smax, scaling);
            sBtnWidth.compute(&bmin, &bmax, scaling);

            if (angle & 1) // Vertical
            {
                // Compute parameters for vertical fader
                r->nMinWidth        = lsp_max(bmin, range);
                r->nMaxWidth        = (bmax >= 0) ? lsp_max(bmax, range) : -1;
                bmin                = lsp_max(r->nMinWidth * aspect, range);

                // Compute boundaries for slider
                r->nMinHeight       = lsp_max(smin, bmin * 3);
                r->nMaxHeight       = (smax >= 0) ? lsp_max(smax, bmin) : -1;
            }
            else // Horizontal
            {
                // Compute parameters for vertical fader
                r->nMinHeight       = lsp_max(bmin, range);
                r->nMaxHeight       = (bmax >= 0) ? lsp_max(bmax, range) : -1;
                bmin                = lsp_max(r->nMinHeight * aspect, range);

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

            ssize_t chamfer     = lsp_max(1, scaling * 3.0f);
            size_t ir           = lsp_max(2, scaling * 2);
            ssize_t range       = (chamfer + ir)*2;

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
                float accel     = 1.0f;;

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
            ssize_t chamfer     = lsp_max(1, scaling * 3.0f);
            size_t angle        = sAngle.get();

            // Prepare palette
            lsp::Color bg_color(sBgColor);
            lsp::Color color(sColor);
            lsp::Color hole(bg_color);
            color.scale_lightness(sBrightness.get());

            // Clear surface
            s->clear(bg_color);

            // Draw the hole
            ws::rectangle_t h   = sHole;
            bool aa             = s->set_antialiasing(true);
            float delta         = (angle & 1) ? sSize.nHeight : sSize.nWidth;
            ssize_t xr          = (angle & 1) ? sHole.nWidth >> 1 : sHole.nHeight >> 1;
            h.nLeft            -= sSize.nLeft;
            h.nTop             -= sSize.nTop;

            for (ssize_t i=0; i<chamfer; ++i)
            {
                // Compute color
                float bright = float(chamfer - i) / chamfer;
                hole.lightness(bright);

                if (angle & 1) // vertical
                    g = s->radial_gradient(0, sSize.nHeight, scaling, 0, sSize.nHeight, delta);
                else // horizontal
                    g = s->radial_gradient(0, sSize.nHeight, scaling, 0, sSize.nHeight, delta);

                g->add_color(0.0, hole);
                g->add_color(1.0, 0.5 * hole.red(), 0.5 *  hole.green(), 0.5 * hole.blue());
                s->fill_round_rect(g, SURFMASK_ALL_CORNER, xr--, &h);
                delete g;

                h.nLeft        += 1;
                h.nTop         += 1;
                h.nWidth       -= 2;
                h.nHeight      -= 2;
            }

            hole.copy(sHoleColor);
            s->fill_round_rect(hole, SURFMASK_ALL_CORNER, xr--, &h);

            // Draw button
            delta           = sqrtf(sButton.nWidth * sButton.nWidth + sButton.nHeight * sButton.nHeight) * 0.5f;
            h               = sButton;
            h.nLeft        -= sSize.nLeft;
            h.nTop         -= sSize.nTop;
            xr              = chamfer + scaling;
            float xb        = color.lightness();

            for (ssize_t i=0; i<chamfer; ++i)
            {
                // Compute color
                float bright        = float(i + 1.0f) / (chamfer + 1);

                // Create gradient
                g = s->radial_gradient(h.nLeft + h.nWidth + chamfer, h.nTop - chamfer, 0, h.nLeft + h.nWidth + chamfer, h.nTop - chamfer, delta);
                color.lightness(1.0f);
                g->add_color(0.0, color.red(), color.green(), color.blue());
                color.lightness(xb * bright);
                g->add_color(1.0, color.red(), color.green(), color.blue());
                s->fill_round_rect(g, SURFMASK_ALL_CORNER, xr--, &h);
                delete g;

                // Update rect
                h.nLeft        += 1;
                h.nTop         += 1;
                h.nWidth       -= 2;
                h.nHeight      -= 2;
            }

            // Draw button face
            g = s->radial_gradient(h.nLeft + h.nWidth + chamfer, h.nTop - chamfer, 0, h.nLeft + h.nWidth + chamfer, h.nTop - chamfer, delta);
            color.lightness(1.0f);
            g->add_color(0.0, color.red(), color.green(), color.blue());
            color.lightness(xb);
            g->add_color(1.0, color.red(), color.green(), color.blue());
            s->fill_round_rect(g, SURFMASK_ALL_CORNER, xr, &h);
            delete g;

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
