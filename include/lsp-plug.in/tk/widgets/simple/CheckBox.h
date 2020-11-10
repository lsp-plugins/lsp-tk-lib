/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_CHECKBOX_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_CHECKBOX_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_DEF_BEGIN(CheckBox, Widget)
                prop::SizeConstraints   sConstraints;           // Size constraints
                prop::Integer           sBorderSize;            // Size of border
                prop::Integer           sBorderRadius;          // Radius of border
                prop::Integer           sBorderGapSize;         // Size of gap between border and check
                prop::Integer           sCheckGapSize;          // Size of gap for check
                prop::Integer           sCheckMinSize;          // Minimum size of check
                prop::Boolean           sChecked;               // Checked flag
                prop::Color             sColor;                 // Color in checked state
                prop::Color             sHoverColor;            // Color in checked state + hover
                prop::Color             sFillColor;             // Color in unchecked state
                prop::Color             sFillHoverColor;        // Color in unchecked state + hover
                prop::Color             sBorderColor;           // Border color
                prop::Color             sBorderHoverColor;      // Border color + hover
                prop::Color             sBorderGapColor;        // Color of border gap
                prop::Color             sBorderGapHoverColor;   // Color of border gap + hover
            LSP_TK_STYLE_DEF_END
        }

        /**
         * CheckBox widget: simple on/off toggle
         */
        class CheckBox: public Widget
        {
            private:
                CheckBox & operator = (const CheckBox &);

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

            protected:
                ssize_t                 nRadius;
                size_t                  nState;
                size_t                  nBMask;
                ws::rectangle_t         sArea;

                prop::SizeConstraints   sConstraints;           // Size constraints
                prop::Integer           sBorderSize;            // Size of border
                prop::Integer           sBorderRadius;          // Radius of border
                prop::Integer           sBorderGapSize;         // Size of gap between border and check
                prop::Integer           sCheckGapSize;          // Size of gap for check
                prop::Integer           sCheckMinSize;          // Minimum size of check
                prop::Boolean           sChecked;               // Checked flag
                prop::Color             sColor;                 // Color in checked state
                prop::Color             sHoverColor;            // Color in checked state + hover
                prop::Color             sFillColor;             // Color in unchecked state
                prop::Color             sFillHoverColor;        // Color in unchecked state + hover
                prop::Color             sBorderColor;           // Border color
                prop::Color             sBorderHoverColor;      // Border color + hover
                prop::Color             sBorderGapColor;        // Color of border gap
                prop::Color             sBorderGapHoverColor;   // Color of border gap + hover

            protected:
                static status_t     slot_on_submit(Widget *sender, void *ptr, void *data);

            protected:
                virtual void        property_changed(Property *prop);
                virtual void        size_request(ws::size_limit_t *r);
                virtual void        realize(const ws::rectangle_t *r);

            public:
                explicit CheckBox(Display *dpy);
                virtual ~CheckBox();

                virtual status_t                init();

            public:
                LSP_TK_PROPERTY(SizeConstraints,            constraints,            &sConstraints);
                LSP_TK_PROPERTY(Integer,                    border_size,            &sBorderSize);
                LSP_TK_PROPERTY(Integer,                    border_radius,          &sBorderRadius);
                LSP_TK_PROPERTY(Integer,                    border_gap_size,        &sBorderGapSize);
                LSP_TK_PROPERTY(Integer,                    check_gap_size,         &sCheckGapSize);
                LSP_TK_PROPERTY(Integer,                    check_min_size,         &sCheckMinSize);
                LSP_TK_PROPERTY(Boolean,                    checked,                &sChecked);
                LSP_TK_PROPERTY(Color,                      color,                  &sColor);
                LSP_TK_PROPERTY(Color,                      hover_color,            &sHoverColor);
                LSP_TK_PROPERTY(Color,                      fill_color,             &sFillColor);
                LSP_TK_PROPERTY(Color,                      fill_hover_color,       &sFillHoverColor);
                LSP_TK_PROPERTY(Color,                      border_color,           &sBorderColor);
                LSP_TK_PROPERTY(Color,                      border_hover_color,     &sBorderHoverColor);
                LSP_TK_PROPERTY(Color,                      border_gap_color,       &sBorderGapColor);
                LSP_TK_PROPERTY(Color,                      border_gap_hover_color, &sBorderGapHoverColor);

            public:
                virtual void        draw(ws::ISurface *s);

                virtual status_t    on_mouse_down(const ws::event_t *e);

                virtual status_t    on_mouse_up(const ws::event_t *e);

                virtual status_t    on_mouse_move(const ws::event_t *e);

                virtual status_t    on_mouse_out(const ws::event_t *e);

                virtual status_t    on_key_down(const ws::event_t *e);

                virtual status_t    on_submit();
        };
    }
}



#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_CHECKBOX_H_ */
