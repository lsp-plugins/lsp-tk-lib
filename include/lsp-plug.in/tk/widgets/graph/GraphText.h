/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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
        // Style definition
        namespace style
        {
            LSP_TK_STYLE_DEF_BEGIN(GraphText, GraphItem)
                prop::String            sText;
                prop::Font              sFont;
                prop::Color             sColor;
                prop::Layout            sLayout;
                prop::TextLayout        sTextLayout;
                prop::TextAdjust        sTextAdjust;
                prop::Float             sHValue;
                prop::Float             sVValue;
                prop::Integer           sHAxis;
                prop::Integer           sVAxis;
                prop::Integer           sOrigin;
                prop::Boolean           sBg;
                prop::Integer           sBgRadius;
                prop::Padding           sIPadding;
            LSP_TK_STYLE_DEF_END
        }

        class GraphText: public GraphItem
        {
            public:
                static const w_class_t    metadata;

            private:
                prop::String            sText;
                prop::Font              sFont;
                prop::Color             sColor;
                prop::Layout            sLayout;
                prop::TextLayout        sTextLayout;
                prop::TextAdjust        sTextAdjust;
                prop::Float             sHValue;
                prop::Float             sVValue;
                prop::Integer           sHAxis;
                prop::Integer           sVAxis;
                prop::Integer           sOrigin;
                prop::Boolean           sBg;
                prop::Integer           sBgRadius;
                prop::Padding           sIPadding;

            protected:
                virtual void                property_changed(Property *prop) override;

            public:
                explicit GraphText(Display *dpy);
                GraphText(const GraphText &) = delete;
                GraphText(GraphText &&) = delete;
                virtual ~GraphText() override;

                GraphText & operator = (const GraphText &) = delete;
                GraphText & operator = (GraphText &&) = delete;

                virtual status_t            init() override;

            public:
                LSP_TK_PROPERTY(String,             text,               &sText)
                LSP_TK_PROPERTY(Font,               font,               &sFont)
                LSP_TK_PROPERTY(Color,              color,              &sColor)
                LSP_TK_PROPERTY(Layout,             layout,             &sLayout)
                LSP_TK_PROPERTY(TextLayout,         text_layout,        &sTextLayout)
                LSP_TK_PROPERTY(TextAdjust,         text_adjust,        &sTextAdjust)
                LSP_TK_PROPERTY(Float,              hvalue,             &sHValue)
                LSP_TK_PROPERTY(Float,              vvalue,             &sVValue)
                LSP_TK_PROPERTY(Integer,            haxis,              &sHAxis)
                LSP_TK_PROPERTY(Integer,            vaxis,              &sVAxis)
                LSP_TK_PROPERTY(Integer,            origin,             &sOrigin)
                LSP_TK_PROPERTY(Boolean,            bg,                 &sBg)
                LSP_TK_PROPERTY(Integer,            bg_radius,          &sBgRadius)
                LSP_TK_PROPERTY(Padding,            ipadding,           &sIPadding)

            public:
                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force) override;

                virtual bool                bound_box(ws::ISurface *s, ws::rectangle_t *r) override;
        };
    } /* namespace tk */
} /* namespace lsp */


#endif /* LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHTEXT_H_ */
