/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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
                virtual void                size_request(ws::size_limit_t *r);
                virtual void                property_changed(Property *prop);

            public:
                explicit Separator(Display *dpy);
                virtual ~Separator();

                virtual status_t init();

            public:
                LSP_TK_PROPERTY(Color,              color,                      &sColor)
                LSP_TK_PROPERTY(Orientation,        orientation,                &sOrientation)
                LSP_TK_PROPERTY(SizeRange,          size,                       &sSizeRange)
                LSP_TK_PROPERTY(Integer,            thickness,                  &sThickness)

            public:
                virtual void render(ws::ISurface *s, bool force);

        };

        STYLE_INITIALIZER_DEF(Separator, Widget);
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_Separator_H_ */
