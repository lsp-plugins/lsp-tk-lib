/*
 * Boolean.cpp
 *
 *  Created on: 7 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/prop.h>
#include <lsp-plug.in/tk/sys/Display.h>
#include <lsp-plug.in/tk/base.h>

namespace lsp
{
    namespace tk
    {
        void Boolean::Listener::notify(atom_t property)
        {
            if (pValue->nAtom == property)
                pValue->sync();
        }

        Boolean::Boolean():
            sListener(this)
        {
            nAtom       = -1;
            bValue      = 0.0f;
        }

        Boolean::~Boolean()
        {
            unbind();
        }

        status_t Boolean::unbind()
        {
            if ((pStyle != NULL) && (nAtom >= 0))
            {
                status_t res = pStyle->unbind(nAtom, &sListener);
                if (res != STATUS_OK)
                    return res;
            }

            pStyle      = NULL;
            pListener   = NULL;
            nAtom       = -1;

            return STATUS_NOT_BOUND;
        }

        status_t Boolean::bind(prop::Listener *listener, atom_t property, Style *style)
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
            res = style->bind(property, PT_BOOL, &sListener);
            if (res == STATUS_OK)
            {
                pStyle      = style;
                pListener   = listener;
                nAtom       = property;
            }
            style->end();

            return res;
        }

        void Boolean::sync()
        {
            // Handle change: remember new value
            if (pStyle != NULL)
            {
                if (pStyle->get_bool(nAtom, &bValue) != STATUS_OK)
                    return;
            }

            // Delegate event
            if (pListener != NULL)
                pListener->notify(this);
        }

        bool Boolean::set(bool v)
        {
            bool prev = bValue;
            if (v == bValue)
                return prev;

            bValue  = v;
            if ((pStyle != NULL) && (nAtom >= 0))
                pStyle->set_bool(nAtom, v);
            else if (pListener != NULL)
                pListener->notify(this);
            return prev;
        }

        void Boolean::swap(Boolean *dst)
        {
            if (dst == this)
                return;

            bool tmp = dst->bValue;
            dst->set(bValue);
            set(tmp);
        }

        namespace prop
        {
            Boolean::Boolean() :
                tk::Boolean()
            {
            }

            status_t Boolean::bind(prop::Listener *listener, const char *property, Widget *widget)
            {
                if ((widget == NULL) || (property == NULL))
                    return STATUS_BAD_ARGUMENTS;

                Display *dpy    = widget->display();
                atom_t id       = (dpy != NULL) ? dpy->atom_id(property) : -1;
                if (id < 0)
                    return STATUS_UNKNOWN_ERR;

                return tk::Boolean::bind(listener, id, widget->style());
            }

            status_t Boolean::bind(prop::Listener *listener, atom_t property, Widget *widget)
            {
                return tk::Boolean::bind(listener, property, widget->style());
            }

            status_t Boolean::bind(prop::Listener *listener, const char *property, Display *dpy, Style *style)
            {
                if ((dpy == NULL) || (style == NULL) || (property < 0))
                    return STATUS_BAD_ARGUMENTS;

                atom_t id       = dpy->atom_id(property);
                if (id < 0)
                    return STATUS_UNKNOWN_ERR;

                return tk::Boolean::bind(listener, id, style);
            }

            status_t Boolean::bind(prop::Listener *listener, atom_t property, Style *style)
            {
                return tk::Boolean::bind(listener, property, style);
            }
        }

    } /* namespace calc */
} /* namespace lsp */



