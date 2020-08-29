/*
 * GraphText.cpp
 *
 *  Created on: 20 авг. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t GraphText::metadata             = { "GraphText", &GraphItem::metadata };

        GraphText::GraphText(Display *dpy):
            GraphItem(dpy),
            sText(&sProperties),
            sFont(&sProperties),
            sColor(&sProperties),
            sLayout(&sProperties),
            sTextLayout(&sProperties),
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
        }

        void GraphText::destroy()
        {
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
            sHValue.bind("hvalue", &sStyle);
            sVValue.bind("vvalue", &sStyle);
            sHAxis.bind("haxis", &sStyle);
            sVAxis.bind("vaxis", &sStyle);
            sOrigin.bind("origin", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sFont.init(sclass, 10.0f);
                sColor.init(sclass, "#ffffff");
                sLayout.init(sclass, 1.0f, 1.0f, 0.0f, 0.0f);
                sTextLayout.init(sclass, 0.5f, 0.5f);
                sHValue.init(sclass, 0.0f);
                sVValue.init(sclass, 0.0f);
                sHAxis.init(sclass, 0);
                sVAxis.init(sclass, 0);
                sOrigin.init(sclass, 0);
                sPadding.override(sclass, 2);
            }

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

            // Graph
            Graph *cv = graph();
            if (cv == NULL)
                return;

            // Get palette
            float scaling = lsp_max(0.0f, sScaling.get());
            float bright  = sBrightness.get();

            lsp::Color font_color(sColor);
            font_color.scale_lightness(bright);

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

            sFont.get_parameters(s, scaling, &fp);
            sFont.get_multitext_parameters(s, &tp, scaling, &text);

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
            float halign    = lsp_limit(sTextLayout.halign() + 1.0f, 0.0f, 2.0f);
            float valign    = lsp_limit(sTextLayout.valign() + 1.0f, 0.0f, 2.0f);
            float dy        = (r.nHeight - tp.Height) * 0.5f;
            ssize_t ty       = r.nTop + dy * valign - fp.Descent;

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
                sFont.get_text_parameters(s, &tp, scaling, &text, last, tail);
                float dx    = (r.nWidth - tp.Width) * 0.5f;
                ssize_t tx  = r.nLeft   + dx * halign - tp.XBearing;
                ty         += fp.Height;

                sFont.draw(s, font_color, tx, ty, scaling, &text, last, tail);
                last    = curr + 1;
            }
        }
    }
}


