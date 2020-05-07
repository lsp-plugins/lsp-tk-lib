/*
 * LSPColor.h
 *
 *  Created on: 5 нояб. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_SYS_LSPCOLOR_H_
#define UI_TK_SYS_LSPCOLOR_H_

#include <lsp-plug.in/tk/version.h>
#include <lsp-plug.in/tk/types.h>

#include <lsp-plug.in/tk/style.h>
#include <lsp-plug.in/tk/prop.h>
#include <lsp-plug.in/runtime/Color.h>

namespace lsp
{
    namespace tk
    {
        class LSPDisplay;
        class LSPWidget;
        class LSPStyle;

        /**
         * Color property interface
         */
        class Color: public Property
        {
            protected:
                Color & operator = (const Color &);

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

                class Listener: public IStyleListener
                {
                    private:
                        Color   *pValue;

                    public:
                        inline Listener(Color *color)   { pValue = color; };

                    public:
                        virtual void    notify(atom_t property);
                };

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                lsp::Color          sColor;             // Color holder
                Listener            sListener;          // Listener

            protected:
                status_t            unbind();
                status_t            bind(prop::Listener *listener, const char *property, Style *style, Display *dpy);
                void                sync();
                void                commit(atom_t property);

            public:
                explicit Color();
                ~Color();

            public:
                void            set_default();

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

            public:
                void red(float r);
                void green(float g);
                void blue(float b);
                void alpha(float a);

                void set_rgb(float r, float g, float b);
                void set_rgba(float r, float g, float b, float a);

                void hue(float h);
                void saturation(float s);
                void lightness(float l);

                void set_hsl(float h, float s, float l);
                void set_hsla(float h, float s, float l, float a);

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

                inline uint32_t rgb24() const           { return sColor.rgb24(); }

            public:
                inline const lsp::Color *color() const       { return &sColor; }

                operator const lsp::Color *() const          { return &sColor; }
        };

        namespace prop
        {
            /**
             * Color property implementation
             */
            class Color: public tk::Color
            {
                private:
                    Color & operator = (const Color *);

                public:
                    explicit Color();
                    ~Color();

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    status_t            bind(prop::Listener *listener, const char *property, Widget *widget);
                    status_t            bind(prop::Listener *listener, atom_t property, Widget *widget);
                    status_t            bind(prop::Listener *listener, const char *property, Style *style, Display *dpy);
                    status_t            bind(prop::Listener *listener, atom_t property, Style *style, Display *dpy);

                    /**
                     * Unbind property
                     */
                    status_t            unbind();
            };
        }

    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_SYS_LSPCOLOR_H_ */
