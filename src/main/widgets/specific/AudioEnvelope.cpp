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
#include <lsp-plug.in/common/debug.h>
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
                sPointHoverColor.bind("point.hover.color", this);

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
                sPointSize.set(4);
                sPointColor.set_rgb24(0xffff00);
                sPointHoverColor.set_rgb24(0xff0000);

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
            sPointHoverColor(&sProperties),
            sConstraints(&sProperties),
            sBorder(&sProperties),
            sBorderRadius(&sProperties),
            sBorderFlat(&sProperties),
            sBorderColor(&sProperties),
            sGlass(&sProperties),
            sGlassColor(&sProperties),
            sIPadding(&sProperties)
        {
            pClass              = &metadata;

            sArea.nLeft         = 0;
            sArea.nTop          = 0;
            sArea.nWidth        = 0;
            sArea.nHeight       = 0;

            sDrawArea.nLeft     = 0;
            sDrawArea.nTop      = 0;
            sDrawArea.nWidth    = 0;
            sDrawArea.nHeight   = 0;

            for (size_t i=0; i<PR_TOTAL; ++i)
            {
                point_t *p          = &vPoints[i];

                p->pX               = NULL;
                p->pY               = NULL;
                p->pZ               = NULL;
                p->nX               = 0;
                p->nY               = 0;
                p->bVisible         = true;
            }

            vPoints[PR_ATTACK].pX           = &sAttackTime;
            vPoints[PR_ATTACK].pZ           = &sAttackCurvature;
            vPoints[PR_HOLD].pX             = &sHoldTime;
            vPoints[PR_DECAY_BREAK].pX      = &sDecayTime;
            vPoints[PR_DECAY_BREAK].pZ      = &sDecayCurvature;
            vPoints[PR_SLOPE_SUSTAIN].pX    = &sSlopeTime;
            vPoints[PR_SLOPE_SUSTAIN].pZ    = &sSlopeCurvature;
            vPoints[PR_RELEASE].pX          = &sReleaseTime;
            vPoints[PR_RELEASE].pZ          = &sReleaseCurvature;

            pHandler            = NULL;
            nBMask              = 0;
            nLastX              = 0;
            nLastY              = 0;

            pGlass              = NULL;
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
            sPointHoverColor.bind("point.hover.color", &sStyle);

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
                sBreakLevel, sSlopeTime, sSlopeCurvature, sSustainLevel, sReleaseTime, sReleaseCurvature))
                query_draw();

            if (prop->is(sHold))
            {
                vPoints[PR_HOLD].bVisible = sHold.get();
                query_draw();
            }

            if (prop->is(sBreak))
            {
                if (sBreak.get())
                {
                    vPoints[PR_DECAY_BREAK].pY          = &sBreakLevel;
                    vPoints[PR_SLOPE_SUSTAIN].pY        = &sSustainLevel;
                    vPoints[PR_RELEASE].pY              = &sSustainLevel;
                    vPoints[PR_SLOPE_SUSTAIN].bVisible  = true;
                }
                else
                {
                    vPoints[PR_DECAY_BREAK].pY          = &sSustainLevel;
                    vPoints[PR_SLOPE_SUSTAIN].pY        = NULL;
                    vPoints[PR_RELEASE].pY              = &sSustainLevel;
                    vPoints[PR_SLOPE_SUSTAIN].bVisible  = false;
                }
                query_draw();
            }

            if (prop->one_of(sLineWidth, sLineColor, sFillColor, sPointSize, sPointColor, sPointHoverColor,
                sBorderColor, sGlass, sGlassColor))
                query_draw();

            if (prop->one_of(sConstraints, sBorder, sBorderRadius, sBorderFlat, sIPadding))
                query_resize();
        }

        void AudioEnvelope::size_request(ws::size_limit_t *r)
        {
            const float scaling     = lsp_max(0.0f, sScaling.get());
            const size_t p_size     = lsp_max(1.0f, sPointSize.get() * scaling);
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

        AudioEnvelope::point_t *AudioEnvelope::find_point(ssize_t x, ssize_t y)
        {
            lsp_trace("x=%d, y=%d", int(x), int(y));

            for (size_t i=0; i<PR_TOTAL; ++i)
            {
                point_t *p          = &vPoints[i];
                if (!p->bVisible)
                    continue;

                const ssize_t dx    = x - p->nX;
                const ssize_t dy    = y - p->nY;
                const ssize_t r     = p->nSize;
                if ((dx*dx + dy*dy) <= r*r)
                {
                    lsp_trace("dx=%d, dy=%d, r=%d", int(dx), int(dy), int(r));
                    return p;
                }
            }

            return NULL;
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
        }

        void AudioEnvelope::draw_curve(ws::ISurface *surface, float bright, float scaling, const ws::rectangle_t *rect)
        {
            const size_t p_size     = lsp_max(1.0f, sPointSize.get() * scaling);

            sDrawArea       = *rect;
            const float ox  = rect->nLeft;
            const float oy  = rect->nTop + rect->nHeight;
            const float dx  = rect->nWidth - 1.0f;      // Inteval 0..1 includes 1
            const float dy  = 1.0f - rect->nHeight;     // Inteval 0..1 includes 1

            lsp::Color pcolor(sPointColor);
            lsp::Color ph_color(sPointHoverColor);
            pcolor.scale_lch_luminance(bright);
            ph_color.scale_hsl_lightness(bright);

            // Release and draw points
            for (size_t i=0; i<PR_TOTAL; ++i)
            {
                point_t *p      = &vPoints[i];
                const float x   = (p->pX != NULL) ? p->pX->get() : 0.0f;
                const float y   = (p->pY != NULL) ? p->pY->get() : 1.0f;

                p->nX           = ox + dx * x;
                p->nY           = oy + dy * y;
                p->nSize        = p_size;

                lsp_trace("this=%p point[%d] = {%d, %d}", this, int(i), int(p->nX), int(p->nY));
            }

            // Draw points
            for (size_t i=0; i<PR_TOTAL; ++i)
            {
                point_t *p      = &vPoints[i];
                if ((p->bVisible) && (p != pHandler))
                    surface->fill_circle(pcolor, p->nX, p->nY, p->nSize);
            }
            if ((pHandler != NULL) && (pHandler->bVisible))
                surface->fill_circle(ph_color, pHandler->nX, pHandler->nY, pHandler->nSize);
        }

        void AudioEnvelope::draw(ws::ISurface *s, bool force)
        {
            // Main parameters
            const float scaling     = lsp_max(0.0f, sScaling.get());
            const float bright      = select_brightness();

            // Draw background
            lsp::Color color(sColor);
            color.scale_lch_luminance(bright);
            s->clear(color);

            // Draw curve and points
            ws::rectangle_t cr = sArea;

            sIPadding.enter(&cr, scaling);
            const ws::point_t origin = s->set_origin(-sArea.nLeft, -sArea.nTop);
            lsp_finally { s->set_origin(origin); };

            draw_curve(s, bright, scaling, &cr);

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

        void AudioEnvelope::sync_handler(const ws::event_t *e)
        {
            point_t *point = find_point(e->nLeft, e->nTop);
            if (point == pHandler)
                return;

            pHandler    = point;
            query_draw();
            lsp_trace("this=%p handler set to %p", this, pHandler);
        }

        status_t AudioEnvelope::on_mouse_in(const ws::event_t *e)
        {
            sync_handler(e);
            return STATUS_OK;
        }

        status_t AudioEnvelope::on_mouse_out(const ws::event_t *e)
        {
            if (pHandler != NULL)
            {
                pHandler    = NULL;
                query_draw();
                lsp_trace("this=%p handler set to %p", this, pHandler);
            }
            return STATUS_OK;
        }

        status_t AudioEnvelope::on_mouse_down(const ws::event_t *e)
        {
            if (nBMask == 0)
            {
                sync_handler(e);

                nLastX      = e->nLeft;
                nLastY      = e->nTop;
            }

            nBMask         |= size_t(1) << e->nCode;

            return STATUS_OK;
        }

        status_t AudioEnvelope::on_mouse_up(const ws::event_t *e)
        {
            nBMask         &= ~(size_t(1) << e->nCode);

            return STATUS_OK;
        }

        status_t AudioEnvelope::on_mouse_move(const ws::event_t *e)
        {
            lsp_trace("x=%d, y=%d", int(e->nLeft), int(e->nTop));

            if (nBMask == 0)
            {
                sync_handler(e);
                return STATUS_OK;
            }

            if ((nBMask != ws::MCF_LEFT) || (pHandler == NULL))
                return STATUS_OK;

            // Apply changes
            bool changed        = false;

            // Modify X coordinate if possible
            if (pHandler->pX != NULL)
            {
                const float ox      = sDrawArea.nLeft;
                const float dx      = sDrawArea.nWidth - 1.0f;      // Inteval 0..1 includes 1
                const float x       = lsp_limit((e->nLeft - ox) / dx, 0.0f, 1.0f);

                for (size_t i=0; i<PR_TOTAL; ++i)
                {
                    point_t *p          = &vPoints[i];
                    if (p->pX == NULL)
                        continue;

                    const float px      = p->pX->get();
                    if (((p < pHandler) && (px > x)) ||
                        ((p > pHandler) && (px < x)) ||
                        ((p == pHandler) && (px != x)))
                    {
                        p->pX->commit_value(x);
                        changed             = true;
                    }
                }
            }

            // Modify Y coordinate if possible
            if (pHandler->pY != NULL)
            {
                const float oy      = sDrawArea.nTop + sDrawArea.nHeight;
                const float dy      = 1.0f - sDrawArea.nHeight;     // Inteval 0..1 includes 1
                const float y       = lsp_limit((e->nTop - oy) / dy, 0.0f, 1.0f);

                pHandler->pY->commit_value(y);
                changed             = true;
            }

            if (changed)
            {
                sSlots.slot(SLOT_SUBMIT)->execute(this, NULL);
                query_draw();
            }

            return STATUS_OK;
        }

        status_t AudioEnvelope::on_mouse_scroll(const ws::event_t *e)
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



