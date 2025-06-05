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

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/tk/helpers/draw.h>
#include <lsp-plug.in/common/alloc.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(AudioEnvelope, Widget)
                sAttackTime.bind("attack.time", this);
                sAttackCurvature.bind("attack.curvature", this);
                sHoldTime.bind("hold.time", this);
                sDecayTime.bind("decay.time", this);
                sDecayCurvature.bind("decay.curvature", this);
                sBreakTime.bind("break.time", this);
                sBreakLevel.bind("break.level", this);
                sSlopeTime.bind("slope.time", this);
                sSlopeCurvature.bind("slope.curvature", this);
                sSustainLevel.bind("sustain.level", this);
                sReleaseTime.bind("release.time", this);
                sReleaseCurvature.bind("relese.curvature", this);
                sHold.bind("hold.enabled", this);
                sCurvature.bind("break.enabled", this);

                sLineWidth.bind("line.width", this);
                sLineColor.bind("line.color", this);
                sFillColor.bind("fill.color", this);
                sPointSize.bind("point.size", this);
                sPointColor.bind("point.color", this);

                sConstraints.bind("size.constraints", this);
                sBorder.bind("border.size", this);
                sBorderRadius.bind("border.radius", this);
                sBorderFlat.bind("border.flat", this);
                sBorderColor.bind("border.color", this);
                sGlass.bind("glass", this);
                sGlassColor.bind("glass.color", this);
                sIPadding.bind("ipadding", this);

                // Configure
                sAttackTime.set(0.1f);
                sAttackCurvature.set(0.5f);
                sHoldTime.set(0.2f);
                sDecayTime.set(0.4f);
                sDecayCurvature.set(0.5f);
                sBreakLevel.set(0.4f);
                sBreakTime.set(0.5f);
                sSlopeTime.set(0.7f);
                sSlopeCurvature.set(0.5f);
                sSustainLevel.set(0.5f);
                sReleaseTime.set(0.8f);
                sReleaseCurvature.set(0.5f);
                sHold.set(false);
                sCurvature.set(false);

                sLineWidth.set(1);
                sLineColor.set_rgb24(0xffff00);
                sFillColor.set_rgba32(0x80ffff00);
                sPointSize.set(3);
                sPointColor.set_rgb24(0xffcc0000);

                sBorder.set(4);
                sBorderRadius.set(12);
                sBorderFlat.set(false);
                sBorderColor.set("#000000");
                sGlass.set(true);
                sGlassColor.set("#ffffff");
                sIPadding.set(1);

            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(AudioEnvelope, "AudioEnvelope", "root");
        }

        const w_class_t AudioEnvelope::metadata      = { "AudioEnvelope", &Widget::metadata };

        AudioEnvelope::AudioEnvelope(Display *dpy):
            Widget(dpy),
            sAttackTime(&sProperties),
            sAttackCurvature(&sProperties),
            sHoldTime(&sProperties),
            sDecayTime(&sProperties),
            sDecayCurvature(&sProperties),
            sBreakTime(&sProperties),
            sBreakLevel(&sProperties),
            sSlopeTime(&sProperties),
            sSlopeCurvature(&sProperties),
            sSustainLevel(&sProperties),
            sReleaseTime(&sProperties),
            sReleaseCurvature(&sProperties),
            sHold(&sProperties),
            sBreak(&sProperties),
            sLineWidth(&sProperties),
            sLineColor(&sProperties),
            sFillColor(&sProperties),
            sPointSize(&sProperties),
            sPointColor(&sProperties),
            sConstraints(&sProperties),
            sBorder(&sProperties),
            sBorderRadius(&sProperties),
            sBorderFlat(&sProperties),
            sBorderColor(&sProperties),
            sGlass(&sProperties),
            sGlassColor(&sProperties),
            sIPadding(&sProperties)
        {
            pClass          = &metadata;

            sArea.nLeft     = 0;
            sArea.nTop      = 0;
            sArea.nWidth    = 0;
            sArea.nHeight   = 0;

            pGlass          = NULL;
        }

        AudioEnvelope::~AudioEnvelope()
        {
            nFlags     |= FINALIZED;
            do_destroy();
        }

        void AudioEnvelope::destroy()
        {
            nFlags     |= FINALIZED;
            Widget::destroy();
            do_destroy();
        }

        void AudioEnvelope::do_destroy()
        {
            drop_glass();
        }

        void AudioEnvelope::drop_glass()
        {
            if (pGlass != NULL)
            {
                pGlass->destroy();
                delete pGlass;
                pGlass      = NULL;
            }
        }

        status_t AudioEnvelope::init()
        {
            status_t res = Widget::init();
            if (res != STATUS_OK)
                return res;

            // Bind properties
            sAttackTime.bind("attack.time", &sStyle);
            sAttackCurvature.bind("attack.curvature", &sStyle);
            sHoldTime.bind("hold.time", &sStyle);
            sDecayTime.bind("decay.time", &sStyle);
            sDecayCurvature.bind("decay.curvature", &sStyle);
            sBreakTime.bind("break.time", &sStyle);
            sBreakLevel.bind("break.level", &sStyle);
            sSlopeTime.bind("slope.time", &sStyle);
            sSlopeCurvature.bind("slope.curvature", &sStyle);
            sSustainLevel.bind("sustain.level", &sStyle);
            sReleaseTime.bind("release.time", &sStyle);
            sReleaseCurvature.bind("relese.curvature", &sStyle);
            sHold.bind("hold.enabled", &sStyle);
            sBreak.bind("Curvature.enabled", &sStyle);

            sLineWidth.bind("line.width", &sStyle);
            sLineColor.bind("line.color", &sStyle);
            sFillColor.bind("fill.color", &sStyle);
            sPointSize.bind("point.size", &sStyle);
            sPointColor.bind("point.color", &sStyle);

            sConstraints.bind("size.constraints", &sStyle);
            sBorder.bind("border.size", &sStyle);
            sBorderRadius.bind("border.radius", &sStyle);
            sBorderFlat.bind("border.flat", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sGlass.bind("glass", &sStyle);
            sGlassColor.bind("glass.color", &sStyle);
            sIPadding.bind("ipadding", &sStyle);

            // Add slots
            handler_id_t id = sSlots.add(SLOT_SUBMIT, slot_on_submit, self());

            return (id >= 0) ? STATUS_OK : -id;
        }

        void AudioEnvelope::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (prop->one_of(sAttackTime, sAttackCurvature, sHoldTime, sDecayTime, sDecayCurvature,
                sBreakTime, sBreakLevel, sSlopeTime, sSlopeCurvature, sSustainLevel, sReleaseTime, sReleaseCurvature,
                sHold, sBreak))
                query_draw();

            if (prop->one_of(sLineWidth, sLineColor, sFillColor, sPointSize, sPointColor, sBorderColor, sGlass, sGlassColor))
                query_draw();

            if (prop->one_of(sConstraints, sBorder, sBorderRadius, sBorderFlat, sIPadding))
                query_resize();
        }

        void AudioEnvelope::size_request(ws::size_limit_t *r)
        {
            const float scaling     = lsp_max(0.0f, sScaling.get());
            const size_t p_size     = lsp_max(1.0f, sPointSize.get());
            const size_t a_size     = lsp_min(p_size, 4u) * 8;

            // Estimate the size of area for drawing samples
            r->nMinWidth        = a_size;
            r->nMinHeight       = a_size;
            r->nMaxWidth        = -1;
            r->nMaxHeight       = -1;
            r->nPreWidth        = -1;
            r->nPreHeight       = -1;

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

        void AudioEnvelope::realize(const ws::rectangle_t *r)
        {
            // Call parent class to realize
            Widget::realize(r);

            // Compute the size of area
            float scaling   = lsp_max(0.0f, sScaling.get());
            float xr        = lsp_max(0.0f, ceilf(sBorderRadius.get() * scaling));  // external radius
            float bw        = lsp_max(0.0f, ceilf(sBorder.get() * scaling));        // border size
            float ir        = lsp_max(0.0f, xr - bw);                               // internal radius
            ssize_t padding = ceilf((1.0f - M_SQRT1_2) * ir + bw);                  // padding of internal area

            sArea.nLeft     = r->nLeft   + padding;
            sArea.nTop      = r->nTop    + padding;
            sArea.nWidth    = r->nWidth  - padding*2;
            sArea.nHeight   = r->nHeight - padding*2;

            sIPadding.enter(&sArea, scaling);
        }


        void AudioEnvelope::draw_curve(ws::ISurface *surface, float bright, const ws::rectangle_t *rect)
        {
            // TODO
        }

        void AudioEnvelope::draw(ws::ISurface *s, bool force)
        {
            // Main parameters
//            float scaling       = lsp_max(0.0f, sScaling.get());
            const float bright        = select_brightness();

            // Draw background
            lsp::Color color(sColor);
            color.scale_lch_luminance(bright);
            s->clear(color);

            // Draw curve and points
            ws::rectangle_t cr;
            cr.nLeft        = 0;
            cr.nTop         = 0;
            cr.nWidth       = sArea.nWidth;
            cr.nHeight      = sArea.nHeight;

            draw_curve(s, bright, &cr);
        }

        void AudioEnvelope::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            const float scaling     = lsp_max(0.0f, sScaling.get());
            const float xr          = lsp_max(0.0f, sBorderRadius.get() * scaling); // external radius
            const float bw          = lsp_max(0.0f, sBorder.get() * scaling);       // border size
            const float bright      = select_brightness();

            // Prepare palette
            ws::ISurface *cv;
            lsp::Color color(sColor);
            lsp::Color bg_color;
            get_actual_bg_color(bg_color);
            color.scale_lch_luminance(bright);

            s->clip_begin(area);
            {
                // Draw widget background
                s->fill_rect(bg_color, SURFMASK_NONE, 0.0f, &sSize);

                const bool aa           = s->set_antialiasing(true);
                lsp_finally { s->set_antialiasing(aa); };

                s->fill_rect(color, SURFMASK_ALL_CORNER, xr, &sSize);

                // Get surface of widget
                cv  = get_surface(s, sArea.nWidth, sArea.nHeight);
                if (cv != NULL)
                    s->draw(cv, sArea.nLeft, sArea.nTop, 1.0f, 1.0f, 0.0f);

                // Draw the glass and the border
                color.copy(sGlassColor);
                bg_color.copy(sColor);
                color.scale_lch_luminance(bright);
                bg_color.scale_lch_luminance(bright);

                const bool flat         = sBorderFlat.get();
                if (sGlass.get())
                {
                    cv = create_border_glass(&pGlass, s,
                            color, bg_color,
                            SURFMASK_ALL_CORNER, bw, xr,
                            sSize.nWidth, sSize.nHeight, flat
                        );
                    if (cv != NULL)
                        s->draw(cv, sSize.nLeft, sSize.nTop, 1.0f, 1.0f, 0.0f);
                }
                else
                {
                    drop_glass();
                    draw_border(s, bg_color, SURFMASK_ALL_CORNER, bw, xr, &sSize, flat);
                }
            }
            s->clip_end();
        }

        status_t AudioEnvelope::on_submit()
        {
            return STATUS_OK;
        }

        status_t AudioEnvelope::slot_on_submit(Widget *sender, void *ptr, void *data)
        {
            AudioEnvelope *self = widget_ptrcast<AudioEnvelope>(ptr);
            return (self != NULL) ? self->on_submit() : STATUS_BAD_ARGUMENTS;
        }
    } /* namespace tk */
} /* namespace lsp */



