/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 6 июл. 2017 г.
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
#include <lsp-plug.in/stdlib/math.h>
#include <private/tk/style/BuiltinStyle.h>
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace tk
    {

        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(Label, Widget)
                // Bind
                sTextLayout.bind("text.layout", this);
                sFont.bind("font", this);
                sColor.bind("text.color", this);
                sConstraints.bind("size.constraints", this);
                // Configure
                sTextLayout.set(0.0f, 0.0f);
                sFont.set_size(12.0f);
                sColor.set("#000000");
                sConstraints.set(-1, -1, -1, -1);
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(Label, "Label");
        }

        const w_class_t Label::metadata =       { "Label", &Widget::metadata };

        Label::Label(Display *dpy):
            Widget(dpy),
            sTextLayout(&sProperties),
            sFont(&sProperties),
            sColor(&sProperties),
            sText(&sProperties),
            sConstraints(&sProperties)
        {
            pClass      = &metadata;
        }

        Label::~Label()
        {
            nFlags     |= FINALIZED;
        }

        status_t Label::init()
        {
            status_t result = Widget::init();
            if (result != STATUS_OK)
                return result;

            sTextLayout.bind("text.layout", &sStyle);
            sFont.bind("font", &sStyle);
            sColor.bind("text.color", &sStyle);
            sText.bind(&sStyle, pDisplay->dictionary());
            sConstraints.bind("size.constraints", &sStyle);

            return STATUS_OK;
        }

        void Label::property_changed(Property *prop)
        {
            Widget::property_changed(prop);
            if (sTextLayout.is(prop))
                query_draw();
            if (sFont.is(prop))
                query_resize();
            if (sColor.is(prop))
                query_draw();
            if (sText.is(prop))
                query_resize();
            if (sConstraints.is(prop))
                query_resize();
        }

        void Label::draw(ws::ISurface *s)
        {
            // Form the text string
            LSPString text;
            sText.format(&text);

            // Estimate sizes
            float scaling   = sScaling.get();
            ws::font_parameters_t fp;
            ws::text_parameters_t tp;
            ws::rectangle_t r;

            sFont.get_parameters(s, scaling, &fp);
            sFont.get_multitext_parameters(s, &tp, scaling, &text);

            // Estimate drawing area
            tp.Height       = lsp_max(tp.Height, fp.Height);
            if (tp.Width <= sSize.nWidth)
            {
                r.nLeft         = 0;
                r.nWidth        = sSize.nWidth;
            }
            else
            {
                r.nLeft         = -0.5f * (tp.Width - sSize.nWidth);
                r.nWidth        = ceil(tp.Width);
            }

            if (tp.Height <= sSize.nHeight)
            {
                r.nTop          = 0;
                r.nHeight       = sSize.nHeight;
            }
            else
            {
                r.nTop          = -0.5f * (tp.Height - sSize.nHeight);
                r.nHeight       = ceil(tp.Height);
            }

            // Initialize palette
            lsp::Color bg_color(sBgColor);
            lsp::Color f_color(sColor);
            f_color.scale_lightness(sBrightness.get());

            // Draw background
            s->clear(bg_color);

            float halign    = lsp_limit(sTextLayout.halign() + 1.0f, 0.0f, 2.0f);
            float valign    = lsp_limit(sTextLayout.valign() + 1.0f, 0.0f, 2.0f);
            float dy        = (r.nHeight - tp.Height) * 0.5f;
            ssize_t y       = r.nTop + dy * valign - fp.Descent;

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
                ssize_t x   = r.nLeft   + dx * halign - tp.XBearing;
                y          += fp.Height;

                sFont.draw(s, f_color, x, y, scaling, &text, last, tail);
                last    = curr + 1;
            }
        }

        void Label::size_request(ws::size_limit_t *r)
        {
            r->nMinWidth    = 0;
            r->nMinHeight   = 0;
            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;
            r->nPreWidth    = -1;
            r->nPreHeight   = -1;

            // Form the text string
            LSPString text;
            sText.format(&text);

            // Estimate sizes
            float scaling   = sScaling.get();
            ws::font_parameters_t fp;
            ws::text_parameters_t tp;

            sFont.get_parameters(pDisplay, scaling, &fp);
            sFont.get_multitext_parameters(pDisplay, &tp, scaling, &text);

            r->nMinWidth    = ceil(tp.Width);
            r->nMinHeight   = ceil(lsp_max(tp.Height, fp.Height));

            // Apply size constraints
            sConstraints.apply(r, scaling);
        }
    } /* namespace tk */
} /* namespace lsp */
