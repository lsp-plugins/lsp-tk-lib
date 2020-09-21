/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 5 нояб. 2017 г.
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

namespace lsp
{
    namespace tk
    {
        const prop::desc_t Color::DESC[] =
        {
            { "",           PT_STRING   },
            { ".r",         PT_FLOAT    },
            { ".g",         PT_FLOAT    },
            { ".b",         PT_FLOAT    },
            { ".h",         PT_FLOAT    },
            { ".s",         PT_FLOAT    },
            { ".l",         PT_FLOAT    },
            { ".a",         PT_FLOAT    },
            { ".rgb",       PT_STRING   },
            { ".rgba",      PT_STRING   },
            { ".hsl",       PT_STRING   },
            { ".hsla",      PT_STRING   },
            { NULL,         PT_UNKNOWN  }
        };

        void Color::Listener::notify(atom_t property)
        {
            pValue->commit(property);
        }

        Color::Color(prop::Listener *listener):
            MultiProperty(vAtoms, P_COUNT, listener),
            sListener(this)
        {
        }

        Color::~Color()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void Color::sync()
        {
            if (pStyle != NULL)
            {
                pStyle->begin(&sListener);
                {
                    lsp::Color &c = sColor;
                    char buf[32];

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
                        pStyle->set_string(vAtoms[P_VALUE], buf);
                    }
                }
                pStyle->end();
            }

            if (pListener != NULL)
                pListener->notify(this);
        }

        void Color::commit(atom_t property)
        {
            if ((pStyle == NULL) || (property < 0))
                return;

            float v;
            lsp::Color &c = sColor;

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
                status_t res = c.parse4(s);
                if (res != STATUS_OK)
                    res = c.parse3(s);
                if (res != STATUS_OK)
                {
                    const lsp::Color *col = pStyle->schema()->color(s);
                    if (col != NULL)
                        c.copy(col);
                }
            }

