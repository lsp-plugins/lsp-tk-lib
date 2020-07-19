/*
 * Void.cpp
 *
 *  Created on: 11 апр. 2020 г.
 *      Author: sadko
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
