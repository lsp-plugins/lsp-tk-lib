/*
 * GraphAxis.h
 *
 *  Created on: 20 авг. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHAXIS_H_
#define LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHAXIS_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class GraphAxis: public GraphItem
        {
            public:
                static const w_class_t    metadata;

            private:
                GraphAxis & operator = (const GraphAxis &);

            protected:
                prop::Vector2D              sDirection;     // Direction
                prop::Float                 sMin;           // Minimum value
                prop::Float                 sMax;           // Maximum value
                prop::Boolean               sLogScale;      // Logarithmic flag
                prop::Boolean               sBasis;         // Basis flag
                prop::Integer               sWidth;         // Thickness
                prop::Float                 sLength;        // Length of the axis
                prop::Integer               sOrigin;        // Origin index
                prop::Color                 sColor;         // Color of the axis

            protected:
                virtual void                property_changed(Property *prop);

            public:
                explicit GraphAxis(Display *dpy);
                virtual ~GraphAxis();

                virtual status_t            init();
                virtual void                destroy();

            public:
                LSP_TK_PROPERTY(Vector2D,           direction,              &sDirection);
                LSP_TK_PROPERTY(Float,              min,                    &sMin);
                LSP_TK_PROPERTY(Float,              max,                    &sMax);
                LSP_TK_PROPERTY(Boolean,            log_scale,              &sLogScale);
                LSP_TK_PROPERTY(Boolean,            basis,                  &sBasis);
                LSP_TK_PROPERTY(Integer,            width,                  &sWidth);
                LSP_TK_PROPERTY(Float,              length,                 &sLength);
                LSP_TK_PROPERTY(Integer,            origin,                 &sOrigin);
                LSP_TK_PROPERTY(Color,              color,                  &sColor);

            public:
                bool                        apply(float *x, float *y, const float *dv, size_t count);
                float                       project(float x, float y);
                bool                        parallel(float x, float y, float &a, float &b, float &c);
                void                        ortogonal_shift(float x, float y, float shift, float &nx, float &ny);
                bool                        angle(float x, float y, float angle, float &a, float &b, float &c);
                void                        rotate_shift(float x, float y, float angle, float shift, float &nx, float &ny);

            public:
                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force);
        };
    }
}


#endif /* LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHAXIS_H_ */
