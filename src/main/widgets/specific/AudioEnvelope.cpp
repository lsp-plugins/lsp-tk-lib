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
                sAttackSlope.bind("attack.slope", this);
                sHoldTime.bind("hold.time", this);
                sDecayTime.bind("decay.time", this);
                sDecaySlope.bind("decay.slope", this);
                sSlopeTime.bind("slope.time", this);
                sSlopeLevel.bind("slope.level", this);
                sSlopeSlope.bind("slope.slope", this);
                sSustainLevel.bind("sustain.level", this);
                sReleaseTime.bind("release.time", this);
                sReleaseSlope.bind("relese.slope", this);
                sHold.bind("hold.enabled", this);
                sSlope.bind("slope.enabled", this);

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
                sAttackSlope.set(0.5f);
                sHoldTime.set(0.2f);
                sDecayTime.set(0.4f);
                sDecaySlope.set(0.5f);
                sSlopeTime.set(0.5f);
                sSlopeLevel.set(0.5f);
                sSlopeSlope.set(0.5f);
                sSustainLevel.set(0.5f);
                sReleaseTime.set(0.8f);
                sReleaseSlope.set(0.5f);
                sHold.set(false);
                sSlope.set(false);

                sLineWidth.set(1);
                sLineColor.set_rgb24(0xffff00);
                sFillColor.set_rgba32(0x80ffff00);
                sPointSize.set(3);
                sPointColor.set_rgb24(0xffcc0000);

                sConstraints.bind("size.constraints", this);
                sBorder.bind("border.size", this);
                sBorderRadius.bind("border.radius", this);
                sBorderFlat.bind("border.flat", this);
                sBorderColor.bind("border.color", this);
                sGlass.bind("glass", this);
                sGlassColor.bind("glass.color", this);
                sIPadding.bind("ipadding", this);

            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(AudioEnvelope, "AudioEnvelope", "root");
        }

        const w_class_t AudioEnvelope::metadata      = { "AudioEnvelope", &Widget::metadata };

        AudioEnvelope::AudioEnvelope(Display *dpy):
            Widget(dpy),
            sAttackTime(&sProperties),
            sAttackSlope(&sProperties),
            sHoldTime(&sProperties),
            sDecayTime(&sProperties),
            sDecaySlope(&sProperties),
            sSlopeTime(&sProperties),
            sSlopeLevel(&sProperties),
            sSlopeSlope(&sProperties),
            sSustainLevel(&sProperties),
            sReleaseTime(&sProperties),
            sReleaseSlope(&sProperties),
            sHold(&sProperties),
            sSlope(&sProperties),
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
        }

        AudioEnvelope::~AudioEnvelope()
        {
            nFlags     |= FINALIZED;
        }

        status_t AudioEnvelope::init()
        {
            status_t res = Widget::init();
            if (res != STATUS_OK)
                return res;

            // Bind properties
            sAttackTime.bind("attack.time", &sStyle);
            sAttackSlope.bind("attack.slope", &sStyle);
            sHoldTime.bind("hold.time", &sStyle);
            sDecayTime.bind("decay.time", &sStyle);
            sDecaySlope.bind("decay.slope", &sStyle);
            sSlopeTime.bind("slope.time", &sStyle);
            sSlopeLevel.bind("slope.level", &sStyle);
            sSlopeSlope.bind("slope.slope", &sStyle);
            sSustainLevel.bind("sustain.level", &sStyle);
            sReleaseTime.bind("release.time", &sStyle);
            sReleaseSlope.bind("relese.slope", &sStyle);
            sHold.bind("hold.enabled", &sStyle);
            sSlope.bind("slope.enabled", &sStyle);

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

            if (prop->one_of(sAttackTime, sAttackSlope, sHoldTime, sDecayTime, sDecaySlope, sSlopeTime, sSlopeLevel,
                sSlopeSlope, sSustainLevel, sReleaseTime, sReleaseSlope, sHold, sSlope))
                query_draw();

            if (prop->one_of(sLineWidth, sLineColor, sFillColor, sPointSize, sPointColor, sBorderColor, sGlass, sGlassColor))
                query_draw();

            if (prop->one_of(sConstraints, sBorder, sBorderRadius, sBorderFlat, sIPadding))
                query_resize();
        }

        void AudioEnvelope::size_request(ws::size_limit_t *r)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());

            // Estimate the size of area for drawing samples
            r->nMinWidth        = 0;
            r->nMinHeight       = 0;
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

        void AudioEnvelope::draw_curve(ws::ISurface *surface, const ws::rectangle_t *rect)
        {
            // TODO
        }

        void AudioEnvelope::draw(ws::ISurface *s, bool force)
        {
            // TODO
        }

        void AudioEnvelope::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            // TODO
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



