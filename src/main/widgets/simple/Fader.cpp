/*
 * Fader.cpp
 *
 *  Created on: 19 нояб. 2017 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t Fader::metadata         = { "Fader", &Widget::metadata };
        
        Fader::Fader(Display *dpy):
            Widget(dpy),
            sColor(&sProperties),
            sHoleColor(&sProperties),
            sSizeRange(&sProperties),
            sValue(&sProperties),
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
            sBtnWidth.bind("button.width", &sStyle);
            sBtnAspect.bind("button.aspect", &sStyle);
            sAngle.bind("angle", &sStyle);
            sPointer.bind("pointer", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sColor.init(sclass, "#cccccc");
                sHoleColor.init(sclass, "#000000");
                sSizeRange.init(sclass, 64, -1);
                sValue.init(sclass, 0.5f, 0.0f, 1.0f, 0.01f);
                sBtnWidth.init(sclass, 16);
                sBtnAspect.init(sclass, 1.41f);
                sAngle.init(sclass, 0);
                sPointer.init(sclass, ws::MP_DEFAULT);
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
                sButton.nLeft       = sSize.nLeft + (1.0f - v) * (sSize.nWidth  - sButton.nWidth);
            }

            query_draw();
        }

        void Fader::size_request(ws::size_limit_t *r)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t chamfer     = lsp_max(1, scaling * 3.0f);
            size_t angle        = sAngle.get();
            float aspect        = sBtnAspect.get();

            ssize_t smin, smax, bmin, bmax;
            sSizeRange.compute(&smin, &smax, scaling);
            sBtnWidth.compute(&bmin, &bmax, scaling);

            // Compute parameters for horizontal fader
            ssize_t range       = chamfer + 2*scaling;
            r->nMinHeight       = lsp_max(bmin, range);
            r->nMaxHeight       = (bmax >= 0) ? lsp_max(bmax, range) : -1;
            bmin                = r->nMinHeight * aspect;
            bmax                = (r->nMaxHeight >= 0) ? r->nMaxHeight * aspect : -1;

            // Compute boundaries for slider
            r->nMinWidth        = lsp_max(smin, bmin * 3);
            r->nMaxWidth        = (smax >= 0) ? lsp_max(smax, r->nMinWidth) : -1;

            if (angle & 1) // vertical
            {
                swap(r->nMinWidth, r->nMinHeight);
                swap(r->nMaxWidth, r->nMaxHeight);
            }
        }

        void Fader::realize(const ws::rectangle_t *r)
        {
            Widget::realize(r);

            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t chamfer     = lsp_max(1, scaling * 3.0f);
            size_t angle        = sAngle.get();

            sButton.nLeft       = r->nLeft;
            sButton.nTop        = r->nTop;

            if (angle & 1)
            {
                sButton.nWidth      = r->nWidth;
                sButton.nHeight     = lsp_max(r->nWidth, chamfer + scaling*2);

                sHole.nWidth        = chamfer + 2*scaling;
                sHole.nHeight       = r->nHeight + chamfer*2 + scaling*4 - sButton.nHeight;
            }
            else
            {
                sButton.nHeight     = r->nHeight;
                sButton.nWidth      = lsp_max(r->nHeight, chamfer + 2*scaling);

                sHole.nHeight       = chamfer + 2*scaling;
                sHole.nWidth        = r->nWidth + chamfer*2 + scaling*4 - sButton.nWidth;
            }

            // Locate hole at center
            sHole.nLeft         = r->nLeft + ((r->nWidth  - sHole.nWidth) >> 1);
            sHole.nTop          = r->nTop  + ((r->nHeight - sHole.nHeight) >> 1);
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
            ssize_t angle   = sAngle.get();

            ssize_t value = (angle & 1) ? e->nTop : e->nLeft;
            float result  = fLastValue;
            if (value != nLastV)
            {
                ssize_t range = (angle & 1) ? sSize.nHeight - nBtnLength : sSize.nWidth - nBtnLength;
                float delta   = (fMax - fMin) * float(value - nLastV) / float(range);

                if (nXFlags & F_PRECISION)
                    delta       *= (fTinyStep / fStep);
                size_t a      = angle & 3;
                result        = ((a == 1) || (a == 2)) ? result - delta : result + delta;
            }

            // Update value
            fCurrValue      = result;
            update_value(result);

            return STATUS_OK;
        }

        status_t Fader::on_mouse_scroll(const ws::event_t *e)
        {
            ssize_t angle   = sAngle.get();
            float step = (e->nState & ws::MCF_SHIFT) ? sValue.slow() :
                         (e->nState & ws::MCF_CONTROL) ? sValue.quick() :
                         sValue.step();

            if (((angle & 3) == 0) || ((angle & 3) == 3))
                step            = - step;
            float delta     = (e->nCode == ws::MCD_UP) ? step : -step;

            update_value(sValue.get() + delta);

            return STATUS_OK;
        }

        void Fader::draw(ws::ISurface *s)
        {
            // Prepare palette
//            lsp::Color bg_color(sBgColor);
//            lsp::Color color(sColor);
//            color.scale_lightness(brightness());
//
//            // Clear surface
//            s->clear(bg_color);
//            ssize_t l, t;
//
//            if (nAngle & 1) // Vertical
//            {
//                l = (sSize.nWidth - 8) >> 1;
//                t = (nBtnLength - 8) >> 1;
//            }
//            else // Horizontal
//            {
//                l = (nBtnLength - 8) >> 1;
//                t = (sSize.nHeight - 8) >> 1;
//            }
//
//            // Draw the hole
//            bool aa = s->set_antialiasing(true);
//            lsp::Color hole(bg_color);
//            float hlb = hole.lightness() + 0.5f;
//            float hld = 0;
//            float r = (nAngle & 1) ? sqrtf(sSize.nHeight*sSize.nHeight + 64) : sqrtf(sSize.nWidth*sSize.nWidth + 64);
//
//            for (size_t i=0; i<3; ++i)
//            {
//                float bright = (hlb - hld) * (3 - i) / 3 + hld;
//                hole.lightness(bright);
//
//                if (nAngle & 1) // Vertical
//                {
//                    IGradient *gr = s->radial_gradient(l, sSize.nHeight - t, 1, l, sSize.nHeight - t, r);
//                    gr->add_color(0.0, hole);
//                    gr->add_color(1.0, 0.5 * hole.red(), 0.5 *  hole.green(), 0.5 * hole.blue());
//
//                    s->fill_round_rect(l+i, t+i, 8-i*2, sSize.nHeight - nBtnLength + 8 - i*2, 4-i, SURFMASK_ALL_CORNER, gr);
//                    delete gr;
//                }
//                else
//                {
//                    IGradient *gr = s->radial_gradient(l, t, 1, l, t, r);
//                    gr->add_color(0.0, hole);
//                    gr->add_color(1.0, 0.5 * hole.red(), 0.5 *  hole.green(), 0.5 * hole.blue());
//
//                    s->fill_round_rect(l+i, t+i, sSize.nWidth - nBtnLength + 8 - i*2, 8-i*2, 4-i, SURFMASK_ALL_CORNER, gr);
//                    delete gr;
//                }
//            }
//            hole.set_rgb(0.0f, 0.0f, 0.0f);
//
//            if (nAngle & 1) // Vertical
//                s->fill_round_rect(l+3, t+3, 2, sSize.nHeight - nBtnLength + 2, 1, SURFMASK_ALL_CORNER, hole);
//            else
//                s->fill_round_rect(l+3, t+3, sSize.nWidth - nBtnLength + 2, 2, 1, SURFMASK_ALL_CORNER, hole);
//
//            s->set_antialiasing(aa);
//
//            // Draw the button
//            float p         = get_normalized_value();
//            ssize_t rw      = (nAngle & 1) ? nBtnWidth  : nBtnLength;
//            ssize_t rh      = (nAngle & 1) ? nBtnLength : nBtnWidth;
//            ssize_t bl      = (nAngle & 1) ? (sSize.nWidth - nBtnWidth) >> 1 : (sSize.nWidth  - nBtnLength) * p;
//            ssize_t bt      = (nAngle & 1) ? (sSize.nHeight - nBtnLength) * p : (sSize.nHeight - nBtnWidth) >> 1;
//
//            ssize_t b_l     = 4;
//            ssize_t b_rr    = 2;
//            float lightness = sColor.lightness();
//            float b_rad     = sqrtf(nBtnWidth*nBtnWidth + nBtnLength*nBtnLength);
//
//            for (ssize_t i=0; (i++)<b_l; )
//            {
//                float bright = lightness * sqrtf(i * i) / b_l;
//
//                IGradient *gr = s->radial_gradient(bl, bt + rh, b_rad * 0.25f, bl, bt + rh, b_rad * 3.0f);
//
//                Color cl(color);
//                cl.lightness(bright);
//                gr->add_color(0.0f, cl);
//                cl.darken(0.9f);
//                gr->add_color(1.0f, cl);
//
//                s->fill_round_rect(bl + i, bt + i, rw - i*2, rh - i*2, b_rr, SURFMASK_ALL_CORNER, gr);
//                delete gr; // Delete gradient!
//            }
        }

        ws::mouse_pointer_t Fader::current_pointer()
        {
            return ((nXFlags & (F_MOVER | F_IGNORE)) == F_MOVER) ?
                    sBtnPointer.get() :
                    Widget::current_pointer();
        }
    
    } /* namespace tk */
} /* namespace lsp */
