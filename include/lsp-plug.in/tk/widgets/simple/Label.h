/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 6 июл. 2017 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LABEL_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LABEL_H_

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
            LSP_TK_STYLE_DEF_BEGIN(Label, Widget)
                prop::TextLayout            sTextLayout;    // Text layout
                prop::Font                  sFont;          // Font parameters
                prop::Color                 sColor;         // Font color
                prop::String                sText;          // Text to display
                prop::SizeConstraints       sConstraints;   // Size constraints
            LSP_TK_STYLE_DEF_END
        }

        class Label: public Widget
        {
            public:
                static const w_class_t    metadata;

            private:
                Label & operator = (const Label &);
                Label(const Label &);

                friend class MultiLabel;

            protected:
                prop::TextLayout            sTextLayout;    // Text layout
                prop::Font                  sFont;          // Font parameters
                prop::Color                 sColor;         // Font color
                prop::String                sText;          // Text to display
                prop::SizeConstraints       sConstraints;   // Size constraints

            protected:
                virtual void                    size_request(ws::size_limit_t *r);
                virtual void                    property_changed(Property *prop);

            public:
                explicit Label(Display *dpy);
                virtual ~Label();

                virtual status_t                init();

            public:
                LSP_TK_PROPERTY(TextLayout,         text_layout,        &sTextLayout)
                LSP_TK_PROPERTY(Font,               font,               &sFont)
                LSP_TK_PROPERTY(Color,              color,              &sColor)
                LSP_TK_PROPERTY(String,             text,               &sText)
                LSP_TK_PROPERTY(SizeConstraints,    constraints,        &sConstraints)

            public:
                virtual void                    draw(ws::ISurface *s);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LABEL_H_ */
