/*
 * GraphMarker.h
 *
 *  Created on: 20 авг. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHMARKER_H_
#define LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHMARKER_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class GraphMarker: public GraphItem
        {
            public:
                static const w_class_t    metadata;

            private:
                GraphMarker & operator = (const GraphMarker &);

            protected:
                enum flags_t
                {
                    F_HIGHLIGHT     = 1 << 0,
                    F_EDITING       = 1 << 1,
                    F_FINE_TUNE     = 1 << 2
                };

            protected:
                prop::Integer               sOrigin;        // Origin
                prop::Integer               sBasis;         // Index of basis axis
                prop::Integer               sParallel;      // Index of parallel axis
                prop::Float                 sValue;         // Actual value
                prop::StepFloat             sStep;          // Stepping value
                prop::Float                 sMin;           // Minimum possible value when editing
                prop::Float                 sMax;           // Minimum possible value when editing
                prop::Vector2D              sDirection;     // Direction
                prop::Integer               sWidth;         // Thickness
                prop::Integer               sHWidth;        // Hover width
                prop::Boolean               sEditable;      // Editable flag
                prop::Integer               sLBorder;       // Highlighting border
                prop::Integer               sRBorder;       // Highlighting border
                prop::Integer               sHLBorder;      // Highlighting border for hover
                prop::Integer               sHRBorder;      // Highlighting border for hover
                prop::Color                 sColor;         // Color
                prop::Color                 sHColor;        // Selected Color
                prop::Color                 sLBorderColor;  // Border Color
                prop::Color                 sRBorderColor;  // Border Color
                prop::Color                 sHLBorderColor; // Border Hover Color
                prop::Color                 sHRBorderColor; // Border Hover Color

                float                       fOffset;        // Offset
                size_t                      nXFlags;        // Extra flags
                size_t                      nMBState;       // Mouse button state
                ssize_t                     nMouseX;        // Mouse initial X position
                ssize_t                     nMouseY;        // Mouse initial Y position
                float                       fLastValue;     // Last value used before editing started

            protected:
                void                        apply_motion(ssize_t x, ssize_t y, size_t flags);

                static status_t             slot_on_change(Widget *sender, void *ptr, void *data);

            protected:
                virtual void                property_changed(Property *prop);

            public:
                explicit GraphMarker(Display *dpy);
                virtual ~GraphMarker();

                virtual status_t            init();

            public:
                LSP_TK_PROPERTY(Integer,            origin,                 &sOrigin);
                LSP_TK_PROPERTY(Integer,            basis,                  &sBasis);
                LSP_TK_PROPERTY(Integer,            parallel,               &sParallel);
                LSP_TK_PROPERTY(Float,              value,                  &sValue);
                LSP_TK_PROPERTY(StepFloat,          step,                   &sStep);
                LSP_TK_PROPERTY(Float,              min,                    &sMin);
                LSP_TK_PROPERTY(Float,              max,                    &sMax);
                LSP_TK_PROPERTY(Vector2D,           direction,              &sDirection);
                LSP_TK_PROPERTY(Integer,            width,                  &sWidth);
                LSP_TK_PROPERTY(Integer,            hover_width,            &sHWidth);
                LSP_TK_PROPERTY(Boolean,            editable,               &sEditable);
                LSP_TK_PROPERTY(Integer,            left_border,            &sLBorder);
                LSP_TK_PROPERTY(Integer,            right_border,           &sRBorder);
                LSP_TK_PROPERTY(Integer,            hover_left_border,      &sHLBorder);
                LSP_TK_PROPERTY(Integer,            hover_right_border,     &sHRBorder);
                LSP_TK_PROPERTY(Color,              color,                  &sColor);
                LSP_TK_PROPERTY(Color,              hover_color,            &sHColor);
                LSP_TK_PROPERTY(Color,              border_left_color,      &sLBorderColor);
                LSP_TK_PROPERTY(Color,              border_right_color,     &sRBorderColor);
                LSP_TK_PROPERTY(Color,              hover_border_left_color,&sHLBorderColor);
                LSP_TK_PROPERTY(Color,              hover_border_right_color,&sHRBorderColor);

            public:
                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force);

                virtual bool                inside(ssize_t x, ssize_t y);

                virtual status_t            on_mouse_in(const ws::event_t *e);

                virtual status_t            on_mouse_out(const ws::event_t *e);

                virtual status_t            on_mouse_down(const ws::event_t *e);

                virtual status_t            on_mouse_up(const ws::event_t *e);

                virtual status_t            on_mouse_move(const ws::event_t *e);

                virtual status_t            on_change();
        };
    }
}


#endif /* LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHMARKER_H_ */
