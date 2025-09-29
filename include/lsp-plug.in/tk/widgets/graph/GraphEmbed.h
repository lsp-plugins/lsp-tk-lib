/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 29 сент. 2025 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHEMBED_H_
#define LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHEMBED_H_

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
            LSP_TK_STYLE_DEF_BEGIN(GraphEmbed, GraphItem)
                prop::Integer               sOrigin;                // Index of origin widget
                prop::Integer               sHAxis;                 // Horizontal axis
                prop::Integer               sVAxis;                 // Vertical axis
                prop::Float                 sHStartValue;           // Horizontal axis start value
                prop::Float                 sVStartValue;           // Vertical axis start value
                prop::Float                 sHEndValue;             // Horizontal axis end value
                prop::Float                 sVEndValue;             // Vertical axis end value
                prop::Layout                sLayout;                // Layout
                prop::Float                 sTransparency;          // Transparency
            LSP_TK_STYLE_DEF_END
        }

        class GraphEmbed: public WidgetContainer
        {
            public:
                static const w_class_t    metadata;

            protected:
                prop::Integer               sOrigin;                // Index of origin widget
                prop::Integer               sHAxis;                 // Horizontal axis
                prop::Integer               sVAxis;                 // Vertical axis
                prop::Float                 sHStartValue;           // Horizontal axis start value
                prop::Float                 sVStartValue;           // Vertical axis start value
                prop::Float                 sHEndValue;             // Horizontal axis end value
                prop::Float                 sVEndValue;             // Vertical axis end value
                prop::Layout                sLayout;                // Layout
                prop::Float                 sTransparency;          // Transparency
                ws::rectangle_t             sLocation;              // Old widget location
                tk::Widget                 *pWidget;                // Widget

            protected:
                status_t                    calc_point(ssize_t *x, ssize_t *y, Graph *cv, float hvalue, float vvalue);
                void                        do_destroy();
                void                        realize_child(Graph *cv, ws::rectangle_t *widget, const ws::rectangle_t *location);
                void                        invalidate_location();

            protected:
                virtual void                property_changed(Property *prop) override;

            public:
                explicit GraphEmbed(Display *dpy);
                GraphEmbed(const GraphEmbed &) = delete;
                GraphEmbed(GraphEmbed &&) = delete;
                virtual ~GraphEmbed() override;

                GraphEmbed & operator = (const GraphEmbed &) = delete;
                GraphEmbed & operator = (GraphEmbed &&) = delete;

                virtual status_t            init() override;
                virtual void                destroy() override;

            public:
                Graph              *graph();

            public:
                LSP_TK_PROPERTY(Integer,            origin,                 &sOrigin)
                LSP_TK_PROPERTY(Integer,            haxis,                  &sHAxis)
                LSP_TK_PROPERTY(Integer,            vaxis,                  &sVAxis)
                LSP_TK_PROPERTY(Float,              hvalue_start,           &sHStartValue)
                LSP_TK_PROPERTY(Float,              hvalue_end,             &sHEndValue)
                LSP_TK_PROPERTY(Float,              vvalue_start,           &sVStartValue)
                LSP_TK_PROPERTY(Float,              vvalue_end,             &sVEndValue)
                LSP_TK_PROPERTY(Layout,             layout,                 &sLayout)
                LSP_TK_PROPERTY(Float,              transparency,           &sTransparency)

            public:
                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force) override;
                virtual void                draw(ws::ISurface *s, bool force) override;
                virtual status_t            add(Widget *widget) override;
                virtual status_t            remove(Widget *widget) override;
        };

    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHEMBED_H_ */
