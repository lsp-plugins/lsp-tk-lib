/*
 * Scrolling.cpp
 *
 *  Created on: 17 июл. 2020 г.
 *      Author: sadko
 */


#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const prop::enum_t Orientation::ENUM[] =
        {
            { "none",       SCROLL_NONE         },
            { "clip",       SCROLL_CLIP         },
            { "optional",   SCROLL_OPTIONAL     },
            { "always",     SCROLL_ALWAYS       },
            { NULL, -1 }
        };

    } /* namespace tk */
} /* namespace lsp */


