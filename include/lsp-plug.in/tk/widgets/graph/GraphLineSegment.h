/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 17 июн. 2023 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHLINESEGMENT_H_
#define LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHLINESEGMENT_H_

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
            LSP_TK_STYLE_DEF_BEGIN(GraphLineSegment, GraphItem)
                prop::Integer               sOrigin;                // Origin
                prop::Integer               sHAxis;                 // Horizontal axis
                prop::Integer               sVAxis;                 // Vertical axis
                prop::Point2D               sBegin;                 // Begin point
                prop::Integer               sWidth;                 // Thickness
                prop::Integer               sHWidth;                // Hover width
                prop::Integer               sLBorder;               // Highlighting border
                prop::Integer               sRBorder;               // Highlighting border
                prop::Integer               sHLBorder;              // Highlighting border for hover
                prop::Integer               sHRBorder;              // Highlighting border for hover
                prop::Boolean               sInvertMouseVScroll;    // Invert mouse vertical scroll

                prop::Color                 sColor;                 // Color
                prop::Color                 sHColor;                // Selected Color
                prop::Color                 sLBorderColor;          // Border Color
                prop::Color                 sRBorderColor;          // Border Color
                prop::Color                 sHLBorderColor;         // Border Hover Color
                prop::Color                 sHRBorderColor;         // Border Hover Color

                prop::Boolean               sEditable[3];           // Editable flag
                prop::RangeFloat            sValue[3];              // Value
                prop::StepFloat             sStep[3];               // Step
            LSP_TK_STYLE_DEF_END
        }

        class GraphLineSegment: public GraphItem
        {
            public:
                static const w_class_t    metadata;

            private:
                GraphLineSegment & operator = (const GraphLineSegment &);
                GraphLineSegment(const GraphLineSegment &);

            protected:
                enum flags_t
                {
                    F_EDITABLE      = 1 << 0,
                    F_HIGHLIGHT     = 1 << 1,
                    F_EDITING       = 1 << 2,
                    F_FINE_TUNE     = 1 << 3
                };

                typedef struct param_t
                {
                    GraphLineSegment           *pSegment;
                    prop::Boolean               sEditable;              // Editable flag
                    prop::RangeFloat            sValue;                 // Value
                    prop::StepFloat             sStep;                  // Step

                    explicit                    param_t(GraphLineSegment *dot, prop::Listener *lst);
                    void                        property_changed(Property *prop);
                    void                        init(const char *prefix);
                } param_t;

            protected:
                param_t                     sHValue;                // Horizontal value
                param_t                     sVValue;                // Vertical value
                param_t                     sZValue;                // The value associated with scrolling

                prop::Integer               sOrigin;                // Origin
                prop::Integer               sHAxis;                 // Horizontal axis
                prop::Integer               sVAxis;                 // Vertical axis
                prop::Point2D               sBegin;                 // Begin point
                prop::Integer               sWidth;                 // Thickness
                prop::Integer               sHWidth;                // Hover width
                prop::Integer               sLBorder;               // Highlighting border
                prop::Integer               sRBorder;               // Highlighting border
                prop::Integer               sHLBorder;              // Highlighting border for hover
                prop::Integer               sHRBorder;              // Highlighting border for hover
                prop::Boolean               sInvertMouseVScroll;    // Invert mouse vertical scroll

                prop::Color                 sColor;                 // Color
                prop::Color                 sHColor;                // Selected Color
                prop::Color                 sLBorderColor;          // Border Color
                prop::Color                 sRBorderColor;          // Border Color
                prop::Color                 sHLBorderColor;         // Border Hover Color
                prop::Color                 sHRBorderColor;         // Border Hover Color

                size_t                      nXFlags;                // Extra flags
                size_t                      nMBState;               // Mouse button state
                ssize_t                     nMouseX;                // Mouse initial X position
                ssize_t                     nMouseY;                // Mouse initial Y position
                float                       fLastX;                 // Last X value
                float                       fLastY;                 // Last Y value

            protected:
                void                        apply_motion(ssize_t x, ssize_t y, size_t flags);

                static status_t             slot_begin_edit(Widget *sender, void *ptr, void *data);
                static status_t             slot_on_change(Widget *sender, void *ptr, void *data);
                static status_t             slot_end_edit(Widget *sender, void *ptr, void *data);

            protected:
                virtual void                property_changed(Property *prop) override;

            public:
                explicit GraphLineSegment(Display *dpy);
                virtual ~GraphLineSegment() override;

                virtual status_t            init() override;

            public:
                LSP_TK_PROPERTY(RangeFloat,         hvalue,                 &sHValue.sValue)
                LSP_TK_PROPERTY(Boolean,            heditable,              &sHValue.sEditable)
                LSP_TK_PROPERTY(StepFloat,          hstep,                  &sHValue.sStep)
                LSP_TK_PROPERTY(RangeFloat,         vvalue,                 &sVValue.sValue)
                LSP_TK_PROPERTY(Boolean,            veditable,              &sVValue.sEditable)
                LSP_TK_PROPERTY(StepFloat,          vstep,                  &sVValue.sStep)
                LSP_TK_PROPERTY(RangeFloat,         zvalue,                 &sZValue.sValue)
                LSP_TK_PROPERTY(Boolean,            zeditable,              &sZValue.sEditable)
                LSP_TK_PROPERTY(StepFloat,          zstep,                  &sZValue.sStep)

                LSP_TK_PROPERTY(Integer,            origin,                 &sOrigin);
                LSP_TK_PROPERTY(Integer,            haxis,                  &sHAxis);
                LSP_TK_PROPERTY(Integer,            vaxis,                  &sVAxis);
                LSP_TK_PROPERTY(Point2D,            begin,                  &sBegin);
                LSP_TK_PROPERTY(Integer,            width,                  &sWidth);
                LSP_TK_PROPERTY(Integer,            hover_width,            &sHWidth);
                LSP_TK_PROPERTY(Integer,            left_border,            &sLBorder);
                LSP_TK_PROPERTY(Integer,            right_border,           &sRBorder);
                LSP_TK_PROPERTY(Integer,            hover_left_border,      &sHLBorder);
                LSP_TK_PROPERTY(Integer,            hover_right_border,     &sHRBorder);
                LSP_TK_PROPERTY(Boolean,            invert_mouse_vscroll,   &sInvertMouseVScroll);

                LSP_TK_PROPERTY(Color,              color,                  &sColor);
                LSP_TK_PROPERTY(Color,              hover_color,            &sHColor);
                LSP_TK_PROPERTY(Color,              border_left_color,      &sLBorderColor);
                LSP_TK_PROPERTY(Color,              border_right_color,     &sRBorderColor);
                LSP_TK_PROPERTY(Color,              hover_border_left_color,&sHLBorderColor);
                LSP_TK_PROPERTY(Color,              hover_border_right_color,&sHRBorderColor);


            public:
                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force) override;

                virtual bool                inside(ssize_t x, ssize_t y) override;

                virtual status_t            on_mouse_in(const ws::event_t *e) override;
                virtual status_t            on_mouse_out(const ws::event_t *e) override;
                virtual status_t            on_mouse_down(const ws::event_t *e) override;
                virtual status_t            on_mouse_up(const ws::event_t *e) override;
                virtual status_t            on_mouse_move(const ws::event_t *e) override;
                virtual status_t            on_mouse_scroll(const ws::event_t *e) override;

                virtual status_t            on_begin_edit();
                virtual status_t            on_change();
                virtual status_t            on_end_edit();
        };

    } /* namespace tk */
} /* namespace lsp */




#endif /* LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHLINESEGMENT_H_ */
