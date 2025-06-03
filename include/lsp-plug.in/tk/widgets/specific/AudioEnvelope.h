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
                prop::Float             sAttackSlope;           // Normalized attack slope [0..1]
                prop::Float             sHoldTime;              // Normalized Hold time point [0..1]
                prop::Float             sDecayTime;             // Normalized Decay time point [0..1]
                prop::Float             sDecaySlope;            // Normalized Decay slope [0..1]
                prop::Float             sSlopeTime;             // Normalized Slope time point [0..1]
                prop::Float             sSlopeLevel;            // Normalized Slope level point [0..1]
                prop::Float             sSlopeSlope;            // Normalized Slope slope [0..1]
                prop::Float             sSustainLevel;          // Normalized Sustain level [0..1]
                prop::Float             sReleaseTime;           // Normalized Release time [0..1]
                prop::Float             sReleaseSlope;          // Normalized Release slope [0..1]
                prop::Boolean           sHold;                  // Enable hold time point
                prop::Boolean           sSlope;                 // Enable slope time point

                prop::Integer           sLineWidth;             // Line width
                prop::Color             sLineColor;             // Line color
                prop::Color             sFillColor;             // Fill color
                prop::Integer           sPointSize;             // Point size
                prop::Color             sPointColor;            // Point color

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

            private:
                friend class AudioSample;

            protected:
                prop::Float             sAttackTime;            // Normalized attack time [0..1]
                prop::Float             sAttackSlope;           // Normalized attack slope [0..1]
                prop::Float             sHoldTime;              // Normalized Hold time point [0..1]
                prop::Float             sDecayTime;             // Normalized Decay time point [0..1]
                prop::Float             sDecaySlope;            // Normalized Decay slope [0..1]
                prop::Float             sSlopeTime;             // Normalized Slope time point [0..1]
                prop::Float             sSlopeLevel;            // Normalized Slope level point [0..1]
                prop::Float             sSlopeSlope;            // Normalized Slope slope [0..1]
                prop::Float             sSustainLevel;          // Normalized Sustain level [0..1]
                prop::Float             sReleaseTime;           // Normalized Release time [0..1]
                prop::Float             sReleaseSlope;          // Normalized Release slope [0..1]
                prop::Boolean           sHold;                  // Enable hold time point
                prop::Boolean           sSlope;                 // Enable slope time point

                prop::Integer           sLineWidth;             // Line width
                prop::Color             sLineColor;             // Line color
                prop::Color             sFillColor;             // Fill color
                prop::Integer           sPointSize;             // Point size
                prop::Color             sPointColor;            // Point color

                prop::SizeConstraints   sConstraints;           // Size constraints
                prop::Integer           sBorder;                // Border size
                prop::Integer           sBorderRadius;          // Border radius
                prop::Boolean           sBorderFlat;            // Border is flat
                prop::Color             sBorderColor;           // Color of the border
                prop::Boolean           sGlass;                 // Draw glass
                prop::Color             sGlassColor;            // Color of the glass
                prop::Padding           sIPadding;              // Internal padding

            protected:
                static status_t         slot_on_submit(Widget *sender, void *ptr, void *data);

            protected:
                void                    draw_curve(ws::ISurface *surface, const ws::rectangle_t *rect);

            protected:
                virtual void            size_request(ws::size_limit_t *r) override;
                virtual void            property_changed(Property *prop) override;

            public:
                explicit AudioEnvelope(Display *dpy);
                AudioEnvelope(const AudioEnvelope &) = delete;
                AudioEnvelope(AudioEnvelope &&) = delete;
                virtual ~AudioEnvelope() override;

                AudioEnvelope & operator = (const AudioEnvelope &) = delete;
                AudioEnvelope & operator = (AudioEnvelope &) = delete;

                virtual status_t        init() override;

            public:
                LSP_TK_PROPERTY(Float,              attack_time,        &sAttackTime);
                LSP_TK_PROPERTY(Float,              attack_slope,       &sAttackSlope);
                LSP_TK_PROPERTY(Float,              hold_time,          &sHoldTime);
                LSP_TK_PROPERTY(Float,              decay_time,         &sDecayTime);
                LSP_TK_PROPERTY(Float,              decay_slope,        &sDecaySlope);
                LSP_TK_PROPERTY(Float,              slope_time,         &sSlopeTime);
                LSP_TK_PROPERTY(Float,              slope_level,        &sSlopeLevel);
                LSP_TK_PROPERTY(Float,              slope_slope,        &sSlopeSlope);
                LSP_TK_PROPERTY(Float,              sustain_level,      &sSustainLevel);
                LSP_TK_PROPERTY(Float,              release_time,       &sReleaseTime);
                LSP_TK_PROPERTY(Float,              release_slope,      &sReleaseSlope);
                LSP_TK_PROPERTY(Boolean,            hold_enabled,       &sHold);
                LSP_TK_PROPERTY(Boolean,            slope_enabled,      &sSlope);

                LSP_TK_PROPERTY(Integer,            line_width,         &sLineWidth);
                LSP_TK_PROPERTY(Color,              line_color,         &sLineColor);
                LSP_TK_PROPERTY(Color,              fill_color,         &sFillColor);
                LSP_TK_PROPERTY(Integer,            point_size,         &sPointSize);
                LSP_TK_PROPERTY(Color,              point_color,        &sPointColor);

                LSP_TK_PROPERTY(SizeConstraints,    constraints,        &sConstraints);
                LSP_TK_PROPERTY(Integer,            border_size,        &sBorder);
                LSP_TK_PROPERTY(Integer,            border_radius,      &sBorderRadius);
                LSP_TK_PROPERTY(Boolean,            border_flat,        &sBorderFlat);
                LSP_TK_PROPERTY(Color,              border_color,       &sBorderColor);
                LSP_TK_PROPERTY(Boolean,            glass,              &sGlass);
                LSP_TK_PROPERTY(Color,              glass_color,        &sGlassColor);
                LSP_TK_PROPERTY(Padding,            ipadding,           &sIPadding);

            public:
                virtual void            draw(ws::ISurface *s, bool force) override;
                virtual void            render(ws::ISurface *s, const ws::rectangle_t *area, bool force) override;

            public:
                virtual status_t        on_submit();
        };
    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_AUDIOENVELOPE_H_ */
