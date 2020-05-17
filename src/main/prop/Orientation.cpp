/*
 * Orientation.cpp
 *
 *  Created on: 17 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const prop::enum_t Orientation::ENUM[] =
        {
            { "horizontal", O_HORIZONTAL },
            { "vertical",   O_VERTICAL },
            { NULL, -1 }
        };

    } /* namespace tk */
} /* namespace lsp */
