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
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(GraphText, GraphItem)
                // Bind
                sFont.bind("font", this);
                sColor.bind("color", this);
                sLayout.bind("layout", this);
                sTextLayout.bind("text.layout", this);
                sTextAdjust.bind("text.adjust", this);
                sHValue.bind("hvalue", this);
                sVValue.bind("vvalue", this);
                sHAxis.bind("haxis", this);
                sVAxis.bind("vaxis", this);
                sOrigin.bind("origin", this);
                // Configure
                sFont.set_size(10.0f);
                sLayout.set(1.0f, 1.0f, 0.0f, 0.0f);
                sTextLayout.set(0.5f, 0.5f);
                sTextAdjust.set(TA_NONE);
                sHValue.set(0.0f);
                sVValue.set(0.0f);
                sHAxis.set(0);
                sVAxis.set(1);
                sOrigin.set(0);
                // Override
                sPadding.set(2);
                // Commit
                sFont.override();
                sPadding.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(GraphText, "GraphText", "root");
        }

        const w_class_t GraphText::metadata             = { "GraphText", &GraphItem::metadata };

        GraphText::GraphText(Display *dpy):
            GraphItem(dpy),
            sText(&sProperties),
            sFont(&sProperties),
            sColor(&sProperties),
            sLayout(&sProperties),
            sTextLayout(&sProperties),
            sTextAdjust(&sProperties),
            sHValue(&sProperties),
            sVValue(&sProperties),
            sHAxis(&sProperties),
            sVAxis(&sProperties),
            sOrigin(&sProperties)
        {
            pClass              = &metadata;
        }

        GraphText::~GraphText()
        {
            nFlags     |= FINALIZED;
        }

        status_t GraphText::init()
        {
            status_t res        = GraphItem::init();
            if (res != STATUS_OK)
                return res;

            // Init style
            sText.bind(&sStyle, pDisplay->dictionary());
            sFont.bind("font", &sStyle);
            sColor.bind("color", &sStyle);
            sLayout.bind("layout", &sStyle);
            sTextLayout.bind("text.layout", &sStyle);
            sTextAdjust.bind("text.adjust", &sStyle);
            sHValue.bind("hvalue", &sStyle);
            sVValue.bind("vvalue", &sStyle);
            sHAxis.bind("haxis", &sStyle);
            sVAxis.bind("vaxis", &sStyle);
            sOrigin.bind("origin", &sStyle);

            return STATUS_OK;
        }

        void GraphText::property_changed(Property *prop)
        {
            GraphItem::property_changed(prop);

            if (sText.is(prop))
                query_draw();
            if (sFont.is(prop))
                query_draw();
            if (sColor.is(prop))
                query_draw();
            if (sLayout.is(prop))
                query_draw();
            if (sTextAdjust.is(prop))
                query_resize();
            if (sHValue.is(prop))
                query_draw();
            if (sVValue.is(prop))
                query_draw();
            if (sHAxis.is(prop))
                query_draw();
            if (sVAxis.is(prop))
                query_draw();
            if (sOrigin.is(prop))
                query_draw();
        }

        void GraphText::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            // Format the text
            LSPString text;
            sText.format(&text);
            if (text.is_empty())
                return;
            sTextAdjust.apply(&text);

            // Graph
            Graph *cv = graph();
            if (cv == NULL)
                return;

            // Get palette
            float scaling   = lsp_max(0.0f, sScaling.get());
            float fscaling  = lsp_max(0.0f, scaling * sFontScaling.get());
            float bright    = sBrightness.get();

            lsp::Color font_color(sColor);
            font_color.scale_lch_luminance(bright);

            // Get center
            float x = 0.0f, y = 0.0f;
            cv->origin(sOrigin.get(), &x, &y);

            // Apply horizontal axis
            float hvalue = sHValue.get();
            GraphAxis *haxis = cv->axis(sHAxis.get());
            if (haxis == NULL)
                return;
            if (!haxis->apply(&x, &y, &hvalue, 1))
                return;

            // Apply vertical axis
            float vvalue = sVValue.get();
            GraphAxis *vaxis = cv->axis(sVAxis.get());
            if (vaxis == NULL)
                return;
            if (!vaxis->apply(&x, &y, &vvalue, 1))
                return;

            // Now we are ready to output text
            ws::font_parameters_t fp;
            ws::text_parameters_t tp;

            sFont.get_parameters(s, fscaling, &fp);
            sFont.get_multitext_parameters(s, &tp, fscaling, &text);

            // Allocate position
            ws::rectangle_t r;
            r.nLeft         = x;
            r.nTop          = y;
            r.nWidth        = tp.Width;
            r.nHeight       = tp.Height;
            sPadding.add(&r, scaling);

            float lhalign   = (sLayout.halign() - 1.0f) * 0.5f;
            float lvalign   = (1.0f + sLayout.valign()) * 0.5f;
            r.nLeft        += lhalign * r.nWidth;
            r.nTop         -= lvalign * r.nHeight;

//            lsp::Color tmp;
//            tmp.set_rgb24(0x880000);
//            s->fill_rect(tmp, &r);

            // Center point
            sPadding.enter(&r, scaling);
            float halign    = lsp_limit(sTextLayout.halign() + 1.0f, 0.0f, 2.0f);
            float valign    = lsp_limit(sTextLayout.valign() + 1.0f, 0.0f, 2.0f);
            float dy        = (r.nHeight - tp.Height) * 0.5f;
            ssize_t ty      = r.nTop + dy * valign - fp.Descent;

            // Estimate text size
            ssize_t last = 0, curr = 0, tail = 0, len = text.length();

            while (curr < len)
            {
                // Get next line indexes
                curr    = text.index_of(last, '\n');
                if (curr < 0)
                {
                    curr        = len;
                    tail        = len;
                }
                else
                {
                    tail        = curr;
                    if ((tail > last) && (text.at(tail-1) == '\r'))
                        --tail;
                }

                // Calculate text location
                sFont.get_text_parameters(s, &tp, fscaling, &text, last, tail);
                float dx    = (r.nWidth - tp.Width) * 0.5f;
                ssize_t tx  = r.nLeft   + dx * halign - tp.XBearing;
                ty         += fp.Height;

                sFont.draw(s, font_color, tx, ty, fscaling, &text, last, tail);
                last    = curr + 1;
            }
        }
    }
}


