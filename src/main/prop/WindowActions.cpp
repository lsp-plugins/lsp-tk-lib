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
    } /* namespace tk */
} /* namespace lsp */


