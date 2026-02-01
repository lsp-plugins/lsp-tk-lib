/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 31 янв. 2026 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_IMAGE_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_IMAGE_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/mm/Bitmap.h>

namespace lsp
{
    namespace tk
    {
        // Style definition
        namespace style
        {
            typedef struct ImageColors
            {
                prop::Color         sColor;
                prop::Color         sBorderColor;

                void listener(tk::prop::Listener *listener);
                bool property_changed(Property *prop) const;
            } ImageColors;

            enum ImageColorState
            {
                IMAGE_NORMAL        = 0,
                IMAGE_INACTIVE      = 1 << 0,
                IMAGE_HOVER         = 1 << 1,

                IMAGE_TOTAL         = 1 << 2
            };

            LSP_TK_STYLE_DEF_BEGIN(Image, Widget)
                ImageColors             vColors[IMAGE_TOTAL];

                prop::Bitmap            sBitmap;            // Bitmap
                prop::SizeConstraints   sConstraints;       // Size constraints
                prop::Layout            sLayout;            // Image layout
                prop::Fitting           sFitting;           // Image fitting
                prop::Integer           sBorderWidth;       // Border width
                prop::Float             sTransparency;      // Transparency
                prop::Float             sProportion;        // Image fixed proportion
            LSP_TK_STYLE_DEF_END
        } /* namespace style */

        class Image: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum img_flags_t
                {
                    IMG_0       = style::IMAGE_NORMAL,
                    IMG_1       = style::IMAGE_INACTIVE,
                    IMG_2       = IMG_0 | style::IMAGE_NORMAL,
                    IMG_3       = IMG_1 | style::IMAGE_INACTIVE,

                    IMG_TOTAL   = style::IMAGE_TOTAL
                };

            protected:
                ws::rectangle_t         sArea;              // Image area
                ws::rectangle_t         sBorder;            // Border area
                bool                    bMouseIn;           // Mouse in flag

                style::ImageColors      vColors[IMG_TOTAL];

                prop::Bitmap            sBitmap;            // Bitmap
                prop::SizeConstraints   sConstraints;       // Size constraints
                prop::Layout            sLayout;            // Image layout
                prop::Fitting           sFitting;           // Image fitting
                prop::Integer           sBorderWidth;       // Border width
                prop::Float             sTransparency;      // Image transparency
                prop::Float             sProportion;        // Image fixed proportion

            protected:
                static status_t                     slot_on_submit(Widget *sender, void *ptr, void *data);

            protected:
                style::ImageColors             *select_colors();

            protected:
                virtual void                    size_request(ws::size_limit_t *r) override;
                virtual void                    property_changed(Property *prop) override;
                virtual void                    realize(const ws::rectangle_t *r) override;

            public:
                explicit Image(Display *dpy);
                Image(const Image &) = delete;
                Image(Image &&) = delete;
                virtual ~Image() override;
                Image & operator = (const Image &) = delete;
                Image & operator = (Image &&) = delete;

                virtual status_t                init() override;

            public:
                LSP_TK_PROPERTY(Bitmap,             bitmap,                         &sBitmap)

                LSP_TK_PROPERTY(Color,              color,                          &vColors[IMG_0].sColor)
                LSP_TK_PROPERTY(Color,              border_color,                   &vColors[IMG_0].sBorderColor)
                LSP_TK_PROPERTY(Color,              inactive_color,                 &vColors[IMG_1].sColor)
                LSP_TK_PROPERTY(Color,              inactive_border_color,          &vColors[IMG_1].sBorderColor)
                LSP_TK_PROPERTY(Color,              hover_color,                    &vColors[IMG_2].sColor)
                LSP_TK_PROPERTY(Color,              border_hover_color,             &vColors[IMG_2].sBorderColor)
                LSP_TK_PROPERTY(Color,              inactive_hover_color,           &vColors[IMG_3].sColor)
                LSP_TK_PROPERTY(Color,              inactive_border_hover_color,    &vColors[IMG_3].sBorderColor)

                LSP_TK_PROPERTY(SizeConstraints,    constraints,            &sConstraints)
                LSP_TK_PROPERTY(Layout,             layout,                 &sLayout)
                LSP_TK_PROPERTY(Fitting,            fitting,                &sFitting)
                LSP_TK_PROPERTY(Integer,            border_width,           &sBorderWidth)
                LSP_TK_PROPERTY(Float,              transparency,           &sTransparency)
                LSP_TK_PROPERTY(Float,              proportion,             &sProportion)

            public: // tk::Widget
                virtual void                    draw(ws::ISurface *s, bool force) override;
                virtual status_t                on_mouse_in(const ws::event_t *e) override;
                virtual status_t                on_mouse_out(const ws::event_t *e) override;
                virtual status_t                on_mouse_move(const ws::event_t *e) override;
                virtual status_t                on_mouse_down(const ws::event_t *e) override;
                virtual status_t                on_mouse_up(const ws::event_t *e) override;

            public:
                virtual status_t                on_submit();
        };
    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_IMAGE_H_ */
