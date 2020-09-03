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

#ifndef LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHORIGIN_H_
#define LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHORIGIN_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class GraphOrigin: public GraphItem
        {
            public:
                static const w_class_t    metadata;

            private:
                GraphOrigin & operator = (const GraphOrigin &);

            protected:
                prop::RangeFloat            sLeft;
                prop::RangeFloat            sTop;
                prop::Integer               sRadius;
                prop::Color                 sColor;

            protected:
                virtual void                property_changed(Property *prop);

            public:
                explicit GraphOrigin(Display *dpy);
                virtual ~GraphOrigin();

                virtual status_t            init();

            public:
                LSP_TK_PROPERTY(RangeFloat,                 left,               &sLeft);
                LSP_TK_PROPERTY(RangeFloat,                 top,                &sTop);
                LSP_TK_PROPERTY(Integer,                    radius,             &sRadius);
                LSP_TK_PROPERTY(Color,                      color,              &sColor);

            public:
                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force);
        };
    }
}


#endif /* LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHORIGIN_H_ */
