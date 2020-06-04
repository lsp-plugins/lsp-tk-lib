/*
 * Knob.cpp
 *
 *  Created on: 10 июл. 2017 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t Knob::metadata      = { "Knob", &Widget::metadata };

        Knob::Knob(Display *dpy):
            Widget(dpy),
            sColor(&sProperties),
            sScaleColor(&sProperties),
            sHoleColor(&sProperties),
            sTipColor(&sProperties),
            sSizeRange(&sProperties),
            sScale(&sProperties),
            sValue(&sProperties),
            sBalance(&sProperties),
            sCycling(&sProperties)
        {
            nLastY      = -1;
            nState      = 0;
            nButtons    = 0;

            pClass      = &metadata;
        }

        Knob::~Knob()
        {
        }

        status_t Knob::init()
        {
            status_t res = Widget::init();
            if (res != STATUS_OK)
                return res;

            sColor.bind("color", &sStyle);
            sScaleColor.bind("scale.color", &sStyle);
            sHoleColor.bind("hole.color", &sStyle);
            sTipColor.bind("tip.color", &sStyle);
            sSizeRange.bind("size.range", &sStyle);
            sScale.bind("scale.size", &sStyle);
            sValue.bind("value", &sStyle);
            sBalance.bind("value.balance", &sStyle);
            sCycling.bind("value.cycling", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sColor.init(sclass, "#cccccc");
                sScaleColor.init(sclass, "#00cc00");
                sHoleColor.init(sclass, "#000000");
                sTipColor.init(sclass, "#000000");
                sSizeRange.init(sclass, 8, -1);
                sScale.init(sclass, 4);
                sValue.init(sclass, 0.5f, 0.0f, 1.0f, 0.01f);
                sBalance.init(sclass, 0.5f);
                sCycling.init(sclass, false);
            }

            handler_id_t id = sSlots.add(SLOT_CHANGE, slot_on_change, self());
            if (id < 0)
                return -id;

            return STATUS_OK;
        }

        void Knob::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (sColor.is(prop))
                query_draw();
            if (sScaleColor.is(prop))
                query_draw();
            if (sHoleColor.is(prop))
                query_draw();
            if (sTipColor.is(prop))
                query_draw();
            if (sSizeRange.is(prop))
                query_resize();
            if (sScale.is(prop))
                query_resize();
            if (sValue.is(prop))
                query_draw();
            if (sBalance.is(prop))
                query_draw();
            if (sCycling.is(prop))
                query_draw();
        }

        status_t Knob::slot_on_change(Widget *sender, void *ptr, void *data)
        {
            Knob *_this = widget_ptrcast<Knob>(ptr);
            return (_this != NULL) ? _this->on_change() : STATUS_BAD_ARGUMENTS;
        }

        void Knob::update_value(float delta)
        {
            lsp_trace("value=%f, delta=%f", sValue.get(), delta);

            // Check that value is in range
            sValue.add(delta);
            sSlots.execute(SLOT_CHANGE, this);
        }

        void Knob::on_click(ssize_t x, ssize_t y)
        {
            x              -= sSize.nLeft;
            y              -= sSize.nTop;

            ssize_t cx      = ssize_t(sSize.nWidth) >> 1;
            ssize_t cy      = ssize_t(sSize.nHeight) >> 1;
            float dx        = x - cx;
            float dy        = cy - y;
            float d         = sqrtf(dx * dx + dy * dy);
            if (d <= 0.0f)
                return;

            float angle     = asinf(dy / d);
            if (sCycling.get())
            {
                if (dx < 0.0f)
                    angle           = M_PI - angle;
                // Angle is now between -PI/2 .. 3*PI/2
                if (angle < M_PI * 0.5f)
                    angle          += 1.5f * M_PI;
                else
                    angle          -= M_PI * 0.5f;

                sValue.set_normalized(1.0f - angle / (M_PI * 2.0f));
            }
            else
            {
                if (angle < (-M_PI / 3.0))
                {
                    sValue.set_normalized((dx > 0) ? 1.0f : 0.0f);
                    return;
                }
                if (dx < 0.0f)
                    angle           = M_PI - angle;

                angle          += M_PI / 3.0;

                // Update value
                sValue.set_normalized(1.0f - (angle / (5.0f * M_PI  / 3.0f)));
            }
        }

        size_t Knob::check_mouse_over(ssize_t x, ssize_t y)
        {
            float scaling   = lsp_max(0.0f, sScaling.get());

            // Update coordinates
            x              -= sSize.nLeft;
            y              -= sSize.nTop;
            size_t xr       = lsp_min(sSize.nWidth, sSize.nHeight) >> 1;

            // Estimate center coordinates and difference
            ssize_t cx      = ssize_t(sSize.nWidth) >> 1;
            ssize_t cy      = ssize_t(sSize.nHeight) >> 1;
            ssize_t dx      = x - cx;
            ssize_t dy      = y - cy;
            ssize_t delta   = dx*dx + dy*dy;

            // lsp_trace("cx=%d, cy=%d, x=%d, y=%d, dx=%d, dy=%d, r=%d", int(cx), int(cy), int(x), int(y), int(dx), int(dy), int(r));

            // Estimate hole and scale color
            size_t hole     = lsp_max(1, scaling);
            size_t gap      = lsp_max(1, scaling);
            size_t scale    = (sScale.get() > 0) ? lsp_max(0, sScale.get() * scaling) : 0;

            if (delta > ssize_t(xr*xr))
                return S_NONE;

            if (scale > 0)
            {
                xr             -= scale;
                if (delta >= ssize_t(xr*xr))
                    return S_CLICK;
                xr             -= gap;
            }

            xr -= hole;

            return (delta <= ssize_t(xr*xr)) ? S_MOVING : S_NONE;
        }

        void Knob::size_request(ws::size_limit_t *r)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t chamfer     = lsp_max(1, scaling * 3.0f);
            size_t hole         = lsp_max(1, scaling);
            size_t gap          = lsp_max(1, scaling);
            size_t scale        = lsp_max(0, sScale.get() * scaling);
            size_t extra        = hole + ((scale > 0) ? scale + gap : 0);

            ssize_t min, max;
            sSizeRange.compute(&min, &max, scale);

            r->nMinWidth        = lsp_max(min, chamfer * 2) + extra;
            r->nMaxWidth        = (max >= 0) ? lsp_max(max, chamfer * 2) + extra : -1;

            r->nMinHeight       = r->nMinWidth;
            r->nMaxHeight       = r->nMaxWidth;
        }

        status_t Knob::on_mouse_down(const ws::event_t *e)
        {
//            lsp_trace("x=%d, y=%d, state=%x, code=%x", int(e->nLeft), int(e->nTop), int(e->nState), int(e->nCode));
            if ((nButtons == 0) && ((e->nCode == ws::MCB_LEFT) || (e->nCode == ws::MCB_RIGHT)))
                nState  = check_mouse_over(e->nLeft, e->nTop);

            nButtons   |= (1 << e->nCode);
            nLastY      = e->nTop;

            return STATUS_OK;
        }

        status_t Knob::on_mouse_up(const ws::event_t *e)
        {
//            lsp_trace("x=%d, y=%d, state=%x, code=%x", int(e->nLeft), int(e->nTop), int(e->nState), int(e->nCode));
            nButtons &= ~(1 << e->nCode);
            nLastY = e->nTop;
            if (nButtons == 0)
            {
                if ((nState == S_CLICK) && (e->nCode == ws::MCB_LEFT))
                    on_click(e->nLeft, e->nTop);
                nState      = S_NONE;
            }

            return STATUS_OK;
        }

        status_t Knob::on_mouse_move(const ws::event_t *e)
        {
//            lsp_trace("x=%d, y=%d, state=%x, code=%x", int(e->nLeft), int(e->nTop), int(e->nState), int(e->nCode));
            if (nState == S_MOVING)
            {
                if (!(nButtons & (ws::MCF_LEFT | ws::MCF_RIGHT)))
                    return STATUS_OK;

                // Update value
                float scaling   = lsp_max(0.0f, sScaling.get());
                float step      = sValue.step();
                if (nButtons & ws::MCF_RIGHT)
                {
                    step = (e->nState & ws::MCF_SHIFT)   ? sValue.step() :
                           (e->nState & ws::MCF_CONTROL) ? sValue.quick() :
                           sValue.slow();
                }
                else
                {
                    step = (e->nState & ws::MCF_SHIFT) ? sValue.slow() :
                           (e->nState & ws::MCF_CONTROL) ? sValue.quick() :
                           sValue.step();
                }

                update_value(step * (nLastY - e->nTop) / scaling);
                nLastY = e->nTop;
            }
            else if (nState == S_CLICK)
            {
                if (!(nButtons & ws::MCF_LEFT))
                    return STATUS_OK;

                on_click(e->nLeft, e->nTop);
            }

            return STATUS_OK;
        }

        status_t Knob::on_mouse_scroll(const ws::event_t *e)
        {
//            lsp_trace("x=%d, y=%d, state=%x, code=%x", int(e->nLeft), int(e->nTop), int(e->nState), int(e->nCode));
            float step = (e->nState & ws::MCF_SHIFT) ? sValue.slow() :
                         (e->nState & ws::MCF_CONTROL) ? sValue.quick() :
                         sValue.step();

            // Update value
            float delta = 0.0;
            if (e->nCode == ws::MCD_UP)
                delta   = step;
            else if (e->nCode == ws::MCD_DOWN)
                delta   = -step;
            else
                return STATUS_OK;

            update_value(delta);

            return STATUS_OK;
        }

        void Knob::draw(ws::ISurface *s)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());
            float bright        = sBrightness.get();
            float value         = sValue.get_normalized();
            float balance       = sValue.get_normalized(sBalance.get());

            // Calculate knob parameters
            ssize_t c_x         = (sSize.nWidth >> 1);
            ssize_t c_y         = (sSize.nHeight >> 1);
            size_t xr           = lsp_min(sSize.nWidth, sSize.nHeight) >> 1;
            size_t chamfer      = lsp_max(1, scaling * 3.0f);
            size_t hole         = lsp_max(1, scaling);
            size_t gap          = lsp_max(1, scaling);
            size_t scale        = lsp_max(0, sScale.get() * scaling);

            // Prepare the color palette
            lsp::Color scol(sScaleColor);
            lsp::Color sdcol(sScaleColor);
            lsp::Color hcol(sHoleColor);
            lsp::Color bg_color(sBgColor);
            lsp::Color cap(sColor);
            lsp::Color tip(sTipColor);

            scol.scale_lightness(bright);
            sdcol.scale_lightness(0.75f * bright);
            cap.scale_lightness(bright);
            tip.scale_lightness(bright);

            // Draw background
            s->clear(bg_color);
            bool aa = s->set_antialiasing(true);

            size_t nsectors;
            float delta, base, v_angle1, v_angle2;
            if (sCycling.get())
            {
                nsectors      = 24;
                base          = 1.5f * M_PI;
                delta         = 2.0f * M_PI;
                v_angle1      = base + value * delta;
                v_angle2      = base + balance * delta * 0.5f;
            }
            else
            {
                nsectors      = 20;
                base          = 2.0f * M_PI / 3.0f;
                delta         = 5.0f * M_PI / 3.0f;
                v_angle1      = base + value * delta;
                v_angle2      = base + balance * delta;
            }

            // Draw scale
            if (scale > 0)
            {
                if (sCycling.get())
                {
                    s->fill_circle(c_x, c_y, xr, sdcol);
                    if (value < balance)
                        s->fill_sector(c_x, c_y, xr, v_angle1, v_angle2, scol);
                    else
                        s->fill_sector(c_x, c_y, xr, v_angle2, v_angle1, scol);

                }
                else
                {
                    s->fill_sector(c_x, c_y, xr, base, base + delta, sdcol);
                    if (value < balance)
                        s->fill_sector(c_x, c_y, xr, v_angle1, v_angle2, scol);
                    else
                        s->fill_sector(c_x, c_y, xr, v_angle2, v_angle1, scol);
                }

                // Draw scales: overall 10 segments separated by 2 sub-segments
                float r1    = xr + 1;
                float r2    = xr - scale * 0.5f;
                float r3    = xr - scale - 1;
                delta   = 0.25f * M_PI / 3.0f;

                for (size_t i=0; i <= nsectors; ++i)
                {
                    float angle = base + delta * i;
                    float scr   = (i & 1) ? r2 : r3;
                    float f_sin = sinf(angle), f_cos = cosf(angle);

                    s->line(c_x + r1 * f_cos, c_y + r1 * f_sin, c_x + scr * f_cos, c_y + scr * f_sin, scaling, bg_color);
                }

                // Draw hole and update radius
                s->fill_circle(c_x, c_y, xr - scale, bg_color);
                xr             -= (scale + gap);
            }

            // Draw hole
            s->fill_circle(c_x, c_y, xr, hcol);
            xr -= hole;

            // Draw knob
            float f_sin = sinf(v_angle1), f_cos = cosf(v_angle1);

            for (size_t i=0; i<=chamfer; ++i, --xr)
            {
                // Compute color
                float bright = float(i + 1.0f) / (chamfer + 1);
                scol.blend(cap, hcol, bright);
                sdcol.blend(scol, hcol, 0.5f);

                // Draw cap
                ws::IGradient *gr = s->radial_gradient(c_x + xr, c_y - xr, xr, c_x + xr, c_y - xr, xr * 4.0);
                gr->add_color(0.0f, scol);
                gr->add_color(1.0f, sdcol);
                s->fill_circle(c_x, c_y, xr, gr);
                delete gr;

                // Draw tip
                scol.copy(tip);
                scol.blend(hcol, bright);
                s->line(c_x + (xr * 0.25f) * f_cos, c_y + (xr * 0.25f) * f_sin,
                        c_x + xr * f_cos, c_y + xr * f_sin, 3.0f * scaling, scol);
            }

            s->set_antialiasing(aa);
        }

        status_t Knob::on_change()
        {
            return STATUS_OK;
        }

    } /* namespace tk */
} /* namespace lsp */
