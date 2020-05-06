/*
 * tk.cpp
 *
 *  Created on: 6 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/dsp/dsp.h>

namespace lsp
{
    namespace tk
    {
        void init(int argc, const char **argv)
        {
            // Initialize DSP
            dsp::init();
        }
    }
}
