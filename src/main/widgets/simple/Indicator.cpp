/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 7 июл. 2017 г.
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

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(Indicator, Widget)
                // Bind
                sColor.bind("color", this);
                sTextColor.bind("text.color", this);
                sRows.bind("rows", this);
                sColumns.bind("columns", this);
                sShift.bind("text.shift", this);
                sTextGap.bind("text.gap", this);
                sLoop.bind("text.loop", this);

                // Configure
                sColor.set("#111111");
                sTextColor.set("#00ff00");
                sRows.set(1);
                sColumns.set(5);
                sShift.set(0);
                sTextGap.set(0);
                sLoop.set(false);
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(Indicator, "Indicator");
        }

        typedef struct rect_t
        {
            int         x, y, w, h;
        } rect_t;

        /**
         * Indicator segments:
         *
         *
         *      33333
         *     2     4
         *     2     4  9
         *     2  0  4  9
         *      77777
         *     1  0  5  A
         *     1     5  A
         *     1     5
         *      66666   8
         *
         */

        static const rect_t segments[] =
        {
            {   4,  6,  1,  5   },  // 0
            {   0, 10,  1,  5   },  // 1
            {   0,  2,  1,  5   },  // 2
            {   2,  0,  5,  1   },  // 3
            {   8,  2,  1,  5   },  // 4
            {   8, 10,  1,  5   },  // 5
            {   2, 16,  5,  1   },  // 6
            {   2,  8,  5,  1   },  // 7
            {  12, 16,  1,  1   },  // 8
            {  12,  5,  1,  1   },  // 9
            {  12, 11,  1,  1   }   // A
        };

        static const uint16_t ascii_map[] =
        {
            // 0/8  1/9     2/A     3/B     4/C     5/D     6/E     7/F
            0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x00 - 0x07
            0xffff, 0x0000, 0x0000, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, // 0x08 - 0x0f
            0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x10 - 0x17
            0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x18 - 0x1f
            0x0000, 0x0700, 0x0014, 0xffff, 0x00ed, 0xffff, 0xffff, 0x0010, // 0x20 - 0x27    !"#$%&'
            0x000e, 0x0070, 0x00b7, 0x0081, 0x0140, 0x0080, 0x0100, 0x0013, // 0x28 - 0x2f   ()*+,-./
            0x007e, 0x0030, 0x00da, 0x00f8, 0x00b4, 0x00ec, 0x00ee, 0x0038, // 0x30 - 0x37   01234567
            0x00fe, 0x00fc, 0x0600, 0x0600, 0x00b0, 0x00c0, 0x0086, 0x009a, // 0x38 - 0x3f   89:;<=>?
            0xffff, 0x00be, 0x00e6, 0x004e, 0x00f2, 0x00ce, 0x008e, 0x006e, // 0x40 - 0x47   @ABCDEFG
            0x00b6, 0x0006, 0x0072, 0x0027, 0x0066, 0x0038, 0x003e, 0x007e, // 0x48 - 0x4f   HIJKLMNO
            0x009e, 0x017e, 0x001e, 0x00ec, 0x00c6, 0x0076, 0x0062, 0x0070, // 0x50 - 0x57   PQRSTUVW
            0x0037, 0x00f4, 0x005b, 0x000e, 0x0025, 0x0070, 0x0004, 0x0040, // 0x58 - 0x5f   XYZ[\]^_
            0x0004, 0x00be, 0x00e6, 0x00c2, 0x00f2, 0x00de, 0x008e, 0x006e, // 0x60 - 0x67   `abcdefg
            0x00a6, 0x0006, 0x0070, 0x0027, 0x0046, 0x00a0, 0x00a2, 0x00e2, // 0x68 - 0x6f   hijklmno
            0x009e, 0x01e2, 0x0082, 0x00ec, 0x00c6, 0x0076, 0x0062, 0x0060, // 0x70 - 0x77   pqrstuvw
            0x0037, 0x00f4, 0x005b, 0x000e, 0x0036, 0x0070, 0x00e4, 0xffff, // 0x78 - 0x7f   xyz{|}~
            // Special cases: M, W, m, w
        };

        const w_class_t Indicator::metadata =           { "Indicator", &Widget::metadata };

        Indicator::Indicator(Display *dpy):
            Widget(dpy),
            sTextColor(&sProperties),
            sRows(&sProperties),
            sColumns(&sProperties),
            sShift(&sProperties),
            sTextGap(&sProperties),
            sLoop(&sProperties),
            sText(&sProperties)
        {
            pClass      = &metadata;
        }
        
        Indicator::~Indicator()
        {
            nFlags     |= FINALIZED;
        }

        status_t Indicator::init()
        {
            status_t result = Widget::init();
            if (result != STATUS_OK)
                return result;

            sColor.bind("color", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sRows.bind("rows", &sStyle);
            sColumns.bind("columns", &sStyle);
            sShift.bind("text.shift", &sStyle);
            sTextGap.bind("text.gap", &sStyle);
            sLoop.bind("text.loop", &sStyle);
            sText.bind(&sStyle, pDisplay->dictionary());

//            Style *sclass = style_class();
//            if (sclass != NULL)
//            {
//                sColor.init(sclass, "#111111");
//                sTextColor.init(sclass, "#00ff00");
//                sRows.init(sclass, 1);
//                sColumns.init(sclass, 5);
//                sShift.init(sclass, 0);
//                sTextGap.init(sclass, 0);
//                sLoop.init(sclass, false);
//            }

            return STATUS_OK;
        }

        void Indicator::property_changed(Property *prop)
        {
            Widget::property_changed(prop);
            if (sColor.is(prop))
                query_draw();
            if (sTextColor.is(prop))
                query_draw();
            if (sRows.is(prop))
                query_resize();
            if (sColumns.is(prop))
                query_resize();
            if (sShift.is(prop))
                query_draw();
            if (sTextGap.is(prop))
                query_draw();
            if (sLoop.is(prop))
                query_draw();
            if (sText.is(prop))
                query_draw();
        }

        void Indicator::draw_digit(ws::ISurface *s, float x, float y, size_t state, const lsp::Color &on, const lsp::Color &off)
        {
            float scaling   = lsp_max(0.0f, sScaling.get());
            const rect_t *r = segments;

            for (size_t i=0, m=1; i<11; ++i, m <<= 1, ++r)
            {
                const lsp::Color &col = (state & m) ? on : off;
                s->wire_rect(col, x + r->x * scaling - 0.5f, y + r->y * scaling - 0.5f, r->w * scaling, r->h * scaling, scaling);
            }
        }

        void Indicator::size_request(ws::size_limit_t *r)
        {
            float scaling   = lsp_max(0.0f, sScaling.get());
            size_t rows     = lsp_max(1, sRows.get());
            size_t cols     = lsp_max(1, sColumns.get());

            r->nMinWidth    = ceilf((16 * cols + 2) * scaling);
            r->nMinHeight   = ceilf((20 * rows + 2) * scaling);
            r->nMaxWidth    = r->nMinWidth;
            r->nMaxHeight   = r->nMinHeight;
            r->nPreWidth    = -1;
            r->nPreHeight   = -1;
        }

        uint8_t Indicator::get_char(const LSPString *str, size_t index)
        {
            // Compute the real index of character
            size_t length   = lsp_max(0, sTextGap.get()) + str->length();
            length          = lsp_max(1u, length);
            ssize_t shift   = sShift.get() + index;
            if (sLoop.get())
            {
                shift          %= ssize_t(length);
                if (shift < 0)
                    shift          += length;
            }

            // Get character
            lsp_wchar_t ch  = ((shift >= 0) && (size_t(shift) < str->length())) ? str->char_at(shift) : ' ';

            // Check ascii table match
            if (ch >= 0x80)
                ch          = 0;

            return uint8_t(ch);
        }

        void Indicator::draw(ws::ISurface *s)
        {
            float scaling   = lsp_max(0.0f, sScaling.get());
            float bright    = sBrightness.get();
            size_t rows     = lsp_max(1, sRows.get());
            size_t cols     = lsp_max(1, sColumns.get());
            size_t last     = rows * cols;

            // Prepare palette
            lsp::Color color(sColor);
            lsp::Color on(sTextColor);
            lsp::Color off(sTextColor);

            off.blend(color, 0.05f);
            on.scale_lightness(bright);
            off.scale_lightness(bright);
            color.scale_lightness(bright);

            // Draw glass
            s->clear(color);

            bool aa = s->set_antialiasing(true);

            LSPString text;
            sText.format(&text);

            uint8_t unget = 0;
            for (size_t offset = 0, ich = 0; offset < last; )
            {
                // Get character
                uint8_t ch  = (unget > 0) ? unget : get_char(&text, ich++);

                // Check for special case
                if (!unget)
                {
                    switch (ch)
                    {
                        case 'M':
                            unget   = ch;
                            ch      = 'N';
                            break;
                        case 'm':
                            unget   = ch;
                            ch      = 'n';
                            break;
                        case 'W':
                            unget   = ch;
                            ch      = 'U';
                            break;
                        case 'w':
                            unget   = ch;
                            ch      = 'v';
                            break;
                    }
                }
                else
                {
                    unget       = 0;
                    if (ch == '\r')
                        continue;
                }

                // Save state
                size_t state    = ascii_map[ch & 0xff];

                // Lookup for extra characters
                uint8_t ch2     = get_char(&text, ich);
                switch (ch2)
                {
                    case '.':
                    case ':':
                        state      |= ascii_map[ch2];
                        ++ich;
                        break;
                    default:
                        break;
                }

                // Output character
                size_t col      = offset % cols;
                size_t row      = offset / cols;

                if (ch == '\n') // Need to fill up to end-of-line
                {
                    for ( ; col < cols; ++col, ++offset)
                        draw_digit(s, (col*16 + 1) * scaling, (20*row + 1) * scaling, state, on, off);
                }
                else
                {
                    draw_digit(s, (col*16 + 1) * scaling, (20*row + 1) * scaling, state, on, off);
                    ++offset;
                }
            }

            s->set_antialiasing(aa);
        }

    } /* namespace tk */
} /* namespace lsp */
