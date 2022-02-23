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
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace tk
    {
        const prop::desc_t Color::DESC[] =
        {
            { "",           PT_STRING   },
            { ".a",         PT_FLOAT    },
            { NULL,         PT_UNKNOWN  }
        };

        Color::Color(prop::Listener *listener):
            MultiProperty(vAtoms, P_COUNT, listener)
        {
        }

        Color::~Color()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void Color::push()
        {
            lsp::Color &c = sColor;
            char buf[64];

            // Alpha component
            if (vAtoms[P_A] >= 0)
                pStyle->set_float(vAtoms[P_A], c.alpha());

            if (vAtoms[P_VALUE] >= 0)
            {
                c.format4(buf, sizeof(buf)/sizeof(char));
                pStyle->set_string(vAtoms[P_VALUE], buf);
            }
        }

        void Color::commit(atom_t property)
        {
            float v;
            lsp::Color &c = sColor;

            if ((property == vAtoms[P_A]) && (pStyle->get_float(vAtoms[P_A], &v) == STATUS_OK))
                c.alpha(v);

            const char *s;
            if ((property == vAtoms[P_VALUE]) && (pStyle->get_string(vAtoms[P_VALUE], &s) == STATUS_OK))
                parse(&c, s, pStyle);
        }

        float Color::red(float r)
        {
            float old = sColor.red();
            sColor.red(r);
            sync();
            return old;
        };

        float Color::green(float g)
        {
            float old = sColor.green();
            sColor.green(g);
            sync();
            return old;
        };

        float Color::blue(float b)
        {
            float old = sColor.blue();
            sColor.blue(b);
            sync();
            return old;
        };

        float Color::hue(float h)
        {
            float old = sColor.hue();
            sColor.hue(h);
            sync();
            return old;
        };

        float Color::saturation(float s)
        {
            float old = sColor.saturation();
            sColor.saturation(s);
            sync();
            return old;
        };

        float Color::lightness(float l)
        {
            float old = sColor.lightness();
            sColor.lightness(l);
            sync();
            return old;
        };

        float Color::hsl_hue(float h)
        {
            float old = sColor.hsl_hue();
            sColor.hue(h);
            sync();
            return old;
        };

        float Color::hsl_saturation(float s)
        {
            float old = sColor.hsl_saturation();
            sColor.saturation(s);
            sync();
            return old;
        };

        float Color::hsl_lightness(float l)
        {
            float old = sColor.hsl_lightness();
            sColor.lightness(l);
            sync();
            return old;
        };

        float Color::alpha(float a)
        {
            float old = sColor.alpha();
            sColor.alpha(a);
            sync();
            return old;
        };

        void Color::set_rgb(float r, float g, float b)
        {
            sColor.set_rgb(r, g, b);
            sync();
        }

        void Color::set_rgba(float r, float g, float b, float a)
        {
            sColor.set_rgba(r, g, b, a);
            sync();
        }

        void Color::set_rgb24(uint32_t v)
        {
            sColor.set_rgb24(v);
            sync();
        }

        void Color::set_rgba32(uint32_t v)
        {
            sColor.set_rgba32(v);
            sync();
        }

        void Color::set_hsl24(uint32_t v)
        {
            sColor.set_hsl24(v);
            sync();
        }

        void Color::set_hsla32(uint32_t v)
        {
            sColor.set_hsla32(v);
            sync();
        }

        void Color::set_hsl(float h, float s, float l)
        {
            sColor.set_hsl(h, s, l);
            sync();
        }

        void Color::set_hsla(float h, float s, float l, float a)
        {
            sColor.set_hsla(h, s, l, a);
            sync();
        }

        bool Color::parse(lsp::Color *c, const char *s, Style *style)
        {
            // Try to parse color
            status_t res = c->parse(s);
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

        void Color::set(const Color &src)
        {
            sColor.copy(&src.sColor);
            sync();
        }

        void Color::set(const lsp::Color *src)
        {
            sColor.copy(src);
            sync();
        }

        void Color::set(const lsp::Color &src)
        {
            sColor.copy(src);
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

        float Color::xyz_x(float x)
        {
            float old = sColor.xyz_x();
            sColor.xyz_x(x);
            sync();
            return old;
        };

        float Color::xyz_y(float y)
        {
            float old = sColor.xyz_y();
            sColor.xyz_y(y);
            sync();
            return old;
        };

        float Color::xyz_z(float z)
        {
            float old = sColor.xyz_z();
            sColor.xyz_z(z);
            sync();
            return old;
        };

        void Color::set_xyz(float x, float y, float z)
        {
            sColor.set_xyz(x, y, z);
            sync();
        }

        void Color::set_xyza(float x, float y, float z, float a)
        {
            sColor.set_xyza(x, y, z, a);
            sync();
        }

        float Color::lab_l(float l)
        {
            float old = sColor.lab_l();
            sColor.lab_l(l);
            sync();
            return old;
        };

        float Color::lab_a(float a)
        {
            float old = sColor.lab_a();
            sColor.lab_a(a);
            sync();
            return old;
        };

        float Color::lab_b(float b)
        {
            float old = sColor.lab_b();
            sColor.lab_b(b);
            sync();
            return old;
        };

        void Color::set_lab(float l, float a, float b)
        {
            sColor.set_lab(l, a, b);
            sync();
        }

        void Color::set_laba(float l, float a, float b, float alpha)
        {
            sColor.set_laba(l, a, b, alpha);
            sync();
        }

        float Color::lch_l(float l)
        {
            float old = sColor.lch_l();
            sColor.lch_l(l);
            sync();
            return old;
        }

        float Color::lch_c(float c)
        {
            float old = sColor.lch_c();
            sColor.lch_c(c);
            sync();
            return old;
        }

        float Color::lch_h(float h)
        {
            float old = sColor.lch_h();
            sColor.lch_h(h);
            sync();
            return old;
        }

        float Color::hcl_l(float l)
        {
            float old = sColor.hcl_l();
            sColor.hcl_l(l);
            sync();
            return old;
        }

        float Color::hcl_c(float c)
        {
            float old = sColor.hcl_c();
            sColor.hcl_c(c);
            sync();
            return old;
        }

        float Color::hcl_h(float h)
        {
            float old = sColor.hcl_h();
            sColor.hcl_h(h);
            sync();
            return old;
        }

        void Color::set_lch(float l, float c, float h)
        {
            sColor.set_lch(l, c, h);
            sync();
        }

        void Color::set_lcha(float l, float c, float h, float alpha)
        {
            sColor.set_lcha(l, c, h, alpha);
            sync();
        }

        void Color::set_hcl(float h, float c, float l)
        {
            sColor.set_hcl(h, c, l);
            sync();
        }

        void Color::set_hcla(float h, float c, float l, float alpha)
        {
            sColor.set_hcla(h, c, l, alpha);
            sync();
        }

        float Color::cyan(float c)
        {
            float old = sColor.cyan();
            sColor.cyan(c);
            sync();
            return old;
        }

        float Color::magenta(float m)
        {
            float old = sColor.magenta();
            sColor.magenta(m);
            sync();
            return old;
        }

        float Color::yellow(float y)
        {
            float old = sColor.yellow();
            sColor.yellow(y);
            sync();
            return old;
        }

        float Color::black(float k)
        {
            float old = sColor.black();
            sColor.black(k);
            sync();
            return old;
        }

        void Color::set_cmyk(float c, float m, float y, float k)
        {
            sColor.set_cmyk(c, m, y, k);
            sync();
        }

        void Color::set_cmyka(float c, float m, float y, float k, float a)
        {
            sColor.set_cmyka(c, m, y, k, a);
            sync();
        }

    } /* namespace tk */
} /* namespace lsp */
