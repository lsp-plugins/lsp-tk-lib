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

#include <lsp-plug.in/tk/helpers/keyboard.h>

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
        key_modifier_t key_code_to_modifier(ws::code_t code)
        {
            switch (code)
            {
                case ws::WSK_CONTROL_L: return KM_LCTRL;
                case ws::WSK_CONTROL_R: return KM_RCTRL;
                case ws::WSK_ALT_L:     return KM_LALT;
                case ws::WSK_ALT_R:     return KM_RALT;
                case ws::WSK_SHIFT_L:   return KM_LSHIFT;
                case ws::WSK_SHIFT_R:   return KM_RSHIFT;
                case ws::WSK_META_L:    return KM_LMETA;
                case ws::WSK_META_R:    return KM_RMETA;
                case ws::WSK_SUPER_L:   return KM_LSUPER;
                case ws::WSK_SUPER_R:   return KM_RSUPER;
                case ws::WSK_HYPER_L:   return KM_LHYPER;
                case ws::WSK_HYPER_R:   return KM_RHYPER;
                default: break;
            }

            return KM_NONE;
        }

    } /* namespace tk */
} /* namespace lsp */


