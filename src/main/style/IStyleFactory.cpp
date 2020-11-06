/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 4 нояб. 2020 г.
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

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        IStyleFactory::IStyleFactory(const char *name)
        {
            sName       = name;
        }

        IStyleFactory::~IStyleFactory()
        {
        }

        Style *IStyleFactory::create(Schema *schema)
        {
            return NULL;
        }

        Style *IStyleFactory::init(Style *s)
        {
            // Return immediately if style is NULL
            if (s == NULL)
                return NULL;

            // Initialize style
            s->set_init_mode(true);     // Set initialization mode
            s->set_sync_mode(true);
            status_t res = s->init();
            if (res != STATUS_OK)
            {
                delete s;
                return NULL;
            }
            s->set_sync_mode(false);
            s->set_init_mode(false);    // Unset initialization mode

            // Return initialized style
            return s;
        }
    }
}

