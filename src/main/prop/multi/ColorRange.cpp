/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 21 сент. 2020 г.
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
#include <lsp-plug.in/io/InStringSequence.h>
#include <lsp-plug.in/expr/Tokenizer.h>

namespace lsp
{
    namespace tk
    {
        const prop::desc_t ColorRange::DESC[] =
        {
            { "",               PT_STRING   },
            { ".r",             PT_FLOAT    },
            { ".g",             PT_FLOAT    },
            { ".b",             PT_FLOAT    },
            { ".h",             PT_FLOAT    },
            { ".s",             PT_FLOAT    },
            { ".l",             PT_FLOAT    },
            { ".a",             PT_FLOAT    },
            { ".rgb",           PT_STRING   },
            { ".rgba",          PT_STRING   },
            { ".hsl",           PT_STRING   },
            { ".hsla",          PT_STRING   },
            { ".min",           PT_FLOAT    },
            { ".max",           PT_FLOAT    },
            { NULL,             PT_UNKNOWN  }
        };


        ColorRange::ColorRange(prop::Listener *listener):
            MultiProperty(vAtoms, P_COUNT, listener)
        {
            fMin        = 0.0f;
            fMax        = 1.0f;
            nFlags      = 0;
        }

        ColorRange::~ColorRange()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void ColorRange::push()
        {
            char buf[32];
            LSPString s;
            lsp::Color &c = sColor;

            // Min and max components
            if (vAtoms[P_MIN] >= 0)
                pStyle->set_float(vAtoms[P_MIN], fMin);
            if (vAtoms[P_MAX] >= 0)
                pStyle->set_float(vAtoms[P_MAX], fMax);

            // R, G, B components
            if (vAtoms[P_R] >= 0)
                pStyle->set_float(vAtoms[P_R], c.red());
            if (vAtoms[P_G] >= 0)
                pStyle->set_float(vAtoms[P_G], c.green());
            if (vAtoms[P_B] >= 0)
                pStyle->set_float(vAtoms[P_B], c.blue());

            // H, S, L components
            if (vAtoms[P_H] >= 0)
                pStyle->set_float(vAtoms[P_H], c.hue());
            if (vAtoms[P_S] >= 0)
                pStyle->set_float(vAtoms[P_S], c.saturation());
            if (vAtoms[P_L] >= 0)
                pStyle->set_float(vAtoms[P_L], c.lightness());

            // Alpha component
            if (vAtoms[P_A] >= 0)
                pStyle->set_float(vAtoms[P_A], c.alpha());

            // Mixed components
            if (vAtoms[P_RGB] >= 0)
            {
                c.format_rgb(buf, sizeof(buf)/sizeof(char));
                pStyle->set_string(vAtoms[P_RGB], buf);
            }
            if (vAtoms[P_RGBA] >= 0)
            {
                c.format_rgba(buf, sizeof(buf)/sizeof(char));
                pStyle->set_string(vAtoms[P_RGBA], buf);
            }
            if (vAtoms[P_HSL] >= 0)
            {
                c.format_hsl(buf, sizeof(buf)/sizeof(char));
                pStyle->set_string(vAtoms[P_HSL], buf);
            }
            if (vAtoms[P_HSLA] >= 0)
            {
                c.format_hsla(buf, sizeof(buf)/sizeof(char));
                pStyle->set_string(vAtoms[P_HSLA], buf);
            }
            if (vAtoms[P_VALUE] >= 0)
            {
                if (c.is_hsl())
                    c.format_hsla(buf, sizeof(buf)/sizeof(char));
                else
                    c.format_rgba(buf, sizeof(buf)/sizeof(char));

                s.fmt_ascii("%.10f %.10f %s", fMin, fMax, buf);
                pStyle->set_string(vAtoms[P_VALUE], &s);
            }
        }

