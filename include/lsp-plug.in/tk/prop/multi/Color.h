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
                    P_R, P_G, P_B,
                    P_H, P_S, P_L,
                    P_A,
                    P_RGB, P_RGBA,
                    P_HSL, P_HSLA,

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
                inline void         set_default()   { MultiProperty::set_default(vAtoms, DESC); };

            public:
                inline float red() const        { return sColor.red();      }
                inline float green() const      { return sColor.green();    }
                inline float blue() const       { return sColor.blue();     }
                inline float alpha() const      { return sColor.alpha();    }

                inline void get_rgb(float &r, float &g, float &b) const             { sColor.get_rgb(r, g, b); }
                inline void get_rgba(float &r, float &g, float &b, float &a) const  { sColor.get_rgba(r, g, b, a); }

                inline float hue() const        { return sColor.hue();          }
                inline float saturation() const { return sColor.saturation();   }
                inline float lightness() const  { return sColor.lightness();    }

                inline void get_hsl(float &h, float &s, float &l) const             { sColor.get_hsl(h, s, l); }
                inline void get_hsla(float &h, float &s, float &l, float &a) const  { sColor.get_hsla(h, s, l, a); }

                inline uint32_t rgb24() const   { return sColor.rgb24();        }
                inline uint32_t rgba32() const  { return sColor.rgba32();       }

                inline uint32_t hsl24() const   { return sColor.hsl24();        }
                inline uint32_t hsla32() const  { return sColor.hsla32();       }

            public:
                float red(float r);
                float green(float g);
                float blue(float b);
                float alpha(float a);

                void set_rgb(float r, float g, float b);
                void set_rgba(float r, float g, float b, float a);
                void set_rgb24(uint32_t v);
                void set_rgba32(uint32_t v);
                void set_hsl24(uint32_t v);
                void set_hsla32(uint32_t v);

                float hue(float h);
                float saturation(float s);
                float lightness(float l);

                void set_hsl(float h, float s, float l);
                void set_hsla(float h, float s, float l, float a);
                void set(const char *text);
                void set(const LSPString *text);
                void set(const Color *src);
                void set(const Color &src);
                void set(const lsp::Color *src);
                void set(const lsp::Color &src);
                void set(const ColorRange *src);

            public:
                void blend(const lsp::Color &c, float alpha)                { sColor.blend(c, alpha); sync(); }
                void blend(float r, float g, float b, float alpha)          { sColor.blend(r, g, b, alpha); sync(); }
                void darken(float amount)                                   { sColor.darken(amount); sync(); }
                void lighten(float amount)                                  { sColor.lighten(amount); sync(); }

                void copy(const lsp::Color &c)                              { sColor.copy(c); sync(); }
                void copy(const lsp::Color *c)                              { sColor.copy(c); sync(); }

                void copy(const Color &c)                                   { sColor.copy(c.sColor); sync(); }
                void copy(const Color *c)                                   { sColor.copy(c->sColor); sync(); }

                inline int format_rgb(char *dst, size_t len, size_t tolerance = 2) const
                {
                    return sColor.format_rgb(dst, len, tolerance);
                }

                inline int format_rgba(char *dst, size_t len, size_t tolerance = 2) const
                {
                    return sColor.format_rgba(dst, len, tolerance);
                }

                inline int format_hsl(char *dst, size_t len, size_t tolerance = 2) const
                {
                    return sColor.format_hsl(dst, len, tolerance);
                }

                inline int format_hsla(char *dst, size_t len, size_t tolerance = 2) const
                {
                    return sColor.format_hsla(dst, len, tolerance);
                }

            public:
                inline const lsp::Color *color() const       { return &sColor; }

                operator const lsp::Color *() const          { return &sColor; }

            public:
                static bool             parse(lsp::Color *c, const char *text, Style *style);

                void                    swap(Color *c);
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