            if (pListener != NULL)
                pListener->notify(this);
        }

        float Color::red(float r)
        {
            float old = sColor.red();
            if (old == r)
                return old;

            sColor.red(r);
            sync();
            return old;
        };

        float Color::green(float g)
        {
            float old = sColor.green();
            if (old == g)
                return old;

            sColor.green(g);
            sync();
            return old;
        };

        float Color::blue(float b)
        {
            float old = sColor.blue();
            if (old == b)
                return old;

            sColor.blue(b);
            sync();
            return old;
        };

        float Color::hue(float h)
        {
            float old = sColor.hue();
            if (old == h)
                return old;

            sColor.hue(h);
            sync();
            return old;
        };

        float Color::saturation(float s)
        {
            float old = sColor.saturation();
            if (old == s)
                return old;

            sColor.saturation(s);
            sync();
            return old;
        };

        float Color::lightness(float l)
        {
            float old = sColor.lightness();
            if (old == l)
                return old;

            sColor.lightness(l);
            sync();
            return old;
        };

        float Color::alpha(float a)
        {
            float old = sColor.alpha();
            if (old == a)
                return old;

            sColor.alpha(a);
            sync();
            return old;
        };

        void Color::set_rgb(float r, float g, float b)
        {
            float xr, xg, xb;
            sColor.get_rgb(xr, xg, xb);
            if ((xr == r) && (xg == g) && (xb == b))
                return;

            sColor.set_rgb(r, g, b);
            sync();
        }

        void Color::set_rgba(float r, float g, float b, float a)
        {
            float xr, xg, xb, xa;
            sColor.get_rgba(xr, xg, xb, xa);
            if ((xr == r) && (xg == g) && (xb == b) && (xa == a))
                return;

            sColor.set_rgba(r, g, b, a);
            sync();
        }

        void Color::set_rgb24(uint32_t v)
        {
            uint32_t old = sColor.rgb24();
            if (old == v)
                return;

            sColor.set_rgb24(v);
            sync();
        }

        void Color::set_rgba32(uint32_t v)
        {
            uint32_t old = sColor.rgba32();
            if (old == v)
                return;

            sColor.set_rgba32(v);
            sync();
        }

        void Color::set_hsl24(uint32_t v)
        {
            uint32_t old = sColor.hsl24();
            if (old == v)
                return;

            sColor.set_hsl24(v);
            sync();
        }

        void Color::set_hsla32(uint32_t v)
        {
            uint32_t old = sColor.hsla32();
            if (old == v)
                return;

            sColor.set_hsla32(v);
            sync();
        }

        void Color::set_hsl(float h, float s, float l)
        {
            float xh, xs, xl;
            sColor.get_hsl(xh, xs, xl);
            if ((xh == h) && (xs == s) && (xl == l))
                return;

            sColor.set_hsl(h, s, l);
            sync();
        }

        void Color::set_hsla(float h, float s, float l, float a)
        {
            float xh, xs, xl, xa;
            sColor.get_hsla(xh, xs, xl, xa);
            if ((xh == h) && (xs == s) && (xl == l) && (xa == a))
                return;

            sColor.set_hsla(h, s, l, a);
            sync();
        }

        bool Color::parse(lsp::Color *c, const char *s, Style *style)
        {
            // Try to parse color
            status_t res = c->parse4(s);
            if (res != STATUS_OK)
                res = c->parse3(s);
            if ((res != STATUS_OK) && (style != NULL))
            {
                const lsp::Color *col = style->schema()->color(s);
                if (col != NULL)
                {
                    c->copy(col);
                    res = STATUS_OK;
                }
            }

            return res == STATUS_OK;
        }

        void Color::set(const char *text)
        {
            if (parse(&sColor, text, pStyle))
                sync();
        }

        void Color::set(const LSPString *text)
        {
            if (parse(&sColor, text->get_utf8(), pStyle))
                sync();
        }

        void Color::set(const Color *src)
        {
            sColor.copy(&src->sColor);
            sync();
        }

        void Color::set(const ColorRange *src)
        {
            sColor.copy(src->color());
            sync();
        }

        void Color::swap(Color *c)
        {
            sColor.swap(&c->sColor);
            sync();
            c->sync();
        }

        namespace prop
        {
            status_t Color::init(Style *style, const char *value)
            {
                if ((style == NULL) || (value == NULL))
                    return STATUS_BAD_ARGUMENTS;
                else if (pStyle == NULL)
                    return STATUS_BAD_STATE;

                lsp::Color tmp;
                parse(&tmp, value, style);
                return init(style, &tmp);
            }

            status_t Color::init(Style *style, const LSPString *value)
            {
                if ((style == NULL) || (value == NULL))
                    return STATUS_BAD_ARGUMENTS;
                else if (pStyle == NULL)
                    return STATUS_BAD_STATE;

                lsp::Color tmp;
                parse(&tmp, value->get_utf8(), style);
                return init(style, &tmp);
            }

            status_t Color::init(Style *style, const lsp::Color *c)
            {
                if ((style == NULL) || (c == NULL))
                    return STATUS_BAD_ARGUMENTS;
                else if (pStyle == NULL)
                    return STATUS_BAD_STATE;

                char buf[32];

                style->begin();
                {
                    // R, G, B components
                    style->create_float(vAtoms[P_R], c->red());
                    style->create_float(vAtoms[P_G], c->green());
                    style->create_float(vAtoms[P_B], c->blue());

                    // H, S, L components
                    style->create_float(vAtoms[P_H], c->hue());
                    style->create_float(vAtoms[P_S], c->saturation());
                    style->create_float(vAtoms[P_L], c->lightness());

                    // Alpha component
                    style->create_float(vAtoms[P_A], c->alpha());

                    // Mixed components
                    c->format_rgb(buf, sizeof(buf)/sizeof(char));
                    style->create_string(vAtoms[P_RGB], buf);

                    c->format_rgba(buf, sizeof(buf)/sizeof(char));
                    style->create_string(vAtoms[P_RGBA], buf);

                    c->format_hsl(buf, sizeof(buf)/sizeof(char));
                    style->create_string(vAtoms[P_HSL], buf);

                    c->format_hsla(buf, sizeof(buf)/sizeof(char));
                    style->create_string(vAtoms[P_HSLA], buf);

                    if (c->is_hsl())
                        c->format_hsla(buf, sizeof(buf)/sizeof(char));
                    else
                        c->format_rgba(buf, sizeof(buf)/sizeof(char));
                    style->create_string(vAtoms[P_VALUE], buf);
                }
                style->end();

                return STATUS_OK;
            }

            status_t Color::override(Style *style, const char *value)
            {
                if ((style == NULL) || (value == NULL))
                    return STATUS_BAD_ARGUMENTS;
                else if (pStyle == NULL)
                    return STATUS_BAD_STATE;

                lsp::Color tmp;
                parse(&tmp, value, style);
                return override(style, &tmp);
            }

            status_t Color::override(Style *style, const LSPString *value)
            {
                if ((style == NULL) || (value == NULL))
                    return STATUS_BAD_ARGUMENTS;
                else if (pStyle == NULL)
                    return STATUS_BAD_STATE;

                lsp::Color tmp;
                parse(&tmp, value->get_utf8(), style);
                return override(style, &tmp);
            }

            status_t Color::override(Style *style, const lsp::Color *c)
            {
                if ((style == NULL) || (c == NULL))
                    return STATUS_BAD_ARGUMENTS;
                else if (pStyle == NULL)
                    return STATUS_BAD_STATE;

                char buf[32];

                style->begin();
                {
                    // R, G, B components
                    style->override_float(vAtoms[P_R], c->red());
                    style->override_float(vAtoms[P_G], c->green());
                    style->override_float(vAtoms[P_B], c->blue());

                    // H, S, L components
                    style->override_float(vAtoms[P_H], c->hue());
                    style->override_float(vAtoms[P_S], c->saturation());
                    style->override_float(vAtoms[P_L], c->lightness());

                    // Alpha component
                    style->override_float(vAtoms[P_A], c->alpha());

                    // Mixed components
                    c->format_rgb(buf, sizeof(buf)/sizeof(char));
                    style->override_string(vAtoms[P_RGB], buf);

                    c->format_rgba(buf, sizeof(buf)/sizeof(char));
                    style->override_string(vAtoms[P_RGBA], buf);

                    c->format_hsl(buf, sizeof(buf)/sizeof(char));
                    style->override_string(vAtoms[P_HSL], buf);

                    c->format_hsla(buf, sizeof(buf)/sizeof(char));
                    style->override_string(vAtoms[P_HSLA], buf);

                    if (c->is_hsl())
                        c->format_hsla(buf, sizeof(buf)/sizeof(char));
                    else
                        c->format_rgba(buf, sizeof(buf)/sizeof(char));
                    style->override_string(vAtoms[P_VALUE], buf);
                }
                style->end();

                return STATUS_OK;
            }
        }
    } /* namespace tk */
} /* namespace lsp */
