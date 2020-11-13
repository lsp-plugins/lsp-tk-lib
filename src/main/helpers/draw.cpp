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

#include <lsp-plug.in/tk/helpers/draw.h>
#include <lsp-plug.in/stdlib/math.h>

namespace lsp
{
    namespace tk
    {
        void draw_border(ws::ISurface *s,
                const lsp::Color &c, size_t mask, ssize_t thick, size_t iradius,
                ssize_t left, ssize_t top, ssize_t width, ssize_t height
        )
        {
            // Draw border
            bool aa = s->set_antialiasing(true);

            float pr = sqrtf(float(width*width) + float(height*height));

            for (ssize_t i=0; i < thick; ++i)
            {
                float bright = float(thick - i) / thick;
                lsp::Color l(1.0f, 1.0f, 1.0f);
                l.blend(c, bright);

                ws::IGradient *g = s->radial_gradient(left, top + height, i, left, top + height, pr * 1.5f);
                g->add_color(0.0f, l);
                g->add_color(1.0f, c);
                s->wire_round_rect(
                        g, mask, iradius - i,
                        left + i + 0.5f, top + i + 0.5f,
                        width - (i << 1) - 1, height - (i << 1) - 1,
                        1.0f
                );
                delete g;
            }

            s->fill_round_rect(
                    c, mask, iradius - thick,
                    left + thick + 0.5f, top + thick + 0.5f,
                    width - (thick << 1) - 1, height - (thick << 1) - 1);
            s->set_antialiasing(aa);
        }

        void draw_border(ws::ISurface *s,
                const lsp::Color &c, size_t mask, ssize_t thick, size_t iradius,
                const ws::rectangle_t *size
        )
        {
            draw_border(s, c, mask, thick, iradius,
                    size->nLeft, size->nTop, size->nWidth, size->nHeight);
        }

        ws::ISurface *create_glass(ws::ISurface **g, ws::ISurface *s,
                const lsp::Color &c,
                size_t mask, ssize_t radius, size_t width, size_t height
        )
        {
            // Check surface
            if (*g != NULL)
            {
                if ((width != (*g)->width()) || (height != (*g)->height()))
                {
                    (*g)->destroy();
                    delete *g;
                    (*g)        = NULL;
                }
            }

            // Create new surface if needed
            if ((*g) != NULL)
                return *g;

            if (s == NULL)
                return NULL;
            *g          = s->create(width, height);
            if ((*g) == NULL)
                return NULL;

            // Draw glass effect
            size_t pr   = sqrtf(float(width)*float(width) + float(height)*float(height));

            ws::IGradient *gr = (*g)->radial_gradient(width, 0, 1, width, 0, pr);
            gr->add_color(0.0f, c, 0.85f);
            gr->add_color(1.0f, c, 1.0f);

            bool aa = (*g)->set_antialiasing(true);
            (*g)->fill_round_rect(gr, mask, radius, 0, 0, width, height);
            (*g)->set_antialiasing(aa);
            delete gr;

            return *g;
        }

        ws::ISurface * create_border_glass(
            ws::ISurface **g, ws::ISurface *s,
            const lsp::Color &gc, const lsp::Color &bc,
            size_t mask, ssize_t thick, ssize_t radius,
            size_t width, size_t height
        )
        {
            // Check surface
            if (*g != NULL)
            {
                if ((width != (*g)->width()) || (height != (*g)->height()))
                {
                    (*g)->destroy();
                    delete *g;
                    (*g)        = NULL;
                }
            }

            // Create new surface if needed
            if ((*g) != NULL)
                return *g;

            if (s == NULL)
                return NULL;
            *g          = s->create(width, height);
            if ((*g) == NULL)
                return NULL;

            // Pre-calculate params
            ws::IGradient *gr = NULL;
            bool aa = (*g)->set_antialiasing(true);
            float pr = sqrtf(float(width)*float(width) + float(height)*float(height));

            // Draw border
            for (ssize_t i=0; i < thick; ++i)
            {
                float bright = float(thick - i) / thick;
                lsp::Color l(1.0f, 1.0f, 1.0f);
                l.blend(bc, bright);
                ssize_t xrr = lsp_max(0, radius - i);

                gr = (*g)->radial_gradient(0, height, i, 0, height, pr * 1.5f);
                gr->add_color(0.0f, l);
                gr->add_color(1.0f, bc);
                (*g)->wire_round_rect(
                        gr, mask, xrr,
                        i + 0.5f, i + 0.5f, width - (i << 1) - 1, height - (i << 1) - 1,
                        1.0f
                    );
                delete gr;
            }

            (*g)->wire_round_rect(
                    bc, mask, lsp_max(0, radius - thick),
                    thick + 0.5f, thick + 0.5f,
                    width - (thick << 1) - 1, height - (thick << 1) - 1,
                    1.0f
                );

            // Draw glass effect
            gr = (*g)->radial_gradient(width, 0, 1, width, 0, pr);
            gr->add_color(0.0f, gc, 0.85f);
            gr->add_color(1.0f, gc, 1.0f);

            (*g)->fill_round_rect(
                    gr, mask, lsp_max(0, radius - thick),
                    thick, thick,
                    width - (thick << 1), height - (thick << 1)
                );
            (*g)->set_antialiasing(aa);
            delete gr;

            s->set_antialiasing(aa);

            return *g;
        }

        void draw_border_back(
            ws::ISurface *s,
            const lsp::Color &c, size_t mask,
            ssize_t thick, size_t radius,
            ssize_t left, ssize_t top,
            ssize_t width, ssize_t height
        )
        {
            // Draw border
            bool aa = s->set_antialiasing(true);
            s->fill_round_rect(
                    c, mask, radius - thick,
                    left + thick, top + thick,
                    width - (thick << 1) - 1, height - (thick << 1) - 1);
            s->set_antialiasing(aa);
        }

        void draw_border_back(
            ws::ISurface *s,
            const lsp::Color &c, size_t mask,
            ssize_t thick, size_t radius,
            const ws::rectangle_t *size
        )
        {
            draw_border_back(s, c, mask, thick, radius,
                    size->nLeft, size->nTop, size->nWidth, size->nHeight);
        }

        void draw_multiline_text(
            ws::ISurface *s,
            Font *font,
            const ws::rectangle_t *r,
            const lsp::Color &color,
            const ws::font_parameters_t *fp,
            const ws::text_parameters_t *tp,
            float halign, float valign, float scaling,
            const LSPString *text
        )
        {
            ws::text_parameters_t xtp;
            halign         += 1.0f;
            valign         += 1.0f;

            float dy        = (r->nHeight - tp->Height) * 0.5f;
            ssize_t y       = r->nTop + dy * valign - fp->Descent;

            // Estimate text size
            ssize_t last = 0, curr = 0, tail = 0, len = text->length();

            while (curr < len)
            {
                // Get next line indexes
                curr    = text->index_of(last, '\n');
                if (curr < 0)
                {
                    curr        = len;
                    tail        = len;
                }
                else
                {
                    tail        = curr;
                    if ((tail > last) && (text->at(tail-1) == '\r'))
                        --tail;
                }

                // Calculate text location
                font->get_text_parameters(s, &xtp, scaling, text, last, tail);
                float dx    = (r->nWidth - xtp.Width) * 0.5f;
                ssize_t x   = r->nLeft   + dx * halign - xtp.XBearing;
                y          += fp->Height;

                font->draw(s, color, x, y, scaling, text, last, tail);
                last    = curr + 1;
            }
        }
    }
}


