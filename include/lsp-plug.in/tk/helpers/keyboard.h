/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 9 дек. 2024 г.
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

#ifndef LSP_PLUG_IN_TK_HELPERS_KEYBOARD_H_
#define LSP_PLUG_IN_TK_HELPERS_KEYBOARD_H_

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        /**
         * Translate key code to key modifier
         * @param code code to translate
         * @return key modifier or KM_NONE
         */
        LSP_TK_LIB_PUBLIC
        key_modifier_t          key_code_to_modifier(ws::code_t code);

    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_HELPERS_KEYBOARD_H_ */
