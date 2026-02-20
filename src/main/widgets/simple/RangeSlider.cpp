/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 23 сент. 2025 г.
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
#include <lsp-plug.in/common/debug.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(RangeSlider, Widget)
                // Init
                RangeSliderColors *c = &vColors[RSLIDER_NORMAL];
                c->sBtnColor.bind("button.color", this);
                c->sBtnBorderColor.bind("button.border.color", this);
                c->sScaleColor.bind("scale.color", this);
                c->sScaleBorderColor.bind("scale.border.color", this);
                c->sBalanceColor.bind("balance.color", this);

                c = &vColors[RSLIDER_INACTIVE];
                c->sBtnColor.bind("inactive.button.color", this);
                c->sBtnBorderColor.bind("inactive.button.border.color", this);
                c->sScaleColor.bind("inactive.scale.color", this);
                c->sScaleBorderColor.bind("inactive.scale.border.color", this);
                c->sBalanceColor.bind("inactive.balance.color", this);

                sSizeRange.bind("size", this);
                sLimits.bind("limits", this);
                sValues.bind("values", this);
                sDistance.bind("distance", this);
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
                sScaleBrightness.bind("scale.brightness", this);
                sBalanceColorCustom.bind("balance.color.custom", this);
                sInvertMouseVScroll.bind("mouse.vscroll.invert", this);

                // Configure
                c = &vColors[RSLIDER_NORMAL];
                c->sBtnColor.set("#cccccc");
                c->sBtnBorderColor.set("#cccccc");
                c->sScaleColor.set("#000000");
                c->sScaleBorderColor.set("#ffffff");
                c->sBalanceColor.set("#000000");

                c = &vColors[RSLIDER_INACTIVE];
                c->sBtnColor.set("#888888");
                c->sBtnBorderColor.set("#888888");
                c->sScaleColor.set("#000000");
                c->sScaleBorderColor.set("#cccccc");
                c->sBalanceColor.set("#000000");

                sSizeRange.set(64, -1);
                sLimits.set(0.0f, 1.0f);
                sValues.set(0.25f, 0.75f);
                sDistance.set(0.1f);
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
                sScaleBrightness.set(0.75f);
                sBalanceColorCustom.set(false);
                sInvertMouseVScroll.set(false);
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(RangeSlider, "RangeSlider", "root");

            void RangeSliderColors::listener(tk::prop::Listener *listener)
            {
                sBtnColor.listener(listener);
                sBtnBorderColor.listener(listener);
                sScaleColor.listener(listener);
                sScaleBorderColor.listener(listener);
                sBalanceColor.listener(listener);
            }

            bool RangeSliderColors::property_changed(Property *prop)
            {
                return prop->one_of(sBtnColor, sBtnBorderColor, sScaleColor, sScaleBorderColor, sBalanceColor);
            }
        }

        const w_class_t RangeSlider::metadata           = { "RangeSlider", &Widget::metadata };

        RangeSlider::RangeSlider(Display *dpy):
            Widget(dpy),
            sSizeRange(&sProperties),
            sLimits(&sProperties),
            sValues(&sProperties),
            sDistance(&sProperties),
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
            sScaleBrightness(&sProperties),
            sBalanceColorCustom(&sProperties),
            sInvertMouseVScroll(&sProperties)
        {
            nLastV          = 0;
            nButtons        = 0;
            nXFlags         = 0;
            fButtonRange    = 1.0f;
            fCurrValue[0]   = 0.0f;
            fCurrValue[1]   = 0.0f;
            fLastValue[0]   = 0.0f;
            fLastValue[1]   = 0.0f;

            for (size_t i=0; i<BTN_TOTAL; ++i)
            {
                ws::rectangle_t *btn = &vButtons[i];

                btn->nLeft      = -1;
                btn->nTop       = -1;
                btn->nWidth     = 0;
                btn->nHeight    = 0;
            }
            nCurrButton     = -1;

            sHole.nLeft     = -1;
            sHole.nTop      = -1;
            sHole.nWidth    = 0;
            sHole.nHeight   = 0;

            for (size_t i=0; i<RSLIDER_TOTAL; ++i)
                vColors[i].listener(&sProperties);

            pClass          = &metadata;
        }

        RangeSlider::~RangeSlider()
        {
            nFlags     |= FINALIZED;
            do_destroy();
        }

        status_t RangeSlider::init()
        {
            status_t result = Widget::init();
            if (result != STATUS_OK)
                return result;

            style::RangeSliderColors *c = &vColors[style::RSLIDER_NORMAL];
            c->sBtnColor.bind("button.color", &sStyle);
            c->sBtnBorderColor.bind("button.border.color", &sStyle);
            c->sScaleColor.bind("scale.color", &sStyle);
            c->sScaleBorderColor.bind("scale.border.color", &sStyle);
            c->sBalanceColor.bind("balance.color", &sStyle);

            c = &vColors[style::RSLIDER_INACTIVE];
            c->sBtnColor.bind("inactive.button.color", &sStyle);
            c->sBtnBorderColor.bind("inactive.button.border.color", &sStyle);
            c->sScaleColor.bind("inactive.scale.color", &sStyle);
            c->sScaleBorderColor.bind("inactive.scale.border.color", &sStyle);
            c->sBalanceColor.bind("inactive.balance.color", &sStyle);

            sSizeRange.bind("size", &sStyle);
            sLimits.bind("limits", &sStyle);
            sDistance.bind("distance", &sStyle);
            sValues.bind("values", &sStyle);
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
            sScaleBrightness.bind("scale.brightness", &sStyle);
            sBalanceColorCustom.bind("balance.color.custom", &sStyle);
            sInvertMouseVScroll.bind("mouse.vscroll.invert", &sStyle);

            handler_id_t id = 0;
            id = sSlots.add(SLOT_CHANGE, slot_on_change, self());
            if (id >= 0) id = sSlots.add(SLOT_BEGIN_EDIT, slot_begin_edit, self());
            if (id >= 0) id = sSlots.add(SLOT_END_EDIT, slot_end_edit, self());
            if (id < 0)
                return -id;

            return STATUS_OK;
        }

        style::RangeSliderColors *RangeSlider::select_colors()
        {
            size_t flags = (sActive.get()) ? style::RSLIDER_NORMAL : style::RSLIDER_INACTIVE;
            return &vColors[flags];
        }

        void RangeSlider::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            // Self properties
            style::RangeSliderColors *cols = select_colors();
            if (cols->property_changed(prop))
                query_draw();

            if (prop->one_of(sValues, sLimits, sDistance))
                sync_button_pos();

            if (prop->one_of(sSizeRange, sBtnWidth, sBtnAspect, sAngle, sScaleWidth,
                sScaleBorder, sScaleRadius, sBtnBorder, sBtnRadius))
                query_resize();

            if (prop->one_of(sScaleGradient, sBtnGradient, sScaleBrightness, sBalanceColorCustom))
                query_draw();
        }

        void RangeSlider::sync_button_pos()
        {
            const size_t angle      = sAngle.get();
            float min               = sLimits.get_normalized(sValues.min());
            float max               = sLimits.get_normalized(sValues.max());

            ws::rectangle_t *bmin   = &vButtons[BTN_MIN];
            ws::rectangle_t *bmax   = &vButtons[BTN_MAX];
            ws::rectangle_t *brange = &vButtons[BTN_RANGE];
            if (sLimits.inversed())
            {
                lsp::swap(min, max);
                lsp::swap(bmin, bmax);
            }

//            lsp_trace("min = %f, max = %f", min, max);

            switch (angle & 3)
            {
                case 0: // horizontal left -> right
                {
                    bmin->nTop          = sSize.nTop;
                    bmax->nTop          = sSize.nTop;
                    bmin->nLeft         = sSize.nLeft + min * fButtonRange;
                    bmax->nLeft         = sSize.nLeft + bmin->nWidth + max * fButtonRange;
                    break;
                }

                case 1: // vertical bottom -> top
                {
                    bmin->nLeft         = sSize.nLeft;
                    bmax->nLeft         = sSize.nLeft;
                    bmax->nTop          = sSize.nTop + (1.0f - max) * fButtonRange;
                    bmin->nTop          = sSize.nTop + bmax->nHeight + (1.0f - min) * fButtonRange;
                    break;
                }

                case 2: // horizontal right -> left
                {
                    bmin->nTop          = sSize.nTop;
                    bmax->nTop          = sSize.nTop;
                    bmax->nLeft         = sSize.nLeft + (1.0f - max) * fButtonRange;
                    bmin->nLeft         = sSize.nLeft + bmax->nWidth + (1.0f - min) * fButtonRange;
                    break;
                }

                case 3: // vertical top -> bottom
                default:
                {
                    bmin->nLeft         = sSize.nLeft;
                    bmax->nLeft         = sSize.nLeft;
                    bmin->nTop          = sSize.nTop + min * fButtonRange;
                    bmax->nTop          = sSize.nTop + bmin->nHeight + max * fButtonRange;
                    break;
                }
            }
            brange->nLeft           = lsp_min(bmin->nLeft, bmax->nLeft);
            brange->nTop            = lsp_min(bmin->nTop, bmax->nTop);
            brange->nWidth          = lsp_max(bmin->nLeft + bmin->nWidth, bmax->nLeft + bmax->nWidth) - brange->nLeft;
            brange->nHeight         = lsp_max(bmin->nTop + bmin->nHeight, bmax->nTop + bmax->nHeight) - brange->nTop;

            query_draw();
        }

        void RangeSlider::size_request(ws::size_limit_t *r)
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
                // Compute parameters for vertical slider
                r->nMinWidth        = lsp_max(bmin, range);
                r->nMaxWidth        = (bmax >= 0) ? lsp_max(bmax, range) : -1;
                bmin                = lsp_max(bmin, ssize_t(r->nMinWidth * aspect), range);

                // Compute boundaries for slider
                r->nMinHeight       = lsp_max(smin, bmin * 4);
                r->nMaxHeight       = (smax >= 0) ? lsp_max(smax, bmin) : -1;
            }
            else // Horizontal
            {
                // Compute parameters for vertical slider
                r->nMinHeight       = lsp_max(bmin, range);
                r->nMaxHeight       = (bmax >= 0) ? lsp_max(bmax, range) : -1;
                bmin                = lsp_max(bmin, ssize_t(r->nMinHeight * aspect), range);

                // Compute boundaries for slider
                r->nMinWidth        = lsp_max(smin, bmin * 4);
                r->nMaxWidth        = (smax >= 0) ? lsp_max(smax, bmin) : -1;
            }

            r->nPreWidth        = -1;
            r->nPreHeight       = -1;
        }

        bool RangeSlider::realize(const ws::rectangle_t *r)
        {
            const bool needs_redraw = Widget::realize(r);

            float scaling       = lsp_max(0.0f, sScaling.get());
            float aspect        = lsp_max(0.0f, sBtnAspect.get());
            size_t angle        = sAngle.get();
            ssize_t hole_width  = lsp_max(0.0f, sScaleWidth.get() * scaling);

            ssize_t schamfer    = (sScaleBorder.get() > 0) ? lsp_max(1, scaling * sScaleBorder.get()) : 0.0f;
            ssize_t sradius     = (sScaleRadius.get() > 0) ? lsp_max(1, scaling * sScaleRadius.get()) : 0.0f;
            ssize_t range       = lsp_max(schamfer*2 + hole_width, sradius * 2);

            ws::rectangle_t *bt = &vButtons[BTN_MIN];
            bt->nLeft           = r->nLeft;
            bt->nTop            = r->nTop;

            if (angle & 1)
            {
                bt->nWidth          = r->nWidth;
                bt->nHeight         = lsp_max(r->nWidth * aspect, range);

                sHole.nWidth        = range;
                sHole.nHeight       = r->nHeight + range - bt->nHeight;
                fButtonRange        = float(r->nHeight - bt->nHeight*2);
            }
            else
            {
                bt->nHeight         = r->nHeight;
                bt->nWidth          = lsp_max(r->nHeight * aspect, range);

                sHole.nHeight       = range;
                sHole.nWidth        = r->nWidth + range - bt->nWidth;
                fButtonRange        = float(r->nWidth - bt->nWidth * 2);
            }

            vButtons[BTN_MAX]   = *bt;
            vButtons[BTN_RANGE] = *bt;

            // Locate hole at center
            sHole.nLeft         = r->nLeft + ((r->nWidth  - sHole.nWidth) >> 1);
            sHole.nTop          = r->nTop  + ((r->nHeight - sHole.nHeight) >> 1);

            // Sync position of the slider buttons
            sync_button_pos();

            return needs_redraw;
        }

        status_t RangeSlider::slot_on_change(Widget *sender, void *ptr, void *data)
        {
            RangeSlider *_this = widget_ptrcast<RangeSlider>(ptr);
            const size_t *flags = static_cast<size_t *>(data);
            return (_this != NULL) ? _this->on_change((flags != NULL) ? *flags : CHANGE_BOTH) : STATUS_BAD_ARGUMENTS;
        }

        status_t RangeSlider::slot_begin_edit(Widget *sender, void *ptr, void *data)
        {
            RangeSlider *_this = widget_ptrcast<RangeSlider>(ptr);
            return (_this != NULL) ? _this->on_begin_edit() : STATUS_BAD_ARGUMENTS;
        }

        status_t RangeSlider::slot_end_edit(Widget *sender, void *ptr, void *data)
        {
            RangeSlider *_this = widget_ptrcast<RangeSlider>(ptr);
            return (_this != NULL) ? _this->on_end_edit() : STATUS_BAD_ARGUMENTS;
        }

        status_t RangeSlider::on_change(size_t flags)
        {
            return STATUS_OK;
        }

        status_t RangeSlider::on_begin_edit()
        {
            return STATUS_OK;
        }

        status_t RangeSlider::on_end_edit()
        {
            return STATUS_OK;
        }

        void RangeSlider::update_values(const float *values, ssize_t button_id)
        {
            if (button_id < 0)
                return;

            const bool inversed = sLimits.inversed();
            const float old_min = sValues.min();
            const float old_max = sValues.max();

//            lsp_trace("in: old_min=%f, old_max=%f, dist=%f, min=%f, max=%f, flags=0x%x",
//                old_min, old_max, dist, min, max, int(flags));
            float min       = values[0];
            float max       = values[1];

            switch (button_id)
            {
                case BTN_MIN:
                {
                    const float dist    = sDistance.get();
                    min                 = sLimits.limit(min);
                    max                 = (inversed) ? lsp_min(min - dist, old_max) : lsp_max(min + dist, old_max);
                    max                 = sLimits.limit(max);
                    min                 = (inversed) ? lsp_max(max + dist, min) : lsp_min(max - dist, min);
                    min                 = sLimits.limit(min);
                    break;
                }

                case BTN_MAX:
                {
                    const float dist    = sDistance.get();
                    max                 = sLimits.limit(max);
                    min                 = (inversed) ? lsp_max(max + dist, old_min) : lsp_min(max - dist, old_min);
                    min                 = sLimits.limit(min);
                    max                 = (inversed) ? lsp_min(min - dist, max) : lsp_max(min + dist, max);
                    max                 = sLimits.limit(max);
                    break;
                }

                case BTN_RANGE:
                default:
                {
                    float delta         = max - old_max;
                    if (delta == 0.0f)
                        delta               = min - old_min;

                    if (sLimits.inversed())
                    {
                        const float dist    = old_min - old_max;
                        if (delta < 0.0f)
                        {
                            max                 = sLimits.limit(max);
                            min                 = sLimits.limit(lsp_max(min, max + dist));
                        }
                        else
                        {
                            min                 = sLimits.limit(min);
                            max                 = sLimits.limit(lsp_min(max, min - dist));
                        }
                    }
                    else
                    {
                        const float dist    = old_max - old_min;
                        if (delta < 0.0f)
                        {
                            min                 = sLimits.limit(min);
                            max                 = sLimits.limit(lsp_max(max, min + dist));
                        }
                        else
                        {
                            max                 = sLimits.limit(max);
                            min                 = sLimits.limit(lsp_min(min, max - dist));
                        }
                    }

                    break;
                }
            }

            size_t flags    = 0;
            if (old_min != min)
                flags              |= CHANGE_MIN;
            if (old_max != max)
                flags              |= CHANGE_MAX;

//            lsp_trace("out: min=%f, max=%f, flags=0x%x", min, max, flags);

            sValues.set(min, max);
            if (flags != 0)
                sSlots.execute(SLOT_CHANGE, this, &flags);
        }

        void RangeSlider::add_values(float delta)
        {
            const float old_min = sValues.min();
            const float old_max = sValues.max();
            const float dist    = sDistance.get();
            float min, max;

            if (sLimits.inversed())
            {
                if (delta < 0.0f)
                {
                    max     = sLimits.limit(old_max - delta);
                    delta   = old_max - max;
                    min     = sLimits.limit(lsp_max(old_min - delta, max + dist));
                }
                else
                {
                    min     = sLimits.limit(old_min + delta);
                    delta   = min - old_min;
                    max     = sLimits.limit(lsp_min(old_max + delta, min - dist));
                }
            }
            else
            {
                if (delta < 0.0f)
                {
                    min     = sLimits.limit(old_min + delta);
                    delta   = min - old_min;
                    max     = sLimits.limit(lsp_max(old_max + delta, min + dist));
                }
                else
                {
                    max     = sLimits.limit(old_max + delta);
                    delta   = max - old_max;
                    min     = sLimits.limit(lsp_min(old_min + delta, max - dist));
                }
            }

            size_t flags    = 0;
            if (old_min != min)
                flags      |= CHANGE_MIN;
            if (old_max != max)
                flags      |= CHANGE_MAX;

            if (flags != 0)
            {
                sValues.set(min, max);
                sSlots.execute(SLOT_BEGIN_EDIT, this, &flags);
                sSlots.execute(SLOT_CHANGE, this, &flags);
                sSlots.execute(SLOT_END_EDIT, this, &flags);
            }
        }

        ssize_t RangeSlider::find_button(const ws::event_t *e)
        {
            for (size_t i=0; i<BTN_TOTAL; ++i)
            {
                if (Position::inside(&vButtons[i], e->nLeft, e->nTop))
                    return i;
            }

            return -1;
        }

        status_t RangeSlider::on_mouse_down(const ws::event_t *e)
        {
            if (nButtons == 0)
            {
                nCurrButton = find_button(e);
                if (nCurrButton >= 0)
                {
                    if (e->nCode == ws::MCB_RIGHT)
                        nXFlags        |= F_PRECISION;
                    else if (e->nCode != ws::MCB_LEFT)
                        nXFlags        |= F_IGNORE;
                }
                else
                    nXFlags        |= F_IGNORE;

                if (!(nXFlags & F_IGNORE))
                {
                    nLastV          = (sAngle.get() & 1) ? e->nTop : e->nLeft;
                    fLastValue[0]   = sValues.min();
                    fLastValue[1]   = sValues.max();
                    fCurrValue[0]   = fLastValue[0];
                    fCurrValue[1]   = fLastValue[1];

//                    lsp_trace("curr button is %s, last_value=%f",
//                        (pCurrButton == &vButtons[0]) ? "MIN" : "MAX", fLastValue);

                    sSlots.execute(SLOT_BEGIN_EDIT, this);
                }
            }

            nButtons       |= (size_t(1) << e->nCode);
            if (nXFlags & F_IGNORE)
                return STATUS_OK;

            const size_t key    = (nXFlags & F_PRECISION) ? ws::MCB_RIGHT : ws::MCB_LEFT;

            // Update value
            const float *values = (nButtons == size_t(size_t(1) << key)) ? fCurrValue : fLastValue;
            update_values(values, nCurrButton);

            return STATUS_OK;
        }

        status_t RangeSlider::on_mouse_up(const ws::event_t *e)
        {
            nButtons       &= ~(size_t(1) << e->nCode);
            if (nXFlags & F_IGNORE)
            {
                if (nButtons == 0)
                {
                    nCurrButton = find_button(e);
                    nXFlags     = 0;
                }
                return STATUS_OK;
            }

            size_t key          = (nXFlags & F_PRECISION) ? ws::MCB_RIGHT : ws::MCB_LEFT;
            const float *values = fLastValue;

            if (nButtons == 0) // All mouse buttons are released now
            {
                nCurrButton = find_button(e);
                nXFlags     = 0;
                values      = (e->nCode == key) ? fCurrValue : fLastValue;
            }
            else if (nButtons == (size_t(1) << key)) // Currently pressed initially selected button
                values      = fCurrValue;

            // Update value
            update_values(values, nCurrButton);

            if (nButtons == 0)
                sSlots.execute(SLOT_END_EDIT, this);

            return STATUS_OK;
        }

        status_t RangeSlider::on_mouse_move(const ws::event_t *e)
        {
            if (nXFlags & F_IGNORE)
                return STATUS_OK;

            size_t key = (nXFlags & F_PRECISION) ? ws::MCB_RIGHT : ws::MCB_LEFT;
            if (nButtons != (size_t(1) << key))
            {
                nCurrButton     = find_button(e);
                return STATUS_OK;
            }
            else if (nCurrButton < 0)
                return STATUS_OK;

            // Different behaviour for slider
            const ssize_t angle = sAngle.get() & 3;

            ssize_t value   = (angle & 1) ? e->nTop : e->nLeft;

            // Update value
            for (size_t i=0; i<2; ++i)
            {
                if (value != nLastV)
                {
                    float delta     = sLimits.range() * float(value - nLastV) / fButtonRange; // normalized
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

    //                lsp_trace("last_value = %f, delta = %f, accel = %f", fLastValue, delta, accel);

                    fCurrValue[i]   = ((angle == 1) || (angle == 2)) ? fLastValue[i] - delta*accel : fLastValue[i] + delta*accel;
                }
            }

//            lsp_trace("fCurrValue=%f, fLastValue=%f", fCurrValue, fLastValue);
            update_values(fCurrValue, nCurrButton);

            return STATUS_OK;
        }

        status_t RangeSlider::on_mouse_scroll(const ws::event_t *e)
        {
            float step      = sStep.get(e->nState & ws::MCF_CONTROL, e->nState & ws::MCF_SHIFT);
            if (sInvertMouseVScroll.get())
                step            = -step;

            // Update value
            float delta = 0.0f;
            if (e->nCode == ws::MCD_UP)
                delta   = step;
            else if (e->nCode == ws::MCD_DOWN)
                delta   = -step;
            else
                return STATUS_OK;

            add_values(delta);

            return STATUS_OK;
        }

        void RangeSlider::draw(ws::ISurface *s, bool force)
        {
            ws::IGradient *g;
            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t schamfer    = (sScaleBorder.get() > 0) ? lsp_max(1, scaling * sScaleBorder.get()) : 0.0f;
            ssize_t sradius     = (sScaleRadius.get() > 0) ? lsp_max(1, scaling * sScaleRadius.get()) : 0.0f;
            ssize_t bchamfer    = (sBtnBorder.get() > 0) ? lsp_max(1, scaling * sBtnBorder.get()) : 0.0f;
            ssize_t bradius     = (sBtnRadius.get() > 0) ? lsp_max(1, scaling * sBtnRadius.get()) : 0.0f;
            size_t angle        = sAngle.get();
            float bright        = select_brightness();
            const style::RangeSliderColors *colors = select_colors();

            // Prepare palette
            lsp::Color bg_color;
            lsp::Color button(colors->sBtnColor);
            lsp::Color scol, sdcol;
            lsp::Color bborder(colors->sBtnBorderColor);
            lsp::Color sborder(colors->sScaleBorderColor);

            if (sBalanceColorCustom.get())
            {
                scol.copy(colors->sBalanceColor);
                sdcol.copy(colors->sScaleColor);
            }
            else
            {
                scol.copy(colors->sScaleColor);
                sdcol.copy(colors->sScaleColor);
                sdcol.scale_hsl_lightness(sScaleBrightness.get());
            }

            get_actual_bg_color(bg_color);
            button.scale_lch_luminance(bright);
            scol.scale_lch_luminance(bright);
            sdcol.scale_lch_luminance(bright);

            // Clear surface
            s->clear(bg_color);
            const bool aa       = s->set_antialiasing(true);
            lsp_finally {
                // Restore antialiasing
                s->set_antialiasing(aa);
            };

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
                    sborder.scale_lch_luminance(bright);

                    g = s->radial_gradient(0, sSize.nHeight, scaling, sSize.nHeight, delta);
                    g->set_start(sborder);
                    g->set_stop(0.5 * sborder.red(), 0.5 *  sborder.green(), 0.5 * sborder.blue());
                    s->fill_rect(g, SURFMASK_ALL_CORNER, sradius, &h);
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
                sborder.scale_lch_luminance(bright);
                s->fill_rect(sborder, SURFMASK_ALL_CORNER, sradius, &h);
                sradius         = lsp_max(0, sradius - schamfer);
                h.nLeft        += schamfer;
                h.nTop         += schamfer;
                h.nWidth       -= (schamfer << 1);
                h.nHeight      -= (schamfer << 1);
            }

            // Draw the scale
            {
                s->fill_rect(sdcol, SURFMASK_ALL_CORNER, sradius, &h);

                ssize_t minv, maxv;
                ws::rectangle_t c   = sSize;
                c.nLeft        -= sSize.nLeft;
                c.nTop         -= sSize.nTop;

                switch (angle & 3)
                {
                    case 0:
                    case 2:
                        minv            = vButtons[BTN_MIN].nLeft - sSize.nLeft + vButtons[BTN_MIN].nWidth / 2;
                        maxv            = vButtons[BTN_MAX].nLeft - sSize.nLeft + vButtons[BTN_MAX].nWidth / 2;
                        c.nLeft         = lsp_min(minv, maxv);
                        c.nWidth        = lsp_max(minv, maxv) - c.nLeft;
                        break;
                    case 1:
                    case 3:
                    default:
                        minv            = vButtons[BTN_MIN].nTop - sSize.nTop + vButtons[BTN_MIN].nHeight / 2;
                        maxv            = vButtons[BTN_MAX].nTop - sSize.nTop + vButtons[BTN_MAX].nHeight / 2;
                        c.nTop          = lsp_min(minv, maxv);
                        c.nHeight       = lsp_max(minv, maxv) - c.nTop;
                        break;
                }

                s->clip_begin(&c);
                s->fill_rect(scol, SURFMASK_ALL_CORNER, sradius, &h);
                s->clip_end();
            }


            // Draw buttons
            for (size_t i=BTN_MIN; i<=BTN_MAX; ++i)
            {
                h               = vButtons[i];
                h.nLeft        -= sSize.nLeft;
                h.nTop         -= sSize.nTop;

                if (sBtnGradient.get())
                {
                    float delta     = sqrtf(h.nWidth * h.nWidth + h.nHeight * h.nHeight) * 0.5f;
                    float xb        = button.lightness();

                    for (ssize_t j=0; j<bchamfer; ++j)
                    {
                        // Compute button
                        const float l       = float(j + 1.0f) / (bchamfer + 1);
                        const float rrad    = lsp_max(0, bradius - j);

                        // Create gradient
                        g = s->radial_gradient(h.nLeft + h.nWidth + bchamfer, h.nTop - bchamfer, h.nLeft + h.nWidth + bchamfer, h.nTop - bchamfer, delta);
                        bborder.lightness(1.0f);
                        g->set_start(bborder.red(), bborder.green(), bborder.blue());
                        bborder.lightness(xb * l);
                        g->set_stop(bborder.red(), bborder.green(), bborder.blue());
                        s->fill_rect(g, SURFMASK_ALL_CORNER, rrad, &h);
                        delete g;

                        // Update rect
                        h.nLeft        += 1;
                        h.nTop         += 1;
                        h.nWidth       -= 2;
                        h.nHeight      -= 2;
                    }

                    // Draw button face
                    g = s->radial_gradient(h.nLeft + h.nWidth + bchamfer, h.nTop - bchamfer, h.nLeft + h.nWidth + bchamfer, h.nTop - bchamfer, delta);
                    button.lightness(1.0f);
                    g->set_start(button.red(), button.green(), button.blue());
                    button.lightness(xb);
                    g->set_stop(button.red(), button.green(), button.blue());
                    s->fill_rect(g, SURFMASK_ALL_CORNER, bradius, &h);
                    delete g;
                }
                else
                {
                    // Just draw simple border
                    bborder.scale_lch_luminance(bright);
                    s->fill_rect(bborder, SURFMASK_ALL_CORNER, bradius, &h);
                    bradius         = lsp_max(0, bradius - bchamfer);
                    h.nLeft        += bchamfer;
                    h.nTop         += bchamfer;
                    h.nWidth       -= (bchamfer << 1);
                    h.nHeight      -= (bchamfer << 1);

                    // Fill the button
                    s->fill_rect(button, SURFMASK_ALL_CORNER, bradius, &h);
                }
            }
        }

        status_t RangeSlider::on_mouse_pointer(pointer_event_t *e)
        {
            if ((!(nXFlags & F_IGNORE)) && (nCurrButton >= 0))
            {
                ws::mouse_pointer_t dfl_pointer = (sAngle.get() & 1) ? ws::MP_VSIZE : ws::MP_HSIZE;
                e->enPointer    = sBtnPointer.get(dfl_pointer);
            }
            return STATUS_OK;
        }

    } /* namespace tk */
} /* namespace lsp */



