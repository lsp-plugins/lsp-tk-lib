/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 5 мая 2020 г.
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

#ifndef LSP_PLUG_IN_TK_VERSION_H_
#define LSP_PLUG_IN_TK_VERSION_H_

#define LSP_TK_LIB_MAJOR            1
#define LSP_TK_LIB_MINOR            0
#define LSP_TK_LIB_MICRO            5

#if defined(LSP_TK_LIB_PUBLISHER)
    #define LSP_TK_LIB_PUBLIC               LSP_EXPORT_MODIFIER
#elif defined(LSP_TK_LIB_BUILTIN) || defined(LSP_IDE_DEBUG)
    #define LSP_TK_LIB_PUBLIC
#else
    #define LSP_TK_LIB_PUBLIC               LSP_IMPORT_MODIFIER
#endif

#endif /* LSP_PLUG_IN_TK_VERSION_H_ */
