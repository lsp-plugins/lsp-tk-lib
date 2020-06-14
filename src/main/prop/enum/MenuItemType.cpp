/*
 * MenuItemType.cpp
 *
 *  Created on: 14 июн. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const prop::enum_t MenuItemType::ENUM[] =
        {
            { "normal",     MI_NORMAL       },
            { "check",      MI_CHECK        },
            { "checkbox",   MI_CHECK        },
            { "radio",      MI_RADIO        },
            { "separator",  MI_SEPARATOR    },
            { "split",      MI_SEPARATOR    },
            { NULL, -1 }
        };

    } /* namespace tk */
} /* namespace lsp */



