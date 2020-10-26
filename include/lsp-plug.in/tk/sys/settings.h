/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 26 окт. 2020 г.
 *
 * lsp-tk-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-tk-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-tk-lib. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef LSP_PLUG_IN_TK_SYS_SETTINGS_H_
#define LSP_PLUG_IN_TK_SYS_SETTINGS_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/resource/ILoader.h>

namespace lsp
{
    namespace tk
    {
        struct display_settings_t
        {
            /**
             * Resource loader
             */
            resource::ILoader      *resources;

            /**
             * UTF-8 encoded path to JSON dictionary root
             * within the resource registry
             */
            const char             *dictionary;

            /**
             * Default constructor
             */
            explicit                display_settings_t();
            void                    construct();
        };
    }
}



#endif /* LSP_PLUG_IN_TK_SYS_SETTINGS_H_ */