        void ColorRange::commit(atom_t property)
        {
            float v;
            lsp::Color &c = sColor;

            if ((property == vAtoms[P_MIN]) && (pStyle->get_float(vAtoms[P_MIN], &v) == STATUS_OK))
                fMin            = v;
            if ((property == vAtoms[P_MAX]) && (pStyle->get_float(vAtoms[P_MAX], &v) == STATUS_OK))
                fMax            = v;

            if ((property == vAtoms[P_R]) && (pStyle->get_float(vAtoms[P_R], &v) == STATUS_OK))
                c.red(v);
            if ((property == vAtoms[P_G]) && (pStyle->get_float(vAtoms[P_G], &v) == STATUS_OK))
                c.green(v);
            if ((property == vAtoms[P_B]) && (pStyle->get_float(vAtoms[P_B], &v) == STATUS_OK))
                c.blue(v);

            if ((property == vAtoms[P_H]) && (pStyle->get_float(vAtoms[P_H], &v) == STATUS_OK))
                c.hue(v);
            if ((property == vAtoms[P_S]) && (pStyle->get_float(vAtoms[P_S], &v) == STATUS_OK))
                c.saturation(v);
            if ((property == vAtoms[P_L]) && (pStyle->get_float(vAtoms[P_L], &v) == STATUS_OK))
                c.lightness(v);

            if ((property == vAtoms[P_A]) && (pStyle->get_float(vAtoms[P_A], &v) == STATUS_OK))
                c.alpha(v);

            const char *s;
            if ((property == vAtoms[P_HSL]) && (pStyle->get_string(vAtoms[P_HSL], &s) == STATUS_OK))
                c.parse_hsl(s);
            if ((property == vAtoms[P_HSLA]) && (pStyle->get_string(vAtoms[P_HSLA], &s) == STATUS_OK))
                c.parse_hsla(s);

            if ((property == vAtoms[P_RGB]) && (pStyle->get_string(vAtoms[P_RGB], &s) == STATUS_OK))
                c.parse_rgb(s);
            if ((property == vAtoms[P_RGBA]) && (pStyle->get_string(vAtoms[P_RGBA], &s) == STATUS_OK))
                c.parse_rgba(s);

            if ((property == vAtoms[P_VALUE]) && (pStyle->get_string(vAtoms[P_VALUE], &s) == STATUS_OK))
            {
                status_t res;
                io::InStringSequence is;
                ColorRange color;

                if ((res = is.wrap(s)) == STATUS_OK)
                {
                    if ((res = parse_range(&color, &is)) == STATUS_OK)
                    {
                        fMin        = color.fMin;
                        fMax        = color.fMax;
                        sColor.copy(color.sColor);
                    }
                }
            }
        }

        float ColorRange::set_min(float v)
        {
            float old = fMin;
            if (v == old)
                return old;

            fMin                = v;
            sync();
            return old;
        }

        float ColorRange::set_max(float v)
        {
            float old = fMax;
            if (v == old)
                return old;

            fMax                = v;
            sync();
            return old;
        }

        void ColorRange::set_range(float min, float max)
        {
            if ((fMin == min) && (fMax == max))
                return;

            fMin        = min;
            fMax        = max;
            sync();
        }

        status_t ColorRange::parse_range(ColorRange *range, io::IInSequence *is)
        {
            status_t res, res2;
            expr::Tokenizer t(is);

            if ((res = parse(range, &t, pStyle)) == STATUS_OK)
            {
                res = t.get_token(expr::TF_GET);
                res = (res != expr::TT_EOF) ? STATUS_BAD_FORMAT : STATUS_OK;
            }

            res2            = is->close();
            return (res == STATUS_OK) ? res2 : res;
        }

        status_t ColorRange::set(const char *s)
        {
            status_t res;
            ColorRange color;
            io::InStringSequence is;
            if ((res = is.wrap(s)) != STATUS_OK)
                return res;

            if ((res = parse_range(&color, &is)) == STATUS_OK)
            {
                fMin        = color.fMin;
                fMax        = color.fMax;
                sColor.copy(color.sColor);

                sync();
            }
            return res;
        }

        status_t ColorRange::set(const LSPString *s)
        {
            status_t res;
            ColorRange color;
            io::InStringSequence is;
            if ((res = is.wrap(s)) != STATUS_OK)
                return res;

            if ((res = parse_range(&color, &is)) == STATUS_OK)
            {
                fMin        = color.fMin;
                fMax        = color.fMax;
                sColor.copy(color.sColor);

                sync();
            }
            return res;
        }

        void ColorRange::set_color(const char *s)
        {
            if (Color::parse(&sColor, s, pStyle))
                sync();
        }

        void ColorRange::set_color(const LSPString *s)
        {
            if (Color::parse(&sColor, s->get_utf8(), pStyle))
                sync();
        }

        void ColorRange::set_color(const lsp::Color *c)
        {
            sColor.copy(c);
            sync();
        }

        void ColorRange::set_color(const lsp::Color &c)
        {
            sColor.copy(c);
            sync();
        }

        void ColorRange::set(const lsp::Color *c)
        {
            sColor.copy(c);
            sync();
        }

        void ColorRange::set(const lsp::Color &c)
        {
            sColor.copy(c);
            sync();
        }

        void ColorRange::set(const Color *src)
        {
            sColor.copy(src->color());
            sync();
        }

        void ColorRange::set(const ColorRange *src)
        {
            sColor.copy(src->color());
            fMin        = src->fMin;
            fMax        = src->fMax;

            sync();
        }

        float ColorRange::red(float r)
        {
            float old = sColor.red();
            if (old == r)
                return old;

            sColor.red(r);
            sync();
            return old;
        };

