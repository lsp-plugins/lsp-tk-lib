/*
 * LSPBasis.cpp
 *
 *  Created on: 19 июл. 2017 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk-old/widgets/graph/LSPBasis.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPBasis::metadata = { "LSPBasis", &LSPGraphItem::metadata };

        LSPBasis::LSPBasis(LSPDisplay *dpy): LSPGraphItem(dpy)
        {
            nID = -1;
            pClass          = &metadata;
        }

        LSPBasis::~LSPBasis()
        {
        }
    } /* namespace tk */
} /* namespace lsp */
