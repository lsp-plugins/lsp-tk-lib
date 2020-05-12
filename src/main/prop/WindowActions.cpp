/*
 * WindowActions.cpp
 *
 *  Created on: 8 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const prop::enum_t WindowActions::ENUM[] =
        {
            { "move",           ws::WA_MOVE         },
            { "resize",         ws::WA_RESIZE       },
            { "minimize",       ws::WA_MINIMIZE     },
            { "maximize",       ws::WA_MAXIMIZE     },
            { "close",          ws::WA_CLOSE        },
            { "stick",          ws::WA_STICK        },
            { "shade",          ws::WA_SHADE        },
            { "fullscreen",     ws::WA_FULLSCREEN   },
            { "change_desk",    ws::WA_CHANGE_DESK  },

            { NULL,             -1                  }
        };

        namespace prop
        {
            status_t WindowActions::bind(const char *property, Widget *widget)
            {
                if ((widget == NULL) || (property == NULL))
                    return STATUS_BAD_ARGUMENTS;

                Display *dpy    = widget->display();
                atom_t id       = (dpy != NULL) ? dpy->atom_id(property) : -1;
                if (id < 0)
                    return STATUS_UNKNOWN_ERR;

                return tk::WindowActions::bind(id, widget->style());
            }

            status_t WindowActions::bind(atom_t property, Widget *widget)
            {
                return tk::WindowActions::bind(property, widget->style());
            }

            status_t WindowActions::bind(const char *property, Display *dpy, Style *style)
            {
                if ((dpy == NULL) || (style == NULL) || (property < 0))
                    return STATUS_BAD_ARGUMENTS;

                atom_t id       = dpy->atom_id(property);
                if (id < 0)
                    return STATUS_UNKNOWN_ERR;

                return tk::WindowActions::bind(id, style);
            }

            status_t WindowActions::bind(atom_t property, Style *style)
            {
                return tk::WindowActions::bind(property, style);
            }
        }

    } /* namespace tk */
} /* namespace lsp */


