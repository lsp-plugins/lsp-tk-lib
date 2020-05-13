/*
 * BorderStyle.cpp
 *
 *  Created on: 8 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const prop::enum_t BorderStyle::ENUM[] =
        {
            { "dialog",         ws::BS_DIALOG       },
            { "single",         ws::BS_SINGLE       },
            { "none",           ws::BS_NONE         },
            { "popup",          ws::BS_POPUP        },
            { "combo",          ws::BS_COMBO        },
            { "sizeable",       ws::BS_SIZEABLE     },
            { NULL,             -1                  }
        };

        namespace prop
        {
            status_t BorderStyle::bind(const char *property, Widget *widget)
            {
                if ((widget == NULL) || (property == NULL))
                    return STATUS_BAD_ARGUMENTS;

                Display *dpy    = widget->display();
                atom_t id       = (dpy != NULL) ? dpy->atom_id(property) : -1;
                if (id < 0)
                    return STATUS_UNKNOWN_ERR;

                return tk::BorderStyle::bind(id, widget->style());
            }

            status_t BorderStyle::bind(atom_t property, Widget *widget)
            {
                return tk::BorderStyle::bind(property, widget->style());
            }

            status_t BorderStyle::bind(const char *property, Atoms *atoms, Style *style)
            {
                if ((atoms == NULL) || (style == NULL) || (property < 0))
                    return STATUS_BAD_ARGUMENTS;

                atom_t id       = atoms->atom_id(property);
                if (id < 0)
                    return STATUS_UNKNOWN_ERR;

                return tk::BorderStyle::bind(id, style);
            }

            status_t BorderStyle::bind(atom_t property, Style *style)
            {
                return tk::BorderStyle::bind(property, style);
            }
        }

    } /* namespace tk */
} /* namespace lsp */


