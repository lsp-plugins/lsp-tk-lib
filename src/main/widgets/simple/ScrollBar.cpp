/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 3 авг. 2017 г.
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
            LSP_TK_STYLE_IMPL_BEGIN(ScrollBar, Widget)
                // Bind
                sValue.bind("value", this);
                sStep.bind("step", this);
                sAccelStep.bind("accel.step", this);
                sConstraints.bind("size.constraints", this);
                sOrientation.bind("orientation", this);
                sSliderPointer.bind("slider.pointer", this);
                sIncPointer.bind("inc.pointer", this);
                sDecPointer.bind("dec.pointer", this);
                sBorderRadius.bind("border.radius", this);
                sBorderSize.bind("border.size", this);
                sBorderGap.bind("border.gap", this);
                sSliderBorderSize.bind("slider.border.size", this);
                sButtonColor.bind("button.color", this);
                sButtonActiveColor.bind("button.active.color", this);
                sIncColor.bind("inc.color", this);
                sIncActiveColor.bind("inc.active.color", this);
                sDecColor.bind("dec.color", this);
                sDecActiveColor.bind("dec.active.color", this);
                sBorderColor.bind("border.color", this);
                sBorderGapColor.bind("border.gap.color", this);
                sSliderColor.bind("slider.color", this);
                sSliderBorderColor.bind("slider.border.color", this);
                sSliderActiveColor.bind("slider.active.color", this);
                sTextColor.bind("text.color", this);
                sTextActiveColor.bind("text.active.color", this);
                // Configure
                sValue.set(0.5f);
                sStep.set(0.01f);
                sAccelStep.set(0.05f);
                sConstraints.set(16, -1, 16, -1);
                sOrientation.set(O_HORIZONTAL);
                sSliderPointer.set(ws::MP_DEFAULT);
                sIncPointer.set(ws::MP_DEFAULT);
                sDecPointer.set(ws::MP_DEFAULT);
                sBorderRadius.set(4);
                sBorderSize.set(1);
                sBorderGap.set(1);
                sSliderBorderSize.set(1);
                sButtonColor.set("#cccccc");
                sButtonActiveColor.set("#ffffff");
                sIncColor.set("#888888");
                sIncActiveColor.set("#ffffff");
                sDecColor.set("#888888");
                sDecActiveColor.set("#ffffff");
                sBorderColor.set("#000000");
                sBorderGapColor.set("#888888");
                sSliderColor.set("#cccccc");
                sSliderBorderColor.set("#000000");
                sSliderActiveColor.set("#ffffff");
                sTextColor.set("#000000");
                sTextActiveColor.set("#000000");
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(ScrollBar, "ScrollBar", "root");
        }

        const w_class_t ScrollBar::metadata         = { "ScrollBar", &Widget::metadata };

        ScrollBar::ScrollBar(Display *dpy):
            Widget(dpy),
            sValue(&sProperties),
            sStep(&sProperties),
            sAccelStep(&sProperties),
            sConstraints(&sProperties),
            sOrientation(&sProperties),
            sSliderPointer(&sProperties),
            sIncPointer(&sProperties),
            sDecPointer(&sProperties),
            sBorderRadius(&sProperties),
            sBorderSize(&sProperties),
            sBorderGap(&sProperties),
            sSliderBorderSize(&sProperties),
            sButtonColor(&sProperties),
            sButtonActiveColor(&sProperties),
            sIncColor(&sProperties),
            sIncActiveColor(&sProperties),
            sDecColor(&sProperties),
            sDecActiveColor(&sProperties),
            sBorderColor(&sProperties),
            sBorderGapColor(&sProperties),
            sSliderColor(&sProperties),
            sSliderBorderColor(&sProperties),
            sSliderActiveColor(&sProperties),
            sTextColor(&sProperties),
            sTextActiveColor(&sProperties)
        {
            nXFlags             = 0;
            nButtons            = 0;
            nKeys               = 0;
            nLastV              = 0;
            fLastValue          = 0.0f;
            fCurrValue          = 0.0f;
            enMousePointer      = ws::MP_DEFAULT;

            sIncButton.nLeft    = -1;
            sIncButton.nTop     = -1;
            sIncButton.nWidth   = 0;
            sIncButton.nHeight  = 0;

            sDecButton.nLeft    = -1;
            sDecButton.nTop     = -1;
            sDecButton.nWidth   = 0;
            sDecButton.nHeight  = 0;

            sSpareSpace.nLeft   = -1;
            sSpareSpace.nTop    = -1;
            sSpareSpace.nWidth  = 0;
            sSpareSpace.nHeight = 0;

            sSlider.nLeft       = -1;
            sSlider.nTop        = -1;
            sSlider.nWidth      = 0;
            sSlider.nHeight     = 0;

            pClass          = &metadata;
        }

        ScrollBar::~ScrollBar()
        {
            nFlags     |= FINALIZED;
            do_destroy();
        }

        void ScrollBar::do_destroy()
        {
            cancel_timer();
        }

        status_t ScrollBar::init()
        {
            // Initialize widget
            status_t result = Widget::init();
            if (result != STATUS_OK)
                return result;

            // Bind timer
            sTimer.bind(pDisplay);
            sTimer.set_handler(timer_handler, this);

            sValue.bind("value", &sStyle);
            sStep.bind("step", &sStyle);
            sAccelStep.bind("accel.step", &sStyle);
            sConstraints.bind("size.constraints", &sStyle);
            sOrientation.bind("orientation", &sStyle);
            sSliderPointer.bind("slider.pointer", &sStyle);
            sIncPointer.bind("inc.pointer", &sStyle);
            sDecPointer.bind("dec.pointer", &sStyle);
            sBorderRadius.bind("border.radius", &sStyle);
            sBorderSize.bind("border.size", &sStyle);
            sBorderGap.bind("border.gap", &sStyle);
            sSliderBorderSize.bind("slider.border.size", &sStyle);
            sButtonColor.bind("button.color", &sStyle);
            sButtonActiveColor.bind("button.active.color", &sStyle);
            sIncColor.bind("inc.color", &sStyle);
            sIncActiveColor.bind("inc.active.color", &sStyle);
            sDecColor.bind("dec.color", &sStyle);
            sDecActiveColor.bind("dec.active.color", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sBorderGapColor.bind("border.gap.color", &sStyle);
            sSliderColor.bind("slider.color", &sStyle);
            sSliderBorderColor.bind("slider.border.color", &sStyle);
            sSliderActiveColor.bind("slider.active.color", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sTextActiveColor.bind("text.active.color", &sStyle);

            handler_id_t id = 0;
            id = sSlots.add(SLOT_CHANGE, slot_on_change, self());
            if (id < 0)
                return -id;

            return STATUS_OK;
        }

        void ScrollBar::destroy()
        {
            do_destroy();
            Widget::destroy();
        }

        void ScrollBar::launch_timer()
        {
            lsp_trace("launch timer");
            sTimer.launch(0, 100, 200);
        }

        void ScrollBar::cancel_timer()
        {
            lsp_trace("cancel timer");
            sTimer.cancel();
        }

        void ScrollBar::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (sValue.is(prop))
                update_slider();
            if (sConstraints.is(prop))
                query_resize();
            if (sOrientation.is(prop))
                query_resize();
            if (sBorderSize.is(prop))
                query_resize();
            if (sBorderGap.is(prop))
                query_resize();
            if (sSliderBorderSize.is(prop))
                query_resize();

            if (sButtonColor.is(prop))
                query_draw();
            if (sButtonActiveColor.is(prop))
                query_draw();
            if (sIncColor.is(prop))
                query_draw();
            if (sIncActiveColor.is(prop))
                query_draw();
            if (sDecColor.is(prop))
                query_draw();
            if (sDecActiveColor.is(prop))
                query_draw();
            if (sBorderColor.is(prop))
                query_draw();
            if (sBorderGapColor.is(prop))
                query_draw();
            if (sSliderColor.is(prop))
                query_draw();
            if (sSliderBorderColor.is(prop))
                query_draw();
            if (sSliderActiveColor.is(prop))
                query_draw();
        }

        void ScrollBar::size_request(ws::size_limit_t *r)
        {
            float scaling   = lsp_max(0.0f, sScaling.get());
            size_t border   = (sBorderSize.get() > 0) ? lsp_max(1.0f, sBorderSize.get() * scaling) : 0;
            size_t radius   = (sBorderRadius.get() > 0) ? lsp_max(1.0f, sBorderRadius.get() * scaling) : 0;
            size_t gap      = (sBorderGap.get() > 0) ? lsp_max(1.0f, sBorderGap.get() * scaling) : 0;
            size_t sborder  = (sSliderBorderSize.get() > 0) ? lsp_max(1.0f, sSliderBorderSize.get() * scaling) : 0;

            size_t ssize    = lsp_max(4.0f, 4.0f * scaling);    // Minimum slider/button size

            // Estimate minimum size for vertical scroll bar
            r->nMinWidth    = lsp_max(radius * 2, ssize + (sborder + gap + border)*2);
            r->nMinHeight   = lsp_max(radius * 2, ssize*5 + gap*4 + (border + sborder)*2);
            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;
            r->nPreWidth    = -1;
            r->nPreHeight   = -1;

            // Apply constraints to the vertical scroll bar parameters
            sConstraints.apply(r, scaling);

            // Transpose for horizontal
            if (sOrientation.horizontal())
            {
                swap(r->nMinWidth, r->nMinHeight);
                swap(r->nMaxWidth, r->nMaxHeight);
            }
        }

        void ScrollBar::realize(const ws::rectangle_t *r)
        {
            Widget::realize(r);

            float scaling   = lsp_max(0.0f, sScaling.get());
            ssize_t border  = (sBorderSize.get() > 0) ? lsp_max(1.0f, sBorderSize.get() * scaling) : 0;
            ssize_t gap     = (sBorderGap.get() > 0) ? lsp_max(1.0f, sBorderGap.get() * scaling) : 0;
            ssize_t sborder = (sSliderBorderSize.get() > 0) ? lsp_max(1.0f, sSliderBorderSize.get() * scaling) : 0;

            // Compute parameters of each element
            if (sOrientation.horizontal())
            {
                ssize_t req             = (r->nWidth - gap*4 + (border + sborder)*2) / 5;

                sDecButton.nHeight      = r->nHeight - border*2;
                sDecButton.nWidth       = lsp_min(req, sDecButton.nHeight);
                sDecButton.nLeft        = r->nLeft  + border;
                sDecButton.nTop         = r->nTop   + border;

                sIncButton.nWidth       = sDecButton.nWidth;
                sIncButton.nHeight      = sDecButton.nHeight;
                sIncButton.nTop         = sDecButton.nTop;
                sIncButton.nLeft        = r->nLeft + r->nWidth - border - sIncButton.nWidth;

                sSpareSpace.nLeft       = sDecButton.nLeft + sDecButton.nWidth + gap;
                sSpareSpace.nTop        = sDecButton.nTop  + gap;
                sSpareSpace.nWidth      = sIncButton.nLeft - sSpareSpace.nLeft - gap;
                sSpareSpace.nHeight     = sDecButton.nHeight - gap*2;
            }
            else
            {
                ssize_t req             = (r->nHeight - gap*4 + (border + sborder)*2) / 5;

                sDecButton.nWidth       = r->nWidth  - border*2;
                sDecButton.nHeight      = lsp_min(req, sDecButton.nWidth);
                sDecButton.nLeft        = r->nLeft  + border;
                sDecButton.nTop         = r->nTop   + border;

                sIncButton.nWidth       = sDecButton.nWidth;
                sIncButton.nHeight      = sDecButton.nHeight;
                sIncButton.nTop         = r->nTop + r->nHeight - border - sIncButton.nHeight;
                sIncButton.nLeft        = sDecButton.nLeft;

                sSpareSpace.nLeft       = sDecButton.nLeft + gap;
                sSpareSpace.nTop        = sDecButton.nTop + sDecButton.nHeight + gap;
                sSpareSpace.nWidth      = sDecButton.nWidth - gap*2;
                sSpareSpace.nHeight     = sIncButton.nTop - sSpareSpace.nTop - gap;
            }

            // Update slider parameters
            update_slider();
        }

        void ScrollBar::update_slider()
        {
            float scaling   = lsp_max(0.0f, sScaling.get());
            ssize_t sborder = (sSliderBorderSize.get() > 0) ? lsp_max(1.0f, sSliderBorderSize.get() * scaling) : 0;

            // Estimate the amount of space
            ssize_t ssize   = lsp_max(4.0f, 4.0f * scaling) + sborder*2;    // Minimum slider/button size
            float range     = sValue.abs_range();
            float step      = sStep.get_abs();
            ssize_t pixels  = ((range > 0.0f) && (step > 0.0f)) ? lsp_max(0, ceil(range / sStep.get_abs())) : 0;

            if (sOrientation.horizontal())
            {
                ssize               = lsp_max(sSpareSpace.nWidth - pixels, ssize);
                ssize_t left        = sSpareSpace.nWidth - ssize;

                sSlider.nLeft       = sSpareSpace.nLeft + left * sValue.get_normalized();
                sSlider.nTop        = sSpareSpace.nTop;
                sSlider.nWidth      = ssize;
                sSlider.nHeight     = sSpareSpace.nHeight;
            }
            else
            {
                ssize               = lsp_max(sSpareSpace.nHeight - pixels, ssize);
                ssize_t left        = sSpareSpace.nHeight - ssize;

                sSlider.nLeft       = sSpareSpace.nLeft;
                sSlider.nTop        = sSpareSpace.nTop + left * sValue.get_normalized();
                sSlider.nWidth      = sSpareSpace.nWidth;
                sSlider.nHeight     = ssize;
            }

            query_draw();
        }

        status_t ScrollBar::timer_handler(ws::timestamp_t sched, ws::timestamp_t time, void *arg)
        {
            ScrollBar *_this = widget_ptrcast<ScrollBar>(arg);
            if (_this == NULL)
                return STATUS_BAD_ARGUMENTS;
            _this->update_by_timer();
            return STATUS_OK;
        }

        status_t ScrollBar::slot_on_change(Widget *sender, void *ptr, void *data)
        {
            ScrollBar *_this = widget_ptrcast<ScrollBar>(ptr);
            return (_this != NULL) ? _this->on_change() : STATUS_BAD_ARGUMENTS;
        }

        ws::mouse_pointer_t ScrollBar::current_pointer()
        {
            return enMousePointer;
        }

        size_t ScrollBar::check_mouse_over(ssize_t x, ssize_t y)
        {
            if (Position::inside(&sIncButton, x, y))
                return F_BTN_UP_ACTIVE;

            if (Position::inside(&sDecButton, x, y))
                return F_BTN_DOWN_ACTIVE;

            if (Position::inside(&sSlider, x, y))
                return F_SLIDER_ACTIVE;

            if (Position::inside(&sSpareSpace, x, y))
            {
                if (sOrientation.horizontal())
                    return (x < sSlider.nLeft) ? F_SPARE_DOWN_ACTIVE : F_SPARE_UP_ACTIVE;
                else
                    return (y < sSlider.nTop) ? F_SPARE_DOWN_ACTIVE : F_SPARE_UP_ACTIVE;
            }

            return 0;
        }

        status_t ScrollBar::on_change()
        {
            return STATUS_OK;
        }

        status_t ScrollBar::on_mouse_down(const ws::event_t *e)
        {
//            lsp_trace("nButtons = %d, code = %d", int(nButtons), int(e->nCode));
            nKeys           = e->nState;
            bool launched   = false;

            if (nButtons == 0)
            {
                // Update state of buttons
                nButtons   |= (1 << e->nCode);

                // Check that we first hit inside the bar
                size_t flags = check_mouse_over(e->nLeft, e->nTop);
                update_cursor_state(e->nLeft, e->nTop, true);

                if (flags == 0)
                {
                    nXFlags         |= F_OUTSIDE;
                    return STATUS_OK;
                }

                // What button was pressed?
                if (e->nCode == ws::MCB_LEFT)
                {
                    nXFlags      = flags | (flags << F_ACTIVITY_BITS);

                    if (flags != F_SLIDER_ACTIVE)
                        launched        = true;
                }
                else if (e->nCode == ws::MCB_RIGHT)
                {
                    // Only slider allows right button
                    if (flags != F_SLIDER_ACTIVE)
                    {
                        nXFlags         |= F_OUTSIDE;
                        return STATUS_OK;
                    }

                    // Slider with precision option
                    nXFlags      = flags | (flags << F_ACTIVITY_BITS) | F_PRECISION;
                }
                else
                {
                    nXFlags         |= F_OUTSIDE;
                    return STATUS_OK;
                }

                // Save current value to edited value
                fLastValue      = sValue.get();
                fCurrValue      = fLastValue;
                nLastV          = (sOrientation.vertical()) ? e->nTop : e->nLeft;
            }
            else
            {
                nButtons   |= (1 << e->nCode);
                if (nXFlags & F_OUTSIDE)
                    return STATUS_OK;

                float value = sValue.get();

                if (nXFlags & F_TRG_SLIDER_ACTIVE) // Slider
                {
                    size_t mask = (nXFlags & F_PRECISION) ? ws::MCF_LEFT : ws::MCF_RIGHT;

                    if (nButtons == mask)
                    {
                        nXFlags  = (nXFlags & (~F_ACTIVITY_MASK)) | ((nXFlags >> F_ACTIVITY_BITS) & F_ACTIVITY_MASK); // Restore activity state
                        value   = fCurrValue;
                    }
                    else
                    {
                        nXFlags &= ~F_ACTIVITY_MASK; // Clear activity state
                        value   = fLastValue;
                    }
                }
                else // Not slider
                {
                    if (nButtons == ws::MCF_LEFT)
                    {
                        nXFlags  = (nXFlags & (~F_ACTIVITY_MASK)) | ((nXFlags >> F_ACTIVITY_BITS) & F_ACTIVITY_MASK); // Restore activity state
                        value   = fCurrValue;

                        launched    = true;
                    }
                    else
                    {
                        cancel_timer();

                        nXFlags &= ~F_ACTIVITY_MASK; // Clear activity state
                        value   = fLastValue;
                    }
                }

                // Update value
                value = sValue.limit(value);
                if (value != sValue.get())
                {
                    sValue.set(value);
                    sSlots.execute(SLOT_CHANGE, this);
                }
            }

            if (launched)
            {
                update_by_timer();
                launch_timer();
            }

            return STATUS_OK;
        }

        void ScrollBar::update_cursor_state(ssize_t x, ssize_t y, bool set)
        {
            size_t flags = (set) ? check_mouse_over(x,y) : 0;
            if (sOrientation.horizontal())
            {
                if (flags & F_SLIDER_ACTIVE)
                    enMousePointer = sSliderPointer.get(ws::MP_HSIZE);
                else if (flags & F_SPARE_UP_ACTIVE)
                    enMousePointer = sIncPointer.get(ws::MP_ARROW_RIGHT);
                else if (flags & F_SPARE_DOWN_ACTIVE)
                    enMousePointer = sIncPointer.get(ws::MP_ARROW_LEFT);
                else
                    enMousePointer = Widget::current_pointer();
            }
            else
            {
                if (flags & F_SLIDER_ACTIVE)
                    enMousePointer = sSliderPointer.get(ws::MP_VSIZE);
                else if (flags & F_SPARE_UP_ACTIVE)
                    enMousePointer = sIncPointer.get(ws::MP_ARROW_DOWN);
                else if (flags & F_SPARE_DOWN_ACTIVE)
                    enMousePointer = sIncPointer.get(ws::MP_ARROW_UP);
                else
                    enMousePointer = Widget::current_pointer();
            }
        }

        status_t ScrollBar::on_key_down(const ws::event_t *e)
        {
            switch (e->nCode)
            {
                case ws::WSK_CONTROL_L:
                case ws::WSK_CONTROL_R:
                    nKeys      |= ws::MCF_CONTROL;
                    break;
                case ws::WSK_SHIFT_L:
                case ws::WSK_SHIFT_R:
                    nKeys      |= ws::MCF_SHIFT;
                    break;
            }
            return STATUS_OK;
        }

        status_t ScrollBar::on_key_up(const ws::event_t *e)
        {
            switch (e->nCode)
            {
                case ws::WSK_CONTROL_L:
                case ws::WSK_CONTROL_R:
                    nKeys      &= ~ws::MCF_CONTROL;
                    break;
                case ws::WSK_SHIFT_L:
                case ws::WSK_SHIFT_R:
                    nKeys      &= ~ws::MCF_SHIFT;
                    break;
            }
            return STATUS_OK;
        }

        status_t ScrollBar::on_mouse_up(const ws::event_t *e)
        {
//            lsp_trace("nButtons = %d, code = %d", int(nButtons), int(e->nCode));
            nButtons   &= ~(1 << e->nCode);
            nKeys       = e->nState;
            if (nXFlags & F_OUTSIDE)
            {
                if (nButtons == 0)
                    nXFlags &= ~F_OUTSIDE;

                return STATUS_OK;
            }

            float value     = sValue.get();

            if (nXFlags & F_TRG_SLIDER_ACTIVE)
            {
                size_t key  = (nXFlags & F_PRECISION) ? ws::MCB_RIGHT : ws::MCB_LEFT;

                if (nButtons == 0) // All mouse buttons are released now
                {
                    nXFlags  &= ~(F_ALL_ACTIVITY_MASK | F_PRECISION);
                    value   = (e->nCode == key) ? fCurrValue : fLastValue;
                }
                else if (nButtons == size_t(1 << key)) // Currently pressed initially selected button
                {
                    nXFlags  = (nXFlags & (~F_ACTIVITY_MASK)) | ((nXFlags >> F_ACTIVITY_BITS) & F_ACTIVITY_MASK); // Restore activity state
                    value   = fCurrValue;
                }
                else
                {
                    nXFlags &= ~F_ACTIVITY_MASK; // Clear activity state
                    value   = fLastValue;
                }
            }
            else
            {
                if (nButtons == 0)
                {
                    cancel_timer();

                    nXFlags  &= ~F_ALL_ACTIVITY_MASK;
                    value = (e->nCode == ws::MCB_LEFT) ? fCurrValue : fLastValue;
                }
                else if (nButtons == ws::MCF_LEFT)
                {
                    size_t flags = check_mouse_over(e->nLeft, e->nTop);
                    size_t mask  = (nXFlags >> F_ACTIVITY_BITS) & F_ACTIVITY_MASK;

                    if (mask == flags)
                    {
                        nXFlags     |= flags;
                        value       = fCurrValue;

                        launch_timer();
                    }
                    else
                    {
                        nXFlags     &= ~F_ACTIVITY_MASK;
                        cancel_timer();
                    }
                }
            }

            // Update value
            if (nButtons == 0)
                update_cursor_state(e->nLeft, e->nTop, false);

            value = sValue.limit(value);
            if (value != sValue.get())
            {
                sValue.set(value);
                sSlots.execute(SLOT_CHANGE, this);
            }
            query_draw();

            return STATUS_OK;
        }

        status_t ScrollBar::on_mouse_move(const ws::event_t *e)
        {
            nKeys       = e->nState;
            if (nXFlags & F_OUTSIDE)
                return STATUS_OK;
            if (nButtons == 0)
            {
                update_cursor_state(e->nLeft, e->nTop, true);
                return STATUS_OK;
            }

            if (nXFlags & F_TRG_SLIDER_ACTIVE)
            {
                size_t key = (nXFlags & F_PRECISION) ? ws::MCB_RIGHT : ws::MCB_LEFT;
                if (nButtons != size_t(1 << key))
                    return STATUS_OK;

                // Different behaviour for slider
                ssize_t range   = (sOrientation.horizontal()) ?
                                    lsp_max(0, sSpareSpace.nWidth - sSlider.nWidth) :
                                    lsp_max(0, sSpareSpace.nHeight - sSlider.nHeight);

                if (range > 0)
                {
                    float value     = (sOrientation.horizontal()) ? e->nLeft : e->nTop;
                    float result    = fLastValue;
                    float delta     = sValue.range() * float(value - nLastV) / range; // normalized
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

                    result      = sValue.limit(result + delta*accel);
                    if (result != sValue.get())
                    {
                        fCurrValue  = result;
                        sValue.set(result);
                        sSlots.execute(SLOT_CHANGE, this);
                    }
                }
            }
            else
            {
                size_t flags = check_mouse_over(e->nLeft, e->nTop);

                if (nXFlags & (F_TRG_SPARE_UP_ACTIVE | F_TRG_SPARE_DOWN_ACTIVE))
                {
                    if (flags == 0)
                    {
                        if (nXFlags & F_ACTIVITY_MASK)
                        {
                            nXFlags &= ~F_ACTIVITY_MASK;
                            cancel_timer();
                        }
                    }
                    else
                    {
                        if ((nXFlags & F_ACTIVITY_MASK) != ((nXFlags >> F_ACTIVITY_BITS) & F_ACTIVITY_MASK))
                        {
                            nXFlags = (nXFlags & (~F_ACTIVITY_MASK)) | ((nXFlags >> F_ACTIVITY_BITS) & F_ACTIVITY_MASK);
                            launch_timer();
                        }
                    }
                }
                else
                {
                    size_t k = ((nXFlags >> F_ACTIVITY_BITS) & F_ACTIVITY_MASK);

                    if (k != flags)
                    {
                        if (nXFlags & F_ACTIVITY_MASK)
                        {
                            nXFlags &= (~F_ACTIVITY_MASK);
                            cancel_timer();
                        }
                    }
                    else
                    {
                        if ((nXFlags & F_ACTIVITY_MASK) != ((nXFlags >> F_ACTIVITY_BITS) & F_ACTIVITY_MASK))
                        {
                            nXFlags = (nXFlags & (~F_ACTIVITY_MASK)) | flags;
                            launch_timer();
                        }
                    }
                }

                query_draw();
            }

            return STATUS_OK;
        }

        status_t ScrollBar::on_mouse_scroll(const ws::event_t *e)
        {
            if (nXFlags & F_ALL_ACTIVITY_MASK)
                return STATUS_OK;

            float step      = sStep.get(e->nState & ws::MCF_CONTROL, e->nState & ws::MCF_SHIFT);
            float delta     = (e->nCode == ws::MCD_UP) ? -step : step;

            float old       = sValue.get();
            sValue.add(delta);
            if (old != sValue.get())
                sSlots.execute(SLOT_CHANGE, this);

            return STATUS_OK;
        }

        void ScrollBar::update_by_timer()
        {
            float value     = fCurrValue;
            float delta     = 0.0f;

            switch (nXFlags & F_ACTIVITY_MASK)
            {
                case F_BTN_UP_ACTIVE:
                    delta       = (nKeys & ws::MCF_SHIFT)   ? sStep.step_decel() :
                                  (nKeys & ws::MCF_CONTROL) ? sStep.step_accel() :
                                  sStep.get();
                    break;

                case F_BTN_DOWN_ACTIVE:
                    delta       = (nKeys & ws::MCF_SHIFT)   ? -sStep.step_decel() :
                                  (nKeys & ws::MCF_CONTROL) ? -sStep.step_accel() :
                                  -sStep.get();
                    break;

                case F_SPARE_UP_ACTIVE:
                    delta       = (nKeys & ws::MCF_SHIFT)   ? sAccelStep.step_decel() :
                                  (nKeys & ws::MCF_CONTROL) ? sAccelStep.step_accel() :
                                  sAccelStep.get();
                    break;

                case F_SPARE_DOWN_ACTIVE:
                    delta       = (nKeys & ws::MCF_SHIFT)   ? -sAccelStep.step_decel() :
                                  (nKeys & ws::MCF_CONTROL) ? -sAccelStep.step_accel() :
                                  -sAccelStep.get();
                    break;
            }

            value = sValue.limit(value + delta);
            if (value != sValue.get())
            {
                fCurrValue = value;
                sValue.set(value);
                sSlots.execute(SLOT_CHANGE, this);
            }
        }

        void ScrollBar::draw(ws::ISurface *s)
        {
            float scaling   = lsp_max(0.0f, sScaling.get());
            float bright    = sBrightness.get();
            ssize_t border  = (sBorderSize.get() > 0) ? lsp_max(1.0f, sBorderSize.get() * scaling) : 0;
            ssize_t radius  = (sBorderRadius.get() > 0) ? lsp_max(1.0f, sBorderRadius.get() * scaling) : 0;
            ssize_t gap     = (sBorderGap.get() > 0) ? lsp_max(1.0f, sBorderGap.get() * scaling) : 0;
            ssize_t sborder = (sSliderBorderSize.get() > 0) ? lsp_max(1.0f, sSliderBorderSize.get() * scaling) : 0;

            // Draw background
            lsp::Color color;
            get_actual_bg_color(color);
            s->clear(color);

            bool aa         = s->set_antialiasing(true);
            ws::rectangle_t xr;
            xr              = sSize;
            xr.nLeft       -= sSize.nLeft;
            xr.nTop        -= sSize.nTop;

            // Draw border
            if (border > 0)
            {
                color.copy(sBorderColor);
                color.scale_lch_luminance(bright);

                s->fill_round_rect(color, SURFMASK_ALL_CORNER, radius, &xr);
                xr.nLeft       += border;
                xr.nTop        += border;
                xr.nWidth      -= border*2;
                xr.nHeight     -= border*2;
                radius          = lsp_max(0, radius - 1);
            }

            // Draw border gap
            if (gap > 0)
            {
                color.copy(sBorderGapColor);
                color.scale_lch_luminance(bright);
                s->fill_round_rect(color, SURFMASK_ALL_CORNER, radius, &xr);
            }

            if (sOrientation.horizontal())
            {
                // Draw dec button
                xr              = sDecButton;
                xr.nLeft       -= sSize.nLeft;
                xr.nTop        -= sSize.nTop;
                color.copy((nXFlags & F_BTN_DOWN_ACTIVE) ? sButtonActiveColor : sButtonColor);
                color.scale_lch_luminance(bright);
                s->fill_round_rect(color, SURFMASK_L_CORNER, radius, &xr);

                color.copy((nXFlags & F_BTN_DOWN_ACTIVE) ? sTextActiveColor : sTextColor);
                s->fill_triangle(
                        xr.nLeft + xr.nWidth * 0.25f, xr.nTop + xr.nHeight * 0.5f,
                        xr.nLeft + xr.nWidth * 0.75f, xr.nTop + xr.nHeight * 0.25f,
                        xr.nLeft + xr.nWidth * 0.75f, xr.nTop + xr.nHeight * 0.75f,
                        color);

                // Draw inc button
                xr              = sIncButton;
                xr.nLeft       -= sSize.nLeft;
                xr.nTop        -= sSize.nTop;
                color.copy((nXFlags & F_BTN_UP_ACTIVE) ? sButtonActiveColor : sButtonColor);
                color.scale_lch_luminance(bright);
                s->fill_round_rect(color, SURFMASK_R_CORNER, radius, &xr);

                color.copy((nXFlags & F_BTN_UP_ACTIVE) ? sTextActiveColor : sTextColor);
                s->fill_triangle(
                        xr.nLeft + xr.nWidth * 0.75f, xr.nTop + xr.nHeight * 0.5f,
                        xr.nLeft + xr.nWidth * 0.25f, xr.nTop + xr.nHeight * 0.75f,
                        xr.nLeft + xr.nWidth * 0.25f, xr.nTop + xr.nHeight * 0.25f,
                        color);

                // Draw the dec spare
                xr.nLeft        = sSpareSpace.nLeft - sSize.nLeft;
                xr.nTop         = sSpareSpace.nTop  - sSize.nTop;
                xr.nWidth       = sSlider.nLeft - sSpareSpace.nLeft - gap;
                xr.nHeight      = sSpareSpace.nHeight;

                if (xr.nWidth > 0)
                {
                    color.copy((nXFlags & F_SPARE_DOWN_ACTIVE) ? sIncActiveColor : sIncColor);
                    color.scale_lch_luminance(bright);
                    s->fill_rect(color, &xr);
                }

                // Draw the inc spare
                xr.nLeft        = sSlider.nLeft - sSize.nLeft + sSlider.nWidth + gap;
                xr.nTop         = sSpareSpace.nTop  - sSize.nTop;
                xr.nWidth       = sSpareSpace.nLeft - sSize.nLeft + sSpareSpace.nWidth - xr.nLeft;
                xr.nHeight      = sSpareSpace.nHeight;

                if (xr.nWidth > 0)
                {
                    color.copy((nXFlags & F_SPARE_UP_ACTIVE) ? sDecActiveColor : sDecColor);
                    color.scale_lch_luminance(bright);
                    s->fill_rect(color, &xr);
                }
            }
            else
            {
                // Draw dec button
                xr              = sDecButton;
                xr.nLeft       -= sSize.nLeft;
                xr.nTop        -= sSize.nTop;
                color.copy((nXFlags & F_BTN_DOWN_ACTIVE) ? sButtonActiveColor : sButtonColor);
                color.scale_lch_luminance(bright);
                s->fill_round_rect(color, SURFMASK_T_CORNER, radius, &xr);

                color.copy((nXFlags & F_BTN_DOWN_ACTIVE) ? sTextActiveColor : sTextColor);
                s->fill_triangle(
                        xr.nLeft + xr.nWidth * 0.5f,  xr.nTop + xr.nHeight * 0.25f,
                        xr.nLeft + xr.nWidth * 0.75f, xr.nTop + xr.nHeight * 0.75f,
                        xr.nLeft + xr.nWidth * 0.25f, xr.nTop + xr.nHeight * 0.75f,
                        color);

                // Draw inc button
                xr              = sIncButton;
                xr.nLeft       -= sSize.nLeft;
                xr.nTop        -= sSize.nTop;
                color.copy((nXFlags & F_BTN_UP_ACTIVE) ? sButtonActiveColor : sButtonColor);
                color.scale_lch_luminance(bright);
                s->fill_round_rect(color, SURFMASK_B_CORNER, radius, &xr);

                color.copy((nXFlags & F_BTN_UP_ACTIVE) ? sTextActiveColor : sTextColor);
                s->fill_triangle(
                        xr.nLeft + xr.nWidth * 0.5f,  xr.nTop + xr.nHeight * 0.75f,
                        xr.nLeft + xr.nWidth * 0.25f, xr.nTop + xr.nHeight * 0.25f,
                        xr.nLeft + xr.nWidth * 0.75f, xr.nTop + xr.nHeight * 0.25f,
                        color);

                // Draw the dec spare
                xr.nLeft        = sSpareSpace.nLeft - sSize.nLeft;
                xr.nTop         = sSpareSpace.nTop  - sSize.nTop;
                xr.nWidth       = sSpareSpace.nWidth;
                xr.nHeight      = sSlider.nTop - sSpareSpace.nTop - gap;

                if (xr.nHeight > 0)
                {
                    color.copy((nXFlags & F_SPARE_DOWN_ACTIVE) ? sIncActiveColor : sIncColor);
                    color.scale_lch_luminance(bright);
                    s->fill_rect(color, &xr);
                }

                // Draw the inc spare
                xr.nLeft        = sSpareSpace.nLeft - sSize.nLeft;
                xr.nTop         = sSlider.nTop - sSize.nTop + sSlider.nHeight + gap;
                xr.nWidth       = sSpareSpace.nWidth;
                xr.nHeight      = sSpareSpace.nTop - sSize.nTop + sSpareSpace.nHeight - xr.nTop;

                if (xr.nHeight > 0)
                {
                    color.copy((nXFlags & F_SPARE_UP_ACTIVE) ? sDecActiveColor : sDecColor);
                    color.scale_lch_luminance(bright);
                    s->fill_rect(color, &xr);
                }
            }

            // Draw the slider
            xr.nLeft        = sSlider.nLeft - sSize.nLeft;
            xr.nTop         = sSlider.nTop  - sSize.nTop;
            xr.nWidth       = sSlider.nWidth;
            xr.nHeight      = sSlider.nHeight;

            if (sborder > 0)
            {
                color.copy(sSliderBorderColor);
                color.scale_lch_luminance(bright);
                s->fill_rect(color, &xr);

                xr.nLeft       += sborder;
                xr.nTop        += sborder;
                xr.nWidth      -= sborder * 2;
                xr.nHeight     -= sborder * 2;
            }

            color.copy((nXFlags & F_SLIDER_ACTIVE) ? sSliderActiveColor : sSliderColor);
            color.scale_lch_luminance(bright);
            s->fill_rect(color, &xr);

            s->set_antialiasing(aa);
        }

    } /* namespace tk */
} /* namespace lsp */
