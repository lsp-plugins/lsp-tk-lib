/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 11 апр. 2020 г.
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
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t Void::metadata      = { "Void", &Widget::metadata };
        
        Void::Void(Display *dpy):
            Widget(dpy),
            sConstraints(&sProperties)
        {
            pClass          = &metadata;
        }
        
        Void::~Void()
        {
            nFlags     |= FINALIZED;
        }

        status_t Void::init()
        {
            status_t res = Widget::init();
            if (res != STATUS_OK)
                return res;

            sConstraints.bind("size.constraints", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sConstraints.init(sclass, -1, -1, -1, -1);
            }

            return res;
        }

        void Void::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            if ((sSize.nWidth > 0) && (sSize.nHeight > 0))
            {
                lsp::Color color(sBgColor);
                s->clip_begin(area);
                s->fill_rect(color, sSize.nLeft, sSize.nTop, sSize.nWidth, sSize.nHeight);
                s->clip_end();

                // Debug bounds
//                color.set_rgb24(0x000000);
//                s->wire_rect(color, sSize.nLeft, sSize.nTop, sSize.nWidth-1, sSize.nHeight-1, 1);
//                s->line(sSize.nLeft, sSize.nTop, sSize.nLeft + sSize.nWidth, sSize.nTop + sSize.nHeight, 1, color);
//                s->line(sSize.nLeft, sSize.nTop + sSize.nHeight, sSize.nLeft + sSize.nWidth, sSize.nTop, 1, color);

                // Debug padding
//                ws::rectangle_t xr;
//                bg_color.set_rgb24(0xcccccc);
//                sPadding.enter(&xr, &sSize, sScaling.get());
//                s->fill_frame(color, &sSize, &xr);
            }
        }

        void Void::size_request(ws::size_limit_t *r)
        {
            // Add external size constraints
            float scaling = lsp_max(0.0f, sScaling.get());
            sConstraints.compute(r, scaling);

            r->nPreWidth        = -1;
            r->nPreHeight       = -1;

            lsp_trace("this=%p, w={%d, %d}, h={%d, %d}", this, int(r->nMinWidth), int(r->nMaxWidth), int(r->nMinHeight), int(r->nMaxHeight));
        }

        void Void::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (sConstraints.is(prop))
                query_resize();
        }

    } /* namespace tk */
} /* namespace lsp */
