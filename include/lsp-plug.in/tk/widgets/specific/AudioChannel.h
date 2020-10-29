/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 28 сент. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_AUDIOCHANNEL_H_
#define LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_AUDIOCHANNEL_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class AudioSample;

        /**
         * Audio channel: implements one single audio channel
         */
        class AudioChannel: public Widget
        {
            public:
                static const w_class_t    metadata;

            private:
                AudioChannel & operator = (const AudioChannel &);
                friend class AudioSample;

            protected:
                prop::FloatArray        vSamples;

                prop::Integer           sFadeIn;            // Number of samples for fade-in
                prop::Integer           sFadeOut;           // Number of samples for fade-out
                prop::Integer           sWaveBorder;        // Wave border
                prop::Integer           sFadeInBorder;      // Fade in border
                prop::Integer           sFadeOutBorder;     // Fade out border
                prop::Integer           sLineWidth;         // Line width
                prop::Color             sColor;             // Color of the audio channel
                prop::Color             sLineColor;         // Line color
                prop::Color             sWaveBorderColor;   // Color of the wave border
                prop::Color             sFadeInColor;       // Color of fade-in
                prop::Color             sFadeOutColor;      // Fade-out color
                prop::Color             sFadeInBorderColor; // Color of fade-in
                prop::Color             sFadeOutBorderColor;// Fade-out color
                prop::SizeConstraints   sConstraints;       // Size constraints

            protected:
                virtual void            size_request(ws::size_limit_t *r);
                virtual void            property_changed(Property *prop);

                void                    draw_samples(const ws::rectangle_t *r, ws::ISurface *s, size_t samples, float scaling, float bright);
                void                    draw_fades(const ws::rectangle_t *r, ws::ISurface *s, size_t samples, float scaling, float bright);

            public:
                explicit AudioChannel(Display *dpy);
                virtual ~AudioChannel();

                virtual status_t        init();

            public:
                LSP_TK_PROPERTY(FloatArray,             samples,                &vSamples);
                LSP_TK_PROPERTY(Integer,                fade_in,                &sFadeIn);
                LSP_TK_PROPERTY(Integer,                fade_out,               &sFadeOut);
                LSP_TK_PROPERTY(Integer,                wave_border,            &sWaveBorder);
                LSP_TK_PROPERTY(Integer,                fade_in_border,         &sFadeInBorder);
                LSP_TK_PROPERTY(Integer,                fade_out_border,        &sFadeOutBorder);
                LSP_TK_PROPERTY(Integer,                line_width,             &sLineWidth);
                LSP_TK_PROPERTY(Color,                  color,                  &sColor);
                LSP_TK_PROPERTY(Color,                  wave_border_color,      &sWaveBorderColor);
                LSP_TK_PROPERTY(Color,                  fade_in_color,          &sFadeInColor);
                LSP_TK_PROPERTY(Color,                  fade_out_color,         &sFadeOutColor);
                LSP_TK_PROPERTY(Color,                  fade_in_border_color,   &sFadeInColor);
                LSP_TK_PROPERTY(Color,                  fade_out_border_color,  &sFadeOutColor);
                LSP_TK_PROPERTY(Color,                  line_color,             &sLineColor);
                LSP_TK_PROPERTY(SizeConstraints,        constraints,            &sConstraints);

            public:
                virtual void            draw(ws::ISurface *s);
        };
    }
}

#endif /* LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_AUDIOCHANNEL_H_ */
