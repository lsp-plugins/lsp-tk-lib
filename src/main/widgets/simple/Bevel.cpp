/*
 * Copyright (C) 2021 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2021 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 13 июн. 2021 г.
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
#include <lsp-plug.in/tk/helpers/graphics.h>
#include <lsp-plug.in/common/debug.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(Bevel, Widget)
                // Bind
                sConstraints.bind("size.constraints", this);
                sColor.bind("color", this);
                sBorderColor.bind("border.color", this);
                sBorder.bind("border.size", this);
                sDirection.bind("direction", this);
                sArrangement.bind("arrangement", this);

                // Configure
                sConstraints.set(-1, -1, -1, -1);
                sColor.set("#cccccc");
                sBorderColor.set("#ffffff");
                sBorder.set(0);
                sDirection.set_dangle(45.0f);
                sArrangement.set(0.0f, 0.0f);
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(Bevel, "Bevel");
        }

        const w_class_t Bevel::metadata      = { "Bevel", &Widget::metadata };

        Bevel::Bevel(Display *dpy):
            Widget(dpy),
            sConstraints(&sProperties)
        {
            pClass          = &metadata;
        }

        Bevel::~Bevel()
        {
            nFlags     |= FINALIZED;
        }

        status_t Bevel::init()
        {
            status_t res = Widget::init();
            if (res != STATUS_OK)
                return res;

            sConstraints.bind("size.constraints", &sStyle);
            sColor.bind("color", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sBorder.bind("border.size", &sStyle);
            sDirection.bind("direction", &sStyle);
            sArrangement.bind("arrangement", &sStyle);

            return res;
        }

        void Bevel::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (sConstraints.is(prop))
                query_resize();
            if (sColor.is(prop))
                query_draw();
            if (sBorderColor.is(prop))
                query_draw();
            if (sBorder.is(prop))
                query_resize();
            if (sDirection.is(prop))
                query_draw();
            if (sArrangement.is(prop))
                query_draw();
        }

        void Bevel::size_request(ws::size_limit_t *r)
        {
            // Add external size constraints
            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t border      = (sBorder.get() > 0) ? lsp_max(1.0f, sBorder.get() * scaling) : -1;

            r->nMinWidth        = border;
            r->nMinHeight       = border;
            r->nMaxWidth        = -1;
            r->nMaxHeight       = -1;
            r->nPreWidth        = -1;
            r->nPreHeight       = -1;

            sConstraints.apply(r, scaling);
        }

        bool Bevel::check_point(point2d_t *dp, const point2d_t *sp, const point2d_t *cx, float dx, float dy)
        {
            float dx2 = sp->x - cx->x;
            float dy2 = sp->y - cx->y;
            float mul = dx*dx2 + dy*dy2;

            if (mul < 0.0f)
                return false;

            dp->x   = sp->x;
            dp->y   = sp->y;

            return true;
        }

        void Bevel::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            if ((sSize.nWidth <= 0) || (sSize.nHeight <= 0))
                return;

            lsp::Color bg;
            get_actual_bg_color(bg);

            // Check that we can not compute the angle
            if (fabs(sDirection.rho()) <= 1e-5f)
            {
                s->clip_begin(area);
                    s->fill_rect(bg, &sSize);
                s->clip_end();
                return;
            }

            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t border      = (sBorder.get() > 0) ? lsp_max(1.0f, sBorder.get() * scaling) : 0;

            ws::rectangle_t xr, sr;
            point2d_t cp, ap, bp;
            float dx = sDirection.dx(), dy = -sDirection.dy();
            sr.nLeft            = sSize.nLeft   - border;
            sr.nTop             = sSize.nTop    - border;
            sr.nWidth           = sSize.nWidth  + border * 2;
            sr.nHeight          = sSize.nHeight + border * 2;

            // Compute the equation of the line according to the specified size
            cp.x                = sr.nLeft + sr.nWidth  * 0.5f;
            cp.y                = sr.nTop  + sr.nHeight * 0.5f;

            float a, b, c;
            if (!line2d_delta_equation(cp.x, cp.y, dx, dy, a, b, c))
            {
                s->clip_begin(area);
                    s->fill_rect(bg, &sSize);
                s->clip_end();
                return;
            }

            // Now we need to clip the line
            if (!clip_line2d(a, b, c,
                sr.nLeft, sr.nLeft + sr.nWidth,
                sr.nTop,  sr.nTop  + sr.nHeight,
                ap.x, ap.y, bp.x, bp.y))
            {
                s->clip_begin(area);
                    s->fill_rect(bg, &sSize);
                s->clip_end();
                return;
            }

            // Compute the bounding rectangle around the line
            xr.nLeft    = lsp_min(ap.x, bp.x);
            xr.nTop     = lsp_min(ap.y, bp.y);
            xr.nWidth   = ceilf(lsp_max(ap.x, bp.x) - xr.nLeft);
            xr.nHeight  = ceilf(lsp_max(ap.y, bp.y) - xr.nTop);
            float xx    = xr.nLeft;
            float xy    = xr.nTop;

            sArrangement.apply(&xr, &sr);
            xx         -= xr.nLeft;
            xy         -= xr.nTop;

            // Update coordinates of a and b points
            ap.x       += xx;
            ap.y       += xy;
            bp.x       += xx;
            bp.y       += xy;

            lsp::Color col(sColor), bcol(sBorderColor);
            float brightness    = sBrightness.get();
            col.scale_lightness(brightness);
            bcol.scale_lightness(brightness);

            // Find across all corner points the points which lay 'below' the split boundary
            // This can be done by computing the scalar multiplication between normal and a
            // difference vector between the point and the center of the located rectangle
            point2d_t vp[4];
            vp[0].x     = sr.nLeft;
            vp[0].y     = sr.nTop;
            vp[1].x     = vp[0].x;                  // sr.nLeft
            vp[1].y     = sr.nTop + sr.nHeight;
            vp[2].x     = sr.nLeft + sr.nWidth;
            vp[2].y     = vp[0].y;                  // sr.nTop
            vp[3].x     = vp[2].x;                  // sr.nLeft + sr.nWidth
            vp[3].y     = vp[1].y;                  // sr.nTop + sr.nHeight

            size_t n    = 0;
            for (size_t i=0; i<4; ++i)
                if (check_point(&vp[n], &vp[i], &cp, a, b))
                    ++n;

            // Finally, draw the bevel
            bool aa = s->set_antialiasing(true);
            s->clip_begin(area);
            {
                s->fill_rect(bg, &sSize);

                if (n > 0)
                {
                    s->fill_triangle(ap.x, ap.y, bp.x, bp.y, vp[0].x, vp[0].y, col);
                    if (n > 1)
                    {
                        s->fill_triangle(ap.x, ap.y, bp.x, bp.y, vp[1].x, vp[1].y, col);
                        s->fill_triangle(ap.x, ap.y, vp[0].x, vp[0].y, vp[1].x, vp[1].y, col);
                        s->fill_triangle(bp.x, bp.y, vp[0].x, vp[0].y, vp[1].x, vp[1].y, col);
                    }
                }

                // Draw the border
                if (border > 0)
                    s->line(ap.x, ap.y, bp.x, bp.y, border, bcol);
            }
            s->clip_end();
            s->set_antialiasing(aa);
        }


    }
}


