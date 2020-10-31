/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 23 окт. 2020 г.
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
        const prop::enum_t FileDialogMode::ENUM[] =
        {
            { "open_file",      FDM_OPEN_FILE       },
            { "save_file",      FDM_SAVE_FILE       },
            { NULL,             -1                  }
        };

        namespace prop
        {
            status_t FileDialogMode::init(const char *name, Style *style, file_dialog_mode_t value)
            {
                prop::FileDialogMode v;
                LSP_STATUS_ASSERT(v.bind(name, style));
                return v.init(value);
            }

            status_t FileDialogMode::override(const char *name, Style *style, file_dialog_mode_t value)
            {
                prop::FileDialogMode v;
                LSP_STATUS_ASSERT(v.bind(name, style));
                return v.override(value);
            }
        }
    } /* namespace tk */
} /* namespace lsp */


