/*
 * ScrollBar.cpp
 *
 *  Created on: 3 авг. 2017 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace tk
    {
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
            sSliderActiveColor(&sProperties)
        {
            nFlags              = 0;
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
            do_destroy();
        }

        void ScrollBar::do_destroy()
        {
            sTimer.cancel();
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
            sConstraints.bind("size", &sStyle);
            sOrientation.bind("orientation", &sStyle);
            sSliderPointer.bind("slider.pointer", &sStyle);
            sIncPointer.bind("inc.pointer", &sStyle);
            sDecPointer.bind("dec.pointer", &sStyle);
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

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sValue.init(sclass, 0.5f);
                sStep.init(sclass, 0.01);
                sAccelStep.init(sclass, 0.05);
                sConstraints.init(sclass, -1, -1, 16, -1);
                sOrientation.init(sclass, O_HORIZONTAL);
                sSliderPointer.init(sclass, ws::MP_DEFAULT);
                sIncPointer.init(sclass, ws::MP_DEFAULT);
                sDecPointer.init(sclass, ws::MP_DEFAULT);
                sBorderSize.init(sclass, 1);
                sBorderGap.init(sclass, 1);
                sSliderBorderSize.init(sclass, 1);
                sButtonColor.init(sclass, "#cccccc");
                sButtonActiveColor.init(sclass, "#ffffff");
                sIncColor.init(sclass, "#cccccc");
                sIncActiveColor.init(sclass, "#ffffff");
                sDecColor.init(sclass, "#cccccc");
                sDecActiveColor.init(sclass, "#ffffff");
                sBorderColor.init(sclass, "#000000");
                sBorderGapColor.init(sclass, "#cccccc");
                sSliderColor.init(sclass, "#cccccc");
                sSliderBorderColor.init(sclass, "#000000");
                sSliderActiveColor.init(sclass, "#ffffff");
            }

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

        void ScrollBar::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (sValue.is(prop))
            {
                update_slider();
                sSlots.execute(SLOT_CHANGE, this);
            }
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
            size_t gap      = (sBorderGap.get() > 0) ? lsp_max(1.0f, sBorderGap.get()) : 0;
            size_t sborder  = (sSliderBorderSize.get() > 0) ? lsp_max(1.0f, sSliderBorderSize.get() * scaling) : 0;

            size_t ssize    = lsp_max(4.0f, 4.0f * scaling);    // Minimum slider/button size

            // Estimate minimum size for vertical scroll bar
            r->nMinWidth    = lsp_max(radius * 2, ssize + (sborder + gap + border)*2);
            r->nMinHeight   = lsp_max(radius * 2, ssize*5 + gap*4 + (border + sborder)*2);
            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;

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
            ssize_t radius  = (sBorderRadius.get() > 0) ? lsp_max(1.0f, sBorderRadius.get() * scaling) : 0;
            ssize_t gap     = (sBorderGap.get() > 0) ? lsp_max(1.0f, sBorderGap.get()) : 0;
            ssize_t sborder = (sSliderBorderSize.get() > 0) ? lsp_max(1.0f, sSliderBorderSize.get() * scaling) : 0;

            // Compute parameters of each element
            if (sOrientation.horizontal())
            {
                ssize_t req             = (r->nWidth - gap*4 + (border + sborder)*2) / 5;

                sDecButton.nHeight      = r->nHeight - border;
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

                sDecButton.nWidth       = r->nWidth  - border;
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
            size_t ssize    = lsp_max(4.0f, 4.0f * scaling) + sborder*2;    // Minimum slider/button size
            float range     = sValue.abs_range();

            if (sOrientation.horizontal())
            {
                if (range > 0.0f)
                    ssize           = lsp_max((sSpareSpace.nWidth * sStep.get()) / range, ssize);
                ssize_t left        = sSpareSpace.nHeight - ssize;

                sSlider.nLeft       = sSpareSpace.nLeft + left * sValue.get_normalized();
                sSlider.nTop        = sSpareSpace.nTop;
                sSlider.nWidth      = ssize;
                sSlider.nHeight     = sSpareSpace.nHeight;
            }
            else
            {
                if (range > 0.0f)
                    ssize           = lsp_max((sSpareSpace.nHeight * sStep.get()) / range, ssize);
                ssize_t left        = sSpareSpace.nHeight - ssize;

                sSlider.nLeft       = sSpareSpace.nLeft;
                sSlider.nTop        = sSpareSpace.nTop + left * sValue.get_normalized();
                sSlider.nWidth      = sSpareSpace.nWidth;
                sSlider.nHeight     = ssize;
            }

            query_draw();
        }

        status_t ScrollBar::timer_handler(ws::timestamp_t time, void *arg)
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
            return 0;
// TODO
//            float value     = get_normalized_value();
//
//            realize_t r     = sSize;
//            ssize_t wsize   = nSize + 1;
//
//            if (enOrientation == O_VERTICAL) // Vertical
//            {
//                // Update dimensions
//                if (!(nFlags & F_FILL))
//                {
//                    r.nLeft += (r.nWidth - nSize) >> 1;
//                    r.nWidth = nSize;
//                }
//                r.nHeight   --;
//            }
//            else
//            {
//                // Update dimensions
//                if (!(nFlags & F_FILL))
//                {
//                    r.nTop     += (r.nHeight - nSize) >> 1;
//                    r.nHeight   = nSize;
//                }
//                r.nWidth    --;
//            }
//
//            // Check overall coordinates
//            if ((x < r.nLeft) ||
//                (x > (r.nLeft + r.nWidth)) ||
//                (y < r.nTop) ||
//                (y > (r.nTop + r.nHeight)))
//                return 0;
//
//            if (enOrientation == O_VERTICAL) // Vertical
//            {
//                y -= r.nTop;
//                if (y < wsize)
//                    return F_BTN_DOWN_ACTIVE;
//                y -= wsize;
//
//                ssize_t spare_space     = r.nHeight - (wsize << 1) - wsize;
//                ssize_t spare_up_size   = spare_space * value;
//                ssize_t spare_down_size = spare_space * (1.0f - value);
//
//                if (y < spare_up_size)
//                    return F_SPARE_DOWN_ACTIVE;
//                y -= spare_up_size;
//
//                if (y < wsize)
//                    return F_SLIDER_ACTIVE;
//                y -= wsize;
//
//                return (y < spare_down_size) ? F_SPARE_UP_ACTIVE : F_BTN_UP_ACTIVE;
//            }
//            else
//            {
//                x -= r.nLeft;
//                if (x < wsize)
//                    return F_BTN_DOWN_ACTIVE;
//                x -= wsize;
//
//                ssize_t spare_space     = r.nWidth - (wsize << 1) - wsize;
//                ssize_t spare_down_size = spare_space * value;
//                ssize_t spare_up_size   = spare_space * (1.0f - value);
//
//                if (x < spare_down_size)
//                    return F_SPARE_DOWN_ACTIVE;
//                x -= spare_down_size;
//
//                if (x < wsize)
//                    return F_SLIDER_ACTIVE;
//                x -= wsize;
//
//                return (x < spare_up_size) ? F_SPARE_UP_ACTIVE : F_BTN_UP_ACTIVE;
//            }
        }

        status_t ScrollBar::on_change()
        {
            return STATUS_OK;
        }

        status_t ScrollBar::on_mouse_down(const ws::event_t *e)
        {
            take_focus();

//            lsp_trace("nButtons = %d, code = %d", int(nButtons), int(e->nCode));
            if (nButtons == 0)
            {
                // Update state of buttons
                nButtons   |= (1 << e->nCode);

                // Check that we first hit inside the bar
                size_t flags = check_mouse_over(e->nLeft, e->nTop);
                update_cursor_state(e->nLeft, e->nTop, true);

                if (flags == 0)
                {
                    nFlags         |= F_OUTSIDE;
                    return STATUS_OK;
                }

                // What button was pressed?
                if (e->nCode == ws::MCB_LEFT)
                {
                    nFlags      = flags | (flags << F_ACTIVITY_BITS);

                    if (flags != F_SLIDER_ACTIVE)
                    {
                        lsp_trace("launch timer");
                        sTimer.launch(0, 100);
                    }
                }
                else if (e->nCode == ws::MCB_RIGHT)
                {
                    // Only slider allows right button
                    if (flags != F_SLIDER_ACTIVE)
                    {
                        nFlags         |= F_OUTSIDE;
                        return STATUS_OK;
                    }

                    // Slider with precision option
                    nFlags      = flags | (flags << F_ACTIVITY_BITS) | F_PRECISION;
                }
                else
                {
                    nFlags         |= F_OUTSIDE;
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
                if (nFlags & F_OUTSIDE)
                    return STATUS_OK;

                float value = sValue.get();

                if (nFlags & F_TRG_SLIDER_ACTIVE) // Slider
                {
                    size_t mask = (nFlags & F_PRECISION) ? ws::MCF_LEFT : ws::MCF_RIGHT;

                    if (nButtons == mask)
                    {
                        nFlags  = (nFlags & (~F_ACTIVITY_MASK)) | ((nFlags >> F_ACTIVITY_BITS) & F_ACTIVITY_MASK); // Restore activity state
                        value   = fCurrValue;
                    }
                    else
                    {
                        nFlags &= ~F_ACTIVITY_MASK; // Clear activity state
                        value   = fLastValue;
                    }
                }
                else // Not slider
                {
                    if (nButtons == ws::MCF_LEFT)
                    {
                        nFlags  = (nFlags & (~F_ACTIVITY_MASK)) | ((nFlags >> F_ACTIVITY_BITS) & F_ACTIVITY_MASK); // Restore activity state
                        value   = fCurrValue;

                        lsp_trace("launch timer");
                        sTimer.launch(0, 100);
                    }
                    else
                    {
                        lsp_trace("cancel timer");
                        sTimer.cancel();

                        nFlags &= ~F_ACTIVITY_MASK; // Clear activity state
                        value   = fLastValue;
                    }
                }

                // Update value
                value   = sValue.limit(value);
                if (value != sValue.set(value))
                    sSlots.execute(SLOT_CHANGE, this);
            }

            query_draw();

            return STATUS_OK;
        }

        void ScrollBar::update_cursor_state(ssize_t x, ssize_t y, bool set)
        {
            size_t flags = (set) ? check_mouse_over(x,y) : 0;
            if (flags & F_SLIDER_ACTIVE)
                enMousePointer = (sOrientation.vertical()) ? ws::MP_VSIZE : ws::MP_HSIZE;
            else
                enMousePointer = pointer()->get();
        }

        status_t ScrollBar::on_mouse_up(const ws::event_t *e)
        {
//            lsp_trace("nButtons = %d, code = %d", int(nButtons), int(e->nCode));
            nButtons   &= ~(1 << e->nCode);
            if (nFlags & F_OUTSIDE)
            {
                if (nButtons == 0)
                    nFlags &= ~F_OUTSIDE;

                return STATUS_OK;
            }

            float value     = sValue.get();

            if (nFlags & F_TRG_SLIDER_ACTIVE)
            {
                size_t key  = (nFlags & F_PRECISION) ? ws::MCB_RIGHT : ws::MCB_LEFT;

                if (nButtons == 0) // All mouse buttons are released now
                {
                    nFlags  &= ~(F_ALL_ACTIVITY_MASK | F_PRECISION);
                    value = (e->nCode == key) ? fCurrValue : fLastValue;
                }
                else if (nButtons == size_t(1 << key)) // Currently pressed initially selected button
                {
                    nFlags  = (nFlags & (~F_ACTIVITY_MASK)) | ((nFlags >> F_ACTIVITY_BITS) & F_ACTIVITY_MASK); // Restore activity state
                    value = fCurrValue;
                }
                else
                {
                    nFlags &= ~F_ACTIVITY_MASK; // Clear activity state
                    value   = fLastValue;
                }
            }
            else
            {
                if (nButtons == 0)
                {
                    lsp_trace("cancel timer");
                    sTimer.cancel();

                    nFlags  &= ~F_ALL_ACTIVITY_MASK;
                    value = (e->nCode == ws::MCB_LEFT) ? fCurrValue : fLastValue;
                }
                else if (nButtons == ws::MCF_LEFT)
                {
                    size_t flags = check_mouse_over(e->nLeft, e->nTop);
                    size_t mask  = (nFlags >> F_ACTIVITY_BITS) & F_ACTIVITY_MASK;

                    if (mask == flags)
                    {
                        nFlags     |= flags;
                        value       = fCurrValue;

                        lsp_trace("launch timer");
                        sTimer.launch(0, 100);
                    }
                    else
                    {
                        nFlags     &= ~F_ACTIVITY_MASK;
                        lsp_trace("cancel timer");
                        sTimer.cancel();
                    }
                }
            }

            // Update value
            value   = sValue.limit(value);
            query_draw();

            if (nButtons == 0)
                update_cursor_state(e->nLeft, e->nTop, false);

            if (value != sValue.set(value))
                sSlots.execute(SLOT_CHANGE, this);

            return STATUS_OK;
        }

        status_t ScrollBar::on_mouse_move(const ws::event_t *e)
        {
            if (nFlags & F_OUTSIDE)
                return STATUS_OK;
            if (nButtons == 0)
            {
                update_cursor_state(e->nLeft, e->nTop, true);
                return STATUS_OK;
            }

            if (nFlags & F_TRG_SLIDER_ACTIVE)
            {
                size_t key = (nFlags & F_PRECISION) ? ws::MCB_RIGHT : ws::MCB_LEFT;
                if (nButtons != size_t(1 << key))
                    return STATUS_OK;

                // Different behaviour for slider
                ssize_t value = (sOrientation.vertical()) ? e->nTop : e->nLeft;
                float result  = fLastValue;
//                if (value != nLastV)
//                {
//                    ssize_t range = (sOrientation.vertical()) ? sSize.nHeight : sSize.nWidth;
//                    ssize_t spare_space = range - ((nSize + 1) << 1) - nSize - 2;
//
//                    float delta   = (fMax - fMin) * float(value - nLastV) / float(spare_space);
//                    if (nFlags & F_PRECISION)
//                        delta       *= 0.1f;
//                    result        = sValue.limit(result + delta);
//                }

                if (fCurrValue != result)
                {
                    lsp_trace("set value to %f", result);
                    fCurrValue  = result;
                    sValue.set(value);
                    query_draw();

                    sSlots.execute(SLOT_CHANGE, this);
                }
            }
            else
            {
                size_t flags = check_mouse_over(e->nLeft, e->nTop);

                if (nFlags & (F_TRG_SPARE_UP_ACTIVE | F_TRG_SPARE_DOWN_ACTIVE))
                {
                    if (flags == 0)
                    {
                        if (nFlags & F_ACTIVITY_MASK)
                        {
                            nFlags &= ~F_ACTIVITY_MASK;
                            lsp_trace("cancel timer");
                            sTimer.cancel();
                        }
                    }
                    else
                    {
                        if ((nFlags & F_ACTIVITY_MASK) != ((nFlags >> F_ACTIVITY_BITS) & F_ACTIVITY_MASK))
                        {
                            nFlags = (nFlags & (~F_ACTIVITY_MASK)) | ((nFlags >> F_ACTIVITY_BITS) & F_ACTIVITY_MASK);
                            lsp_trace("launch timer");
                            sTimer.launch(0, 100);
                        }
                    }
                }
                else
                {
                    size_t k = ((nFlags >> F_ACTIVITY_BITS) & F_ACTIVITY_MASK);

                    if (k != flags)
                    {
                        if (nFlags & F_ACTIVITY_MASK)
                        {
                            nFlags &= (~F_ACTIVITY_MASK);
                            lsp_trace("cancel timer");
                            sTimer.cancel();
                        }
                    }
                    else
                    {
                        if ((nFlags & F_ACTIVITY_MASK) != ((nFlags >> F_ACTIVITY_BITS) & F_ACTIVITY_MASK))
                        {
                            nFlags = (nFlags & (~F_ACTIVITY_MASK)) | flags;
                            lsp_trace("launch timer");
                            sTimer.launch(0, 100);
                        }
                    }
                }

                query_draw();
            }

            return STATUS_OK;
        }

        status_t ScrollBar::on_mouse_scroll(const ws::event_t *e)
        {
            if (nFlags & F_ALL_ACTIVITY_MASK)
                return STATUS_OK;

            float step      = (e->nState & ws::MCF_SHIFT) ? sStep.step_decel() :
                              (e->nState & ws::MCF_CONTROL) ? sStep.step_accel() :
                              sStep.get();
            float delta     = (e->nCode == ws::MCD_UP) ? -step : step;
            float result    = sValue.limit(sValue.get() + delta);

            if (result != sValue.set(result))
            {
                query_draw();
                sSlots.execute(SLOT_CHANGE, this);
            }

            return STATUS_OK;
        }

        void ScrollBar::update_by_timer()
        {
            float value     = fCurrValue;
            float delta     = 0.0f;

            switch (nFlags & F_ACTIVITY_MASK)
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

            value   = sValue.limit(value + delta);
            if (value != sValue.set(value))
            {
                lsp_trace("set value to %f", value);
                fCurrValue  = value;
                query_draw();

                sSlots.execute(SLOT_CHANGE, this);
            }
        }

        void ScrollBar::draw(ws::ISurface *s)
        {
// TODO
//            // Prepare palette
//            Color bg_color(sBgColor);
//            Color color(sColor);
//            Color quarter(sSelColor, 0.25f);
//            Color half(sSelColor, 0.5f);
//
//            color.scale_lightness(brightness());
//            quarter.scale_lightness(brightness());
//            half.scale_lightness(brightness());
//
//            // Draw background
//            s->fill_rect(0, 0, sSize.nWidth, sSize.nHeight, bg_color);
//
//            float value     = get_normalized_value();
//            float aa        = s->set_antialiasing(true);
//
//            realize_t r     = sSize;
//            r.nLeft         = 0;
//            r.nTop          = 0;
//            ssize_t size3   = nSize/3;
//            ssize_t w       = nSize + 1;
//
//            if (enOrientation == O_VERTICAL) // Vertical
//            {
//                // Update dimensions
//                if (!(nFlags & F_FILL))
//                {
//                    r.nLeft += (r.nWidth - nSize) >> 1;
//                    r.nWidth = nSize;
//                }
//                r.nHeight   --;
//
//                // Draw button up
//                if (nFlags & F_BTN_UP_ACTIVE)
//                {
//                    float top = r.nTop + r.nHeight - nSize + 0.5f;
//                    s->fill_round_rect(r.nLeft, r.nTop + r.nHeight - nSize + 1, r.nWidth + 1, nSize - 1, 3.0f, SURFMASK_B_CORNER, half);
//                    s->line(r.nLeft + 0.5f, top, r.nLeft + r.nWidth + 0.5f, top, 1.0f, color);
//                    s->fill_triangle(
//                            r.nLeft + 0.2f*w, r.nTop + r.nHeight + 1 - size3*2,
//                            r.nLeft + 0.5f*w, r.nTop + r.nHeight + 1 - size3,
//                            r.nLeft + 0.8f*w, r.nTop + r.nHeight + 1 - size3*2,
//                            color);
//                }
//                else
//                {
//                    s->fill_round_rect(r.nLeft, r.nTop + r.nHeight - nSize, r.nWidth + 1, nSize, 3.0f, SURFMASK_B_CORNER, color);
//                    s->fill_triangle(
//                            r.nLeft + 0.2f*w, r.nTop + r.nHeight + 1 - size3*2,
//                            r.nLeft + 0.5f*w, r.nTop + r.nHeight + 1 - size3,
//                            r.nLeft + 0.8f*w, r.nTop + r.nHeight + 1 - size3*2,
//                            bg_color);
//                }
//
//                // Draw button down
//                if (nFlags & F_BTN_DOWN_ACTIVE)
//                {
//                    float top = r.nTop + nSize + 0.5f;
////                    s->fill_rect(r.nLeft + 2, 2, nSize - 3, nSize - 3, sColor);
//                    s->fill_round_rect(r.nLeft, r.nTop+1, r.nWidth + 1, nSize-1, 3.0f, SURFMASK_T_CORNER, half);
//                    s->line(r.nLeft + 0.5f, top, r.nLeft + r.nWidth + 0.5f, top, 1.0f, color);
//                    s->fill_triangle(
//                            r.nLeft + 0.2f*w, r.nTop + size3*2,
//                            r.nLeft + 0.5f*w, r.nTop + size3,
//                            r.nLeft + 0.8f*w, r.nTop + size3*2,
//                            color);
//                }
//                else
//                {
//                    s->fill_round_rect(r.nLeft, r.nTop+1, r.nWidth + 1, nSize, 3.0f, SURFMASK_T_CORNER, color);
//                    s->fill_triangle(
//                            r.nLeft + 0.2f*w, r.nTop + size3*2,
//                            r.nLeft + 0.5f*w, r.nTop + size3,
//                            r.nLeft + 0.8f*w, r.nTop + size3*2,
//                            bg_color);
//                }
//
//                ssize_t spare_space     = r.nHeight - ((nSize + 1) << 1) - nSize - 1;
//                ssize_t spare_up_size   = spare_space * value;
//                ssize_t spare_down_size = spare_space * (1.0f - value);
//
//                // Draw slider
//                if (nFlags & F_SLIDER_ACTIVE)
//                {
//                    s->wire_rect(r.nLeft + 2.5f, r.nTop + nSize + spare_up_size + 2.5f, r.nWidth - 4, nSize - 1, 1.0f, color);
//                    s->fill_rect(r.nLeft + 3, r.nTop + nSize + spare_up_size + 3, r.nWidth - 5, nSize - 2, quarter);
//                }
//                else
//                    s->fill_rect(r.nLeft + 2, r.nTop + nSize + spare_up_size + 2, r.nWidth - 3, nSize, color);
//
//                // Draw spares
//                if ((nFlags & F_SPARE_UP_ACTIVE) && (spare_down_size > 0))
//                    s->fill_rect(r.nLeft + 2, r.nTop + spare_up_size + nSize*2 + 3, r.nWidth - 3, spare_down_size, quarter);
//
//                if ((nFlags & F_SPARE_DOWN_ACTIVE) && (spare_up_size > 1))
//                    s->fill_rect(r.nLeft + 2, r.nTop + nSize + 2, r.nWidth - 3, spare_up_size - 1, quarter);
//
//                // Draw binding
//                s->wire_round_rect(r.nLeft + 0.5f, r.nTop + 0.5f, r.nWidth, r.nHeight, 3, SURFMASK_ALL_CORNER, 1.0f, color);
//            }
//            else // Horizontal
//            {
//                // Update dimensions
//                if (!(nFlags & F_FILL))
//                {
//                    r.nTop     += (r.nHeight - nSize) >> 1;
//                    r.nHeight   = nSize;
//                }
//                r.nWidth    --;
//
//                // Draw button up
//                if (nFlags & F_BTN_UP_ACTIVE)
//                {
//                    float left = r.nLeft + r.nWidth - nSize + 0.5f;
//                    s->fill_round_rect(r.nLeft + r.nWidth - nSize + 1, r.nTop+1, nSize, r.nHeight, 3.0f, SURFMASK_R_CORNER, half);
//                    s->line(left, r.nTop + 0.5f, left, r.nTop + r.nWidth + 0.5f, 1.0f, color);
//                    s->fill_triangle(
//                            r.nLeft + r.nWidth + 1 - size3*2, r.nTop + 0.2f*w,
//                            r.nLeft + r.nWidth + 1 - size3,   r.nTop + 0.5f*w,
//                            r.nLeft + r.nWidth + 1 - size3*2, r.nTop + 0.8f*w,
//                            color);
//                }
//                else
//                {
//                    s->fill_round_rect(r.nLeft + r.nWidth - nSize, r.nTop+1, nSize+1, r.nHeight, 3.0f, SURFMASK_R_CORNER, color);
//                    s->fill_triangle(
//                            r.nLeft + r.nWidth + 1 - size3*2, r.nTop + 0.2f*w,
//                            r.nLeft + r.nWidth + 1 - size3,   r.nTop + 0.5f*w,
//                            r.nLeft + r.nWidth + 1 - size3*2, r.nTop + 0.8f*w,
//                            bg_color);
//                }
//
//                // Draw button down
//                if (nFlags & F_BTN_DOWN_ACTIVE)
//                {
//                    float left = r.nLeft + nSize + 0.5f;
//                    s->fill_round_rect(r.nLeft, r.nTop, nSize, r.nHeight, 3.0f, SURFMASK_L_CORNER, half);
//                    s->line(left, r.nTop + 0.5f, left, r.nLeft + r.nHeight + 0.5f, 1.0f, color);
//                    s->fill_triangle(
//                            r.nLeft + size3*2, r.nTop + 0.2f*w,
//                            r.nLeft + size3,   r.nTop + 0.5f*w,
//                            r.nLeft + size3*2, r.nTop + 0.8f*w,
//                            color);
//                }
//                else
//                {
//                    s->fill_round_rect(r.nLeft+1, r.nTop, nSize, r.nHeight, 3.0f, SURFMASK_L_CORNER, color);
//                    s->fill_triangle(
//                            r.nLeft + size3*2, r.nTop + 0.2f*w,
//                            r.nLeft + size3,   r.nTop + 0.5f*w,
//                            r.nLeft + size3*2, r.nTop + 0.8f*w,
//                            bg_color);
//                }
//
//                ssize_t spare_space     = r.nWidth - ((nSize + 1) << 1) - nSize - 1;
//                ssize_t spare_down_size = spare_space * value;
//                ssize_t spare_up_size   = spare_space * (1.0f - value);
//
//                // Draw slider
//                if (nFlags & F_SLIDER_ACTIVE)
//                {
//                    s->wire_rect(r.nLeft + nSize + spare_down_size + 2.5f, r.nTop + 2.5f, nSize - 1, r.nHeight - 4, 1.0f, color);
//                    s->fill_rect(r.nLeft + nSize + spare_down_size + 3, r.nTop + 3, nSize - 2, r.nHeight - 5, quarter);
//                }
//                else
//                    s->fill_rect(r.nLeft + nSize + spare_down_size + 2, r.nTop + 2, nSize, r.nHeight - 3, color);
//
//                // Draw spares
//                if ((nFlags & F_SPARE_UP_ACTIVE) && (spare_up_size > 0))
//                    s->fill_rect(r.nLeft + spare_down_size + nSize*2 + 3, r.nTop + 2, spare_up_size, r.nHeight - 3, quarter);
//
//                if ((nFlags & F_SPARE_DOWN_ACTIVE) && (spare_down_size > 1))
//                    s->fill_rect(r.nLeft + nSize + 2, r.nTop + 2, spare_down_size - 1, r.nHeight - 3, quarter);
//
//                // Draw binding
//                s->wire_round_rect(r.nLeft + 0.5f, r.nTop + 0.5f, r.nWidth, r.nHeight, 3, SURFMASK_ALL_CORNER, 1.0f, color);
//            }
//
//            s->set_antialiasing(aa);
        }

    } /* namespace tk */
} /* namespace lsp */
