/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 17 мая 2020 г.
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
        const prop::enum_t Pointer::ENUM[] =
        {
            { "none",           ws::MP_NONE             },
            { "arrow",          ws::MP_ARROW            },
            { "hand",           ws::MP_HAND             },
            { "cross",          ws::MP_CROSS            },
            { "ibeam",          ws::MP_IBEAM            },
            { "draw",           ws::MP_DRAW             },
            { "plus",           ws::MP_PLUS             },
            { "size_nesw",      ws::MP_SIZE_NESW        },
            { "size_ns",        ws::MP_SIZE_NS          },
            { "size_we",        ws::MP_SIZE_WE          },
            { "size_nwse",      ws::MP_SIZE_NWSE        },
            { "up_arrow",       ws::MP_UP_ARROW         },
            { "hourglass",      ws::MP_HOURGLASS        },
            { "drag",           ws::MP_DRAG             },
            { "no_drop",        ws::MP_NO_DROP          },
            { "danger",         ws::MP_DANGER           },
            { "hsplit",         ws::MP_HSPLIT           },
            { "vsplit",         ws::MP_VSPLIT           },
            { "multidrag",      ws::MP_MULTIDRAG        },
            { "app_start",      ws::MP_APP_START        },
            { "help",           ws::MP_HELP             },
            { "size",           ws::MP_SIZE             },
            { "default",        ws::MP_DEFAULT          },
            { "text",           ws::MP_TEXT             },
            { "vsize",          ws::MP_VSIZE            },
            { "hsize",          ws::MP_HSIZE            },
            { "wait",           ws::MP_WAIT             },
            { "arrow_wait",     ws::MP_ARROW_WAIT       },
            { "hyperlink",      ws::MP_HYPERLINK        },
            { "hlink",          ws::MP_HYPERLINK        },
            { "pencil",         ws::MP_PENCIL           },
            { "table_cell",     ws::MP_TABLE_CELL       },
            { NULL,             -1 }
        };
        
    } /* namespace tk */
} /* namespace lsp */
