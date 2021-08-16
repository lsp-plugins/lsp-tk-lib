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

#ifndef LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHFRAMEBUFFER_H_
#define LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHFRAMEBUFFER_H_

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
            LSP_TK_STYLE_DEF_BEGIN(GraphFrameBuffer, GraphItem)
                prop::GraphFrameData        sData;              // Framebuffer data
                prop::Float                 sTransparency;      // Framebuffer transparency
                prop::Integer               sAngle;             // Framebuffer rotation angle
                prop::Float                 sHPos;              // Horizontal position
                prop::Float                 sVPos;              // Vertical position
                prop::Float                 sHScale;            // Width, proportional to the graph size
                prop::Float                 sVScale;            // Height, proportional to the graph size
                prop::Color                 sColor;             // Base color
                prop::GraphFrameFunction    sFunction;          // Function
            LSP_TK_STYLE_DEF_END
        }

        class GraphFrameBuffer: public GraphItem
        {
            public:
                static const w_class_t    metadata;

            private:
                GraphFrameBuffer & operator = (const GraphFrameBuffer &);
                GraphFrameBuffer(const GraphFrameBuffer &);

            protected:
                typedef void (GraphFrameBuffer::*calc_color_t)(float *rgba, const float *value, size_t n);

            protected:
                prop::GraphFrameData        sData;              // Framebuffer data
                prop::Float                 sTransparency;      // Framebuffer transparency
                prop::Integer               sAngle;             // Framebuffer rotation angle
                prop::Float                 sHPos;              // Horizontal position
                prop::Float                 sVPos;              // Vertical position
                prop::Float                 sHScale;            // Width, proportional to the graph size
                prop::Float                 sVScale;            // Height, proportional to the graph size
                prop::Color                 sColor;             // Base color
                prop::GraphFrameFunction    sFunction;          // Function

                bool                        bClear;             // Perform full cleanup of image
                size_t                      nRows;              // Cached number of rows
                size_t                      nCols;              // Cached number of columns
                calc_color_t                pCalcColor;         // Function to compute

                float                      *fRGBA;              // RGBA buffer
                uint8_t                    *pfRGBA;             // Unaligned RGBA buffer
                size_t                      nCapacity;          // RGBA buffer capacity

            protected:
                void                        calc_rainbow_color(float *rgba, const float *value, size_t n);
                void                        calc_fog_color(float *rgba, const float *v, size_t n);
                void                        calc_color(float *rgba, const float *value, size_t n);
                void                        calc_lightness(float *rgba, const float *value, size_t n);
                void                        calc_lightness2(float *rgba, const float *value, size_t n);

                void                        destroy_data();

                virtual void                property_changed(Property *prop);

            public:
                explicit GraphFrameBuffer(Display *dpy);
                virtual ~GraphFrameBuffer();

                virtual status_t            init();
                virtual void                destroy();

            public:
                LSP_TK_PROPERTY(GraphFrameData,         data,               &sData)
                LSP_TK_PROPERTY(Float,                  transparency,       &sTransparency)
                LSP_TK_PROPERTY(Integer,                angle,              &sAngle)
                LSP_TK_PROPERTY(Float,                  hpos,               &sHPos)
                LSP_TK_PROPERTY(Float,                  vpos,               &sVPos)
                LSP_TK_PROPERTY(Float,                  hscale,             &sHScale)
                LSP_TK_PROPERTY(Float,                  vscale,             &sVScale)
                LSP_TK_PROPERTY(Color,                  color,              &sColor)
                LSP_TK_PROPERTY(GraphFrameFunction,     function,           &sFunction)

            public:
                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force);

                virtual void                draw(ws::ISurface *s);
        };
    }
}


#endif /* LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHFRAMEBUFFER_H_ */
