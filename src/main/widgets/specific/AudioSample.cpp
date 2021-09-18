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

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/tk/helpers/draw.h>
#include <lsp-plug.in/common/debug.h>
#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/stdlib/math.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            static const char *label_text_color[] =
            {
                "label.0.text.color",
                "label.1.text.color",
                "label.2.text.color",
                "label.3.text.color",
                "label.4.text.color"
            };

            static const char *label_layout[] =
            {
                "label.0.layout",
                "label.1.layout",
                "label.2.layout",
                "label.3.layout",
                "label.4.layout"
            };

            static const char *label_text_layout[] =
            {
                "label.0.text.layout",
                "label.1.text.layout",
                "label.2.text.layout",
                "label.3.text.layout",
                "label.4.text.layout"
            };

            static const char *label_visible[] =
            {
                "label.0.visible",
                "label.1.visible",
                "label.2.visible",
                "label.3.visible",
                "label.4.visible"
            };

            LSP_TK_STYLE_IMPL_BEGIN(AudioSample, WidgetContainer)
                // Bind
                sWaveBorder.bind("wave.border", this);
                sFadeInBorder.bind("fade_in.border", this);
                sFadeOutBorder.bind("fade_out.border", this);
                sLineWidth.bind("line.width", this);
                sLineColor.bind("line.color", this);
                sConstraints.bind("size.constraints", this);
                sActive.bind("active", this);
                sSGroups.bind("stereo_groups", this);
                sMainTextLayout.bind("main.text.layout", this);
                sMainFont.bind("main.font", this);
                sMainColor.bind("main.color", this);
                sMainVisibility.bind("main.visibility", this);
                sLabelFont.bind("label.font", this);
                sLabelBgColor.bind("label.bg.color", this);
                sLabelRadius.bind("label.radius", this);
                sBorder.bind("border.size", this);
                sBorderRadius.bind("border.radius", this);
                sBorderFlat.bind("border.flat", this);
                sGlass.bind("glass", this);
                sColor.bind("color", this);
                sBorderColor.bind("border.color", this);
                sGlassColor.bind("glass.color", this);
                sIPadding.bind("ipadding", this);

                for (size_t i=0; i<LABELS; ++i)
                {
                    sLabelColor[i].bind(label_text_color[i], this);
                    sLabelLayout[i].bind(label_layout[i], this);
                    sLabelTextLayout[i].bind(label_text_layout[i], this);
                    sLabelVisibility[i].bind(label_visible[i], this);
                }

                // Configure
                sWaveBorder.set(1);
                sFadeInBorder.set(1);
                sFadeOutBorder.set(1);
                sLineWidth.set(1);
                sLineColor.set("#ffffff");
                sConstraints.set_all(-1);
                sActive.set(false);
                sSGroups.set(false);

                sMainTextLayout.set(0.0f, 0.0f);
                sMainFont.set_size(16.0f);
                sMainFont.set_bold(true);
                sMainColor.set("#00ff00");
                sMainVisibility.set(false);
                sLabelFont.set_size(10.0f);
                sLabelBgColor.set("#44000000");
                sLabelRadius.set(4);

                sBorder.set(4);
                sBorderRadius.set(12);
                sBorderFlat.set(false);
                sGlass.set(true);
                sColor.set("#000000");
                sBorderColor.set("#000000");
                sGlassColor.set("#ffffff");
                sIPadding.set(1);

                for (size_t i=0; i<LABELS; ++i)
                {
                    sLabelColor[i].set("#ffffff");
                    sLabelLayout[i].set(0.0f, 0.0f, 0.0f, 0.0f);
                    sLabelTextLayout[i].set(0.0f, 0.0f);
                    sLabelVisibility[i].set(false);
                }
                // Override
                sMainFont.override();
                sLabelFont.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(AudioSample, "AudioSample", "root");
        }

        const w_class_t AudioSample::metadata           = { "AudioSample", &WidgetContainer::metadata };

        AudioSample::AudioSample(Display *dpy):
            WidgetContainer(dpy),
            vChannels(&sProperties, &sIListener),
            sWaveBorder(&sProperties),
            sFadeInBorder(&sProperties),
            sFadeOutBorder(&sProperties),
            sLineWidth(&sProperties),
            sLineColor(&sProperties),
            sConstraints(&sProperties),
            sActive(&sProperties),
            sSGroups(&sProperties),
            sMainText(&sProperties),
            sMainTextLayout(&sProperties),
            sMainFont(&sProperties),
            sMainColor(&sProperties),
            sMainVisibility(&sProperties),
            sLabelFont(&sProperties),
            sLabelBgColor(&sProperties),
            sLabelRadius(&sProperties),
            sBorder(&sProperties),
            sBorderRadius(&sProperties),
            sBorderFlat(&sProperties),
            sGlass(&sProperties),
            sColor(&sProperties),
            sBorderColor(&sProperties),
            sGlassColor(&sProperties),
            sIPadding(&sProperties),
            sPopup(&sProperties)
        {
            for (size_t i=0; i<LABELS; ++i)
            {
                sLabel[i].listener(&sProperties);
                sLabelColor[i].listener(&sProperties);
                sLabelLayout[i].listener(&sProperties);
                sLabelTextLayout[i].listener(&sProperties);
                sLabelVisibility[i].listener(&sProperties);
            }

            nBMask              = 0;
            nXFlags             = 0;

            sGraph.nLeft        = 0;
            sGraph.nTop         = 0;
            sGraph.nWidth       = 0;
            sGraph.nHeight      = 0;

            pGlass              = NULL;

            pClass              = &metadata;
        }

        AudioSample::~AudioSample()
        {
            nFlags     |= FINALIZED;
            do_destroy();
        }

        void AudioSample::destroy()
        {
            nFlags     |= FINALIZED;
            WidgetContainer::destroy();
            do_destroy();
        }

        void AudioSample::drop_glass()
        {
            if (pGlass != NULL)
            {
                pGlass->destroy();
                delete pGlass;
                pGlass      = NULL;
            }
        }

        void AudioSample::do_destroy()
        {
            // Unlink all items
            for (size_t i=0, n=vChannels.size(); i<n; ++i)
            {
                AudioChannel *item   = vChannels.get(i);
                if (item != NULL)
                    unlink_widget(item);
            }

            // Drop glass
            drop_glass();

            // Flush containers
            vChannels.flush();
            vVisible.flush();
        }

        status_t AudioSample::init()
        {
            status_t res = WidgetContainer::init();
            if (res != STATUS_OK)
                return res;

            // Init listener
            sIListener.bind_all(this, on_add_item, on_remove_item);

            // Bind properties
            sWaveBorder.bind("wave.border", &sStyle);
            sFadeInBorder.bind("fade_in.border", &sStyle);
            sFadeOutBorder.bind("fade_out.border", &sStyle);
            sLineWidth.bind("line.width", &sStyle);
            sLineColor.bind("line.color", &sStyle);
            sConstraints.bind("size.constraints", &sStyle);
            sActive.bind("active", &sStyle);
            sSGroups.bind("stereo_groups", &sStyle);
            sMainText.bind(&sStyle, pDisplay->dictionary());
            sMainTextLayout.bind("main.text.layout", &sStyle);
            sMainFont.bind("main.font", &sStyle);
            sMainColor.bind("main.color", &sStyle);
            sMainVisibility.bind("main.visibility", &sStyle);
            sLabelFont.bind("label.font", &sStyle);
            sLabelBgColor.bind("label.bg.color", &sStyle);
            sLabelRadius.bind("label.radius", &sStyle);
            sBorder.bind("border.size", &sStyle);
            sBorderRadius.bind("border.radius", &sStyle);
            sBorderFlat.bind("border.flat", &sStyle);
            sGlass.bind("glass", &sStyle);
            sColor.bind("color", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sGlassColor.bind("glass.color", &sStyle);
            sIPadding.bind("ipadding", &sStyle);

            for (size_t i=0; i<LABELS; ++i)
            {
                LSPString id;

                sLabel[i].bind(&sStyle, pDisplay->dictionary());
                sLabelColor[i].bind(style::label_text_color[i], &sStyle);
                sLabelLayout[i].bind(style::label_layout[i], &sStyle);
                sLabelTextLayout[i].bind(style::label_text_layout[i], &sStyle);
                sLabelVisibility[i].bind(style::label_visible[i], &sStyle);
            }

            // Add slots
            handler_id_t id = 0;
            if (id >= 0) id = sSlots.add(SLOT_SUBMIT, slot_on_submit, self());

            return (id >= 0) ? STATUS_OK : -id;
        }

        void AudioSample::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);

            if (vChannels.is(prop))
                query_resize();
            if (sWaveBorder.is(prop))
                query_resize();
            if (sFadeInBorder.is(prop))
                query_draw();
            if (sFadeOutBorder.is(prop))
                query_draw();
            if (sLineWidth.is(prop))
                query_draw();
            if (sLineColor.is(prop))
                query_draw();
            if (sConstraints.is(prop))
                query_resize();
            if (sSGroups.is(prop))
                query_resize();

            if ((sMainText.is(prop)) && (sMainVisibility.get()))
                query_draw();
            if ((sMainTextLayout.is(prop)) && (sMainVisibility.get()))
                query_draw();
            if ((sMainFont.is(prop)) && (sMainVisibility.get()))
                query_draw();
            if (sMainVisibility.is(prop))
                query_draw();
            if (sLabelFont.is(prop))
                query_draw();
            if (sLabelBgColor.is(prop))
                query_draw();
            if (sLabelRadius.is(prop))
                query_draw();

            if (sBorder.is(prop))
                query_resize();
            if (sBorderRadius.is(prop))
                query_resize();
            if (sBorderFlat.is(prop))
                query_draw();
            if (sGlass.is(prop))
                query_draw();
            if (sColor.is(prop))
                query_draw();
            if (sBorderColor.is(prop))
                query_draw();
            if (sGlassColor.is(prop))
                query_draw();
            if (sIPadding.is(prop))
                query_resize();

            for (size_t i=0; i<LABELS; ++i)
            {
                if (sLabelColor[i].is(prop))
                    query_draw();
                if (sLabelLayout[i].is(prop))
                    query_draw();
                if (sLabelTextLayout[i].is(prop))
                    query_draw();
                if (sLabelVisibility[i].is(prop))
                    query_draw();
            }
        }

        void AudioSample::size_request(ws::size_limit_t *r)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());
            float fscaling      = lsp_max(0.0f, scaling * sFontScaling.get());
            bool sgroups        = sSGroups.get();

            lltl::parray<AudioChannel> channels;
            get_visible_items(&channels);

            // Estimate the size of area for drawing samples
            ws::size_limit_t sl;
            r->nMinWidth        = 0;
            r->nMinHeight       = 0;
            r->nMaxWidth        = -1;
            r->nMaxHeight       = -1;
            r->nPreWidth        = -1;
            r->nPreHeight       = -1;

            if (sMainVisibility.get())
            {
                ws::text_parameters_t tp;
                LSPString text;
                sMainText.format(&text);
                sMainFont.get_multitext_parameters(pDisplay, &tp, fscaling, &text);
                r->nMinWidth            = tp.Width;
                r->nMinHeight           = tp.Height;
            }
            else
            {
                for (size_t i=0, n=channels.size(); i<n; ++i)
                {
                    AudioChannel *c     = channels.uget(i);
                    c->constraints()->compute(&sl, scaling);
                    ssize_t h           = lsp_max(0, sl.nMinHeight);
                    r->nMinWidth        = lsp_max(r->nMinWidth, sl.nMinWidth);
                    r->nMinHeight      += (sgroups) ? (h >> 1) : h;
                }
            }

            // Add padding to the rectangle
            sIPadding.add(r, scaling);

            // Compute additional space around the sample area
            float xr        = lsp_max(0.0f, sBorderRadius.get() * scaling); // external radius
            float bw        = lsp_max(0.0f, sBorder.get() * scaling);       // border size
            float ir        = lsp_max(0.0f, xr - bw);                       // internal radius
            float bp        = (1.0f - M_SQRT1_2) * ir;                      // padding to not to cross internal radius
            ssize_t padding = ceilf(bp + bw);
            ssize_t wh      = lsp_max(padding * 2, xr * 2);                 // minimum possible width and height

            r->nMinWidth    = lsp_max(r->nMinWidth  + padding * 2, wh);
            r->nMinHeight   = lsp_max(r->nMinHeight + padding * 2, wh);

            // Apply size constraints
            sConstraints.apply(r, scaling);
        }

        void AudioSample::realize(const ws::rectangle_t *r)
        {
            // Call parent class to realize
            WidgetContainer::realize(r);
            lltl::parray<AudioChannel> channels;
            get_visible_items(&channels);

            // Compute the size of area
            float scaling   = lsp_max(0.0f, sScaling.get());
            float xr        = lsp_max(0.0f, ceilf(sBorderRadius.get() * scaling));  // external radius
            float bw        = lsp_max(0.0f, ceilf(sBorder.get() * scaling));        // border size
            float ir        = lsp_max(0.0f, xr - bw);                               // internal radius
            ssize_t padding = ceilf((1.0f - M_SQRT1_2) * ir + bw);                  // padding of internal area

            sGraph.nLeft    = r->nLeft   + padding;
            sGraph.nTop     = r->nTop    + padding;
            sGraph.nWidth   = r->nWidth  - padding*2;
            sGraph.nHeight  = r->nHeight - padding*2;

            sIPadding.enter(&sGraph, scaling);
            vVisible.swap(&channels);
        }

        void AudioSample::draw_channel1(const ws::rectangle_t *r, ws::ISurface *s, AudioChannel *c, size_t samples)
        {
            // Check limits
            if ((samples <= 0) || (r->nWidth <= 1) || (r->nHeight <= 1))
                return;

            float scaling       = lsp_max(0.0f, sScaling.get());
            float bright        = sBrightness.get();

            // Init decimation buffer
            ssize_t n_draw      = lsp_min(ssize_t(samples), r->nWidth);
            size_t n_points     = n_draw + 2;
            size_t n_decim      = lsp::align_size(n_points, 16); // 2 additional points at start and end

            // Try to allocate memory
            uint8_t *data       = NULL;
            float *x            = lsp::alloc_aligned<float>(data, n_decim * 2);
            float *y            = &x[n_decim];
            if (x == NULL)
                return;

            // Form the x and y values
            FloatArray *vsamp   = &c->vSamples;
            float border        = (sWaveBorder.get() > 0) ? lsp_max(1.0f, sWaveBorder.get() * scaling) : 0.0f;
            float dx            = lsp_max(1.0f, float(r->nWidth) / float(samples));
            float kx            = lsp_max(1.0f, float(samples) / float(r->nWidth));
            float ky            = -0.5f * (r->nHeight - border);
            float sy            = r->nTop + r->nHeight * 0.5f;

            x[0]                = -1.0f;
            y[0]                = sy;
            x[n_points-1]       = r->nWidth;
            y[n_points-1]       = sy;

            for (ssize_t i=1; i <= n_draw; ++i)
            {
                ssize_t xx          = i - 1;
                x[i]                = xx * dx;
                y[i]                = sy + ky * vsamp->get(ssize_t(xx * kx));
            }

            // Draw the poly
            lsp::Color fill(c->sColor);
            lsp::Color wire(c->sWaveBorderColor);
            fill.scale_lch_luminance(bright);
            wire.scale_lch_luminance(bright);

            bool aa             = s->set_antialiasing(true);
            s->draw_poly(fill, wire, border, x, y, n_points);
            s->set_antialiasing(aa);

            // Free allocated data
            lsp::free_aligned(data);
        }

        void AudioSample::draw_fades1(const ws::rectangle_t *r, ws::ISurface *s, AudioChannel *c, size_t samples)
        {
            // Check limits
            if ((samples <= 0) || (r->nWidth <= 1) || (r->nHeight <= 1))
                return;

            float scaling       = lsp_max(0.0f, sScaling.get());
            float bright        = sBrightness.get();

            float x[6], y[6];

            bool aa             = s->set_antialiasing(true);

            // Draw fade in
            if (c->sFadeIn.get() > 0)
            {
                float border        = (sFadeInBorder.get() > 0) ? lsp_max(1.0f, sFadeInBorder.get() * scaling) : 0.0f;
                float xx            = float(c->sFadeIn.get() * r->nWidth) / float(samples);

                // Form the arrays
                x[0]                = r->nLeft;
                x[1]                = xx;
                x[2]                = x[0];
                x[3]                = x[1];
                x[4]                = x[0];
                x[5]                = x[0];

                y[0]                = r->nTop;
                y[1]                = y[0];
                y[2]                = y[0] + (r->nHeight >> 1);
                y[3]                = y[0] + r->nHeight;
                y[4]                = y[3];
                y[5]                = y[0];

                lsp::Color fill(c->sFadeInColor);
                lsp::Color wire(c->sFadeInBorderColor);
                fill.scale_lch_luminance(bright);
                wire.scale_lch_luminance(bright);

                s->draw_poly(fill, wire, border, x, y, 6);
            }

            // Draw fade out
            if (c->sFadeOut.get() > 0)
            {
                float border        = (sFadeOutBorder.get() > 0) ? lsp_max(1.0f, sFadeOutBorder.get() * scaling) : 0.0f;
                float xx            = r->nLeft + r->nWidth - float(c->sFadeOut.get() * r->nWidth) / float(samples);

                // Form the arrays
                x[0]                = r->nLeft + r->nWidth;
                x[1]                = xx;
                x[2]                = x[0];
                x[3]                = x[1];
                x[4]                = x[0];
                x[5]                = x[0];

                y[0]                = r->nTop;
                y[1]                = y[0];
                y[2]                = y[0] + (r->nHeight >> 1);
                y[3]                = y[0] + r->nHeight;
                y[4]                = y[3];
                y[5]                = y[0];

                lsp::Color fill(c->sFadeOutColor);
                lsp::Color wire(c->sFadeOutBorderColor);
                fill.scale_lch_luminance(bright);
                wire.scale_lch_luminance(bright);

                s->draw_poly(fill, wire, border, x, y, 6);
            }

            s->set_antialiasing(aa);
        }

        void AudioSample::draw_channel2(const ws::rectangle_t *r, ws::ISurface *s, AudioChannel *c, size_t samples, bool down)
        {
            // Check limits
            if ((samples <= 0) || (r->nWidth <= 1) || (r->nHeight <= 1))
                return;

            float scaling       = lsp_max(0.0f, sScaling.get());
            float bright        = sBrightness.get();

            // Init decimation buffer
            ssize_t n_draw      = lsp_min(ssize_t(samples), r->nWidth);
            size_t n_points     = n_draw + 2;
            size_t n_decim      = lsp::align_size(n_points, 16); // 2 additional points at start and end

            // Try to allocate memory
            uint8_t *data       = NULL;
            float *x            = lsp::alloc_aligned<float>(data, n_decim * 2);
            float *y            = &x[n_decim];
            if (x == NULL)
                return;

            bool aa             = s->set_antialiasing(true);

            // Form the x and y values for sample 1
            FloatArray *vsamp   = &c->vSamples;
            float border        = (sWaveBorder.get() > 0) ? lsp_max(1.0f, sWaveBorder.get() * scaling) : 0.0f;
            float dx            = lsp_max(1.0f, float(r->nWidth) / float(samples));
            float kx            = lsp_max(1.0f, float(samples) / float(r->nWidth));
            float ky            = ((down) ? 1.0f : -1.0f) * (r->nHeight - border);
            float sy            = (down) ? r->nTop : r->nTop + r->nHeight;

            x[0]                = -1.0f;
            y[0]                = sy;
            x[n_points-1]       = r->nWidth;
            y[n_points-1]       = sy;

            for (ssize_t i=1; i <= n_draw; ++i)
            {
                ssize_t xx          = i - 1;
                x[i]                = xx * dx;
                y[i]                = sy + ky * fabs(vsamp->get(ssize_t(xx * kx)));
            }

            // Draw the poly
            lsp::Color fill(c->sColor);
            lsp::Color wire(c->sWaveBorderColor);
            fill.scale_lch_luminance(bright);
            wire.scale_lch_luminance(bright);
            s->draw_poly(fill, wire, border, x, y, n_points);

            // Free allocated data
            s->set_antialiasing(aa);
            lsp::free_aligned(data);
        }

        void AudioSample::draw_fades2(const ws::rectangle_t *r, ws::ISurface *s, AudioChannel *c, size_t samples, bool down)
        {
            // Check limits
            if ((samples <= 0) || (r->nWidth <= 1) || (r->nHeight <= 1))
                return;

            float scaling       = lsp_max(0.0f, sScaling.get());
            float bright        = sBrightness.get();

            float x[4], y[4];

            bool aa             = s->set_antialiasing(true);

            // Draw fade in
            if (c->sFadeIn.get() > 0)
            {
                float border        = (sFadeInBorder.get() > 0) ? lsp_max(1.0f, sFadeInBorder.get() * scaling) : 0.0f;
                float xx            = float(c->sFadeIn.get() * r->nWidth) / float(samples);

                // Form the arrays
                x[0]                = r->nLeft;
                x[1]                = xx;
                x[2]                = x[0];
                x[3]                = x[0];

                y[0]                = (down) ? r->nTop : r->nTop + r->nHeight;
                y[1]                = (down) ? r->nTop + r->nHeight : r->nTop;
                y[2]                = y[1];
                y[3]                = y[0];

                lsp::Color fill(c->sFadeInColor);
                lsp::Color wire(c->sFadeInBorderColor);
                fill.scale_lch_luminance(bright);
                wire.scale_lch_luminance(bright);

                s->draw_poly(fill, wire, border, x, y, 4);
            }

            // Draw fade out
            if (c->sFadeOut.get() > 0)
            {
                float border        = (sFadeOutBorder.get() > 0) ? lsp_max(1.0f, sFadeOutBorder.get() * scaling) : 0.0f;
                float xx            = r->nLeft + r->nWidth - float(c->sFadeOut.get() * r->nWidth) / float(samples);

                // Form the arrays
                x[0]                = r->nLeft + r->nWidth;
                x[1]                = xx;
                x[2]                = x[0];
                x[3]                = x[0];

                y[0]                = (down) ? r->nTop : r->nTop + r->nHeight;
                y[1]                = (down) ? r->nTop + r->nHeight : r->nTop;
                y[2]                = y[1];
                y[3]                = y[0];

                lsp::Color fill(c->sFadeOutColor);
                lsp::Color wire(c->sFadeOutBorderColor);
                fill.scale_lch_luminance(bright);
                wire.scale_lch_luminance(bright);

                s->draw_poly(fill, wire, border, x, y, 4);
            }

            s->set_antialiasing(aa);
        }

        void AudioSample::draw_main_text(ws::ISurface *s)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());
            float fscaling      = lsp_max(0.0f, scaling * sFontScaling.get());
            float bright        = sBrightness.get();

            LSPString text;
            ws::font_parameters_t fp;
            ws::text_parameters_t tp;
            ws::rectangle_t xr;

            // Draw main text
            xr.nLeft            = 0;
            xr.nTop             = 0;
            xr.nWidth           = sGraph.nWidth;
            xr.nHeight          = sGraph.nHeight;

            // Get main text parameters
            sMainText.format(&text);
            sMainFont.get_parameters(s, fscaling, &fp);
            sMainFont.get_multitext_parameters(s, &tp, fscaling, &text);

            // Draw main text
            lsp::Color color(sMainColor);
            color.scale_lch_luminance(bright);

            draw_multiline_text(
                s, &sMainFont, &xr, color, &fp, &tp,
                sMainTextLayout.halign(), sMainTextLayout.valign(), fscaling,
                &text
            );
        }

        void AudioSample::draw_label(ws::ISurface *s, size_t idx)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());
            float fscaling      = lsp_max(0.0f, scaling * sFontScaling.get());
            float bright        = sBrightness.get();

            ws::font_parameters_t fp;
            ws::text_parameters_t tp;
            ws::rectangle_t       xr, gr;
            ws::size_limit_t      sr;
            LSPString text;

            // Get label text parameters
            sLabel[idx].format(&text);
            sLabelFont.get_parameters(s, fscaling, &fp);
            sLabelFont.get_multitext_parameters(s, &tp, fscaling, &text);

            ssize_t rad         = (sLabelRadius.get() > 0) ? lsp_max(1.0f, sLabelRadius.get() * scaling) : 0.0f;
            size_t padding      = ceilf(rad * M_SQRT1_2);

            // Allocate space for the label
            sr.nMinWidth        = tp.Width  + padding * 2;
            sr.nMinHeight       = tp.Height + padding * 2;
            sr.nMaxWidth        = sGraph.nWidth;
            sr.nMaxHeight       = sGraph.nHeight;
            sr.nPreWidth        = -1;
            sr.nPreHeight       = -1;

            gr.nLeft            = 0;
            gr.nTop             = 0;
            gr.nWidth           = sGraph.nWidth;
            gr.nHeight          = sGraph.nHeight;

            sLabelLayout[idx].apply(&xr, &gr, &sr);

            bool aa             = s->set_antialiasing(true);

            // Draw label background
            lsp::Color color(sLabelBgColor);
            color.scale_lch_luminance(bright);
            s->fill_round_rect(color, SURFMASK_ALL_CORNER, rad, &xr);

            // Draw label text
            xr.nLeft           += padding;
            xr.nTop            += padding;
            xr.nWidth          -= padding * 2;
            xr.nHeight         -= padding * 2;

            color.copy(sLabelColor[idx]);
            color.scale_lch_luminance(bright);

            draw_multiline_text(
                s, &sLabelFont, &xr, color, &fp, &tp,
                sLabelTextLayout[idx].halign(), sLabelTextLayout[idx].valign(), fscaling,
                &text
            );

            s->set_antialiasing(aa);
        }

        void AudioSample::draw(ws::ISurface *s)
        {
            // Main parameters
            float scaling       = lsp_max(0.0f, sScaling.get());
            float bright        = sBrightness.get();

            // Draw background
            lsp::Color color(sColor);
            color.scale_lch_luminance(bright);
            s->clear(color);

            // Draw main text if it is required to be shown
            if (sMainVisibility.get())
            {
                draw_main_text(s);
                return;
            }

            // Draw all samples
            size_t items = vVisible.size();
            if (items > 0)
            {
                ws::rectangle_t xr;
                xr.nLeft            = 0;
                xr.nWidth           = sGraph.nWidth;
                ssize_t line_w      = (sLineWidth.get() > 0) ? lsp_max(1.0f, sLineWidth.get() * scaling) : 0.0f;

                // Estimate overall number of samples
                size_t samples  = 0;
                for (size_t i=0; i<items; ++i)
                {
                    AudioChannel *c = vVisible.uget(i);
                    samples         = lsp_max(samples, c->samples()->size());
                }

                xr.nHeight          = sGraph.nHeight / items;
                ssize_t place       = xr.nHeight * items;
                ssize_t y           = (sGraph.nHeight - place) >> 1;

                // Draw depending of the view type
                if (sSGroups.get())
                {
                    // Draw stereo samples
                    xr.nTop             = y;
                    for (size_t i=0; i<items; ++i)
                    {
                        AudioChannel *c     = vVisible.uget(i);
                        draw_channel2(&xr, s, c, samples, i & 1);
                        xr.nTop            += xr.nHeight;
                    }

                    // Draw fades
                    xr.nTop             = y;
                    for (size_t i=0; i<items; ++i)
                    {
                        AudioChannel *c     = vVisible.uget(i);
                        draw_fades2(&xr, s, c, samples, i & 1);
                        xr.nTop            += xr.nHeight;
                    }

                    // Draw lines
                    color.copy(sLineColor);
                    xr.nTop             = y + xr.nHeight;
                    color.scale_lch_luminance(bright);
                    bool aa             = s->set_antialiasing(false);
                    for (size_t i=0; i<items; i += 2)
                    {
                        s->line(xr.nLeft, xr.nTop, xr.nLeft + xr.nWidth, xr.nTop, line_w, color);
                        xr.nTop            += xr.nHeight * 2;
                    }
                    s->set_antialiasing(aa);
                }
                else
                {
                    // Draw monophonic samples
                    xr.nTop             = y;
                    for (size_t i=0; i<items; ++i)
                    {
                        AudioChannel *c     = vVisible.uget(i);
                        draw_channel1(&xr, s, c, samples);
                        xr.nTop            += xr.nHeight;
                    }

                    // Draw fades
                    xr.nTop             = y;
                    for (size_t i=0; i<items; ++i)
                    {
                        AudioChannel *c     = vVisible.uget(i);
                        draw_fades1(&xr, s, c, samples);
                        xr.nTop            += xr.nHeight;
                    }

                    // Draw lines
                    color.copy(sLineColor);
                    xr.nTop             = y;
                    color.scale_lch_luminance(bright);
                    float sy            = xr.nHeight * 0.5f;
                    bool aa             = s->set_antialiasing(false);
                    for (size_t i=0; i<items; ++i)
                    {
                        s->line(xr.nLeft, xr.nTop + sy, xr.nLeft + xr.nWidth, xr.nTop + sy, line_w, color);
                        xr.nTop            += xr.nHeight;
                    }
                    s->set_antialiasing(aa);
                }
            }

            // Draw labels
            for (size_t i=0; i<LABELS; ++i)
            {
                if (!sLabelVisibility[i].get())
                    continue;
                draw_label(s, i);
            }
        }

        void AudioSample::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            if (nFlags & REDRAW_SURFACE)
                force = true;

            float scaling   = lsp_max(0.0f, sScaling.get());
            float xr        = lsp_max(0.0f, sBorderRadius.get() * scaling); // external radius
            float bw        = lsp_max(0.0f, sBorder.get() * scaling);       // border size
            float bright    = sBrightness.get();
            bool pressed    = nXFlags & XF_DOWN;

            // Prepare palette
            ws::ISurface *cv;
            lsp::Color color(sColor);
            lsp::Color bg_color;
            get_actual_bg_color(bg_color);
            color.scale_lch_luminance(bright);

            s->clip_begin(area);
            {
                // Draw widget background
                s->fill_rect(bg_color, &sSize);

                bool aa = s->set_antialiasing(true);
                s->fill_round_rect(color, SURFMASK_ALL_CORNER, xr, &sSize);

                // Get surface of widget
                cv  = get_surface(s, sGraph.nWidth, sGraph.nHeight);
                if (cv != NULL)
                {
                    // Draw the surface
                    if (pressed)
                    {
                        ssize_t xbw         = lsp_max(1.0f, scaling);
                        ws::rectangle_t xr  = sGraph;
                        xr.nLeft           += xbw;
                        xr.nTop            += xbw;
                        xr.nWidth           = lsp_max(0, xr.nWidth  - xbw * 2);
                        xr.nHeight          = lsp_max(0, xr.nHeight - xbw * 2);

                        s->draw(cv, &xr);
                    }
                    else
                        s->draw(cv, sGraph.nLeft, sGraph.nTop);
                }

                // Draw the glass and the border
                color.copy(sGlassColor);
                bg_color.copy(sColor);
                color.scale_lch_luminance(bright);
                bg_color.scale_lch_luminance(bright);

                // Update border width if widget is in pressed state
                if (pressed)
                    bw         += lsp_max(1.0f, scaling);

                bool flat   = sBorderFlat.get();
                if (sGlass.get())
                {
                    cv = create_border_glass(&pGlass, s,
                            color, bg_color,
                            SURFMASK_ALL_CORNER, bw, xr,
                            sSize.nWidth, sSize.nHeight, flat
                        );
                    if (cv != NULL)
                        s->draw(cv, sSize.nLeft, sSize.nTop);
                }
                else
                {
                    drop_glass();
                    draw_border(s, bg_color, SURFMASK_ALL_CORNER, bw, xr, &sSize, flat);
                }

                s->set_antialiasing(aa);
            }
            s->clip_end();
        }

        void AudioSample::hide_widget()
        {
            WidgetContainer::hide_widget();
            drop_glass();
        }

        void AudioSample::get_visible_items(lltl::parray<AudioChannel> *dst)
        {
            for (size_t i=0, n=vChannels.size(); i<n; ++i)
            {
                AudioChannel *c = vChannels.get(i);
                if ((c == NULL) || (!c->visibility()->get()))
                    continue;
                if (!dst->add(c))
                    return;
            }
        }

        void AudioSample::on_add_item(void *obj, Property *prop, void *w)
        {
            AudioChannel *item = widget_ptrcast<AudioChannel>(w);
            if (item == NULL)
                return;

            AudioSample *_this = widget_ptrcast<AudioSample>(obj);
            if (_this == NULL)
                return;

            item->set_parent(_this);
            _this->query_resize();
        }

        void AudioSample::on_remove_item(void *obj, Property *prop, void *w)
        {
            AudioChannel *item = widget_ptrcast<AudioChannel>(w);
            if (item == NULL)
                return;

            AudioSample *_this = widget_ptrcast<AudioSample>(obj);
            if (_this == NULL)
                return;

            // Remove widget from supplementary structures
            _this->unlink_widget(item);
            _this->query_resize();
        }

        status_t AudioSample::add(Widget *widget)
        {
            AudioChannel *item  = widget_cast<AudioChannel>(widget);
            return (item != NULL) ? vChannels.add(item) : STATUS_BAD_TYPE;
        }

        status_t AudioSample::remove(Widget *child)
        {
            AudioChannel *item  = widget_cast<AudioChannel>(child);
            return (item != NULL) ? vChannels.premove(item) : STATUS_BAD_TYPE;
        }

        status_t AudioSample::remove_all()
        {
            vChannels.clear();
            return STATUS_OK;
        }

        status_t AudioSample::on_mouse_down(const ws::event_t *e)
        {
            // Handle mouse event
            float scaling   = lsp_max(0.0f, sScaling.get());
            float xr        = lsp_max(0.0f, sBorderRadius.get() * scaling); // external radius

            if (nBMask == 0)
            {
                if (Position::rinside(&sSize, e->nLeft, e->nTop, xr))
                {
                    if (e->nCode == ws::MCB_LEFT)
                        nXFlags    |= XF_LBUTTON;
                    else if (e->nCode == ws::MCB_RIGHT)
                        nXFlags    |= XF_RBUTTON;
                }
            }

            nBMask         |= 1 << e->nCode;

            return handle_mouse_move(e);
        }

        status_t AudioSample::on_mouse_up(const ws::event_t *e)
        {
            size_t mask = nBMask;
            nBMask &= ~(1 << e->nCode);

            if (mask != (1U << e->nCode))
                return handle_mouse_move(e);

            // Last button released, process the vent
            size_t flags    = nXFlags;
            nXFlags         = 0;

            float scaling   = lsp_max(0.0f, sScaling.get());
            float xr        = lsp_max(0.0f, sBorderRadius.get() * scaling); // external radius

            if (Position::rinside(&sSize, e->nLeft, e->nTop, xr))
            {
                if ((e->nCode == ws::MCB_LEFT) && (flags & XF_LBUTTON))
                {
                    if (sActive.get())
                        sSlots.execute(SLOT_SUBMIT, this, NULL);
                }
                else if ((e->nCode == ws::MCB_RIGHT) && (flags & XF_RBUTTON))
                {
                    Menu *popup = sPopup.get();
                    if (popup != NULL)
                    {
                        ws::rectangle_t sr;
                        Window *wnd = widget_cast<Window>(this->toplevel());
                        wnd->get_screen_rectangle(&sr);
                        sr.nLeft       += e->nLeft;
                        sr.nTop        += e->nTop;
                        popup->show(this, sr.nLeft, sr.nTop);
                    }
                }
            }

            if (flags != nXFlags)
            {
                drop_glass();
                query_draw();
            }

            return STATUS_OK;
        }

        status_t AudioSample::on_mouse_move(const ws::event_t *e)
        {
            // Widget is not active?
            if (nXFlags == 0)
                return STATUS_OK;

            return handle_mouse_move(e);
        }

        status_t AudioSample::handle_mouse_move(const ws::event_t *e)
        {
            lsp_trace("nBMask = %x, nXFlags = %x", int(nBMask), int(nXFlags));
            if (nXFlags & XF_LBUTTON)
            {
                float scaling   = lsp_max(0.0f, sScaling.get());
                float xr        = lsp_max(0.0f, sBorderRadius.get() * scaling); // external radius
                bool pressed    = (sActive.get()) && (nBMask == ws::MCF_LEFT) && (Position::rinside(&sSize, e->nLeft, e->nTop, xr));

                size_t old      = nXFlags;
                nXFlags         = lsp_setflag(nXFlags, XF_DOWN, pressed);

                if (old != nXFlags)
                {
                    drop_glass();
                    query_draw();
                }
            }

            return STATUS_OK;
        }

        status_t AudioSample::on_before_popup(Menu *menu)
        {
            return STATUS_OK;
        }

        status_t AudioSample::on_popup(Menu *menu)
        {
            return STATUS_OK;
        }

        status_t AudioSample::on_submit()
        {
            return STATUS_OK;
        }

        status_t AudioSample::slot_on_before_popup(Widget *sender, void *ptr, void *data)
        {
            AudioSample *_this = widget_ptrcast<AudioSample>(ptr);
            Menu *_menu = widget_ptrcast<Menu>(sender);
            return (_this != NULL) ? _this->on_before_popup(_menu) : STATUS_BAD_ARGUMENTS;
        }

        status_t AudioSample::slot_on_popup(Widget *sender, void *ptr, void *data)
        {
            AudioSample *_this = widget_ptrcast<AudioSample>(ptr);
            Menu *_menu = widget_ptrcast<Menu>(sender);
            return (_this != NULL) ? _this->on_popup(_menu) : STATUS_BAD_ARGUMENTS;
        }

        status_t AudioSample::slot_on_submit(Widget *sender, void *ptr, void *data)
        {
            AudioSample *_this = widget_ptrcast<AudioSample>(ptr);
            return (_this != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }
    }
}


