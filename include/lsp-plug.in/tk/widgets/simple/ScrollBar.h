/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SCROLLBAR_H_
#define LSP_PLUG_IN_TK_WIDGETS_SCROLLBAR_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        // Style definition
        namespace style
        {
            LSP_TK_STYLE_DEF_BEGIN(ScrollBar, Widget)
                prop::RangeFloat        sValue;
                prop::StepFloat         sStep;
                prop::StepFloat         sAccelStep;
                prop::SizeConstraints   sConstraints;
                prop::Orientation       sOrientation;
                prop::Pointer           sSliderPointer;
                prop::Pointer           sIncPointer;
                prop::Pointer           sDecPointer;
                prop::Integer           sBorderRadius;
                prop::Integer           sBorderSize;
                prop::Integer           sBorderGap;
                prop::Integer           sSliderBorderSize;
                prop::Boolean           sInvertMouseHScroll;
                prop::Boolean           sInvertMouseVScroll;

                prop::Color             sButtonColor;
                prop::Color             sButtonActiveColor;
                prop::Color             sIncColor;
                prop::Color             sIncActiveColor;
                prop::Color             sDecColor;
                prop::Color             sDecActiveColor;
                prop::Color             sBorderColor;
                prop::Color             sBorderGapColor;
                prop::Color             sSliderColor;
                prop::Color             sSliderBorderColor;
                prop::Color             sSliderActiveColor;
                prop::Color             sTextColor;
                prop::Color             sTextActiveColor;
            LSP_TK_STYLE_DEF_END
        }

        /**
         * Scroll bar implementation
         */
        class ScrollBar: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum flags_t
                {
                    F_ACTIVITY_BITS         = 5,

                    F_BTN_UP_ACTIVE         = 1 << 0,
                    F_BTN_DOWN_ACTIVE       = 1 << 1,
                    F_SLIDER_ACTIVE         = 1 << 2,
                    F_SPARE_UP_ACTIVE       = 1 << 3,
                    F_SPARE_DOWN_ACTIVE     = 1 << 4,

                    F_TRG_BTN_UP_ACTIVE     = F_BTN_UP_ACTIVE << F_ACTIVITY_BITS,
                    F_TRG_BTN_DOWN_ACTIVE   = F_BTN_DOWN_ACTIVE << F_ACTIVITY_BITS,
                    F_TRG_SLIDER_ACTIVE     = F_SLIDER_ACTIVE << F_ACTIVITY_BITS,
                    F_TRG_SPARE_UP_ACTIVE   = F_SPARE_UP_ACTIVE << F_ACTIVITY_BITS,
                    F_TRG_SPARE_DOWN_ACTIVE = F_SPARE_DOWN_ACTIVE << F_ACTIVITY_BITS,

                    F_FILL                  = 1 << (F_ACTIVITY_BITS << 1),
                    F_OUTSIDE               = 1 << ((F_ACTIVITY_BITS << 1) + 1),
                    F_PRECISION             = 1 << ((F_ACTIVITY_BITS << 1) + 2),

                    F_ACTIVITY_MASK         = F_BTN_UP_ACTIVE | F_BTN_DOWN_ACTIVE | F_SLIDER_ACTIVE | F_SPARE_UP_ACTIVE | F_SPARE_DOWN_ACTIVE,
                    F_TRG_ACTIVITY_MASK     = F_ACTIVITY_MASK << F_ACTIVITY_BITS,
                    F_ALL_ACTIVITY_MASK     = F_ACTIVITY_MASK | F_TRG_ACTIVITY_MASK
                };

            protected:
                size_t                  nXFlags;
                size_t                  nButtons;
                size_t                  nKeys;
                ssize_t                 nLastV;
                float                   fLastValue;
                float                   fCurrValue;
                ws::rectangle_t         sIncButton;
                ws::rectangle_t         sDecButton;
                ws::rectangle_t         sSpareSpace;
                ws::rectangle_t         sSlider;

                prop::RangeFloat        sValue;
                prop::StepFloat         sStep;
                prop::StepFloat         sAccelStep;

                prop::SizeConstraints   sConstraints;
                prop::Orientation       sOrientation;
                prop::Pointer           sSliderPointer;
                prop::Pointer           sIncPointer;
                prop::Pointer           sDecPointer;
                prop::Integer           sBorderRadius;
                prop::Integer           sBorderSize;
                prop::Integer           sBorderGap;
                prop::Integer           sSliderBorderSize;
                prop::Boolean           sInvertMouseHScroll;
                prop::Boolean           sInvertMouseVScroll;

                prop::Color             sButtonColor;
                prop::Color             sButtonActiveColor;
                prop::Color             sIncColor;
                prop::Color             sIncActiveColor;
                prop::Color             sDecColor;
                prop::Color             sDecActiveColor;
                prop::Color             sBorderColor;
                prop::Color             sBorderGapColor;
                prop::Color             sSliderColor;
                prop::Color             sSliderBorderColor;
                prop::Color             sSliderActiveColor;
                prop::Color             sTextColor;
                prop::Color             sTextActiveColor;

                Timer                   sTimer;

            protected:
                size_t                          check_mouse_over(ssize_t x, ssize_t y);
                void                            do_destroy();
                void                            update_by_timer();
                void                            update_slider();
                void                            launch_timer();
                void                            cancel_timer();

            protected:
                static status_t                 slot_on_change(Widget *sender, void *ptr, void *data);
                static status_t                 slot_begin_edit(Widget *sender, void *ptr, void *data);
                static status_t                 slot_end_edit(Widget *sender, void *ptr, void *data);
                static status_t                 timer_handler(ws::timestamp_t sched, ws::timestamp_t time, void *arg);

            protected:
                virtual void                    size_request(ws::size_limit_t *r) override;
                virtual void                    property_changed(Property *prop) override;
                virtual void                    realize(const ws::rectangle_t *r) override;

            public:
                explicit ScrollBar(Display *dpy);
                ScrollBar(const ScrollBar &) = delete;
                ScrollBar(ScrollBar &&) = delete;
                virtual ~ScrollBar() override;

                ScrollBar & operator = (const ScrollBar &) = delete;
                ScrollBar & operator = (ScrollBar &&) = delete;

                virtual status_t                init() override;
                virtual void                    destroy() override;

            public:
                LSP_TK_PROPERTY(RangeFloat,         value,                  &sValue)
                LSP_TK_PROPERTY(StepFloat,          step,                   &sStep)
                LSP_TK_PROPERTY(StepFloat,          accel_step,             &sAccelStep)

                LSP_TK_PROPERTY(SizeConstraints,    constraints,            &sConstraints)
                LSP_TK_PROPERTY(Orientation,        orientation,            &sOrientation)

                LSP_TK_PROPERTY(Pointer,            slider_pointer,         &sSliderPointer)
                LSP_TK_PROPERTY(Pointer,            inc_pointer,            &sIncPointer)
                LSP_TK_PROPERTY(Pointer,            dec_pointer,            &sDecPointer)

                LSP_TK_PROPERTY(Integer,            border_radius,          &sBorderRadius)
                LSP_TK_PROPERTY(Integer,            border_size,            &sBorderSize)
                LSP_TK_PROPERTY(Integer,            border_gap_size,        &sBorderGap)
                LSP_TK_PROPERTY(Integer,            slider_border_size,     &sSliderBorderSize)

                LSP_TK_PROPERTY(Boolean,            invert_mouse_hscroll,   &sInvertMouseHScroll)
                LSP_TK_PROPERTY(Boolean,            invert_mouse_vscroll,   &sInvertMouseVScroll)

                LSP_TK_PROPERTY(Color,              button_color,           &sButtonColor)
                LSP_TK_PROPERTY(Color,              button_active_color,    &sButtonActiveColor)
                LSP_TK_PROPERTY(Color,              inc_color,              &sIncColor)
                LSP_TK_PROPERTY(Color,              inc_active_color,       &sIncActiveColor)
                LSP_TK_PROPERTY(Color,              dec_color,              &sDecColor)
                LSP_TK_PROPERTY(Color,              dec_active_color,       &sDecActiveColor)
                LSP_TK_PROPERTY(Color,              slider_color,           &sSliderColor)
                LSP_TK_PROPERTY(Color,              slider_border_color,    &sSliderBorderColor)
                LSP_TK_PROPERTY(Color,              slider_active_color,    &sSliderActiveColor)
                LSP_TK_PROPERTY(Color,              border_color,           &sBorderColor)
                LSP_TK_PROPERTY(Color,              border_gap_color,       &sBorderGapColor)
                LSP_TK_PROPERTY(Color,              text_color,             &sTextColor)
                LSP_TK_PROPERTY(Color,              text_active_color,      &sTextActiveColor)

            public:
                virtual status_t                on_mouse_down(const ws::event_t *e) override;
                virtual status_t                on_mouse_up(const ws::event_t *e) override;
                virtual status_t                on_key_down(const ws::event_t *e) override;
                virtual status_t                on_key_up(const ws::event_t *e) override;
                virtual status_t                on_mouse_move(const ws::event_t *e) override;
                virtual status_t                on_mouse_scroll(const ws::event_t *e) override;
                virtual status_t                on_mouse_pointer(pointer_event_t *e) override;
                virtual void                    draw(ws::ISurface *s) override;

            public:
                virtual status_t                on_begin_edit();
                virtual status_t                on_change();
                virtual status_t                on_end_edit();
        };

    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SCROLLBAR_H_ */
