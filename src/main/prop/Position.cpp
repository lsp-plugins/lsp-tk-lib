/*
 * Position.cpp
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
        const prop::desc_t Position::DESC[] =
        {
            { "",           PT_STRING   },
            { ".left",      PT_INT      },
            { ".top",       PT_INT      },
            { NULL,         PT_UNKNOWN  }
        };

        void Position::Listener::notify(atom_t property)
        {
            pValue->commit(property);
        }

        Position::Position(prop::Listener *listener):
            MultiProperty(listener),
            sListener(this)
        {
            nLeft              = 0;
            nTop             = 0;
        }

        Position::~Position()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void Position::commit(atom_t property)
        {
            if ((pStyle == NULL) || (property < 0))
                return;

            ssize_t v;
            if ((property == vAtoms[P_LEFT]) && (pStyle->get_int(vAtoms[P_LEFT], &v) == STATUS_OK))
                nLeft       = v;
            if ((property == vAtoms[P_TOP]) && (pStyle->get_int(vAtoms[P_TOP], &v) == STATUS_OK))
                nTop        = v;

            LSPString s;
            if ((property == vAtoms[P_VALUE]) && (pStyle->get_string(vAtoms[P_VALUE], &s) == STATUS_OK))
            {
                ssize_t xv[2];
                size_t n = Property::parse_ints(xv, 2, &s);
                if (n == 2)
                {
                    nLeft       = xv[0];
                    nTop        = xv[1];
                }
            }

            if (pListener != NULL)
                pListener->notify(this);
        }

        void Position::sync()
        {
            if (pStyle == NULL)
            {
                if (pListener != NULL)
                    pListener->notify(this);
                return;
            }

            pStyle->begin(&sListener);
            {
                // Simple components
                if (vAtoms[P_LEFT] >= 0)
                    pStyle->set_int(vAtoms[P_LEFT], nLeft);
                if (vAtoms[P_TOP] >= 0)
                    pStyle->set_int(vAtoms[P_TOP], nTop);

                // Compound objects
                LSPString s;
                if (vAtoms[P_VALUE] >= 0)
                {
                    if (s.fmt_ascii("%ld %ld", long(nLeft), long(nTop)))
                        pStyle->set_string(vAtoms[P_VALUE], &s);
                }
            }
            pStyle->end();
        }

        ssize_t Position::set_left(ssize_t value)
        {
            ssize_t old     = nLeft;
            if (old == value)
                return value;

            nLeft          = value;
            sync();
            return old;
        }

        ssize_t Position::set_top(ssize_t value)
        {
            ssize_t old     = nTop;
            if (old == value)
                return value;

            nTop            = value;
            sync();
            return old;
        }

        void Position::set(ssize_t left, ssize_t top)
        {
            if ((nLeft == left) &&
                (nTop == top))
                return;

            nLeft       = left;
            nTop        = top;
            sync();
        }

        void Position::set(const Position *p)
        {
            if ((nLeft == p->nLeft) &&
                (nTop == p->nTop))
                return;

            nLeft       = p->nLeft;
            nTop        = p->nTop;
            sync();
        }

        namespace prop
        {
            status_t Position::bind(const char *property, Widget *widget)
            {
                if (widget == NULL)
                    return STATUS_BAD_ARGUMENTS;

                return MultiProperty::bind
                    (
                        vAtoms, DESC, &sListener,
                        property, widget->style(), widget->display()
                    );
            }

            status_t Position::bind(atom_t property, Widget *widget)
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

            status_t Position::bind(const char *property, Style *style, Display *dpy)
            {
                return MultiProperty::bind
                    (
                        vAtoms, DESC, &sListener,
                        property, style, dpy
                    );
            }

            status_t Position::bind(atom_t property, Style *style, Display *dpy)
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


