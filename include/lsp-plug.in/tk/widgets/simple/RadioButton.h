/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 10 нояб. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_RADIOBUTTON_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_RADIOBUTTON_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            typedef struct RadioButtonColors
            {
                prop::Color             sColor;                 // Color in checked state
                prop::Color             sFillColor;             // Color in unchecked state
                prop::Color             sBorderColor;           // Border color
                prop::Color             sBorderGapColor;        // Color of border gap

                void listener(tk::prop::Listener *listener);
                bool property_changed(Property *prop);
            } RadioButtonColors;

            enum RadioButtonColorState
            {
                RADIOBUTTON_NORMAL      = 0,
                RADIOBUTTON_HOVER       = 1 << 0,
                RADIOBUTTON_INACTIVE    = 1 << 1,

                RADIOBUTTON_TOTAL       = 1 << 2
            };

            LSP_TK_STYLE_DEF_BEGIN(RadioButton, Widget)
                RadioButtonColors       vColors[RADIOBUTTON_TOTAL];

                prop::SizeConstraints   sConstraints;           // Size constraints
                prop::Integer           sBorderSize;            // Size of border
                prop::Integer           sBorderGapSize;         // Size of gap between border and check
                prop::Integer           sCheckGapSize;          // Size of gap for check
                prop::Integer           sCheckMinSize;          // Minimum size of check
                prop::Boolean           sChecked;               // Checked flag
            LSP_TK_STYLE_DEF_END
        }

        /**
         * RadioButton widget: simple on/off toggle
         */
        class RadioButton: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum xflags_t
                {
                    XF_HOVER    = 1 << 0,
                    XF_CHECKED  = 1 << 1,
                    XF_ACTIVE   = 1 << 2,
                    XF_OUT      = 1 << 3
                };

                enum chk_flags_t
                {
                    RBTN_0      = style::RADIOBUTTON_NORMAL,
                    RBTN_1      = style::RADIOBUTTON_HOVER,
                    RBTN_2      = style::RADIOBUTTON_INACTIVE,
                    RBTN_3      = style::RADIOBUTTON_HOVER | style::RADIOBUTTON_INACTIVE,
                    RBTN_TOTAL  = style::RADIOBUTTON_TOTAL
                };

            protected:
                size_t                      nState;
                size_t                      nBMask;
                ws::rectangle_t             sArea;

                style::RadioButtonColors    vColors[RBTN_TOTAL];
                prop::SizeConstraints       sConstraints;           // Size constraints
                prop::Integer               sBorderSize;            // Size of border
                prop::Integer               sBorderGapSize;         // Size of gap between border and check
                prop::Integer               sCheckGapSize;          // Size of gap for check
                prop::Integer               sCheckMinSize;          // Minimum size of check
                prop::Boolean               sChecked;               // Checked flag

            protected:
                static status_t             slot_on_submit(Widget *sender, void *ptr, void *data);

            protected:
                bool                        rinside(const ws::rectangle_t *r, float x, float y);
                style::RadioButtonColors   *select_colors();

            protected:
                virtual void                property_changed(Property *prop) override;
                virtual void                size_request(ws::size_limit_t *r) override;
                virtual void                realize(const ws::rectangle_t *r) override;

            public:
                explicit RadioButton(Display *dpy);
                RadioButton(const RadioButton &) = delete;
                RadioButton(RadioButton &&) = delete;
                virtual ~RadioButton() override;
                RadioButton & operator = (const RadioButton &) = delete;
                RadioButton & operator = (RadioButton &&) = delete;

                virtual status_t            init() override;

            public:
                LSP_TK_PROPERTY(Color,                      color,                              &vColors[RBTN_0].sColor);
                LSP_TK_PROPERTY(Color,                      fill_color,                         &vColors[RBTN_0].sFillColor);
                LSP_TK_PROPERTY(Color,                      border_color,                       &vColors[RBTN_0].sBorderColor);
                LSP_TK_PROPERTY(Color,                      border_gap_color,                   &vColors[RBTN_0].sBorderGapColor);
                LSP_TK_PROPERTY(Color,                      hover_color,                        &vColors[RBTN_1].sColor);
                LSP_TK_PROPERTY(Color,                      fill_hover_color,                   &vColors[RBTN_1].sFillColor);
                LSP_TK_PROPERTY(Color,                      border_hover_color,                 &vColors[RBTN_1].sBorderColor);
                LSP_TK_PROPERTY(Color,                      border_gap_hover_color,             &vColors[RBTN_1].sBorderGapColor);
                LSP_TK_PROPERTY(Color,                      inactive_color,                     &vColors[RBTN_0].sColor);
                LSP_TK_PROPERTY(Color,                      inactive_fill_color,                &vColors[RBTN_0].sFillColor);
                LSP_TK_PROPERTY(Color,                      inactive_border_color,              &vColors[RBTN_0].sBorderColor);
                LSP_TK_PROPERTY(Color,                      inactive_border_gap_color,          &vColors[RBTN_0].sBorderGapColor);
                LSP_TK_PROPERTY(Color,                      inactive_hover_color,               &vColors[RBTN_1].sColor);
                LSP_TK_PROPERTY(Color,                      inactive_fill_hover_color,          &vColors[RBTN_1].sFillColor);
                LSP_TK_PROPERTY(Color,                      inactive_border_hover_color,        &vColors[RBTN_1].sBorderColor);
                LSP_TK_PROPERTY(Color,                      inactive_border_gap_hover_color,    &vColors[RBTN_1].sBorderGapColor);

                LSP_TK_PROPERTY(SizeConstraints,            constraints,                        &sConstraints);
                LSP_TK_PROPERTY(Integer,                    border_size,                        &sBorderSize);
                LSP_TK_PROPERTY(Integer,                    border_gap_size,                    &sBorderGapSize);
                LSP_TK_PROPERTY(Integer,                    check_gap_size,                     &sCheckGapSize);
                LSP_TK_PROPERTY(Integer,                    check_min_size,                     &sCheckMinSize);
                LSP_TK_PROPERTY(Boolean,                    checked,                            &sChecked);

            public:
                virtual void        draw(ws::ISurface *s, bool force) override;
                virtual status_t    on_mouse_down(const ws::event_t *e) override;
                virtual status_t    on_mouse_up(const ws::event_t *e) override;
                virtual status_t    on_mouse_move(const ws::event_t *e) override;
                virtual status_t    on_mouse_out(const ws::event_t *e) override;
                virtual status_t    on_key_down(const ws::event_t *e) override;

            public:
                virtual status_t    on_submit();
        };

    } /* namespace tk */
} /* namespace lsp */


#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_RADIOBUTTON_H_ */
