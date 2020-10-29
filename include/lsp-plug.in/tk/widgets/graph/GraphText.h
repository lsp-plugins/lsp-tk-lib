/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 20 авг. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHTEXT_H_
#define LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHTEXT_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class GraphText: public GraphItem
        {
            public:
                static const w_class_t    metadata;

            private:
                GraphText & operator = (const GraphText &);

            private:
                prop::String            sText;
                prop::Font              sFont;
                prop::Color             sColor;
                prop::Layout            sLayout;
                prop::TextLayout        sTextLayout;
                prop::Float             sHValue;
                prop::Float             sVValue;
                prop::Integer           sHAxis;
                prop::Integer           sVAxis;
                prop::Integer           sOrigin;

            protected:
                virtual void                property_changed(Property *prop);

            public:
                explicit GraphText(Display *dpy);
                virtual ~GraphText();

                virtual status_t            init();

            public:
                LSP_TK_PROPERTY(String,             text,               &sText)
                LSP_TK_PROPERTY(Font,               font,               &sFont)
                LSP_TK_PROPERTY(Color,              color,              &sColor)
                LSP_TK_PROPERTY(Layout,             layout,             &sLayout)
                LSP_TK_PROPERTY(TextLayout,         text_layout,        &sTextLayout)
                LSP_TK_PROPERTY(Float,              hvalue,             &sHValue)
                LSP_TK_PROPERTY(Float,              vvalue,             &sVValue)
                LSP_TK_PROPERTY(Integer,            haxis,              &sHAxis)
                LSP_TK_PROPERTY(Integer,            vaxis,              &sVAxis)
                LSP_TK_PROPERTY(Integer,            origin,             &sOrigin)

            public:
                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force);
        };
    }
}


#endif /* LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHTEXT_H_ */
