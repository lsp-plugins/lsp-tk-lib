/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 3 июн. 2025 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_AUDIOENVELOPE_H_
#define LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_AUDIOENVELOPE_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class AudioSample;

        // Style definition
        namespace style
        {
            LSP_TK_STYLE_DEF_BEGIN(AudioEnvelope, Widget)
                prop::Float             sAttackTime;            // Normalized attack time [0..1]
                prop::Float             sAttackCurvature;       // Normalized attack curvature [0..1]
                prop::StepFloat         sAttackStep;            // Attack curvature step
                prop::Float             sHoldTime;              // Normalized Hold time point [0..1]
                prop::Float             sDecayTime;             // Normalized Decay time point [0..1]
                prop::Float             sDecayCurvature;        // Normalized Decay curvature [0..1]
                prop::StepFloat         sDecayStep;             // Decay curvature step
                prop::Float             sBreakLevel;            // Normalized Break level point [0..1]
                prop::Float             sSlopeTime;             // Normalized Slope time point [0..1]
                prop::Float             sSlopeCurvature;        // Normalized Slope curvature [0..1]
                prop::StepFloat         sSlopeStep;             // Slope curvature step
                prop::Float             sSustainLevel;          // Normalized Sustain level [0..1]
                prop::Float             sReleaseTime;           // Normalized Release time [0..1]
                prop::Float             sReleaseCurvature;      // Normalized Release slope [0..1]
                prop::StepFloat         sReleaseStep;           // Release slope step
                prop::Boolean           sHold;                  // Enable hold time point
                prop::Boolean           sBreak;                 // Enable slope time point
                prop::Boolean           sQuadPoint;             // Use quad point instead of circle
                prop::Boolean           sFill;                  // Use fill color for drawing
                prop::Boolean           sInvertMouseVScroll;    // Invert mouse vertical scroll

                prop::Integer           sLineWidth;             // Line width
                prop::Color             sLineColor;             // Line color
                prop::Color             sFillColor;             // Fill color
                prop::Integer           sPointSize;             // Point size
                prop::Color             sPointColor;            // Point color
                prop::Color             sPointHoverColor;       // Point hover color

                prop::SizeConstraints   sConstraints;           // Size constraints
                prop::Integer           sBorder;                // Border size
                prop::Integer           sBorderRadius;          // Border radius
                prop::Boolean           sBorderFlat;            // Border is flat
                prop::Color             sBorderColor;           // Color of the border
                prop::Boolean           sGlass;                 // Draw glass
                prop::Color             sGlassColor;            // Color of the glass
                prop::Padding           sIPadding;              // Internal padding

            LSP_TK_STYLE_DEF_END
        }

        /**
         * Audio envelope: implements audio envelope curve
         */
        class AudioEnvelope: public Widget
        {
            public:
                static const w_class_t    metadata;

            public:
                typedef void (*curve_function_t)(float *y, const float *x, size_t count, const AudioEnvelope *sender, void *data);

            private:
                friend class AudioSample;

                enum point_role_t
                {
                    PR_ATTACK,
                    PR_HOLD,
                    PR_DECAY_BREAK,
                    PR_SLOPE_SUSTAIN,
                    PR_RELEASE,

                    PR_TOTAL
                };

                typedef struct point_t
                {
                    prop::Float            *pX;
                    prop::Float            *pY;
                    prop::Float            *pZ;
                    prop::StepFloat        *pStep;
                    ssize_t                 nX;
                    ssize_t                 nY;
                    bool                    bVisible;
                } point_t;

            protected:
                ws::rectangle_t         sArea;                  // The area where the main contents is drawn
                ws::rectangle_t         sDrawArea;              // The actual draw area used
                point_t                 vPoints[PR_TOTAL];      // Edit points
                point_t                *pHandler;               // Current event handler
                size_t                  nBMask;                 // Mouse button mask
                ssize_t                 nLastX;                 // Last position X
                ssize_t                 nLastY;                 // Last position Y
                size_t                  nPointSize;             // Size of point
                bool                    bQuadPoint;             // Quadratic point

                prop::Float             sAttackTime;            // Normalized attack time [0..1]
                prop::Float             sAttackCurvature;       // Normalized attack slope [0..1]
                prop::StepFloat         sAttackStep;            // Attack curvature step
                prop::Float             sHoldTime;              // Normalized Hold time point [0..1]
                prop::Float             sDecayTime;             // Normalized Decay time point [0..1]
                prop::Float             sDecayCurvature;        // Normalized Decay slope [0..1]
                prop::StepFloat         sDecayStep;             // Decay curvature step
                prop::Float             sBreakLevel;            // Normalized Break level point [0..1]
                prop::Float             sSlopeTime;             // Normalized Slope time point [0..1]
                prop::Float             sSlopeCurvature;        // Normalized Slope slope point [0..1]
                prop::StepFloat         sSlopeStep;             // Slope curvature step
                prop::Float             sSustainLevel;          // Normalized Sustain level [0..1]
                prop::Float             sReleaseTime;           // Normalized Release time [0..1]
                prop::Float             sReleaseCurvature;      // Normalized Release slope [0..1]
                prop::StepFloat         sReleaseStep;           // Release slope step
                prop::Boolean           sHold;                  // Enable hold time point
                prop::Boolean           sBreak;                 // Enable slope time point
                prop::Boolean           sQuadPoint;             // Use quad point instead of circle
                prop::Boolean           sFill;                  // Use fill color for drawing
                prop::Boolean           sInvertMouseVScroll;    // Invert mouse vertical scroll

                prop::Integer           sLineWidth;             // Line width
                prop::Color             sLineColor;             // Line color
                prop::Color             sFillColor;             // Fill color
                prop::Integer           sPointSize;             // Point size
                prop::Color             sPointColor;            // Point color
                prop::Color             sPointHoverColor;       // Point hover color

                prop::SizeConstraints   sConstraints;           // Size constraints
                prop::Color             sColor;                 // Main color of the widget
                prop::Integer           sBorder;                // Border size
                prop::Integer           sBorderRadius;          // Border radius
                prop::Boolean           sBorderFlat;            // Border is flat
                prop::Color             sBorderColor;           // Color of the border
                prop::Boolean           sGlass;                 // Draw glass
                prop::Color             sGlassColor;            // Color of the glass
                prop::Padding           sIPadding;              // Internal padding

                ws::ISurface           *pGlass;                 // Surface to draw glass

                curve_function_t        pFunction;              // Curve function
                void                   *pFuncData;              // Curve function supplementary data
                float                  *vBuffer;                // Drawing buffer
                size_t                  nBufCapacity;           // Drawing buffer capacity in floats

            protected:
                static status_t         slot_on_submit(Widget *sender, void *ptr, void *data);

            protected:
                void                    do_destroy();
                void                    drop_glass();
                void                    draw_curve(ws::ISurface *surface, float bright, float scaling, const ws::rectangle_t *rect);
                void                    draw_point(ws::ISurface *s, const lsp::Color & color, const point_t *p);
                void                    sync_handler(const ws::event_t *e);
                void                    get_min_area_size(ssize_t *width, ssize_t *height, float scaling);
                point_t                *find_point(ssize_t x, ssize_t y);
                float                  *reserve_buffer(size_t count);

            protected:
                virtual void            size_request(ws::size_limit_t *r) override;
                virtual void            realize(const ws::rectangle_t *r) override;
                virtual void            property_changed(Property *prop) override;

            public:
                explicit AudioEnvelope(Display *dpy);
                AudioEnvelope(const AudioEnvelope &) = delete;
                AudioEnvelope(AudioEnvelope &&) = delete;
                virtual ~AudioEnvelope() override;

                AudioEnvelope & operator = (const AudioEnvelope &) = delete;
                AudioEnvelope & operator = (AudioEnvelope &) = delete;

                virtual status_t        init() override;
                virtual void            destroy() override;

            public:
                LSP_TK_PROPERTY(Float,              attack_time,            &sAttackTime);
                LSP_TK_PROPERTY(Float,              attack_curvature,       &sAttackCurvature);
                LSP_TK_PROPERTY(StepFloat,          attack_step,            &sAttackStep);
                LSP_TK_PROPERTY(Float,              hold_time,              &sHoldTime);
                LSP_TK_PROPERTY(Float,              decay_time,             &sDecayTime);
                LSP_TK_PROPERTY(Float,              decay_curvature,        &sDecayCurvature);
                LSP_TK_PROPERTY(StepFloat,          decay_step,             &sDecayStep);
                LSP_TK_PROPERTY(Float,              break_level,            &sBreakLevel);
                LSP_TK_PROPERTY(Float,              slope_time,             &sSlopeTime);
                LSP_TK_PROPERTY(Float,              slope_curvature,        &sSlopeCurvature);
                LSP_TK_PROPERTY(StepFloat,          slope_step,             &sSlopeStep);
                LSP_TK_PROPERTY(Float,              sustain_level,          &sSustainLevel);
                LSP_TK_PROPERTY(Float,              release_time,           &sReleaseTime);
                LSP_TK_PROPERTY(Float,              release_curvature,      &sReleaseCurvature);
                LSP_TK_PROPERTY(StepFloat,          release_step,           &sReleaseStep);
                LSP_TK_PROPERTY(Boolean,            hold_enabled,           &sHold);
                LSP_TK_PROPERTY(Boolean,            break_enabled,          &sBreak);
                LSP_TK_PROPERTY(Boolean,            quad_point,             &sQuadPoint);
                LSP_TK_PROPERTY(Boolean,            fill,                   &sFill);
                LSP_TK_PROPERTY(Boolean,            invert_mouse_vscroll,   &sInvertMouseVScroll)

                LSP_TK_PROPERTY(Integer,            line_width,             &sLineWidth);
                LSP_TK_PROPERTY(Color,              line_color,             &sLineColor);
                LSP_TK_PROPERTY(Color,              fill_color,             &sFillColor);
                LSP_TK_PROPERTY(Integer,            point_size,             &sPointSize);
                LSP_TK_PROPERTY(Color,              point_color,            &sPointColor);
                LSP_TK_PROPERTY(Color,              point_hover,            &sPointHoverColor);

                LSP_TK_PROPERTY(SizeConstraints,    constraints,            &sConstraints);
                LSP_TK_PROPERTY(Color,              color,                  &sColor);
                LSP_TK_PROPERTY(Integer,            border_size,            &sBorder);
                LSP_TK_PROPERTY(Integer,            border_radius,          &sBorderRadius);
                LSP_TK_PROPERTY(Boolean,            border_flat,            &sBorderFlat);
                LSP_TK_PROPERTY(Color,              border_color,           &sBorderColor);
                LSP_TK_PROPERTY(Boolean,            glass,                  &sGlass);
                LSP_TK_PROPERTY(Color,              glass_color,            &sGlassColor);
                LSP_TK_PROPERTY(Padding,            ipadding,               &sIPadding);

            public:
                void set_curve_function(curve_function_t function, void *data = NULL);

            public: // ui::IWidget
                virtual void            draw(ws::ISurface *s, bool force) override;
                virtual void            render(ws::ISurface *s, const ws::rectangle_t *area, bool force) override;
                virtual status_t        on_mouse_in(const ws::event_t *e) override;
                virtual status_t        on_mouse_out(const ws::event_t *e) override;
                virtual status_t        on_mouse_down(const ws::event_t *e) override;
                virtual status_t        on_mouse_up(const ws::event_t *e) override;
                virtual status_t        on_mouse_move(const ws::event_t *e) override;
                virtual status_t        on_mouse_scroll(const ws::event_t *e) override;

            public:
                virtual status_t        on_submit();
        };
    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_AUDIOENVELOPE_H_ */
