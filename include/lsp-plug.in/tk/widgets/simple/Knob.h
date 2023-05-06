/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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
            LSP_TK_STYLE_DEF_BEGIN(Knob, Widget)
                prop::Color         sColor;
                prop::Color         sScaleColor;
                prop::Color         sBalanceColor;
                prop::Color         sHoleColor;
                prop::Color         sTipColor;
                prop::Color         sBalanceTipColor;
                prop::SizeRange     sSizeRange;
                prop::Float         sScale;
                prop::RangeFloat    sValue;
                prop::StepFloat     sStep;
                prop::Float         sBalance;
                prop::Boolean       sCycling;
                prop::Boolean       sScaleMarks;
                prop::Boolean       sBalanceColorCustom;
                prop::Boolean       sFlat;
                prop::Boolean       sScaleActive;
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

            private:
                Knob & operator = (const Knob &);
                Knob(const Knob &);

            protected:
                enum state_t
                {
                    S_NONE,
                    S_MOVING,
                    S_CLICK
                };

            protected:
                ssize_t             nLastY;
                size_t              nState;
                size_t              nButtons;

                prop::Color         sColor;
                prop::Color         sScaleColor;
                prop::Color         sBalanceColor;
                prop::Color         sHoleColor;
                prop::Color         sTipColor;
                prop::Color         sBalanceTipColor;
                prop::SizeRange     sSizeRange;
                prop::Float         sScale;

                prop::RangeFloat    sValue;
                prop::StepFloat     sStep;
                prop::Float         sBalance;
                prop::Boolean       sCycling;
                prop::Boolean       sScaleMarks;
                prop::Boolean       sBalanceColorCustom;
                prop::Boolean       sFlat;
                prop::Boolean       sScaleActive;
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

            protected:
                static status_t                 slot_begin_edit(Widget *sender, void *ptr, void *data);
                static status_t                 slot_on_change(Widget *sender, void *ptr, void *data);
                static status_t                 slot_end_edit(Widget *sender, void *ptr, void *data);

            protected:
                virtual void                    size_request(ws::size_limit_t *r);
                virtual void                    property_changed(Property *prop);

            public:
                explicit Knob(Display *dpy);
                virtual ~Knob();

                virtual status_t                init();

            public:
                LSP_TK_PROPERTY(Color,              color,                      &sColor)
                LSP_TK_PROPERTY(Color,              scale_color,                &sScaleColor)
                LSP_TK_PROPERTY(Color,              balance_color,              &sBalanceColor)
                LSP_TK_PROPERTY(Color,              hole_color,                 &sHoleColor)
                LSP_TK_PROPERTY(Color,              tip_color,                  &sTipColor)
                LSP_TK_PROPERTY(Color,              balance_tip_color,          &sBalanceTipColor)
                LSP_TK_PROPERTY(SizeRange,          size,                       &sSizeRange)
                LSP_TK_PROPERTY(Float,              scale,                      &sScale)
                LSP_TK_PROPERTY(RangeFloat,         value,                      &sValue)
                LSP_TK_PROPERTY(StepFloat,          step,                       &sStep)
                LSP_TK_PROPERTY(Float,              balance,                    &sBalance)
                LSP_TK_PROPERTY(Boolean,            cycling,                    &sCycling)
                LSP_TK_PROPERTY(Boolean,            scale_marks,                &sScaleMarks)
                LSP_TK_PROPERTY(Boolean,            balance_color_custom,       &sBalanceColorCustom)
                LSP_TK_PROPERTY(Boolean,            flat,                       &sFlat)
                LSP_TK_PROPERTY(Boolean,            scale_active,               &sScaleActive)
                LSP_TK_PROPERTY(Integer,            hole_size,                  &sHoleSize)
                LSP_TK_PROPERTY(Integer,            gap_size,                   &sGapSize)
                LSP_TK_PROPERTY(Float,              scale_brightness,           &sScaleBrightness)
                LSP_TK_PROPERTY(Integer,            balance_tip_size,           &sBalanceTipSize)
                LSP_TK_PROPERTY(Boolean,            balance_tip_color_custom,   &sBalanceTipColorCustom)
                LSP_TK_PROPERTY(Boolean,            invert_mouse_vscroll,       &sInvertMouseVScroll)

            public:
                virtual status_t                on_mouse_down(const ws::event_t *e);

                virtual status_t                on_mouse_up(const ws::event_t *e);

                virtual status_t                on_mouse_move(const ws::event_t *e);

                virtual status_t                on_mouse_scroll(const ws::event_t *e);

                virtual status_t                on_begin_edit();

                virtual status_t                on_change();

                virtual status_t                on_end_edit();

                virtual void                    draw(ws::ISurface *s);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_KNOB_H_ */
