/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
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

        bool clip_line2d_vec(
            float dx, float dy,
            float lc, float rc, float tc, float bc,
            float error,
            float &cx1, float &cy1, float &cx2, float &cy2
        )
        {
            return clip_line2d_eq(dy, dx, 0.0f, lc, rc, tc, bc, error, cx1, cy1, cx2, cy2);
        }

        bool clip_line2d_coord(
            float x1, float x2, float y1, float y2,
            float lc, float rc, float tc, float bc,
            float error,
            float &cx1, float &cy1, float &cx2, float &cy2
        )
        {
            float a, b, c;
            if (!line2d_equation(x1, y1, x2, y2, a, b, c))
                return false;

            return clip_line2d_eq(a, b, c, lc, rc, tc, bc, error, cx1, cy1, cx2, cy2);
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

        bool clip_line2d_eq(
            float a, float b, float c,
            float lc, float rc, float tc, float bc,
            float error,
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
                if ((p[0].y > (bc + error)) || (p[1].y < (tc - error)))
                    return false;

                // Now compute clipping of line at top
                if (p[0].y < (tc - error))
                {
                    p[0].y  = tc;
                    p[0].x  = -(c + b*p[0].y) / a;
                }

                // And at bottom
                if (p[1].y > (bc + error))
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
                if ((p[0].x > (rc + error)) || (p[1].x < (lc - error)))
                    return false;

                // Now compute clipping of line at left
                if (p[0].x < (lc - error))
                {
                    p[0].x  = lc;
                    p[0].y  = -(c + a*p[0].x) / b;
                }

                // And at right
                if (p[1].x > (rc + error))
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

        vec2f_t normalize(const vec2f_t &v)
        {
            const float len = v.dx * v.dx + v.dy * v.dy;
            if (len <= 1e-8f)
                return v;
            const float norm  = 1.0f / sqrtf(len);

            return vec2f_t {
                v.dx * norm,
                v.dy * norm
            };
        }

        bool normalize(vec2f_t &v, const vec2f_t &sv)
        {
            const float len = sv.dx * sv.dx + sv.dy * sv.dy;
            if (len <= 1e-8f)
            {
                v = sv;
                return false;
            }

            const float norm  = 1.0f / sqrtf(len);
            v = vec2f_t {
                sv.dx * norm,
                sv.dy * norm
            };

            return true;
        }

        vec2f_t vec2f(float x1, float y1, float x2, float y2)
        {
            return vec2f_t {
                x2 - x1,
                y2 - y1
            };
        }

        vec2f_t vec2f(const dot2f_t &begin, const dot2f_t &end)
        {
            return vec2f_t {
                end.x - begin.x,
                end.y - begin.y
            };
        }

        vec2f_t normalized_vec2f(float x1, float y1, float x2, float y2)
        {
            return normalize(vec2f(x1, y1, x2, y2));
        }

        vec2f_t normalized_vec2f(const dot2f_t &begin, const dot2f_t &end)
        {
            return normalize(vec2f(begin, end));
        }

        bool normalized_vec2f(vec2f_t &v, float x1, float y1, float x2, float y2)
        {
            return normalize(v, vec2f(x1, y1, x2, y2));
        }

        bool normalized_vec2f(vec2f_t &v, const dot2f_t &begin, const dot2f_t &end)
        {
            return normalize(v, vec2f(begin, end));
        }

        vec2f_t perp2f(const vec2f_t &v)
        {
            return vec2f_t { -v.dy, v.dx };
        }

        vec2f_t perp2f(float x1, float y1, float x2, float y2)
        {
            return vec2f_t {
                y1 - y2,
                x2 - x1
            };
        }

        vec2f_t perp2f(const dot2f_t &begin, const dot2f_t &end)
        {
            return vec2f_t {
                begin.y - end.y,
                end.x - begin.x
            };
        }

        vec2f_t normalized_perp2f(float x1, float y1, float x2, float y2)
        {
            return normalize(perp2f(x1, y1, x2, y2));
        }

        vec2f_t normalized_perp2f(const dot2f_t &begin, const dot2f_t &end)
        {
            return normalize(perp2f(begin, end));
        }

        bool normalized_perp2f(vec2f_t &v, float x1, float y1, float x2, float y2)
        {
            return normalize(v, perp2f(x1, y1, x2, y2));
        }

        bool normalized_perp2f(vec2f_t &v, const dot2f_t &begin, const dot2f_t &end)
        {
            return normalize(v, perp2f(begin, end));
        }

        bool inside(const triangle2f_t &t, float x, float y)
        {
            return inside(t, dot2f_t{ x, y });
        }

        bool inside(const triangle2f_t &t, const dot2f_t &p)
        {
            float f = cross_factor(
                vec2f(t.v[0], p),
                vec2f(p, t.v[1]));
            if (f < 0.0f)
                return false;

            f = cross_factor(
                vec2f(t.v[1], p),
                vec2f(p, t.v[2]));
            if (f < 0.0f)
                return false;

            f = cross_factor(
                vec2f(t.v[2], p),
                vec2f(p, t.v[0]));
            if (f < 0.0f)
                return false;

            return true;
        }

        bool inside(const dot2f_t &a, const dot2f_t &b, const dot2f_t &c, float x, float y)
        {
            return inside(a, b, c, dot2f_t{ x, y });
        }

        bool inside(const dot2f_t &a, const dot2f_t &b, const dot2f_t &c, const dot2f_t &p)
        {
            float f = cross_factor(
                vec2f(a, p),
                vec2f(p, b));
            if (f < 0.0f)
                return false;

            f = cross_factor(
                vec2f(b, p),
                vec2f(p, c));
            if (f < 0.0f)
                return false;

            f = cross_factor(
                vec2f(c, p),
                vec2f(p, a));
            if (f < 0.0f)
                return false;

            return true;
        }

        float cross_factor(const triangle2f_t &t)
        {
            return cross_factor(
                vec2f(t.v[0], t.v[1]),
                vec2f(t.v[1], t.v[2])
            );
        }

        float cross_factor(const vec2f_t &v1, const vec2f_t &v2)
        {
            return v1.dx * v2.dy - v1.dy * v2.dx;
        }

        triangle2f_t reorder_triangle2f(const triangle2f_t &t)
        {
            if (cross_factor(t) < 0.0f)
                return triangle2f_t {
                    { t.v[0], t.v[2], t.v[1] }
                };
            return t;
        }

        dot2f_t shift2f(const dot2f_t &p, const vec2f_t &v, float k)
        {
            return dot2f_t {
                p.x + v.dx * k,
                p.y + v.dy * k
            };
        }

        dot2f_t shift2f(float x, float y, const vec2f_t &v, float k)
        {
            return dot2f_t {
                x + v.dx * k,
                y + v.dy * k
            };
        }

    } /* namespace tk */
} /* namespace lsp */



