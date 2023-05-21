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

#ifndef UI_TK_WIDGETS_LSPFADER_H_
#define UI_TK_WIDGETS_LSPFADER_H_

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
            LSP_TK_STYLE_DEF_BEGIN(Fader, Widget)
                prop::Color                     sBtnColor;
                prop::Color                     sBtnBorderColor;
                prop::Color                     sScaleColor;
                prop::Color                     sScaleBorderColor;
                prop::Color                     sBalanceColor;
                prop::SizeRange                 sSizeRange;
                prop::RangeFloat                sValue;
                prop::StepFloat                 sStep;
                prop::SizeRange                 sBtnWidth;
                prop::Float                     sBtnAspect;
                prop::Integer                   sAngle;
                prop::Integer                   sScaleWidth;
                prop::Integer                   sScaleBorder;
                prop::Integer                   sScaleRadius;
                prop::Boolean                   sScaleGradient;
                prop::Integer                   sBtnBorder;
                prop::Integer                   sBtnRadius;
                prop::Boolean                   sBtnGradient;
                prop::Pointer                   sBtnPointer;
                prop::Float                     sBalance;
                prop::Float                     sScaleBrightness;
                prop::Boolean                   sBalanceColorCustom;
                prop::Boolean                   sInvertMouseVScroll;
            LSP_TK_STYLE_DEF_END
        }

        /**
         * Fader widget
         */
        class Fader: public Widget
        {
            public:
                static const w_class_t    metadata;

            private:
                Fader & operator = (const Fader &);
                Fader(const Fader &);

            protected:
                enum flags_t
                {
                    F_IGNORE        = 1 << 0,
                    F_PRECISION     = 1 << 1,
                    F_MOVER         = 1 << 2
                };

            protected:
                ssize_t                         nLastV;
                size_t                          nButtons;
                size_t                          nXFlags;
                float                           fLastValue;
                float                           fCurrValue;
                ws::rectangle_t                 sButton;
                ws::rectangle_t                 sHole;

                prop::Color                     sBtnColor;
                prop::Color                     sBtnBorderColor;
                prop::Color                     sScaleColor;
                prop::Color                     sScaleBorderColor;
                prop::Color                     sBalanceColor;
                prop::SizeRange                 sSizeRange;
                prop::RangeFloat                sValue;
                prop::StepFloat                 sStep;
                prop::SizeRange                 sBtnWidth;
                prop::Float                     sBtnAspect;
                prop::Integer                   sAngle;
                prop::Integer                   sScaleWidth;
                prop::Integer                   sScaleBorder;
                prop::Integer                   sScaleRadius;
                prop::Boolean                   sScaleGradient;
                prop::Integer                   sBtnBorder;
                prop::Integer                   sBtnRadius;
                prop::Boolean                   sBtnGradient;
                prop::Pointer                   sBtnPointer;
                prop::Float                     sBalance;
                prop::Float                     sScaleBrightness;
                prop::Boolean                   sBalanceColorCustom;
                prop::Boolean                   sInvertMouseVScroll;

            protected:
                float                           limit_value(float value);
                float                           get_normalized_value();
                void                            update();
                void                            update_cursor_state(ssize_t x, ssize_t y, bool set);
                float                           update_value(float value);
                void                            sync_button_pos();

            protected:
                static status_t                 slot_begin_edit(Widget *sender, void *ptr, void *data);
                static status_t                 slot_on_change(Widget *sender, void *ptr, void *data);
                static status_t                 slot_end_edit(Widget *sender, void *ptr, void *data);

            protected:
                virtual void                    size_request(ws::size_limit_t *r);
                virtual void                    property_changed(Property *prop);
                virtual void                    realize(const ws::rectangle_t *r);

            public:
                explicit Fader(Display *dpy);
                virtual ~Fader();

                virtual status_t                init();

            public:
                LSP_TK_PROPERTY(Color,          button_color,               &sBtnColor);
                LSP_TK_PROPERTY(Color,          button_border_color,        &sBtnBorderColor);
                LSP_TK_PROPERTY(Color,          scale_color,                &sScaleColor);
                LSP_TK_PROPERTY(Color,          scale_border_color,         &sScaleBorderColor);
                LSP_TK_PROPERTY(Color,          balance_color,              &sBalanceColor);
                LSP_TK_PROPERTY(SizeRange,      size,                       &sSizeRange);
                LSP_TK_PROPERTY(RangeFloat,     value,                      &sValue);
                LSP_TK_PROPERTY(StepFloat,      step,                       &sStep);
                LSP_TK_PROPERTY(SizeRange,      button_width,               &sBtnWidth);
                LSP_TK_PROPERTY(Float,          button_aspect,              &sBtnAspect);
                LSP_TK_PROPERTY(Pointer,        button_pointer,             &sBtnPointer);
                LSP_TK_PROPERTY(Integer,        angle,                      &sAngle);
                LSP_TK_PROPERTY(Integer,        scale_width,                &sScaleWidth);
                LSP_TK_PROPERTY(Integer,        scale_border,               &sScaleBorder);
                LSP_TK_PROPERTY(Integer,        scale_radius,               &sScaleRadius);
                LSP_TK_PROPERTY(Boolean,        scale_gradient,             &sScaleGradient);
                LSP_TK_PROPERTY(Integer,        button_border,              &sBtnBorder);
                LSP_TK_PROPERTY(Integer,        button_radius,              &sBtnRadius);
                LSP_TK_PROPERTY(Boolean,        button_gradient,            &sBtnGradient);
                LSP_TK_PROPERTY(Float,          balance,                    &sBalance);
                LSP_TK_PROPERTY(Float,          scale_brightness,           &sScaleBrightness);
                LSP_TK_PROPERTY(Boolean,        balance_color_custom,       &sBalanceColorCustom);
                LSP_TK_PROPERTY(Boolean,        invert_mouse_vscroll,       &sInvertMouseVScroll);

            public:

                virtual ws::mouse_pointer_t     current_pointer();

                virtual status_t                on_begin_edit();

                virtual status_t                on_change();

                virtual status_t                on_end_edit();

                virtual status_t                on_mouse_down(const ws::event_t *e);

                virtual status_t                on_mouse_up(const ws::event_t *e);

                virtual status_t                on_mouse_move(const ws::event_t *e);

                virtual status_t                on_mouse_scroll(const ws::event_t *e);

                virtual void                    draw(ws::ISurface *s);
        };

    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_LSPFADER_H_ */
