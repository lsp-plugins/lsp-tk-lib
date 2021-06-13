/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 26 авг. 2020 г.
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

#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/tk/helpers/graphics.h>

namespace lsp
{
    namespace tk
    {
        typedef struct point2d_t
        {
            float x;
            float y;
        } point2d_t;

        bool line2d_equation
        (
            float x1, float y1,
            float x2, float y2,
            float &a, float &b, float &c
        )
        {
            float dx = x1 - x2;
            float dy = y1 - y2;

            if ((dy == 0.0f) && (dx == 0.0f))
                return false;

            a = dy;
            b = - dx;
            c = y1 * dx - x1 * dy;

            return true;
        }

        bool line2d_delta_equation
        (
            float x, float y,
            float dx, float dy,
            float &a, float &b, float &c
        )
        {
            if ((dy == 0.0f) && (dx == 0.0f))
                return false;

            a = dy;
            b = - dx;
            c = y * dx - x * dy;

            return true;
        }

        bool line2d_equation
        (
            float dx, float dy,
            float &a, float &b, float &c
        )
        {
            if ((dx == 0.0f) && (dy == 0.0f))
                return false;

            a = dy;
            b = - dx;
            c = 0.0f;

            return true;
        }

        bool line2d_intersection
        (
            float a1, float b1, float c1,
            float a2, float b2, float c2,
            float &x, float &y
        )
        {
            float d = a1 * b2 - b1 * a2;
            if (d == 0.0)
                return false;

            d = 1.0 / d;
            x = (b1 * c2 - b2 * c1) * d;
            y = (a2 * c1 - a1 * c2) * d;

            return true;
        }

        float distance2d(float x1, float y1, float x2, float y2)
        {
            float dx = x1 - x2;
            float dy = y1 - y2;
            return sqrtf(dx*dx + dy*dy);
        }

        float scalar_product2d(float x1, float y1, float x2, float y2)
        {
            return x1*x2 + y1*y2;
        }

        float vector_product2d(float x1, float y1, float x2, float y2)
        {
            return x1*y2 - x2*y1;
        }

        float get_angle_2d
        (
            float x0, float y0,
            float x, float y
        )
        {
            float dx = x - x0;
            float dy = y - y0;
            float l = sqrtf(dx*dx + dy*dy);
            if (l == 0.0)
                return 0;

            float a;
            if (dx < dy) // calc with acos
            {
                a = acos(dx / l); // 0..PI
                a = (dy >= 0.0) ? a : M_PI*2 - a;
            }
            else // calc with asin
            {
                a = asin(dy / l); // -PI/2 .. PI/2
                a = (dx >= 0.0) ? a : M_PI - a;
            }

            return (a < 0.0) ? M_PI*2 + a : a;
        }

        bool clip_line2d(
            float dx, float dy,
            float lc, float rc, float tc, float bc,
            float &cx1, float &cy1, float &cx2, float &cy2
        )
        {
            return clip_line2d(dy, dx, 0.0f, lc, rc, tc, bc, cx1, cy1, cx2, cy2);
        }

        bool clip_line2d(
            float x1, float x2, float y1, float y2,
            float lc, float rc, float tc, float bc,
            float &cx1, float &cy1, float &cx2, float &cy2
        )
        {
            float a, b, c;
            if (!line2d_equation(x1, y1, x2, y2, a, b, c))
                return false;

            return clip_line2d(a, b, c, lc, rc, tc, bc, cx1, cy1, cx2, cy2);
        }

        bool vclip_line2d(
            float a, float b, float c,                      // Line equation
            float x,                                        // X coordinate of vertical line
            float tc, float bc,                             // Vertical clip corners
            float &cx, float &cy                            // Results
        )
        {
            if (b == 0.0f)
                return false;

            float v = -(c + a*x) / b;
            if ((v < bc) || (v > tc))
                return false;

            cx = x;
            cy = v;

            return true;
        }

        bool hclip_line2d(
            float a, float b, float c,                      // Line equation
            float y,                                        // Y coordinate of horizontal line
            float lc, float rc,                             // Horizontal clip corners
            float &cx, float &cy                            // Results
        )
        {
            if (a == 0.0f)
                return false;

            float v = -(c + b*y)/a;
            if ((v < lc) || (v > rc))
                return false;

            cx = v;
            cy = y;

            return true;
        }

        bool clip_line2d(
            float a, float b, float c,
            float lc, float rc, float tc, float bc,
            float &cx1, float &cy1, float &cx2, float &cy2
        )
        {
            point2d_t p[2];

            if (lc > rc)
                lsp::swap(lc, rc);
            if (tc > bc)
                lsp::swap(tc, bc);

            if (fabs(b) > fabs(a)) // fabs(dx) > fabs(dy) ?
            {
                if (fabs(b) <= 1e-6f)
                    return false;

                // Compute intersection with left and right boundaries
                p[0].x  = lc;
                p[1].x  = rc;
                p[0].y  = -(c + a * p[0].x) / b;
                p[1].y  = -(c + a * p[1].x) / b;

                if (p[0].y > p[1].y)
                {
                    lsp::swap(p[0].x, p[1].x);
                    lsp::swap(p[0].y, p[1].y);
                }

                // Check that line lays between vertical boundaries
                if ((p[0].y >= bc) || (p[1].y <= tc))
                    return false;

                // Now compute clipping of line at top
                if (p[0].y < tc)
                {
                    p[0].y  = tc;
                    p[0].x  = -(c + b*p[0].y) / a;
                }

                // And at bottom
                if (p[1].y > bc)
                {
                    p[1].y  = bc;
                    p[1].x  = -(c + b*p[1].y) / a;
                }
            }
            else
            {
                if (fabs(a) <= 1e-6f)
                    return false;

                // Compute intersection with top and bottom boundaries
                p[0].y  = tc;
                p[1].y  = bc;
                p[0].x  = -(c + b * p[0].y) / a;
                p[1].x  = -(c + b * p[1].y) / a;

                if (p[0].x > p[1].x)
                {
                    lsp::swap(p[0].x, p[1].x);
                    lsp::swap(p[0].y, p[1].y);
                }

                // Check that line lays between vertical boundaries
                if ((p[0].x >= rc) || (p[1].x <= lc))
                    return false;

                // Now compute clipping of line at left
                if (p[0].x < lc)
                {
                    p[0].x  = lc;
                    p[0].y  = -(c + a*p[0].x) / b;
                }

                // And at right
                if (p[1].x > rc)
                {
                    p[1].x  = rc;
                    p[1].y  = -(c + a*p[1].x) / b;
                }
            }

            cx1 = p[0].x;
            cy1 = p[0].y;
            cx2 = p[1].x;
            cy2 = p[1].y;

            return true;
        }

        void locate_line2d(
            float a, float b, float c,                      // Line equation
            float mx, float my,                             // Point of the line
            float &ma, float &mb, float &mc                 // New equation
        )
        {
            ma  = a;
            mb  = b;
            mc  = c - a * mx - b * my;
        }

        bool locate_line2d(
            float dx, float dy,                             // Line equation
            float px, float py,                             // Point of the line
            float &ma, float &mb, float &mc                 // New equation
        )
        {
            if ((dx == 0.0f) && (dy == 0.0f))
                return false;

            ma  = dy;
            mb  = - dx;
            mc  = py * dx - px * dy;
            return true;
        }

        bool clip2d(
            float x, float y,                               // Coordinates of point
            float lc, float rc, float tc, float bc          // Corners: left, right, top, bottom
        )
        {
            return (x >= lc) && (x <= rc) && (y >= bc) && (y <= tc);
        }
    }
}



