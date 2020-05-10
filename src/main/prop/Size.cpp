/*
 * Size.cpp
 *
 *  Created on: 10 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/prop.h>
#include <lsp-plug.in/tk/sys/Display.h>
#include <lsp-plug.in/tk/base.h>

namespace lsp
{
    namespace tk
    {
        const prop::desc_t Size::DESC[] =
        {
            { "",           PT_STRING   },
            { ".width",     PT_INT      },
            { ".height",    PT_INT      },
            { NULL,         PT_UNKNOWN  }
        };

        void Size::Listener::notify(atom_t property)
        {
            pValue->commit(property);
        }

        Size::Size(prop::Listener *listener):
            MultiProperty(listener),
            sListener(this)
        {
            nWidth              = 0;
            nHeight             = 0;
        }

        Size::~Size()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void Size::commit(atom_t property)
        {
            if ((pStyle == NULL) || (property < 0))
                return;

            pStyle->begin();
            {
                ssize_t v;
                if ((property == vAtoms[P_WIDTH]) && (pStyle->get_int(vAtoms[P_WIDTH], &v) == STATUS_OK))
                    nWidth      = lsp_min(v, 0);
                if ((property == vAtoms[P_HEIGHT]) && (pStyle->get_int(vAtoms[P_HEIGHT], &v) == STATUS_OK))
                    nHeight     = lsp_min(v, 0);

                LSPString s;
                if ((property == vAtoms[P_VALUE]) && (pStyle->get_string(vAtoms[P_VALUE], &s) == STATUS_OK))
                {
                    ssize_t xv[2];
                    size_t n = Property::parse_ints(xv, 2, &s);
                    if (n == 2)
                    {
                        nWidth      = xv[0];
                        nHeight     = xv[1];
                    }
                }
            }
            pStyle->end();

            if (pListener != NULL)
                pListener->notify(this);
        }

        void Size::sync()
        {
            if (pStyle == NULL)
            {
                if (pListener != NULL)
                    pListener->notify(this);
                return;
            }

            pStyle->begin();
            {
                // Simple components
                if (vAtoms[P_WIDTH] >= 0)
                    pStyle->set_int(vAtoms[P_WIDTH], nWidth);
                if (vAtoms[P_HEIGHT] >= 0)
                    pStyle->set_int(vAtoms[P_HEIGHT], nHeight);

                // Compound objects
                LSPString s;
                if (vAtoms[P_VALUE] >= 0)
                {
                    if (s.fmt_ascii("%ld %ld", long(nWidth), long(nHeight)))
                        pStyle->set_string(vAtoms[P_VALUE], &s);
                }
            }
            pStyle->end();
        }

        size_t Size::set_width(size_t value)
        {
            size_t old      = nWidth;
            if (old == value)
                return value;

            nWidth          = value;
            sync();
            return old;
        }

        size_t Size::set_height(size_t value)
        {
            size_t old      = nHeight;
            if (old == value)
                return value;

            nHeight         = value;
            sync();
            return old;
        }

        void Size::set(size_t width, size_t height)
        {
            if ((nWidth == width) &&
                (nHeight == height))
                return;

            nWidth      = width;
            nHeight     = height;
            sync();
        }

        void Size::set(const Size *p)
        {
            if ((nWidth == p->nWidth) &&
                (nHeight == p->nHeight))
                return;

            nWidth      = p->nWidth;
            nHeight     = p->nHeight;
            sync();
        }

        namespace prop
        {
            status_t Size::bind(const char *property, Widget *widget)
            {
                if (widget == NULL)
                    return STATUS_BAD_ARGUMENTS;

                return MultiProperty::bind
                    (
                        vAtoms, DESC, &sListener,
                        property, widget->style(), widget->display()
                    );
            }

            status_t Size::bind(atom_t property, Widget *widget)
            {
                if (widget == NULL)
                    return STATUS_BAD_ARGUMENTS;
                Display *dpy = widget->display();
                if (dpy == NULL)
                    return STATUS_BAD_ARGUMENTS;
                return MultiProperty::bind
                    (
                        vAtoms, DESC, &sListener,
                        dpy->atom_name(property), widget->style(), widget->display()
                    );
            }

            status_t Size::bind(const char *property, Style *style, Display *dpy)
            {
                return MultiProperty::bind
                    (
                        vAtoms, DESC, &sListener,
                        property, style, dpy
                    );
            }

            status_t Size::bind(atom_t property, Style *style, Display *dpy)
            {
                if (dpy == NULL)
                    return STATUS_BAD_ARGUMENTS;
                return MultiProperty::bind
                    (
                        vAtoms, DESC, &sListener,
                        dpy->atom_name(property), style, dpy
                    );
            }
        }
    } /* namespace tk */
} /* namespace lsp */



