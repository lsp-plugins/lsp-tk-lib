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
        Integer::Listener::~Listener()
        {
            pValue      = NULL;
        }

        void Integer::Listener::notify(atom_t property)
        {
            if (pValue == NULL)
                return;

            // Handle change: remember new value
            if ((pValue->pStyle != NULL) ||
                (pValue->aValue != NULL))
            {
                if (pStyle->get_int(pValue->aValue, &pValue->nValue) != STATUS_OK)
                    return;
            }

            // Delegate event
            if (pListener != NULL)
                pListener->notify(property);
        }

        status_t Integer::unbind()
        {
            if ((pStyle != NULL) && (aValue >= 0))
            {
                status_t res = pStyle->unbind(aValue, &sListener);
                if (res != STATUS_OK)
                    return res;
            }

            pStyle      = NULL;
            pListener   = NULL;
            aValue      = -1;

            return STATUS_NOT_BOUND;
        }

        status_t Integer::bind(Style *style, IStyleListener *listener, atom_t property)
        {
            if ((style == NULL) || (property < 0))
                return STATUS_BAD_ARGUMENTS;

            // Unbind first
            status_t res;
            if ((pStyle != NULL) && (aValue >= 0))
            {
                res = pStyle->unbind(aValue, &sListener);
                if (res != STATUS_OK)
                    return res;
            }

            // Bind to new handler
            style->begin();
            res = style->bind(property, PT_INT, &sListener);
            if (res == STATUS_OK)
            {
                pStyle      = style;
                pListener   = listener;
                aValue      = property;
            }
            style->end();

            return res;
        }

        Integer::Integer():
            sListener(this)
        {
            nValue      = 0.0f;
            pStyle      = NULL;
            pListener   = NULL;
            aValue      = -1;
        }

        Integer::~Integer()
        {
            unbind();
        }


        ssize_t Integer::set(ssize_t v)
        {
            float prev = (aValue >= 0) ? nValue : v;
            if (v == prev)
                return prev;

            nValue  = v;
            pStyle->set_int(aValue, v);
            return prev;
        }

        namespace prop
        {
            Integer::Integer() :
                tk::Integer()
            {
            }

            status_t Integer::bind(Widget *widget, IStyleListener *listener, const char *property)
            {
                if ((widget == NULL) || (property == NULL))
                    return STATUS_BAD_ARGUMENTS;

                Display *dpy    = widget->display();
                atom_t id       = (dpy != NULL) ? dpy->atom_id(property) : -1;
                if (id < 0)
                    return STATUS_UNKNOWN_ERR;

                return tk::Integer::bind(widget->style(), listener, id);
            }

            status_t Integer::bind(Widget *widget, IStyleListener *listener, atom_t property)
            {
                return tk::Integer::bind(widget->style(), listener, property);
            }

            status_t Integer::bind(Display *dpy, Style *style, IStyleListener *listener, const char *property)
            {
                if ((dpy == NULL) || (style == NULL) || (property < 0))
                    return STATUS_BAD_ARGUMENTS;

                atom_t id       = dpy->atom_id(property);
                if (id < 0)
                    return STATUS_UNKNOWN_ERR;

                return tk::Integer::bind(style, listener, id);
            }

            status_t Integer::bind(Style *style, IStyleListener *listener, atom_t property)
            {
                return tk::Integer::bind(style, listener, property);
            }
        }
    
    } /* namespace tk */
} /* namespace lsp */
