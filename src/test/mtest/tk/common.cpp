/*
 * common.cpp
 *
 *  Created on: 27 мая 2020 г.
 *      Author: sadko
 */

#include <private/mtest/tk/common.h>

namespace lsp
{
    namespace test
    {
        static const uint32_t colors[] =
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


