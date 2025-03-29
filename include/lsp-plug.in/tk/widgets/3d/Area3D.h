/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 9 окт. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_3D_AREA3D_H_
#define LSP_PLUG_IN_TK_WIDGETS_3D_AREA3D_H_

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
            LSP_TK_STYLE_DEF_BEGIN(Area3D, Widget)
                prop::SizeConstraints       sConstraints;   // Size constraints
                prop::Integer               sBorder;        // Border size
                prop::Integer               sBorderRadius;  // Border radius
                prop::Boolean               sBorderFlat;    // Border is flat
                prop::Boolean               sGlass;         // Draw glass
                prop::Color                 sColor;         // Graph color
                prop::Color                 sBorderColor;   // Color of the border
                prop::Color                 sGlassColor;    // Color of the glass
            LSP_TK_STYLE_DEF_END
        }

        /**
         * 3D Area for rendering 3D scenes
         */
        class Area3D: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                prop::SizeConstraints       sConstraints;   // Size constraints
                prop::Integer               sBorder;        // Border size
                prop::Integer               sBorderRadius;  // Border radius
                prop::Boolean               sBorderFlat;    // Border is flat
                prop::Boolean               sGlass;         // Draw glass
                prop::Color                 sColor;         // Graph color
                prop::Color                 sBorderColor;   // Color of the border
                prop::Color                 sGlassColor;    // Color of the glass

                ws::IR3DBackend            *pBackend;       // 3D rendering backend
                ws::ISurface               *pGlass;         // Cached glass gradient
                ws::rectangle_t             sCanvas;        // Actual dimensions of the drawing area (with padding)

            protected:
                virtual void                size_request(ws::size_limit_t *r) override;
                virtual void                property_changed(Property *prop) override;
                virtual void                realize(const ws::rectangle_t *r) override;

            protected:
                static status_t             slot_draw3d(Widget *sender, void *ptr, void *data);

            protected:
                void                        do_destroy();
                virtual void                hide_widget();
                void                        drop_glass();
                void                        drop_backend();
                ws::IR3DBackend            *get_backend();

            public:
                explicit Area3D(Display *dpy);
                Area3D(const Area3D &) = delete;
                Area3D(Area3D &&) = delete;
                virtual ~Area3D() override;

                Area3D & operator = (const Area3D &) = delete;
                Area3D & operator = (Area3D &&) = delete;

            public:
                virtual status_t            init() override;
                virtual void                destroy() override;

            public:
                LSP_TK_PROPERTY(SizeConstraints,            constraints,        &sConstraints);
                LSP_TK_PROPERTY(Integer,                    border_size,        &sBorder);
                LSP_TK_PROPERTY(Integer,                    border_radius,      &sBorderRadius);
                LSP_TK_PROPERTY(Boolean,                    border_flat,        &sBorderFlat);
                LSP_TK_PROPERTY(Boolean,                    glass,              &sGlass);
                LSP_TK_PROPERTY(Color,                      color,              &sColor);
                LSP_TK_PROPERTY(Color,                      border_color,       &sBorderColor);
                LSP_TK_PROPERTY(Color,                      glass_color,        &sGlassColor);

            public:
                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force) override;
                virtual void                draw(ws::ISurface *s, bool force) override;

            public:
                virtual status_t            on_draw3d(ws::IR3DBackend *r3d);
        };

    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_WIDGETS_3D_AREA3D_H_ */
