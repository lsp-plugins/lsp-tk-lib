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
        Boolean::Listener::~Listener()
        {
            pValue      = NULL;
        }

        void Boolean::Listener::notify(atom_t property)
        {
            if (pValue == NULL)
                return;

            // Handle change: remember new value
            if ((pValue->pStyle != NULL) ||
                (pValue->aValue != NULL))
            {
                if (pStyle->get_bool(pValue->aValue, &pValue->bValue) != STATUS_OK)
                    return;
            }

            // Delegate event
            if (pListener != NULL)
                pListener->notify(property);
        }

        status_t Boolean::unbind()
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

        status_t Boolean::bind(Style *style, IStyleListener *listener, atom_t property)
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
            res = style->bind(property, PT_BOOL, &sListener);
            if (res == STATUS_OK)
            {
                pStyle      = style;
                pListener   = listener;
                aValue      = property;
            }
            style->end();

            return res;
        }

        Boolean::Boolean():
            sListener(this)
        {
            bValue      = 0.0f;
            pStyle      = NULL;
            pListener   = NULL;
            aValue      = -1;
        }

        Boolean::~Boolean()
        {
            unbind();
        }


        bool Boolean::set(bool v)
        {
            bool prev = (aValue >= 0) ? bValue : v;
            if (v == prev)
                return prev;

            bValue  = v;
            pStyle->set_bool(aValue, v);
            return prev;
        }

        namespace prop
        {
            Boolean::Boolean() :
                tk::Boolean()
            {
            }

            status_t Boolean::bind(Widget *widget, IStyleListener *listener, const char *property)
            {
                if ((widget == NULL) || (property == NULL))
                    return STATUS_BAD_ARGUMENTS;

                Display *dpy    = widget->display();
                atom_t id       = (dpy != NULL) ? dpy->atom_id(property) : -1;
                if (id < 0)
                    return STATUS_UNKNOWN_ERR;

                return tk::Boolean::bind(widget->style(), listener, id);
            }

            status_t Boolean::bind(Widget *widget, IStyleListener *listener, atom_t property)
            {
                return tk::Boolean::bind(widget->style(), listener, property);
            }

            status_t Boolean::bind(Display *dpy, Style *style, IStyleListener *listener, const char *property)
            {
                if ((dpy == NULL) || (style == NULL) || (property < 0))
                    return STATUS_BAD_ARGUMENTS;

                atom_t id       = dpy->atom_id(property);
                if (id < 0)
                    return STATUS_UNKNOWN_ERR;

                return tk::Boolean::bind(style, listener, id);
            }

            status_t Boolean::bind(Style *style, IStyleListener *listener, atom_t property)
            {
                return tk::Boolean::bind(style, listener, property);
            }
        }

    } /* namespace calc */
} /* namespace lsp */



