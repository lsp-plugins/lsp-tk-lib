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

    } /* namespace tk */
} /* namespace lsp */


