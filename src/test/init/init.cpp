/*
 * init.cpp
 *
 *  Created on: 13 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/test-fw/init.h>
#include <lsp-plug.in/tk/tk.h>

INIT_BEGIN(demo_initializer1)

    INIT_FUNC
    {
        tk::init(0, NULL);
    }

INIT_END


