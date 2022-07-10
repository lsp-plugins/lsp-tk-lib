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
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(GraphFrameBuffer, GraphItem)
                // Bind
                sData.bind("data", this);
                sTransparency.bind("transparency", this);
                sAngle.bind("angle", this);
                sHPos.bind("hpos", this);
                sVPos.bind("vpos", this);
                sHScale.bind("hscale", this);
                sVScale.bind("vscale", this);
                sColor.bind("color", this);
                sFunction.bind("function", this);
                // Configure
                sData.set_size(0, 0);
                sData.set_range(0.0f, 1.0f, 0.0f);
                sTransparency.set(0.5f);
                sAngle.set(0);
                sHPos.set(-1.0f);
                sVPos.set(1.0f);
                sHScale.set(1.0f);
                sVScale.set(1.0f);
                sColor.set("#ff0000");
                sFunction.set(GFF_DEFAULT);
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(GraphFrameBuffer, "GraphFrameBuffer", "root");
        }

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
            vRGBA               = NULL;
            pfRGBA              = NULL;
            nCapacity           = 0;
            nPixels             = 0;

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
            vRGBA               = NULL;
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
                    pCalcColor  = func;
                    bClear      = true;
                    query_draw();
                }
            }
        }

        void GraphFrameBuffer::draw(ws::ISurface *s)
        {
            // Allocate RGBA buffer
            size_t pixels   = nRows * nCols;

            if ((nCapacity != sData.stride()) || (pixels != nPixels))
            {
                // Free previous buffer
                bClear          = true;
                if (pfRGBA != NULL)
                {
                    free_aligned(pfRGBA);
                    fRGBA           = NULL;
                    vRGBA           = NULL;
                }

                // Allocate new buffer
                size_t frgba_size   = sData.stride() * sizeof(float) * 4;  // 4 float components per dot
                size_t vrgba_size   = pixels * sizeof(uint32_t); // 4 bytes per dot

                uint8_t *ptr        = lsp::alloc_aligned<uint8_t>(pfRGBA, frgba_size + vrgba_size, 0x40);
                if (ptr == NULL)
                    return;

                fRGBA           = reinterpret_cast<float *>(ptr);
                ptr            += frgba_size;
                vRGBA           = ptr;
                nCapacity       = sData.stride();
                nPixels         = pixels;
            }

            // Need to deploy new changes?
            size_t changes = (bClear) ? nRows : sData.changes();
            if (changes <= 0)
                return;

            // Process the pixel data
            size_t vstride      = nCols * sizeof(uint32_t);
            uint8_t *xp         = vRGBA;
            if (changes < nRows)
                ::memmove(&xp[vstride * changes], xp, (nRows - changes) * vstride);

            uint32_t row        = sData.last();
            for (size_t i=1; i<=changes; ++i, xp += vstride)
            {
                const float *p = sData.row(row - i);
                if (p == NULL)
                    continue;

                (this->*pCalcColor)(fRGBA, p, nCols);
                dsp::rgba_to_bgra32(xp, fRGBA, nCols);
            }

            lsp::Color c(0.0f, 0.0f, 0.0f, 1.0f);
            s->clear(c);
            s->draw_raw(vRGBA, nCols, nRows, vstride,
                0.0f, 0.0f, 1.0f, 1.0f, 0.0f);

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
                default:
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
            }

            // Draw the buffer
            s->draw_rotate(pp, x, y, sx, sy, ra, sTransparency.get());
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


