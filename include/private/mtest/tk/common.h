/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 27 мая 2020 г.
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

#ifndef PRIVATE_MTEST_TK_COMMON_H_
#define PRIVATE_MTEST_TK_COMMON_H_

#include <lsp-plug.in/common/types.h>

namespace lsp
{
    namespace test
    {
        extern const uint32_t colors[];

        /** Get next color from colors[] array
         *
         * @param idx index reference
         * @return color value (rgb24)
         */
        uint32_t next_color(size_t &idx);
    }
}


#endif /* PRIVATE_MTEST_TK_COMMON_H_ */
