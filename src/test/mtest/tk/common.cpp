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

#include <private/mtest/tk/common.h>

namespace lsp
{
    namespace test
    {
        const uint32_t colors[] =
        {
            0xff0000,
            0x00ff00,
            0x0000ff,
            0xffff00,
            0xff00ff,
            0x00ffff,

            0xcc0000,
            0x00cc00,
            0x0000cc,
            0xcccc00,
            0xcc00cc,
            0x00cccc,

            0x880000,
            0x008800,
            0x000088,
            0x888800,
            0x880088,
            0x008888,

            0x88cccc,
            0xcc88cc,
            0xcccc88,
            0x8888cc,
            0x88cc88,
            0xcc8888,

            0x88ffff,
            0xff88ff,
            0xffff88,
            0x8888ff,
            0x88ff88,
            0xff8888,

            0xffcccc,
            0xccffcc,
            0xccccff,
            0xffffcc,
            0xffccff,
            0xccffff,
        };

        uint32_t next_color(size_t &idx)
        {
            uint32_t res    = colors[idx];
            idx = (idx + 1) % (sizeof(colors) / sizeof(uint32_t));
            return res;
        }
    }
}


