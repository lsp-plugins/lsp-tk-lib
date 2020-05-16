/*
 * WindowPolicy.cpp
 *
 *  Created on: 16 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const prop::enum_t WindowPolicy::ENUM[] =
        {
            { "normal",         WP_NORMAL           },
            { "greedy",         WP_GREEDY           },
            { NULL,             -1                  }
        };
    
    } /* namespace tk */
} /* namespace lsp */
