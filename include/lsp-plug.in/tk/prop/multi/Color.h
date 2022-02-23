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

#ifndef LSP_PLUG_IN_TK_PROP_MULTI_COLOR_H_
#define LSP_PLUG_IN_TK_PROP_MULTI_COLOR_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class ColorRange;

        /**
         * Color property interface
         */
        class Color: public MultiProperty
        {
            protected:
                Color & operator = (const Color &);
                Color(const Color &);

            protected:
                enum property_t
                {
                    P_VALUE,
                    P_A,

                    P_COUNT
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                lsp::Color          sColor;             // Color holder

            protected:
                virtual void        push();
                virtual void        commit(atom_t property);

            protected:
                explicit Color(prop::Listener *listener = NULL);
                virtual ~Color();

            public:
                inline void set_default()       { MultiProperty::set_default(vAtoms, DESC); };

                void set(const char *text);
                void set(const LSPString *text);
                void set(const Color *src);
                void set(const Color &src);
                void set(const lsp::Color *src);
                void set(const lsp::Color &src);
                void set(const ColorRange *src);

                void copy(const lsp::Color &c)                              { sColor.copy(c); sync(); }
                void copy(const lsp::Color *c)                              { sColor.copy(c); sync(); }
                void copy(const Color &c)                                   { sColor.copy(c.sColor); sync(); }
                void copy(const Color *c)                                   { sColor.copy(c->sColor); sync(); }

                void swap(Color *c);
                inline void swap(Color &c)      { swap(&c);                     }

            public: // RGB color manipulation
                inline float red() const        { return sColor.red();          }
                inline float green() const      { return sColor.green();        }
                inline float blue() const       { return sColor.blue();         }

                inline void get_rgb(float &r, float &g, float &b) const             { sColor.get_rgb(r, g, b); }
                inline void get_rgba(float &r, float &g, float &b, float &a) const  { sColor.get_rgba(r, g, b, a); }

                inline uint32_t rgb24() const   { return sColor.rgb24();        }
                inline uint32_t rgba32() const  { return sColor.rgba32();       }

                float red(float r);
                float green(float g);
                float blue(float b);

                void set_rgb(float r, float g, float b);
                void set_rgba(float r, float g, float b, float a);
                void set_rgb24(uint32_t v);
                void set_rgba32(uint32_t v);

                inline int format_rgb(char *dst, size_t len, size_t tolerance = 2) const
                {
                    return sColor.format_rgb(dst, len, tolerance);
                }

                inline int format_rgba(char *dst, size_t len, size_t tolerance = 2) const
                {
                    return sColor.format_rgba(dst, len, tolerance);
                }

            public: // HSL color manipulation
                inline float hue() const                { return sColor.hsl_hue();          }
                inline float saturation() const         { return sColor.hsl_saturation();   }
                inline float lightness() const          { return sColor.hsl_lightness();    }
                inline float hsl_hue() const            { return sColor.hsl_hue();          }
                inline float hsl_saturation() const     { return sColor.hsl_saturation();   }
                inline float hsl_lightness() const      { return sColor.hsl_lightness();    }

                float hue(float h);
                float saturation(float s);
                float lightness(float l);

                float hsl_hue(float h);
                float hsl_saturation(float s);
                float hsl_lightness(float l);

                inline void get_hsl(float &h, float &s, float &l) const             { sColor.get_hsl(h, s, l); }
                inline void get_hsla(float &h, float &s, float &l, float &a) const  { sColor.get_hsla(h, s, l, a); }

                inline uint32_t hsl24() const           { return sColor.hsl24();            }
                inline uint32_t hsla32() const          { return sColor.hsla32();           }

                void set_hsl24(uint32_t v);
                void set_hsla32(uint32_t v);
                void set_hsl(float h, float s, float l);
                void set_hsla(float h, float s, float l, float a);

                inline int format_hsl(char *dst, size_t len, size_t tolerance = 2) const
                {
                    return sColor.format_hsl(dst, len, tolerance);
                }

                inline int format_hsla(char *dst, size_t len, size_t tolerance = 2) const
                {
                    return sColor.format_hsla(dst, len, tolerance);
                }

            public: // CIE XYZ D65 color manipulation
                inline float xyz_x() const              { return sColor.xyz_x();            }
                inline float xyz_y() const              { return sColor.xyz_y();            }
                inline float xyz_z() const              { return sColor.xyz_z();            }

                float xyz_x(float x);
                float xyz_y(float y);
                float xyz_z(float z);

                inline void get_xyz(float &x, float &y, float &z) const             { sColor.get_xyz(x, y, z);      }
                inline void get_xyza(float &x, float &y, float &z, float &a) const  { sColor.get_xyza(x, y, z, a);  }

                void set_xyz(float x, float y, float z);
                void set_xyza(float x, float y, float z, float a);

            public: // CIE LAB D65 color manipulation
                inline float lab_l() const              { return sColor.lab_l();            }
                inline float lab_a() const              { return sColor.lab_a();            }
                inline float lab_b() const              { return sColor.lab_b();            }

                float lab_l(float l);
                float lab_a(float a);
                float lab_b(float b);

                inline void get_lab(float &l, float &a, float &b) const                 { sColor.get_lab(l, a, b);          }
                inline void get_laba(float &l, float &a, float &b, float &alpha) const  { sColor.get_laba(l, a, b, alpha);  }

                void set_lab(float l, float a, float b);
                void set_laba(float l, float a, float b, float alpha);

            public: // CIE LCH D65 color manipulation
                inline float lch_l() const              { return sColor.lch_l();            }
                inline float lch_c() const              { return sColor.lch_c();            }
                inline float lch_h() const              { return sColor.lch_h();            }
                inline float hcl_l() const              { return sColor.hcl_l();            }
                inline float hcl_c() const              { return sColor.hcl_c();            }
                inline float hcl_h() const              { return sColor.hcl_h();            }

                float lch_l(float l);
                float lch_c(float c);
                float lch_h(float h);
                float hcl_l(float l);
                float hcl_c(float c);
                float hcl_h(float h);

                inline void get_lch(float &l, float &c, float &h) const                 { sColor.get_lch(l, c, h);          }
                inline void get_lcha(float &l, float &c, float &h, float &alpha) const  { sColor.get_lcha(l, c, h, alpha);  }
                inline void get_hcl(float &h, float &c, float &l) const                 { sColor.get_hcl(h, c, l);          }
                inline void get_hcla(float &h, float &c, float &l, float &alpha) const  { sColor.get_hcla(h, c, l, alpha);  }

                void set_lch(float l, float c, float h);
                void set_lcha(float l, float c, float h, float alpha);
                void set_hcl(float h, float c, float l);
                void set_hcla(float h, float c, float l, float alpha);

            public:
                inline float cyan() const               { return sColor.cyan();             }
                inline float magenta() const            { return sColor.magenta();          }
                inline float yellow() const             { return sColor.yellow();           }
                inline float black() const              { return sColor.black();            }

                float cyan(float c);
                float magenta(float m);
                float yellow(float y);
                float black(float k);

                inline void get_cmyk(float &c, float &m, float &y, float &k) const              { sColor.get_cmyk(c, m, y, k);      }
                inline void get_cmyk(float &c, float &m, float &y, float &k, float &a) const    { sColor.get_cmyka(c, m, y, k, a);  }

                void set_cmyk(float c, float m, float y, float k);
                void set_cmyka(float c, float m, float y, float k, float a);

            public: // Alpha-channel manipulation
                inline float alpha() const              { return sColor.alpha();            }
                float alpha(float a);

            public:
                void blend(const lsp::Color &c, float alpha)                { sColor.blend(c, alpha); sync(); }
                void blend(float r, float g, float b, float alpha)          { sColor.blend(r, g, b, alpha); sync(); }
                void darken(float amount)                                   { sColor.darken(amount); sync(); }
                void lighten(float amount)                                  { sColor.lighten(amount); sync(); }

            public:
                inline const lsp::Color *color() const       { return &sColor; }
                operator const lsp::Color *() const          { return &sColor; }

            public:
                static bool             parse(lsp::Color *c, const char *text, Style *style);

        };

        namespace prop
        {
            /**
             * Color property implementation
             */
            class Color: public tk::Color
            {
                private:
                    Color & operator = (const Color &);
                    Color(const Color &);

                public:
                    explicit inline Color(prop::Listener *listener = NULL): tk::Color(listener) {};

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::Color::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::Color::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::Color::bind(property, style, vAtoms, DESC, &sListener); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::Color::unbind(vAtoms, DESC, &sListener); };

                    inline void         listener(prop::Listener *listener)              { pListener = listener;                     }
            };
        }

    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_MULTI_COLOR_H_ */
