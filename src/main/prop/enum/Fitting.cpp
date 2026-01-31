/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 31 янв. 2026 г.
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
        const prop::enum_t Fitting::ENUM[] =
        {
            { "none",           FIT_NONE            },
            { "width",          FIT_WIDTH           },
            { "height",         FIT_HEIGHT          },
            { "cover",          FIT_COVER           },
            { "contain",        FIT_CONTAIN         },
            { "hstretch",       FIT_HSTRETCH        },
            { "vstretch",       FIT_VSTRETCH        },
            { "stretch",        FIT_STRETCH         },
            { NULL,             -1                  }
        };

    } /* namespace tk */
} /* namespace lsp */


