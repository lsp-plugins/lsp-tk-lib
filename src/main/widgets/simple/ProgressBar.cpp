/*
 * ProgressBar.cpp
 *
 *  Created on: 2 июл. 2019 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/stdlib/math.h>

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
            sTextArea.nLeft     = -1;
            sTextArea.nTop      = -1;
            sTextArea.nWidth    = 0;
            sTextArea.nHeight   = 0;

            pClass              = &metadata;
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
                sBorderRadius.init(sclass, 4);
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
            if (sBorderGapSize.is(prop))
                query_resize();
            if (sBorderGapColor.is(prop))
                query_draw();
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
            float scaling   = lsp_max(0.0f, sScaling.get());
            size_t border   = (sBorderSize.get() > 0) ? lsp_max(1.0f, sBorderSize.get() * scaling) : 0;
            size_t radius   = (sBorderRadius.get() > 0) ? lsp_max(1.0f, sBorderRadius.get() * scaling) : 0;
            if (border > 0)
                border         += (sBorderGapSize.get() > 0) ? lsp_max(1.0f, sBorderGapSize.get()) : 0;

            size_t extra    = lsp_max(radius, border);
            r->nMinWidth    = lsp_max(extra*2, border*2 + scaling*2);
            r->nMinHeight   = lsp_max(extra*2, border*2 + scaling*2);

            if (sShowText.get())
            {
                size_t rgap    = border + lsp_max(0.0f, ceil((1.0f - M_SQRT1_2) * (radius - border)));

                // Form the text string
                LSPString text;
                sText.format(&text);

                // Estimate sizes
                float scaling   = sScaling.get();
                ws::font_parameters_t fp;
                ws::text_parameters_t tp;

                sFont.get_parameters(pDisplay, scaling, &fp);
                sFont.get_multitext_parameters(pDisplay, &tp, scaling, &text);

                tp.Height       = lsp_max(tp.Height, fp.Height);
                r->nMinHeight   = lsp_max(r->nMinHeight, rgap*2 + tp.Height);
            }

            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;

            // Apply size constraints
            sConstraints.apply(r, scaling);
        }

        void ProgressBar::realize(const ws::rectangle_t *r)
        {
            Widget::realize(r);

            if (sShowText.get())
            {
                float scaling       = lsp_max(0.0f, sScaling.get());
                ssize_t radius      = (sBorderRadius.get() > 0) ? lsp_max(1.0f, sBorderRadius.get() * scaling) : 0;
                ssize_t border      = (sBorderSize.get() > 0) ? lsp_max(1.0f, sBorderSize.get() * scaling) : 0;
                if (border > 0)
                    border             += (sBorderGapSize.get() > 0) ? lsp_max(1.0f, sBorderGapSize.get() * scaling) : 0;

                border             += lsp_max(0.0f, ceil((1.0f - M_SQRT1_2) * (radius - border)));

                sTextArea.nLeft     = sSize.nLeft   + border;
                sTextArea.nTop      = sSize.nTop    + border;
                sTextArea.nWidth    = sSize.nWidth  - border * 2;
                sTextArea.nHeight   = sSize.nHeight - border * 2;
            }
            else
            {
                sTextArea.nLeft     = -1;
                sTextArea.nTop      = -1;
                sTextArea.nWidth    = 0;
                sTextArea.nHeight   = 0;
            }
        }

        void ProgressBar::out_text(ws::ISurface *s, const LSPString *text, lsp::Color &color)
        {
            ws::rectangle_t xr  = sTextArea;
            xr.nLeft       -= sSize.nLeft;
            xr.nTop        -= sSize.nTop;

            // Estimate sizes
            float scaling   = sScaling.get();
            ws::font_parameters_t fp;
            ws::text_parameters_t tp;

            sFont.get_parameters(pDisplay, scaling, &fp);
            sFont.get_multitext_parameters(pDisplay, &tp, scaling, text);

            float halign    = lsp_limit(sTextLayout.halign() + 1.0f, 0.0f, 2.0f);
            float valign    = lsp_limit(sTextLayout.valign() + 1.0f, 0.0f, 2.0f);
            float dy        = (xr.nHeight - tp.Height) * 0.5f;
            ssize_t y       = xr.nTop + dy * valign - fp.Descent;

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
                sFont.get_text_parameters(s, &tp, scaling, text, last, tail);
                float dx    = (xr.nWidth - tp.Width) * 0.5f;
                ssize_t x   = xr.nLeft   + dx * halign - tp.XBearing;
                y          += fp.Height;

                sFont.draw(s, color, x, y, scaling, text, last, tail);
                last    = curr + 1;
            }
        }
    
        void ProgressBar::draw(ws::ISurface *s)
        {
            float scaling   = lsp_max(0.0f, sScaling.get());
            float bright    = sBrightness.get();
            ssize_t border  = (sBorderSize.get() > 0) ? lsp_max(1.0f, sBorderSize.get() * scaling) : 0;
            ssize_t radius  = (sBorderRadius.get() > 0) ? lsp_max(1.0f, sBorderRadius.get() * scaling) : 0;
            ssize_t gap     = (sBorderGapSize.get() > 0) ? lsp_max(1.0f, sBorderGapSize.get() * scaling) : 0;

            ws::rectangle_t xr  = sSize;
            xr.nLeft            = 0;
            xr.nTop             = 0;

            // Draw background
            lsp::Color bg_color(sBgColor);
            s->clear(bg_color);
            bool aa = s->set_antialiasing(true);

            // Draw the border (if it is)
            if (border > 0)
            {
                lsp::Color bcolor(sBorderColor);
                bcolor.scale_lightness(bright);

                s->fill_round_rect(bcolor, SURFMASK_ALL_CORNER, xr.nLeft, xr.nTop, xr.nWidth, xr.nHeight, radius);
                radius      = lsp_max(0, radius - border);
                xr.nLeft   += border;
                xr.nTop    += border;
                xr.nWidth  -= border * 2;
                xr.nHeight -= border * 2;

                // Draw the border gap
                if (gap > 0)
                {
                    bcolor.copy(sBorderGapColor);
                    bcolor.scale_lightness(bright);

                    s->fill_round_rect(bcolor, SURFMASK_ALL_CORNER, xr.nLeft, xr.nTop, xr.nWidth, xr.nHeight, radius);
                    radius      = lsp_max(0, radius - gap);

                    xr.nLeft   += gap;
                    xr.nTop    += gap;
                    xr.nWidth  -= gap * 2;
                    xr.nHeight -= gap * 2;
                }
            }

            // Compute the progress value
            ssize_t split   = xr.nWidth * sValue.get_normalized();

            // Draw progress
            if (split > 0)
            {
                lsp::Color color(sColor);
                color.scale_lightness(bright);

                s->clip_begin(xr.nLeft, xr.nTop, split, xr.nHeight);
                s->fill_round_rect(color, SURFMASK_ALL_CORNER, xr.nLeft, xr.nTop, xr.nWidth, xr.nHeight, radius);
                s->clip_end();
            }
            if (split < xr.nWidth)
            {
                lsp::Color color(sInvColor);
                color.scale_lightness(bright);

                s->clip_begin(xr.nLeft + split, xr.nTop, xr.nWidth - split, xr.nHeight);
                s->fill_round_rect(color, SURFMASK_ALL_CORNER, xr.nLeft, xr.nTop, xr.nWidth, xr.nHeight, radius);
                s->clip_end();
            }

            // Draw text
            if (sShowText.get())
            {
                LSPString text;
                sText.format(&text);

                xr          = sTextArea;
                xr.nLeft   -= sSize.nLeft;
                xr.nTop    -= sSize.nTop;
                split       = xr.nWidth * sValue.get_normalized();

                if (split > 0)
                {
                    lsp::Color color(sTextColor);
                    color.scale_lightness(bright);

                    s->clip_begin(xr.nLeft, xr.nTop, split, xr.nHeight);
                    out_text(s, &text, color);
                    s->clip_end();
                }

                if (split < sTextArea.nWidth)
                {
                    lsp::Color color(sInvTextColor);
                    color.scale_lightness(bright);

                    s->clip_begin(xr.nLeft + split, xr.nTop, xr.nWidth - split, xr.nHeight);
                    out_text(s, &text, color);
                    s->clip_end();
                }
            }

            s->set_antialiasing(aa);
        }
    } /* namespace tk */
} /* namespace lsp */
