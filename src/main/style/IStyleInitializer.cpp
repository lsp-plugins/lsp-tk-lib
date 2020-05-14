/*
 * IStyleInitializer.cpp
 *
 *  Created on: 14 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        
        IStyleInitializer::IStyleInitializer()
        {
        }
        
        IStyleInitializer::~IStyleInitializer()
        {
        }

        status_t IStyleInitializer::init(Style *style, Schema *schema)
        {
            return STATUS_OK;
        }
    
    } /* namespace tk */
} /* namespace lsp */
