/*
 * tk.h
 *
 *  Created on: 6 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_TK_H_
#define LSP_PLUG_IN_TK_TK_H_

#include <lsp-plug.in/tk/version.h>
#include <lsp-plug.in/tk/types.h>


namespace lsp
{
    namespace tk
    {
        /**
         * Initialize toolkit
         * @param argc number of arguments
         * @param argv list of arguments
         */
        void init(int argc, const char **argv);
    }
}

#endif /* LSP_PLUG_IN_TK_TK_H_ */
