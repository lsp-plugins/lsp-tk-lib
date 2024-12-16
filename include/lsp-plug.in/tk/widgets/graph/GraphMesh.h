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

#ifndef LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHMESH_H_
#define LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHMESH_H_

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
            LSP_TK_STYLE_DEF_BEGIN(GraphMesh, GraphItem)
                prop::Integer               sOrigin;        // Index of origin
                prop::Integer               sXAxis;         // Index of X axis
                prop::Integer               sYAxis;         // Index of Y axis
                prop::Integer               sWidth;         // Width of the mesh line
                prop::Integer               sStrobes;       // Number of strobes
                prop::Boolean               sFill;          // Fill poly
                prop::Color                 sColor;         // Mesh color
                prop::Color                 sFillColor;     // Fill color
                prop::GraphMeshData         sData;          // Graph mesh data
            LSP_TK_STYLE_DEF_END
        }

        class GraphMesh: public GraphItem
        {
            public:
                static const w_class_t    metadata;

            public:
                enum coord_t
                {
                    COORD_X,
                    COORD_Y,
                    STROBE
                };

                /**
                 * Waveform transformation function
                 * @param dst destination buffer to store result
                 * @param src source buffer to read result
                 * @param count number of elements to process
                 * @param coord coordinate type
                 * @param data supplementary user data
                 * @return true if modified data has been stored to destination buffer
                 */
                typedef bool transform_t(float *dst, const float *src, size_t count, coord_t coord, void *data);

            protected:
                prop::Integer               sOrigin;        // Index of origin
                prop::Integer               sXAxis;         // Index of X axis
                prop::Integer               sYAxis;         // Index of Y axis
                prop::Integer               sWidth;         // Width of the mesh line
                prop::Integer               sStrobes;       // Number of strobes
                prop::Boolean               sFill;          // Fill poly
                prop::Color                 sColor;         // Mesh color
                prop::Color                 sFillColor;     // Fill color
                prop::GraphMeshData         sData;          // Graph mesh data

                transform_t                *pTransform;     // Transform function
                void                       *pTrData;        // Transform data
                float                      *vBuffer;        // Temporary buffer
                size_t                      nCapacity;      // Capacity of the temporary buffer

            protected:
                void                        do_destroy();
                size_t                      find_offset(size_t *found, const float *v, size_t count, size_t strobes);
                size_t                      get_length(const float *v, size_t off, size_t count);

            protected:
                virtual void                property_changed(Property *prop) override;

            public:
                explicit GraphMesh(Display *dpy);
                GraphMesh(const GraphMesh &) = delete;
                GraphMesh(GraphMesh &&) = delete;
                virtual ~GraphMesh() override;

                GraphMesh & operator = (const GraphMesh &) = delete;
                GraphMesh & operator = (GraphMesh &&) = delete;

                virtual status_t            init() override;
                virtual void                destroy() override;

            public:
                void                        set_transform(transform_t *func, void *data = NULL);

            public:
                LSP_TK_PROPERTY(Integer,            origin,                     &sOrigin)
                LSP_TK_PROPERTY(Integer,            haxis,                      &sXAxis)
                LSP_TK_PROPERTY(Integer,            vaxis,                      &sYAxis)
                LSP_TK_PROPERTY(Integer,            width,                      &sWidth)
                LSP_TK_PROPERTY(Integer,            strobes,                    &sStrobes)
                LSP_TK_PROPERTY(Boolean,            fill,                       &sFill)
                LSP_TK_PROPERTY(Color,              color,                      &sColor)
                LSP_TK_PROPERTY(Color,              fill_color,                 &sFillColor)
                LSP_TK_PROPERTY(GraphMeshData,      data,                       &sData)

            public:
                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force) override;
        };

    } /* namespace tk */
} /* namespace lsp */


#endif /* LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHMESH_H_ */
