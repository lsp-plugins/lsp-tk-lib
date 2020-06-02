/*
 * ButtonMode.cpp
 *
 *  Created on: 2 июн. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const prop::enum_t ButtonMode::ENUM[] =
        {
            { "normal",         BM_NORMAL           },
            { "toggle",         BM_TOGGLE           },
            { "trigger",        BM_TRIGGER          },
            { NULL,             -1                  }
        };

    } /* namespace tk */
} /* namespace lsp */



