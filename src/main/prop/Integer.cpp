/*
 * Integer.cpp
 *
 *  Created on: 6 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/prop.h>
#include <lsp-plug.in/tk/sys/Display.h>
#include <lsp-plug.in/tk/base.h>

namespace lsp
{
    namespace tk
    {
        void Integer::Listener::notify(atom_t property)
        {
            if (property == pValue->nAtom)
                pValue->commit();
        }

        Integer::Integer(prop::Listener *listener):
            Property(listener),
            sListener(this)
        {
            nAtom       = -1;
            nValue      = 0.0f;
        }

        Integer::~Integer()
        {
            unbind();
        }

        status_t Integer::unbind()
        {
            if ((pStyle != NULL) && (nAtom >= 0))
            {
                status_t res = pStyle->unbind(nAtom, &sListener);
                if (res != STATUS_OK)
                    return res;
            }

            pStyle      = NULL;
            nAtom       = -1;

            return STATUS_NOT_BOUND;
        }

        status_t Integer::bind(atom_t property, Style *style)
        {
            if ((style == NULL) || (property < 0))
                return STATUS_BAD_ARGUMENTS;

            // Unbind first
            status_t res;
            if ((pStyle != NULL) && (nAtom >= 0))
            {
                res = pStyle->unbind(nAtom, &sListener);
                if (res != STATUS_OK)
                    return res;
            }

            // Bind to new handler
            style->begin();
            {
                res = style->bind(property, PT_INT, &sListener);
                if (res == STATUS_OK)
                {
                    pStyle      = style;
                    nAtom       = property;
                }
            }
            style->end();

            return res;
        }

        void Integer::commit()
        {
            // Handle change: remember new value
            if (pStyle != NULL)
            {
                if (pStyle->get_int(nAtom, &nValue) != STATUS_OK)
                    return;
            }

            // Delegate event
            if (pListener != NULL)
                pListener->notify(this);
        }

        ssize_t Integer::set(ssize_t v)
        {
            float prev = nValue;
            if (v == nValue)
                return prev;

            nValue  = v;
            if (pStyle != NULL)
            {
                pStyle->begin(&sListener);
                pStyle->set_int(nAtom, v);
                pStyle->end();
            }
            else if (pListener != NULL)
                pListener->notify(this);
            return prev;
        }

        void Integer::swap(Integer *dst)
        {
            if (dst == this)
                return;

            ssize_t tmp = dst->nValue;
            dst->set(nValue);
            set(tmp);
        }

        namespace prop
        {
            status_t Integer::bind(const char *property, Widget *widget)
            {
                if ((widget == NULL) || (property == NULL))
                    return STATUS_BAD_ARGUMENTS;

                Display *dpy    = widget->display();
                atom_t id       = (dpy != NULL) ? dpy->atom_id(property) : -1;
                if (id < 0)
                    return STATUS_UNKNOWN_ERR;

                return tk::Integer::bind(id, widget->style());
            }

            status_t Integer::bind(atom_t property, Widget *widget)
            {
                return tk::Integer::bind(property, widget->style());
            }

            status_t Integer::bind(const char *property, Display *dpy, Style *style)
            {
                if ((dpy == NULL) || (style == NULL) || (property < 0))
                    return STATUS_BAD_ARGUMENTS;

                atom_t id       = dpy->atom_id(property);
                if (id < 0)
                    return STATUS_UNKNOWN_ERR;

                return tk::Integer::bind(id, style);
            }

            status_t Integer::bind(atom_t property, Style *style)
            {
                return tk::Integer::bind(property, style);
            }
        }
    
    } /* namespace tk */
} /* namespace lsp */
