/*
 * Copyright (C) 2021 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2021 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 14 мая 2021 г.
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
        const prop::enum_t TextAdjust::ENUM[] =
        {
            { "none",           TA_NONE             },
            { "tolower",        TA_TOLOWER          },
            { "toupper",        TA_TOUPPER          },
            { NULL,             -1                  }
        };


        status_t TextAdjust::apply(LSPString *dst, const LSPString *src) const
        {
            if ((dst == NULL) || (src == NULL))
                return STATUS_BAD_ARGUMENTS;

            LSPString tmp;
            if (!tmp.set(src))
                return STATUS_NO_MEM;
            status_t res = apply(&tmp);
            if (res != STATUS_OK)
                return res;

            tmp.swap(dst);
            return STATUS_OK;
        }

        status_t TextAdjust::apply(LSPString *dst) const
        {
            if (dst == NULL)
                return STATUS_BAD_ARGUMENTS;

            switch (nValue)
            {
                case TA_TOLOWER:
                    dst->tolower();
                    break;
                case TA_TOUPPER:
                    dst->toupper();
                    break;
                default:
                    break;
            }
            return STATUS_OK;
        }
    } /* namespace tk */
} /* namespace lsp */


