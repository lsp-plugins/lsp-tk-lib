/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 8 мая 2020 г.
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
        const prop::enum_t BorderStyle::ENUM[] =
        {
            { "dialog",         ws::BS_DIALOG       },
            { "single",         ws::BS_SINGLE       },
            { "none",           ws::BS_NONE         },
            { "popup",          ws::BS_POPUP        },
            { "combo",          ws::BS_COMBO        },
            { "sizeable",       ws::BS_SIZEABLE     },
            { NULL,             -1                  }
        };

        namespace prop
        {
            status_t BorderStyle::init(const char *name, Style *style, ws::border_style_t value)
            {
                prop::BorderStyle v;
                LSP_STATUS_ASSERT(v.bind(name, style));
                return v.init(value);
            }

            status_t BorderStyle::override(const char *name, Style *style, ws::border_style_t value)
            {
                prop::BorderStyle v;
                LSP_STATUS_ASSERT(v.bind(name, style));
                return v.override(value);
            }
        }
    } /* namespace tk */
} /* namespace lsp */


