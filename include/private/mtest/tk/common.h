/*
 * common.h
 *
 *  Created on: 27 мая 2020 г.
 *      Author: sadko
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
