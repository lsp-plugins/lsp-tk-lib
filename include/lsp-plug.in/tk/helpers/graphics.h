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

#ifndef LSP_PLUG_IN_TK_HELPERS_GRAPHICS_H_
#define LSP_PLUG_IN_TK_HELPERS_GRAPHICS_H_

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        typedef struct dot2f_t
        {
            float x;
            float y;
        } dot2f_t;

        typedef struct vec2f_t
        {
            float dx;
            float dy;
        } vec2f_t;

        typedef struct triangle2f_t
        {
            dot2f_t     v[3];
        } triangle2f_t;

        bool line2d_equation
        (
            float x1, float y1,
            float x2, float y2,
            float &a, float &b, float &c
        );

        bool line2d_delta_equation
        (
            float x1, float y,
            float dx, float dy,
            float &a, float &b, float &c
        );


        bool line2d_equation
        (
            float dx, float dy,
            float &a, float &b, float &c
        );

        bool line2d_intersection
        (
            float a1, float b1, float c1,
            float a2, float b2, float c2,
            float &x, float &y
        );

        float distance2d(float x1, float y1, float x2, float y2);

        float scalar_product2d(float x1, float y1, float x2, float y2);

        float vector_product2d(float x1, float y1, float x2, float y2);

        float get_angle_2d
        (
            float x0, float y0, // Coordinates of center
            float x, float y    // Coordinates of dot
        );

        bool clip_line2d_vec(
            float dx, float dy,                             // Line direction
            float lc, float rc, float tc, float bc,         // Corners from left, right, top, bottom
            float error,                                    // Allowed error
            float &cx1, float &cy1, float &cx2, float &cy2  // Results
        );

        bool clip_line2d_eq(
            float a, float b, float c,                      // Line equation
            float lc, float rc, float tc, float bc,         // Corners from left, right, top, bottom
            float error,                                    // Allowed error
            float &cx1, float &cy1, float &cx2, float &cy2  // Results
        );

        bool clip_line2d_coord(
            float x1, float x2, float y1, float y2,         // Coordinates of two points laying on line
            float lc, float rc, float tc, float bc,         // Corners from left, right, top, bottom
            float error,                                    // Allowed error
            float &cx1, float &cy1, float &cx2, float &cy2  // Results
        );

        void locate_line2d(
            float a, float b, float c,                      // Line equation
            float px, float py,                             // Point of the line
            float &ma, float &mb, float &mc                 // New equation
        );

        bool locate_line2d(
            float dx, float dy,                             // Line equation
            float px, float py,                             // Point of the line
            float &ma, float &mb, float &mc                 // New equation
        );

        bool clip2d(
            float x, float y,                               // Coordinates of point
            float lc, float rc, float tc, float bc          // Corners: left, right, top, bottom
        );

        vec2f_t normalize(const vec2f_t &v);
        bool    normalize(vec2f_t &v, const vec2f_t &sv);

        vec2f_t vec2f(float x1, float y1, float x2, float y2);
        vec2f_t vec2f(const dot2f_t &begin, const dot2f_t &end);
        vec2f_t perp2f(const vec2f_t &v);
        vec2f_t perp2f(float x1, float y1, float x2, float y2);
        vec2f_t perp2f(const dot2f_t &begin, const dot2f_t &end);

        vec2f_t normalized_vec2f(float x1, float y1, float x2, float y2);
        vec2f_t normalized_vec2f(const dot2f_t &begin, const dot2f_t &end);
        bool    normalized_vec2f(vec2f_t &v, float x1, float y1, float x2, float y2);
        bool    normalized_vec2f(vec2f_t &v, const dot2f_t &begin, const dot2f_t &end);

        vec2f_t normalized_perp2f(float x1, float y1, float x2, float y2);
        vec2f_t normalized_perp2f(const dot2f_t &begin, const dot2f_t &end);
        bool    normalized_perp2f(vec2f_t &v, float x1, float y1, float x2, float y2);
        bool    normalized_perp2f(vec2f_t &v, const dot2f_t &begin, const dot2f_t &end);

        vec2f_t normalized_perp2f(float x1, float y1, float x2, float y2);
        vec2f_t normalized_perp2f(const dot2f_t &begin, const dot2f_t &end);

        bool    inside(const triangle2f_t &t, float x, float y);
        bool    inside(const triangle2f_t &t, const dot2f_t &p);
        bool    inside(const dot2f_t &a, const dot2f_t &b, const dot2f_t &c, float x, float y);
        bool    inside(const dot2f_t &a, const dot2f_t &b, const dot2f_t &c, const dot2f_t &p);


        float   cross_factor(const triangle2f_t &t);
        float   cross_factor(const vec2f_t &v1, const vec2f_t &v2);
        triangle2f_t reorder_triangle2f(const triangle2f_t &t);

        dot2f_t shift2f(const dot2f_t &p, const vec2f_t &v, float k);
        dot2f_t shift2f(float x, float y, const vec2f_t &v, float k);

    } /* namespace tk */
} /* namespace lsp */


#endif /* LSP_PLUG_IN_TK_HELPERS_GRAPHICS_H_ */
