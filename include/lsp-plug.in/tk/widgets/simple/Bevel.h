/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 13 июн. 2021 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_BEVEL_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_BEVEL_H_

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
            LSP_TK_STYLE_DEF_BEGIN(Bevel, Widget)
                prop::SizeConstraints       sConstraints;
                prop::Color                 sColor;
                prop::Color                 sBorderColor;
                prop::Integer               sBorder;
                prop::Vector2D              sDirection;
                prop::Arrangement           sArrangement;
            LSP_TK_STYLE_DEF_END
        }

        /**
         * Void widget that holds some allocated space and fills it with specified color
         */
        class Bevel: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                typedef struct point2d_t
                {
                    float x;
                    float y;
                } point2d_t;

            protected:
                prop::SizeConstraints       sConstraints;
                prop::Color                 sColor;
                prop::Color                 sBorderColor;
                prop::Integer               sBorder;
                prop::Vector2D              sDirection;
                prop::Arrangement           sArrangement;

            protected:
                static bool check_point(point2d_t *dp, const point2d_t *sp, const point2d_t *cx, float dx, float dy);

            public:
                explicit Bevel(Display *dpy);
                Bevel(const Bevel &) = delete;
                Bevel(Bevel &&) = delete;
                virtual ~Bevel() override;

                Bevel & operator = (const Bevel &) = delete;
                Bevel & operator = (Bevel &&) = delete;

            public:
                LSP_TK_PROPERTY(SizeConstraints,    constraints,        &sConstraints)
                LSP_TK_PROPERTY(Color,              color,              &sColor)
                LSP_TK_PROPERTY(Color,              border_color,       &sBorderColor)
                LSP_TK_PROPERTY(Integer,            border,             &sBorder)
                LSP_TK_PROPERTY(Vector2D,           direction,          &sDirection)
                LSP_TK_PROPERTY(Arrangement,        arrangement,        &sArrangement)

            protected:
                virtual void        size_request(ws::size_limit_t *r) override;
                virtual void        property_changed(Property *prop) override;

            public:
                virtual status_t    init() override;

                virtual void        render(ws::ISurface *s, const ws::rectangle_t *area, bool force) override;
        };

    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_BEVEL_H_ */
