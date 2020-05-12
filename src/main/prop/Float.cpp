/*
 * Float.cpp
 *
 *  Created on: 9 окт. 2019 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        void Float::Listener::notify(atom_t property)
        {
            if (pValue->nAtom == property)
                pValue->commit();
        }

        Float::Float(prop::Listener *listener):
            Property(listener),
            sListener(this)
        {
            nAtom       = -1;
            fValue      = 0.0f;
        }

        Float::~Float()
        {
            unbind();
        }

        status_t Float::unbind()
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

        status_t Float::bind(atom_t property, Style *style)
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
                res = style->bind(property, PT_FLOAT, &sListener);
                if (res == STATUS_OK)
                {
                    pStyle      = style;
                    nAtom       = property;
                }
            }
            style->end();

            return res;
        }

        void Float::commit()
        {
            // Handle change: remember new value
            if (pStyle != NULL)
            {
                if (pStyle->get_float(nAtom, &fValue) != STATUS_OK)
                    return;
            }

            // Delegate event
            if (pListener != NULL)
                pListener->notify(this);
        }

        float Float::set(float v)
        {
            float prev = fValue;
            if (v == fValue)
                return prev;

            fValue  = v;
            if (pStyle != NULL)
            {
                pStyle->begin(&sListener);
                pStyle->set_float(nAtom, v);
                pStyle->end();
            }
            else if (pListener != NULL)
                pListener->notify(this);
            return prev;
        }

        void Float::swap(Float *dst)
        {
            if (dst == this)
                return;

            float tmp = dst->fValue;
            dst->set(fValue);
            set(tmp);
        }

        namespace prop
        {
            status_t Float::bind(const char *property, Widget *widget)
            {
                if ((widget == NULL) || (property == NULL))
                    return STATUS_BAD_ARGUMENTS;

                Display *dpy    = widget->display();
                atom_t id       = (dpy != NULL) ? dpy->atom_id(property) : -1;
                if (id < 0)
                    return STATUS_UNKNOWN_ERR;

                return tk::Float::bind(id, widget->style());
            }

            status_t Float::bind(atom_t property, Widget *widget)
            {
                return tk::Float::bind(property, widget->style());
            }

            status_t Float::bind(const char *property, Display *dpy, Style *style)
            {
                if ((dpy == NULL) || (style == NULL) || (property < 0))
                    return STATUS_BAD_ARGUMENTS;

                atom_t id       = dpy->atom_id(property);
                if (id < 0)
                    return STATUS_UNKNOWN_ERR;

                return tk::Float::bind(id, style);
            }

            status_t Float::bind(atom_t property, Style *style)
            {
                return tk::Float::bind(property, style);
            }
        }
    
    } /* namespace calc */
} /* namespace lsp */
