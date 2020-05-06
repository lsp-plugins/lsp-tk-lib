/*
 * Float.cpp
 *
 *  Created on: 9 окт. 2019 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/prop.h>
#include <lsp-plug.in/tk/sys/Display.h>
#include <lsp-plug.in/tk/base.h>

namespace lsp
{
    namespace tk
    {
        Float::Listener::~Listener()
        {
            pValue      = NULL;
        }

        void Float::Listener::notify(atom_t property)
        {
            if (pValue == NULL)
                return;

            // Handle change: remember new value
            if ((pValue->pStyle != NULL) ||
                (pValue->aValue != NULL))
            {
                if (pStyle->get_float(pValue->aValue, &pValue->bValue) != STATUS_OK)
                    return;
            }

            // Delegate event
            if (pListener != NULL)
                pListener->notify(property);
        }

        status_t Float::unbind()
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

        status_t Float::bind(Style *style, IStyleListener *listener, atom_t property)
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
            res = style->bind(property, PT_FLOAT, &sListener);
            if (res == STATUS_OK)
            {
                pStyle      = style;
                pListener   = listener;
                aValue      = property;
            }
            style->end();

            return res;
        }

        Float::Float():
            sListener(this)
        {
            bValue      = 0.0f;
            pStyle      = NULL;
            pListener   = NULL;
            aValue      = -1;
        }

        Float::~Float()
        {
            unbind();
        }


        float Float::set(float v)
        {
            float prev = (aValue >= 0) ? bValue : v;
            if (v == prev)
                return prev;

            bValue  = v;
            pStyle->set_float(aValue, v);
            return prev;
        }

        namespace prop
        {
            Float::Float() :
                tk::Float()
            {
            }

            status_t Float::bind(Widget *widget, IStyleListener *listener, const char *property)
            {
                if ((widget == NULL) || (property == NULL))
                    return STATUS_BAD_ARGUMENTS;

                Display *dpy    = widget->display();
                atom_t id       = (dpy != NULL) ? dpy->atom_id(property) : -1;
                if (id < 0)
                    return STATUS_UNKNOWN_ERR;

                return tk::Float::bind(widget->style(), listener, id);
            }

            status_t Float::bind(Widget *widget, IStyleListener *listener, atom_t property)
            {
                return tk::Float::bind(widget->style(), listener, property);
            }

            status_t Float::bind(Display *dpy, Style *style, IStyleListener *listener, const char *property)
            {
                if ((dpy == NULL) || (style == NULL) || (property < 0))
                    return STATUS_BAD_ARGUMENTS;

                atom_t id       = dpy->atom_id(property);
                if (id < 0)
                    return STATUS_UNKNOWN_ERR;

                return tk::Float::bind(style, listener, id);
            }

            status_t Float::bind(Style *style, IStyleListener *listener, atom_t property)
            {
                return tk::Float::bind(style, listener, property);
            }
        }
    
    } /* namespace calc */
} /* namespace lsp */
