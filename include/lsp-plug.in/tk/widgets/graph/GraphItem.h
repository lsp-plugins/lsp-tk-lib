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

        class GraphItem: public Widget
        {
            public:
                static const w_class_t    metadata;

            private:
                GraphItem & operator = (const GraphItem &);

            protected:
                prop::Boolean       sSmooth;

            protected:
                virtual void            property_changed(Property *prop);

            public:
                explicit GraphItem(Display *dpy);
                virtual ~GraphItem();

            public:
                virtual status_t            init();
                virtual void                destroy();

            public:
                LSP_TK_PROPERTY(Boolean,        smooth,         &sSmooth);

            public:
                /**
                 * Get the pointer to parent graph
                 * @return pointer to parent graph or NULL
                 */
                Graph              *graph();

                virtual void        query_draw(size_t flags = REDRAW_SURFACE);

                /**
                 * Check whether mouse pointer is inside of the graph item
                 * @param x horizontal position of mouse pointer
                 * @param y vertical position of mouse pointer
                 * @return true if item is inside of the graph
                 */
                virtual bool        inside(ssize_t x, ssize_t y);
        };

        STYLE_INITIALIZER_DEF(GraphItem, Widget);
    }
}

#endif /* LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHITEM_H_ */
