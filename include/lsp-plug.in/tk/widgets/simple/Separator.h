/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 9 июл. 2017 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_SEPARATOR_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_SEPARATOR_H_

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
            LSP_TK_STYLE_DEF_BEGIN(Separator, Widget)
                prop::Orientation       sOrientation;
                prop::Color             sColor;
                prop::SizeRange         sSizeRange;
                prop::Integer           sThickness;
            LSP_TK_STYLE_DEF_END
        }

        /**
         * Simple separator widget
         */
        class Separator: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                prop::Orientation       sOrientation;
                prop::Color             sColor;
                prop::SizeRange         sSizeRange;
                prop::Integer           sThickness;

            protected:
                virtual void                size_request(ws::size_limit_t *r) override;
                virtual void                property_changed(Property *prop) override;

            public:
                explicit Separator(Display *dpy);
                Separator(const Separator &) = delete;
                Separator(Separator &&) = delete;
                virtual ~Separator() override;
                Separator & operator = (const Separator &) = delete;
                Separator & operator = (Separator &&) = delete;

                virtual status_t init() override;

            public:
                LSP_TK_PROPERTY(Color,              color,                      &sColor)
                LSP_TK_PROPERTY(Orientation,        orientation,                &sOrientation)
                LSP_TK_PROPERTY(SizeRange,          size,                       &sSizeRange)
                LSP_TK_PROPERTY(Integer,            thickness,                  &sThickness)

            public:
                virtual void render(ws::ISurface *s, const ws::rectangle_t *area, bool force) override;

        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_Separator_H_ */
