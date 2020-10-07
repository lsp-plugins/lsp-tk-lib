/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 7 окт. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_RACKEARS_H_
#define LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_RACKEARS_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class RackEars: public Widget
        {
            public:
                static const w_class_t    metadata;

            private:
                RackEars & operator = (const RackEars &);

            protected:
                enum flags_t
                {
                    XF_LBUTTON      = 1 << 0,
                    XF_RBUTTON      = 1 << 1,
                    XF_DOWN         = 1 << 2
                };

            protected:
                prop::Font          sFont;
                prop::String        sText;
                prop::Color         sColor;
                prop::Color         sTextColor;
                prop::Color         sScrewColor;
                prop::Color         sHoleColor;
                prop::Integer       sAngle;
                prop::Padding       sButtonPadding;
                prop::Padding       sScrewPadding;
                prop::Integer       sScrewSize;

                size_t              nBMask;
                size_t              nXFlags;
                ws::rectangle_t     sButton;
                ws::rectangle_t     sScrew[2];

            public:
                explicit RackEars(Display *dpy);
                virtual ~RackEars();

                virtual status_t            init();

            protected:
                static status_t             slot_on_submit(Widget *sender, void *ptr, void *data);

                virtual void                property_changed(Property *prop);
                virtual void                size_request(ws::size_limit_t *r);
                virtual void                realize(const ws::rectangle_t *r);

                void                        estimate_sizes(ws::rectangle_t *screw, ws::rectangle_t *btn);

                status_t                    handle_mouse_move(const ws::event_t *e);

            public:
                LSP_TK_PROPERTY(Font,               font,               &sFont)
                LSP_TK_PROPERTY(String,             text,               &sText)
                LSP_TK_PROPERTY(Color,              color,              &sColor)
                LSP_TK_PROPERTY(Color,              text_color,         &sTextColor)
                LSP_TK_PROPERTY(Color,              hole_color,         &sHoleColor)
                LSP_TK_PROPERTY(Integer,            angle,              &sAngle)
                LSP_TK_PROPERTY(Padding,            button_padding,     &sButtonPadding)
                LSP_TK_PROPERTY(Padding,            screw_padding,      &sScrewPadding)
                LSP_TK_PROPERTY(Integer,            screw_size,         &sScrewSize)

            public:
                virtual void                draw(ws::ISurface *s);

                virtual status_t            on_mouse_down(const ws::event_t *e);
                virtual status_t            on_mouse_up(const ws::event_t *e);
                virtual status_t            on_mouse_move(const ws::event_t *e);

                virtual status_t            on_submit();
        };
    }
}

#endif /* LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_RACKEARS_H_ */
