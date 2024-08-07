/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 1 июл. 2017 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_SWITCH_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_SWITCH_H_

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
            LSP_TK_STYLE_DEF_BEGIN(Switch, Widget)
                prop::Color                 sColor;
                prop::Color                 sTextColor;
                prop::Color                 sBorderColor;
                prop::Color                 sHoleColor;
                prop::Integer               sBorder;
                prop::SizeRange             sSizeRange;
                prop::Float                 sAspect;
                prop::Integer               sAngle;
                prop::Boolean               sDown;
                prop::Pointer               sButtonPointer;
            LSP_TK_STYLE_DEF_END
        }

        class Switch: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum state_t
                {
                    S_PRESSED   = (1 << 0),
                    S_TOGGLED   = (1 << 1)
                };

            protected:
                size_t                      nState;
                size_t                      nBMask;
                ws::rectangle_t             sButton;

                prop::Color                 sColor;
                prop::Color                 sTextColor;
                prop::Color                 sBorderColor;
                prop::Color                 sHoleColor;
                prop::Integer               sBorder;
                prop::SizeRange             sSizeRange;
                prop::Float                 sAspect;
                prop::Integer               sAngle;
                prop::Boolean               sDown;
                prop::Pointer               sButtonPointer;

            protected:
                bool                        check_mouse_over(ssize_t x, ssize_t y);

                void                        sync_state(bool down);

            protected:
                static status_t             slot_on_change(Widget *sender, void *ptr, void *data);

            protected:
                virtual void                size_request(ws::size_limit_t *r) override;
                virtual void                property_changed(Property *prop) override;
                virtual void                realize(const ws::rectangle_t *r) override;

            public:
                explicit Switch(Display *dpy);
                Switch(const Switch &) = delete;
                Switch(Switch &&) = delete;
                virtual ~Switch() override;

                Switch & operator = (const Switch &) = delete;
                Switch & operator = (Switch &&) = delete;

                virtual status_t            init() override;

            public:
                LSP_TK_PROPERTY(Color,              color,                      &sColor)
                LSP_TK_PROPERTY(Color,              text_color,                 &sTextColor)
                LSP_TK_PROPERTY(Color,              border_color,               &sBorderColor)
                LSP_TK_PROPERTY(Color,              hole_color,                 &sHoleColor)
                LSP_TK_PROPERTY(SizeRange,          size,                       &sSizeRange)
                LSP_TK_PROPERTY(Integer,            border,                     &sBorder)
                LSP_TK_PROPERTY(Float,              aspect,                     &sAspect)
                LSP_TK_PROPERTY(Integer,            angle,                      &sAngle)
                LSP_TK_PROPERTY(Boolean,            down,                       &sDown)
                LSP_TK_PROPERTY(Pointer,            button_pointer,             &sButtonPointer)

            public:
                virtual void                draw(ws::ISurface *s) override;
                virtual status_t            on_mouse_down(const ws::event_t *e) override;
                virtual status_t            on_mouse_up(const ws::event_t *e) override;
                virtual status_t            on_mouse_move(const ws::event_t *e) override;
                virtual status_t            on_mouse_pointer(pointer_event_t *e) override;

            public:
                virtual status_t            on_change(bool set);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_SWITCH_H_ */
