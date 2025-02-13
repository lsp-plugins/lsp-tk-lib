/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 10 июл. 2017 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_KNOB_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_KNOB_H_

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
            typedef struct KnobColors
            {
                prop::Color         sColor;
                prop::Color         sScaleColor;
                prop::Color         sBalanceColor;
                prop::Color         sTipColor;
                prop::Color         sBalanceTipColor;
                prop::Color         sMeterColor;

                void listener(tk::prop::Listener *listener);
                bool property_changed(Property *prop);
            } KnobColors;

            enum KnobColorState
            {
                KNOB_NORMAL         = 0,
                KNOB_INACTIVE       = 1 << 0,

                KNOB_TOTAL          = 1 << 1
            };

            LSP_TK_STYLE_DEF_BEGIN(Knob, Widget)
                KnobColors          vColors[KNOB_TOTAL];
                prop::Color         sHoleColor;

                prop::SizeRange     sSizeRange;
                prop::Float         sScale;
                prop::RangeFloat    sValue;
                prop::StepFloat     sStep;
                prop::Float         sBalance;
                prop::Float         sMeterMin;
                prop::Float         sMeterMax;
                prop::Boolean       sCycling;
                prop::Boolean       sScaleMarks;
                prop::Boolean       sBalanceColorCustom;
                prop::Boolean       sFlat;
                prop::Boolean       sScaleActive;
                prop::Boolean       sMeterActive;
                prop::Boolean       sEditable;
                prop::Boolean       sActive;
                prop::Integer       sHoleSize;
                prop::Integer       sGapSize;
                prop::Float         sScaleBrightness;
                prop::Integer       sBalanceTipSize;
                prop::Boolean       sBalanceTipColorCustom;
                prop::Boolean       sInvertMouseVScroll;
            LSP_TK_STYLE_DEF_END
        }

        class Knob: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum state_t
                {
                    S_NONE,
                    S_MOVING,
                    S_CLICK
                };

                enum knob_flags_t
                {
                    KNOB_0      = style::KNOB_NORMAL,
                    KNOB_1      = style::KNOB_INACTIVE,
                    KNOB_TOTAL  = style::KNOB_TOTAL
                };

            protected:
                ssize_t             nLastY;
                size_t              nState;
                size_t              nButtons;

                style::KnobColors   vColors[style::KNOB_TOTAL];
                prop::Color         sHoleColor;

                prop::SizeRange     sSizeRange;
                prop::Float         sScale;
                prop::RangeFloat    sValue;
                prop::StepFloat     sStep;
                prop::Float         sBalance;
                prop::Float         sMeterMin;
                prop::Float         sMeterMax;
                prop::Boolean       sCycling;
                prop::Boolean       sScaleMarks;
                prop::Boolean       sBalanceColorCustom;
                prop::Boolean       sFlat;
                prop::Boolean       sScaleActive;
                prop::Boolean       sMeterActive;
                prop::Boolean       sEditable;
                prop::Boolean       sActive;
                prop::Integer       sHoleSize;
                prop::Integer       sGapSize;
                prop::Float         sScaleBrightness;
                prop::Integer       sBalanceTipSize;
                prop::Boolean       sBalanceTipColorCustom;
                prop::Boolean       sInvertMouseVScroll;

            protected:
                size_t                          check_mouse_over(ssize_t x, ssize_t y);
                void                            update_value(float delta);
                void                            on_click(ssize_t x, ssize_t y);
                style::KnobColors              *select_colors();

            protected:
                static status_t                 slot_begin_edit(Widget *sender, void *ptr, void *data);
                static status_t                 slot_on_change(Widget *sender, void *ptr, void *data);
                static status_t                 slot_end_edit(Widget *sender, void *ptr, void *data);

            protected:
                virtual void                    size_request(ws::size_limit_t *r) override;
                virtual void                    property_changed(Property *prop) override;

            public:
                explicit Knob(Display *dpy);
                Knob(const Knob &) = delete;
                Knob(Knob &&) = delete;
                virtual ~Knob() override;

                Knob & operator = (const Knob &) = delete;
                Knob & operator = (Knob &&) = delete;

                virtual status_t                init() override;

            public:
                LSP_TK_PROPERTY(Color,              color,                      &vColors[KNOB_0].sColor)
                LSP_TK_PROPERTY(Color,              scale_color,                &vColors[KNOB_0].sScaleColor)
                LSP_TK_PROPERTY(Color,              balance_color,              &vColors[KNOB_0].sBalanceColor)
                LSP_TK_PROPERTY(Color,              tip_color,                  &vColors[KNOB_0].sTipColor)
                LSP_TK_PROPERTY(Color,              balance_tip_color,          &vColors[KNOB_0].sBalanceTipColor)
                LSP_TK_PROPERTY(Color,              meter_color,                &vColors[KNOB_0].sMeterColor)

                LSP_TK_PROPERTY(Color,              inactive_color,             &vColors[KNOB_1].sColor)
                LSP_TK_PROPERTY(Color,              inactive_scale_color,       &vColors[KNOB_1].sScaleColor)
                LSP_TK_PROPERTY(Color,              inactive_balance_color,     &vColors[KNOB_1].sBalanceColor)
                LSP_TK_PROPERTY(Color,              inactive_tip_color,         &vColors[KNOB_1].sTipColor)
                LSP_TK_PROPERTY(Color,              inactive_balance_tip_color, &vColors[KNOB_1].sBalanceTipColor)
                LSP_TK_PROPERTY(Color,              inactive_meter_color,       &vColors[KNOB_1].sMeterColor)

                LSP_TK_PROPERTY(Color,              hole_color,                 &sHoleColor)

                LSP_TK_PROPERTY(SizeRange,          size,                       &sSizeRange)
                LSP_TK_PROPERTY(Float,              scale,                      &sScale)
                LSP_TK_PROPERTY(RangeFloat,         value,                      &sValue)
                LSP_TK_PROPERTY(StepFloat,          step,                       &sStep)
                LSP_TK_PROPERTY(Float,              balance,                    &sBalance)
                LSP_TK_PROPERTY(Float,              meter_min,                  &sMeterMin)
                LSP_TK_PROPERTY(Float,              meter_max,                  &sMeterMax)
                LSP_TK_PROPERTY(Boolean,            cycling,                    &sCycling)
                LSP_TK_PROPERTY(Boolean,            scale_marks,                &sScaleMarks)
                LSP_TK_PROPERTY(Boolean,            balance_color_custom,       &sBalanceColorCustom)
                LSP_TK_PROPERTY(Boolean,            flat,                       &sFlat)
                LSP_TK_PROPERTY(Boolean,            scale_active,               &sScaleActive)
                LSP_TK_PROPERTY(Boolean,            meter_active,               &sMeterActive)
                LSP_TK_PROPERTY(Boolean,            editable,                   &sEditable)
                LSP_TK_PROPERTY(Boolean,            active,                     &sActive)
                LSP_TK_PROPERTY(Integer,            hole_size,                  &sHoleSize)
                LSP_TK_PROPERTY(Integer,            gap_size,                   &sGapSize)
                LSP_TK_PROPERTY(Float,              scale_brightness,           &sScaleBrightness)
                LSP_TK_PROPERTY(Integer,            balance_tip_size,           &sBalanceTipSize)
                LSP_TK_PROPERTY(Boolean,            balance_tip_color_custom,   &sBalanceTipColorCustom)
                LSP_TK_PROPERTY(Boolean,            invert_mouse_vscroll,       &sInvertMouseVScroll)

            public:
                virtual status_t                on_mouse_down(const ws::event_t *e) override;
                virtual status_t                on_mouse_up(const ws::event_t *e) override;
                virtual status_t                on_mouse_move(const ws::event_t *e) override;
                virtual status_t                on_mouse_scroll(const ws::event_t *e) override;
                virtual void                    draw(ws::ISurface *s) override;

                virtual status_t                on_begin_edit();
                virtual status_t                on_change();
                virtual status_t                on_end_edit();
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_KNOB_H_ */
