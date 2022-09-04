/*
 * Copyright (C) 2022 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2022 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 3 авг. 2022 г.
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

#ifndef LSP_PLUG_IN_TK_HELPERS_ARRANGE_H_
#define LSP_PLUG_IN_TK_HELPERS_ARRANGE_H_

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        /**
         * Arrange rectangle with specified size parameters according to the rules
         * defined by the tether array and list of possible areas where the rectangle
         * can be located
         *
         * @param dst pointer to store result rectangle
         * @param trg the rectangle area that has triggered the rectangle arrangement
         * @param ranges the list of possible area ranges where the rectangle can be located
         * @param num_ranges number of possible area ranges
         * @param rules ths list of tether rules
         * @param num_rules number of tether rules
         */
        status_t arrange_rectangle(
            ws::rectangle_t *dst,
            const ws::rectangle_t *trg,
            const ws::size_limit_t *src,
            const ws::rectangle_t *ranges,
            size_t num_ranges,
            const tether_t *rules,
            size_t num_rules);

    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_HELPERS_ARRANGE_H_ */
