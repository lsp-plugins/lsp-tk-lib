/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 20 авг. 2020 г.
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
#include <lsp-plug.in/dsp/dsp.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        STYLE_INITIALIZER_BEGIN(GraphFrameBuffer, GraphItem);

            prop::GraphFrameData::init("data", style, 0, 0, 0.0f, 1.0f, 0.0f);
            prop::Float::init("transparency", style, 0.5f);
            prop::Integer::init("angle", style, 0);
            prop::Float::init("hpos", style, -1.0f);
            prop::Float::init("vpos", style, 1.0f);
            prop::Float::init("hscale", style, 1.0f);
            prop::Float::init("vscale", style, 1.0f);
            prop::Color::init("color", style, "#ff0000");
            prop::GraphFrameFunction::init("function", style, GFF_DEFAULT);

        STYLE_INITIALIZER_END(GraphFrameBuffer, "GraphFrameBuffer");
        LSP_BUILTIN_STYLE(GraphFrameBuffer);

        const w_class_t GraphFrameBuffer::metadata             = { "GraphFrameBuffer", &GraphItem::metadata };

        GraphFrameBuffer::GraphFrameBuffer(Display *dpy):
            GraphItem(dpy),
            sData(&sProperties),
            sTransparency(&sProperties),
            sAngle(&sProperties),
            sHPos(&sProperties),
            sVPos(&sProperties),
            sHScale(&sProperties),
            sVScale(&sProperties),
            sColor(&sProperties),
            sFunction(&sProperties)
        {
            bClear              = true;
            nRows               = 0;
            nCols               = 0;
            pCalcColor          = &GraphFrameBuffer::calc_rainbow_color;
            fRGBA               = NULL;
            pfRGBA              = NULL;
            nCapacity           = 0;

            pClass              = &metadata;
        }

        GraphFrameBuffer::~GraphFrameBuffer()
        {
            nFlags     |= FINALIZED;
            destroy_data();
        }

        void GraphFrameBuffer::destroy()
        {
            nFlags     |= FINALIZED;
            GraphItem::destroy();
            destroy_data();
        }

        void GraphFrameBuffer::destroy_data()
        {
            if (pfRGBA != NULL)
                lsp::free_aligned(pfRGBA);

            fRGBA               = NULL;
            pfRGBA              = NULL;
            nCapacity           = 0;
        }

        status_t GraphFrameBuffer::init()
        {
            status_t res        = GraphItem::init();
            if (res != STATUS_OK)
                return res;

            // Init style
            sData.bind("data", &sStyle);
            sTransparency.bind("transparency", &sStyle);
            sAngle.bind("angle", &sStyle);
            sHPos.bind("hpos", &sStyle);
            sVPos.bind("vpos", &sStyle);
            sHScale.bind("hscale", &sStyle);
            sVScale.bind("vscale", &sStyle);
            sColor.bind("color", &sStyle);
            sFunction.bind("function", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sData.init(sclass, 0, 0, 0.0f, 1.0f, 0.0f);
                sTransparency.init(sclass, 0.5f);
                sAngle.init(sclass, 0);
                sHPos.init(sclass, -1.0f);
                sVPos.init(sclass, 1.0f);
                sHScale.init(sclass, 1.0f);
                sVScale.init(sclass, 1.0f);
                sColor.init(sclass, "#ff0000");
                sFunction.init(sclass, GFF_DEFAULT);
            }

            return STATUS_OK;
        }

        void GraphFrameBuffer::property_changed(Property *prop)
        {
            GraphItem::property_changed(prop);

            if (sVisibility.is(prop))
            {
                if (sVisibility.get())
                    bClear      = true;
            }

            if (sData.is(prop))
            {
                if ((nRows != sData.rows()) || (nCols != sData.columns()))
                    bClear  = true;

                nRows       = sData.rows();
                nCols       = sData.columns();
                query_draw();
            }
            if (sTransparency.is(prop))
                query_draw();
            if (sAngle.is(prop))
                query_draw();
            if (sHPos.is(prop))
                query_draw();
            if (sVPos.is(prop))
                query_draw();
            if (sHScale.is(prop))
                query_draw();
            if (sVScale.is(prop))
                query_draw();
            if (sColor.is(prop))
            {
                bClear      = true;
                query_draw();
            }
            if (sFunction.is(prop))
            {
                calc_color_t func = &GraphFrameBuffer::calc_rainbow_color;
                switch (sFunction.get())
                {
                    case GFF_RAINBOW:       func = &GraphFrameBuffer::calc_rainbow_color;     break;
                    case GFF_FOG:           func = &GraphFrameBuffer::calc_fog_color;         break;
                    case GFF_COLOR:         func = &GraphFrameBuffer::calc_color;             break;
                    case GFF_LIGHTNESS:     func = &GraphFrameBuffer::calc_lightness;         break;
                    case GFF_LIGHTNESS2:    func = &GraphFrameBuffer::calc_lightness2;        break;
                    default:                func = &GraphFrameBuffer::calc_rainbow_color;     break;
                }

                if (pCalcColor != func)
                {
                    bClear      = true;
                    query_draw();
                }
            }
        }

        void GraphFrameBuffer::draw(ws::ISurface *s)
        {
            // Need to deploy new changes?
            size_t changes = (bClear) ? sData.rows() : sData.changes();
            if (changes <= 0)
                return;

            // Allocate RGBA buffer
            if (nCapacity != sData.stride())
            {
                uint8_t *ptr    = NULL;
                float *rgba     = lsp::alloc_aligned<float>(ptr, sData.stride() * 4, 0x40); // 4 components per dot
                if (rgba == NULL)
                    return;
                if (pfRGBA != NULL)
                    lsp::free_aligned(pfRGBA);
                fRGBA           = rgba;
                pfRGBA          = ptr;
                nCapacity       = sData.stride();
            }

            // Get target buffer for rendering
            uint8_t *xp     = static_cast<uint8_t *>(s->start_direct());
            if (xp == NULL)
                return;

            // Shift buffer
            size_t stride   = s->stride();
            ::memmove(&xp[stride * changes], xp, (sData.rows() - changes) * stride);

            // Draw dots
            uint32_t row    = sData.last();

            for (size_t i=1; i<=changes; ++i, xp += stride)
            {
                const float *p = sData.row(row - i);
                if (p == NULL)
                    continue;

                (this->*pCalcColor)(fRGBA, p, nCols);
                dsp::rgba_to_bgra32(xp, fRGBA, nCols);
            }

            s->end_direct();

            // Commit pending changes
            bClear      = false;
            sData.advance();
        }

        void GraphFrameBuffer::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            // Check size
            if ((nRows <= 0) || (nCols <= 0))
                return;

            // Get drawing surface
            ws::ISurface *pp    = get_surface(s, nCols, nRows);
            if (pp == NULL)
                return;

            // Draw surface on the target
            float sx, sy;
            float width     = s->width();
            float height    = s->height();
            float ra        = -0.5f * sAngle.get() * M_PI;
            float x         = 0.5f * (sHPos.get() + 1.0f) * width;
            float y         = 0.5f * (1.0f - sVPos.get()) * height;
            float fw        = sHScale.get();
            float fh        = sVScale.get();

            switch (sAngle.get() & 0x03)
            {
                case 0:
                    sx          = (fw * width ) / nCols;
                    sy          = (fh * height) / nRows;

                    if (sx < 0.0f)
                        x          -= sx * nCols;
                    if (sy < 0.0f)
                        y          -= sy * nRows;
                    break;

                case 1:
                    sx          = (fw * width ) / nRows;
                    sy          = (fh * height) / nCols;

                    if (sx < 0.0f)
                        x          -= sx * nRows;
                    if (sy > 0.0f)
                        y          += sy * nCols;
                    break;

                case 2:
                    sx          = (fw * width ) / nCols;
                    sy          = (fh * height) / nRows;

                    if (sx > 0.0f)
                        x          += sx * nCols;
                    if (sy > 0.0f)
                        y          += sy * nRows;
                    break;

                case 3:
                    sx          = (fw * width ) / nRows;
                    sy          = (fh * height) / nCols;

                    if (sx > 0.0f)
                        x          += sx * nRows;
                    if (sy < 0.0f)
                        y          -= sy * nCols;
                    break;

                default:
                    break;
            }

            // Draw the buffer
            s->draw_rotate_alpha(pp, x, y, sx, sy, ra, sTransparency.get());
        }

        void GraphFrameBuffer::calc_rainbow_color(float *rgba, const float *v, size_t n)
        {
            dsp::hsla_hue_eff_t eff;
            eff.h       = sColor.hue();
            eff.s       = sColor.saturation();
            eff.l       = sColor.lightness();
            eff.a       = sColor.alpha();
            eff.thresh  = 1.0f / 3.0f;

            dsp::eff_hsla_hue(rgba, v, &eff, n);
            dsp::hsla_to_rgba(rgba, rgba, n);
        }

        void GraphFrameBuffer::calc_fog_color(float *rgba, const float *v, size_t n)
        {
            dsp::hsla_alpha_eff_t eff;
            eff.h       = sColor.hue();
            eff.s       = sColor.saturation();
            eff.l       = sColor.lightness();
            eff.a       = sColor.alpha();

            dsp::eff_hsla_alpha(rgba, v, &eff, n);
            dsp::hsla_to_rgba(rgba, rgba, n);
        }

        void GraphFrameBuffer::calc_color(float *rgba, const float *v, size_t n)
        {
            dsp::hsla_sat_eff_t eff;
            eff.h       = sColor.hue();
            eff.s       = sColor.saturation();
            eff.l       = sColor.lightness();
            eff.a       = sColor.alpha();
            eff.thresh  = 0.25f;

            dsp::eff_hsla_sat(rgba, v, &eff, n);
            dsp::hsla_to_rgba(rgba, rgba, n);
        }

        void GraphFrameBuffer::calc_lightness(float *rgba, const float *v, size_t n)
        {
            dsp::hsla_light_eff_t eff;
            eff.h       = sColor.hue();
            eff.s       = sColor.saturation();
            eff.l       = 1.0f;
            eff.a       = sColor.alpha();
            eff.thresh  = 0.25f;

            dsp::eff_hsla_light(rgba, v, &eff, n);
            dsp::hsla_to_rgba(rgba, rgba, n);
        }

        void GraphFrameBuffer::calc_lightness2(float *rgba, const float *v, size_t n)
        {
            dsp::hsla_light_eff_t eff;
            eff.h       = sColor.hue();
            eff.s       = sColor.saturation();
            eff.l       = 0.5f;
            eff.a       = sColor.alpha();
            eff.thresh  = 0.25f;

            dsp::eff_hsla_light(rgba, v, &eff, n);
            dsp::hsla_to_rgba(rgba, rgba, n);
        }
    }
}


