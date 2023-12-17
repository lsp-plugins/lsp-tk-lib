/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHITEM_H_
#define LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHITEM_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Graph;

        // Style definition
        namespace style
        {
            LSP_TK_STYLE_DEF_BEGIN(GraphItem, Widget)
                prop::Boolean       sSmooth;
                prop::Integer       sPriorityGroup; // Priority group
                prop::Integer       sPriority;      // Priority inside of a group
            LSP_TK_STYLE_DEF_END
        }

        class GraphItem: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                prop::Boolean       sSmooth;
                prop::Integer       sPriorityGroup; // Priority group
                prop::Integer       sPriority;      // Priority inside of a group

            protected:
                virtual void            property_changed(Property *prop) override;

            public:
                explicit GraphItem(Display *dpy);
                GraphItem(const GraphItem &) = delete;
                GraphItem(GraphItem &&) = delete;
                virtual ~GraphItem() override;

                GraphItem & operator = (const GraphItem &) = delete;
                GraphItem & operator = (GraphItem &&) = delete;

            public:
                virtual status_t            init() override;
                virtual void                destroy() override;

            public:
                LSP_TK_PROPERTY(Boolean,        smooth,         &sSmooth);
                LSP_TK_PROPERTY(Integer,        priority_group, &sPriorityGroup);
                LSP_TK_PROPERTY(Integer,        priority,       &sPriority);

            public:
                /**
                 * Get the pointer to parent graph
                 * @return pointer to parent graph or NULL
                 */
                Graph              *graph();

                virtual void        query_draw(size_t flags = REDRAW_SURFACE) override;

                /**
                 * Check whether mouse pointer is inside of the graph item
                 * @param x horizontal position of mouse pointer
                 * @param y vertical position of mouse pointer
                 * @return true if item is inside of the graph
                 */
                virtual bool        inside(ssize_t x, ssize_t y) override;

            public:
                /**
                 * Estimate the bounding box for prioritized drawing
                 * @param s surface for drawing
                 * @param r rectangle to store the bound box
                 * @return false if widget does not provide bound box
                 */
                virtual bool        bound_box(ws::ISurface *s, ws::rectangle_t *r);
        };

    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHITEM_H_ */
