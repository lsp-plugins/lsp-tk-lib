/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 3 сент. 2020 г.
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
        const prop::enum_t GraphFrameFunction::ENUM[] =
        {
            { "default",        GFF_DEFAULT         },
            { "rainbow",        GFF_RAINBOW         },
            { "fog_color",      GFF_FOG       },
            { "color",          GFF_COLOR           },
            { "lightness",      GFF_LIGHTNESS       },
            { "lightness2",     GFF_LIGHTNESS2      },
            { NULL,             -1                  }
        };

        namespace prop
        {
            status_t GraphFrameFunction::init(const char *name, Style *style, graph_frame_function_t value)
            {
                prop::GraphFrameFunction v;
                LSP_STATUS_ASSERT(v.bind(name, style));
                return v.init(value);
            }

            status_t GraphFrameFunction::override(const char *name, Style *style, graph_frame_function_t value)
            {
                prop::GraphFrameFunction v;
                LSP_STATUS_ASSERT(v.bind(name, style));
                return v.override(value);
            }
        }
    } /* namespace tk */
} /* namespace lsp */



