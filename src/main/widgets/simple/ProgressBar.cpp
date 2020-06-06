/*
 * ProgressBar.cpp
 *
 *  Created on: 2 июл. 2019 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t ProgressBar::metadata           = { "ProgressBar", &Widget::metadata };

        ProgressBar::ProgressBar(Display *dpy):
            Widget(dpy),
            sValue(&sProperties),
            sConstraints(&sProperties),
            sText(&sProperties),
            sTextLayout(&sProperties),
            sShowText(&sProperties),
            sFont(&sProperties),
            sBorderColor(&sProperties),
            sBorderGapColor(&sProperties),
            sBorderSize(&sProperties),
            sBorderGapSize(&sProperties),
            sBorderRadius(&sProperties),
            sColor(&sProperties),
            sTextColor(&sProperties),
            sInvColor(&sProperties),
            sInvTextColor(&sProperties)
        {
            pClass      = &metadata;
        }
        
        ProgressBar::~ProgressBar()
        {
        }

        status_t ProgressBar::init()
        {
            status_t result = Widget::init();
            if (result != STATUS_OK)
                return result;

            sValue.bind("value", &sStyle);
            sConstraints.bind("size", &sStyle);
            sText.bind(&sStyle, pDisplay->dictionary());
            sTextLayout.bind("text.layout", &sStyle);
            sShowText.bind("text.show", &sStyle);
            sFont.bind("font", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sBorderGapColor.bind("border.gap.color", &sStyle);
            sBorderSize.bind("border.size", &sStyle);
            sBorderGapSize.bind("border.gap.size", &sStyle);
            sBorderRadius.bind("border.radius", &sStyle);
            sColor.bind("color", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sInvColor.bind("inv.color", &sStyle);
            sInvTextColor.bind("text.inv.color", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sValue.init(sclass, 0.5f);
                sConstraints.init(sclass, -1, -1, -1, -1);
                sTextLayout.init(sclass, 0.0f, 0.0f);
                sShowText.init(sclass, true);
                sFont.init(sclass, 12.0f);
                sBorderColor.init(sclass, "#444444");
                sBorderGapColor.init(sclass, "#000000");
                sBorderSize.init(sclass, 1);
                sBorderGapSize.init(sclass, 1);
                sBorderRadius.init(sclass, 3);
                sColor.init(sclass, "#008800");
                sTextColor.init(sclass, "#ffffff");
                sInvColor.init(sclass, "#ffffff");
                sInvTextColor.init(sclass, "#000000");

                // Overrides
                sAllocation.override(sclass, true, false, false, false);
            }

            return STATUS_OK;
        }

        void ProgressBar::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (sValue.is(prop))
                query_draw();
            if (sConstraints.is(prop))
                query_resize();
            if (sText.is(prop))
                query_draw();
            if (sTextLayout.is(prop))
                query_draw();
            if (sShowText.is(prop))
                query_resize();
            if (sFont.is(prop))
                query_resize();
            if (sBorderColor.is(prop))
                query_draw();
            if (sBorderSize.is(prop))
                query_resize();
            if (sBorderRadius.is(prop))
                query_resize();
            if (sColor.is(prop))
                query_draw();
            if (sTextColor.is(prop))
                query_draw();
            if (sInvColor.is(prop))
                query_draw();
            if (sInvTextColor.is(prop))
                query_draw();
        }


        void ProgressBar::size_request(ws::size_limit_t *r)
        {

        }
    
        void ProgressBar::draw(ws::ISurface *s)
        {
//            size_t w    = sSize.nWidth;
//            size_t h    = sSize.nHeight;
//            size_t dw   = sSize.nWidth - 4;
//
//            // Prepare palette
//            Color bg_color(sBgColor);
//            Color color(sColor);
//            Color sel_color(sSelColor);
//            Color font(sFont.raw_color());
//
//            color.scale_lightness(brightness());
//            sel_color.scale_lightness(brightness());
//            font.scale_lightness(brightness());
//
//            // Draw the entire control
//            s->clear(bg_color);
//            bool aa = s->set_antialiasing(true);
//            s->fill_round_rect(0.5f, 0.5f, w-1, h-1, 4.0f, SURFMASK_ALL_CORNER, color);
//            s->set_antialiasing(aa);
//
//            font_parameters_t fp;
//            text_parameters_t tp;
//            if (!sText.is_empty())
//            {
//                sFont.get_parameters(s, &fp);
//                sFont.get_text_parameters(s, &tp, &sText);
//                sFont.draw(s, (w - tp.Width) * 0.5f, (h - fp.Height) * 0.5f + fp.Ascent, font, &sText);
//            }
//
//            // Need to draw the value over?
//            float k = fabs(fValue / (fMax - fMin));
//            if (k < 0.0f)
//                k       = 0.0f;
//            else if (k > 1.0f)
//                k       = 1.0f;
//
//            size_t pixels = dw * k;
//            if (pixels > 0)
//            {
//                ISurface *xs = pDisplay->create_surface(pixels + 2, h);
//                if (xs == NULL)
//                    return;
//
//                xs->clear(bg_color);
//                aa = xs->set_antialiasing(true);
//                xs->fill_round_rect(0.5f, 0.5f, w-1, h-1, 4.0f, SURFMASK_ALL_CORNER, sel_color);
//                xs->set_antialiasing(aa);
//
//                if (!sText.is_empty())
//                    sFont.draw(xs, (w - tp.Width) * 0.5f, (h - fp.Height) * 0.5f + fp.Ascent, color, &sText);
//                s->draw(xs, 0, 0);
//
//                xs->destroy();
//                delete xs;
//            }
//
//            // Finally, draw frames
//            aa = s->set_antialiasing(true);
//            s->wire_round_rect(1.5f, 1.5f, w - 3, h - 3, 4.0f, SURFMASK_ALL_CORNER, 1.0f, bg_color);
//            s->wire_round_rect(0.5f, 0.5f, w - 1, h - 1, 4.0f, SURFMASK_ALL_CORNER, 1.0f, color);
//            s->set_antialiasing(aa);
        }
    } /* namespace tk */
} /* namespace lsp */
