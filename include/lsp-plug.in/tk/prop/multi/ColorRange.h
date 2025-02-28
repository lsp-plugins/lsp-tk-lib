/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef LSP_PLUG_IN_TK_PROP_MULTI_COLORRANGE_H_
#define LSP_PLUG_IN_TK_PROP_MULTI_COLORRANGE_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/expr/Tokenizer.h>

namespace lsp
{
    namespace tk
    {
        /**
         * Color with attached range
         */
        class ColorRange: public MultiProperty
        {
            protected:
                enum property_t
                {
                    P_VALUE,
                    P_R, P_G, P_B,
                    P_H, P_S, P_L,
                    P_A,
                    P_RGB, P_RGBA,
                    P_HSL, P_HSLA,

                    P_MIN, P_MAX,

                    P_COUNT
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t                  vAtoms[P_COUNT];    // Atom bindings
                lsp::Color              sColor;             // Actual color
                float                   fMin;               // Minimum value
                float                   fMax;               // Maximum value
                size_t                  nFlags;             // Flags

            protected:
                virtual void            push() override;
                virtual void            commit(atom_t property) override;

                status_t                parse_range(ColorRange *range, io::IInSequence *s);

            public:
                explicit ColorRange(prop::Listener *listener = NULL);
                ColorRange(const ColorRange &) = delete;
                ColorRange(ColorRange &&) = delete;
                virtual ~ColorRange() override;

                ColorRange & operator = (const ColorRange &) = delete;
                ColorRange & operator = (ColorRange &&) = delete;

            public:
                inline float            min() const         { return fMin;                  }
                inline float            max() const         { return fMax;                  }
                inline float            range() const       { return fMax - fMin;           }
                inline float            abs_range() const   { return (fMax > fMin) ? fMax - fMin : fMin - fMax; }

                inline float            red() const         { return sColor.red();          }
                inline float            green() const       { return sColor.green();        }
                inline float            blue() const        { return sColor.blue();         }
                inline float            alpha() const       { return sColor.alpha();        }

                inline void             get_rgb(float &r, float &g, float &b) const             { sColor.get_rgb(r, g, b); }
                inline void             get_rgba(float &r, float &g, float &b, float &a) const  { sColor.get_rgba(r, g, b, a); }

                inline float            hue() const         { return sColor.hue();          }
                inline float            saturation() const  { return sColor.saturation();   }
                inline float            lightness() const   { return sColor.lightness();    }

                inline void             get_hsl(float &h, float &s, float &l) const             { sColor.get_hsl(h, s, l); }
                inline void             get_hsla(float &h, float &s, float &l, float &a) const  { sColor.get_hsla(h, s, l, a); }

                inline uint32_t         rgb24() const       { return sColor.rgb24();        }
                inline uint32_t         rgba32() const      { return sColor.rgba32();       }

                inline uint32_t         hsl24() const       { return sColor.hsl24();        }
                inline uint32_t         hsla32() const      { return sColor.hsla32();       }

            public:
                inline const lsp::Color *color() const       { return &sColor; }

                operator const lsp::Color *() const          { return &sColor; }

            public:
                float                   red(float r);
                float                   green(float g);
                float                   blue(float b);
                float                   alpha(float a);

                void                    set_rgb(float r, float g, float b);
                void                    set_rgba(float r, float g, float b, float a);
                void                    set_rgb24(uint32_t v);
                void                    set_rgba32(uint32_t v);
                void                    set_hsl24(uint32_t v);
                void                    set_hsla32(uint32_t v);

                float                   hue(float h);
                float                   saturation(float s);
                float                   lightness(float l);

                void                    set_hsl(float h, float s, float l);
                void                    set_hsla(float h, float s, float l, float a);
                void                    set_color(const char *text);
                void                    set_color(const LSPString *text);
                void                    set_color(const lsp::Color *c);
                void                    set_color(const lsp::Color &c);
                void                    set(const lsp::Color *c);
                void                    set(const lsp::Color &c);
                void                    set(const Color *src);
                void                    set(const ColorRange *src);

                float                   set_min(float v);
                float                   set_max(float v);
                void                    set_range(float min, float max);
                status_t                set(const char *s);
                status_t                set(const LSPString *s);

            public:
                static inline float     limit(float v, float min, float max)        { return Property::limit(v, min, max);      }
                inline float            limit(float v) const                        { return Property::limit(v, fMin, fMax);    }
                static inline bool      matches(float v, float min, float max)      { return Property::matches(v, min, max);    }
                inline bool             matches(float v) const                      { return Property::matches(v, fMin, fMax);  }

                void                    swap(ColorRange *src);

            public:
                static status_t         parse(ColorRange *range, expr::Tokenizer *t, Style *style);
        };

        namespace prop
        {
            /**
             * Color property implementation
             */
            class ColorRange: public tk::ColorRange
            {
                public:
                    explicit inline ColorRange(prop::Listener *listener = NULL): tk::ColorRange(listener) {};
                    ColorRange(const ColorRange &) = delete;
                    ColorRange(ColorRange &&) = delete;

                    ColorRange & operator = (const ColorRange &) = delete;
                    ColorRange & operator = (ColorRange &&) = delete;

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::ColorRange::bind(property, style, vAtoms, DESC, &sListener);   }
                    inline status_t     bind(const char *property, Style *style)        { return tk::ColorRange::bind(property, style, vAtoms, DESC, &sListener);   }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::ColorRange::bind(property, style, vAtoms, DESC, &sListener);   }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::ColorRange::unbind(vAtoms, DESC, &sListener);                  };

            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */




#endif /* LSP_PLUG_IN_TK_PROP_MULTI_COLORRANGE_H_ */
