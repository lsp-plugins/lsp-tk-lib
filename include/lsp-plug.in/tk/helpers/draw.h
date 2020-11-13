/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 22 авг. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_HELPERS_DRAW_H_
#define LSP_PLUG_IN_TK_HELPERS_DRAW_H_

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        /** Draw border
         *
         * @param s surface to draw the border
         * @param left the left position
         * @param top the top position
         * @param width the width of the border
         * @param height the height of the border
         * @param thick the thickness of the border
         * @param iradius inner radius of the border
         * @param mask the rounding mask
         * @param c the color of the border
         */
        void draw_border(ws::ISurface *s,
                const lsp::Color &c, size_t mask, ssize_t thick, size_t iradius,
                ssize_t left, ssize_t top, ssize_t width, ssize_t height
        );

        void draw_border(ws::ISurface *s,
                const lsp::Color &c, size_t mask, ssize_t thick, size_t iradius,
                const ws::rectangle_t *size
        );

        /** Create glass
         *
         * @param s the factory surface
         * @param g pointer to pointer that stores address of the surface object
         * @param c color of the glass
         * @param width the width of the glass
         * @param height the height of the glass
         * @param radius the radius of the glass
         * @param mask the radius drawing mask
         * @return pointer to the glass on succes or null on error
         */
        ws::ISurface *create_glass(ws::ISurface **g, ws::ISurface *s,
                const lsp::Color &c,
                size_t mask, ssize_t radius, size_t width, size_t height
        );

        /** Create glass with border
         *
         * @param s the factory surface
         * @param g pointer to pointer that stores address of the surface object
         * @param width the width of the glass
         * @param height the height of the glass
         * @param radius the radius of the glass
         * @param mask the radius drawing mask
         * @param gc the color of the glass
         * @param bc the color of the border
         * @return pointer to the glass on succes or null on error
         */
        ws::ISurface * create_border_glass(
            ws::ISurface **g, ws::ISurface *s,
            const lsp::Color &gc, const lsp::Color &bc,
            size_t mask, ssize_t thick, ssize_t radius,
            size_t width, size_t height
        );

        void draw_border_back(
            ws::ISurface *s,
            const lsp::Color &c, size_t mask,
            ssize_t thick, size_t radius,
            ssize_t left, ssize_t top,
            ssize_t width, ssize_t height
        );

        void draw_border_back(
            ws::ISurface *s,
            const lsp::Color &c, size_t mask,
            ssize_t thick, size_t radius,
            const ws::rectangle_t *size
        );

        /**
         * Draw multiline text
         *
         * @param s destination surface to perform draw
         * @param font font to use
         * @param r rectangle to fit the font
         * @param color color of the rectangle
         * @param fp font parameters
         * @param tp text parameters
         * @param halign horizontal font alignment
         * @param valign vertical font alignment
         * @param scaling font scaling
         * @param text text to output
         */
        void draw_multiline_text(
            ws::ISurface *s,
            Font *font,
            const ws::rectangle_t *r,
            const lsp::Color &color,
            const ws::font_parameters_t *fp,
            const ws::text_parameters_t *tp,
            float halign, float valign, float scaling,
            const LSPString *text
        );
    }
}

#endif /* LSP_PLUG_IN_TK_HELPERS_DRAW_H_ */