        float ColorRange::green(float g)
        {
            float old = sColor.green();
            if (old == g)
                return old;

            sColor.green(g);
            sync();
            return old;
        };

        float ColorRange::blue(float b)
        {
            float old = sColor.blue();
            if (old == b)
                return old;

            sColor.blue(b);
            sync();
            return old;
        };

        float ColorRange::hue(float h)
        {
            float old = sColor.hue();
            if (old == h)
                return old;

            sColor.hue(h);
            sync();
            return old;
        };

        float ColorRange::saturation(float s)
        {
            float old = sColor.saturation();
            if (old == s)
                return old;

            sColor.saturation(s);
            sync();
            return old;
        };

        float ColorRange::lightness(float l)
        {
            float old = sColor.lightness();
            if (old == l)
                return old;

            sColor.lightness(l);
            sync();
            return old;
        };

        float ColorRange::alpha(float a)
        {
            float old = sColor.alpha();
            if (old == a)
                return old;

            sColor.alpha(a);
            sync();
            return old;
        };

        void ColorRange::set_rgb(float r, float g, float b)
        {
            float xr, xg, xb;
            sColor.get_rgb(xr, xg, xb);
            if ((xr == r) && (xg == g) && (xb == b))
                return;

            sColor.set_rgb(r, g, b);
            sync();
        }

        void ColorRange::set_rgba(float r, float g, float b, float a)
        {
            float xr, xg, xb, xa;
            sColor.get_rgba(xr, xg, xb, xa);
            if ((xr == r) && (xg == g) && (xb == b) && (xa == a))
                return;

            sColor.set_rgba(r, g, b, a);
            sync();
        }

        void ColorRange::set_rgb24(uint32_t v)
        {
            uint32_t old = sColor.rgb24();
            if (old == v)
                return;

            sColor.set_rgb24(v);
            sync();
        }

        void ColorRange::set_rgba32(uint32_t v)
        {
            uint32_t old = sColor.rgba32();
            if (old == v)
                return;

            sColor.set_rgba32(v);
            sync();
        }

        void ColorRange::set_hsl24(uint32_t v)
        {
            uint32_t old = sColor.hsl24();
            if (old == v)
                return;

            sColor.set_hsl24(v);
            sync();
        }

        void ColorRange::set_hsla32(uint32_t v)
        {
            uint32_t old = sColor.hsla32();
            if (old == v)
                return;

            sColor.set_hsla32(v);
            sync();
        }

        void ColorRange::set_hsl(float h, float s, float l)
        {
            float xh, xs, xl;
            sColor.get_hsl(xh, xs, xl);
            if ((xh == h) && (xs == s) && (xl == l))
                return;

            sColor.set_hsl(h, s, l);
            sync();
        }

        void ColorRange::set_hsla(float h, float s, float l, float a)
        {
            float xh, xs, xl, xa;
            sColor.get_hsla(xh, xs, xl, xa);
            if ((xh == h) && (xs == s) && (xl == l) && (xa == a))
                return;

            sColor.set_hsla(h, s, l, a);
            sync();
        }

        status_t ColorRange::parse(ColorRange *range, expr::Tokenizer *t, Style *style)
        {
            // Minimum
            switch (t->get_token(expr::TF_GET))
            {
                case expr::TT_IVALUE:
                    range->fMin     = t->int_value();
                    break;
                case expr::TT_FVALUE:
                    range->fMin     = t->float_value();
                    break;
                default:
                    return STATUS_BAD_FORMAT;
            }

            // Maximum
            switch (t->get_token(expr::TF_GET))
            {
                case expr::TT_IVALUE:
                    range->fMax     = t->int_value();
                    break;
                case expr::TT_FVALUE:
                    range->fMax     = t->float_value();
                    break;
                default:
                    return STATUS_BAD_FORMAT;
            }

            // Color
            switch (t->get_token(expr::TF_GET | expr::TF_COLOR | expr::TF_XKEYWORDS))
            {
                case expr::TT_COLOR:
                {
                    status_t res = range->sColor.parse4(t->text_value());
                    if (res != STATUS_OK)
                        res = range->sColor.parse3(t->text_value());
                    break;
                }

                case expr::TT_BAREWORD:
                {
                    const lsp::Color *col = (style != NULL) ? style->schema()->color(t->text_value()) : NULL;
                    if (col != NULL)
                        range->sColor.copy(col);
                    break;
                }

                default:
                    return STATUS_BAD_FORMAT;
            }

            return STATUS_OK;
        }

        void ColorRange::swap(ColorRange *src)
        {
            lsp::swap(fMin, src->fMin);
            lsp::swap(fMax, src->fMax);
            sColor.swap(&src->sColor);

            sync();
            src->sync();
        }
    } /* namespace tk */
} /* namespace lsp */

