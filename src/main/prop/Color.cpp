/*
 * Color.cpp
 *
 *  Created on: 5 нояб. 2017 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/prop.h>
#include <lsp-plug.in/tk/sys/Display.h>
#include <lsp-plug.in/tk/base.h>

namespace lsp
{
    namespace tk
    {
        typedef struct color_binding_t
        {
            const char         *postfix;
            property_type_t     type;
        } color_binding_t;

        static const color_binding_t color_bindings[] =
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
            { ".hsla",      PT_STRING   }
        };

        void Color::Listener::notify(atom_t property)
        {
            pValue->commit(property);
        }

        Color::Color():
            sListener(this)
        {
            for (size_t i=0; i<P_COUNT; ++i)
                vAtoms[i]   = -1;
        }

        status_t Color::unbind()
        {
            if (pStyle == NULL)
                return STATUS_NOT_BOUND;

            // Unbind all atoms
            for (size_t i=0; i<P_COUNT; ++i)
            {
                if (vAtoms[i] < 0)
                    continue;
                pStyle->unbind(vAtoms[i], &sListener);
                vAtoms[i]   = -1;
            }

            pStyle  = NULL;
            return STATUS_OK;
        }
        
        status_t Color::bind(prop::Listener *listener, const char *property, Style *style, Display *dpy)
        {
            if ((style == NULL) || (dpy == NULL) || (property == NULL))
                return STATUS_BAD_ARGUMENTS;

            if (pStyle == style)
                return STATUS_OK;

            // Unbind from previously used style
            unbind();

            LSPString key;
            if (!key.set_utf8(property))
                return STATUS_NO_MEM;
            size_t len = key.length();

            // Bind all ports
            style->begin();

            status_t res = STATUS_OK;
            for (size_t i=0; i<P_COUNT; ++i)
            {
                key.set_length(len);
                if (!key.append_ascii(color_bindings[i].postfix))
                {
                    res = STATUS_NO_MEM;
                    break;
                }
                atom_t atom = dpy->atom_id(key.get_utf8());
                if (atom < 0)
                {
                    res = STATUS_NO_MEM;
                    break;
                }
                res = style->bind(atom, color_bindings[i].type, &sListener);
                if (res != STATUS_OK)
                    break;
                vAtoms[i]   = atom;
            }

            if (res == STATUS_OK)
            {
                pStyle      = style;
                pListener   = listener;
            }
            else
                unbind();

            style->end();

            return res;
        }

        void Color::set_default()
        {
            if (pStyle == NULL)
                return;

            pStyle->begin();

            for (size_t i=0; i<P_COUNT; ++i)
            {
                if (vAtoms[i] >= 0)
                    pStyle->set_default(vAtoms[i]);
            }

            pStyle->end();
        }

        void Color::sync()
        {
            if (pStyle == NULL)
            {
                if (pListener != NULL)
                    pListener->notify(this);
                return;
            }

            lsp::Color &c = sColor;

            char buf[32];
            pStyle->begin();

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

            pStyle->end();
        }

        void Color::commit(atom_t property)
        {
            if ((pStyle == NULL) || (property < 0))
                return;

            lsp::Color &c = sColor;

            float v;
            pStyle->begin();

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
                c.parse4(s);

            pStyle->end();

            if (pListener != NULL)
                pListener->notify(this);
        }

        void Color::red(float r)
        {
            if (sColor.red() == r)
                return;
            sColor.red(r);
            sync();
        };

        void Color::green(float g)
        {
            if (sColor.green() == g)
                return;
            sColor.green(g);
            sync();
        };

        void Color::blue(float b)
        {
            if (sColor.blue() == b)
                return;
            sColor.blue(b);
            sync();
        };

        void Color::hue(float h)
        {
            if (sColor.hue() == h)
                return;
            sColor.hue(h);
            sync();
        };

        void Color::saturation(float s)
        {
            if (sColor.saturation() == s)
                return;
            sColor.saturation(s);
            sync();
        };

        void Color::lightness(float l)
        {
            if (sColor.lightness() == l)
                return;
            sColor.lightness(l);
            sync();
        };

        void Color::alpha(float a)
        {
            if (sColor.alpha() == a)
                return;
            sColor.alpha(a);
            sync();
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


        namespace prop
        {
            Color::Color():
                tk::Color()
            {
            }

            Color::~Color()
            {
            }

            status_t Color::bind(prop::Listener *listener, const char *property, Widget *widget)
            {
                if (widget == NULL)
                    return STATUS_BAD_ARGUMENTS;
                return tk::Color::bind(listener, property, widget->style(), widget->display());
            }

            status_t Color::bind(prop::Listener *listener, atom_t property, Widget *widget)
            {
                if (widget == NULL)
                    return STATUS_BAD_ARGUMENTS;
                Display *dpy = widget->display();
                if (dpy == NULL)
                    return STATUS_BAD_ARGUMENTS;
                return tk::Color::bind(listener, dpy->atom_name(property), widget->style(), widget->display());
            }

            status_t Color::bind(prop::Listener *listener, const char *property, Style *style, Display *dpy)
            {
                return tk::Color::bind(listener, property, style, dpy);
            }

            status_t Color::bind(prop::Listener *listener, atom_t property, Style *style, Display *dpy)
            {
                if (dpy == NULL)
                    return STATUS_BAD_ARGUMENTS;
                return tk::Color::bind(listener, dpy->atom_name(property), style, dpy);
            }

            status_t Color::unbind()
            {
                return tk::Color::unbind();
            }
        }


    } /* namespace tk */
} /* namespace lsp */
