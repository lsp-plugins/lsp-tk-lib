/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 6 мая 2020 г.
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

#ifndef LSP_PLUG_IN_TK_BASE_CAST_H_
#define LSP_PLUG_IN_TK_BASE_CAST_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        template <class Target>
            inline Target *widget_cast(Widget *src)
            {
                return ((src != NULL) && (src->instance_of(&Target::metadata))) ? static_cast<Target *>(src) : NULL;
            }

        template <class Target>
            inline const Target *widget_cast(const Widget *src)
            {
                return ((src != NULL) && (src->instance_of(&Target::metadata))) ? static_cast<const Target *>(src) : NULL;
            }

        template <class Target>
            inline Target *widget_ptrcast(void *src)
            {
                Widget *w = (src != NULL) ? static_cast<Widget *>(src) : NULL;
                return ((w != NULL) && (w->instance_of(&Target::metadata))) ? static_cast<Target *>(w) : NULL;
            }

        template <class Target>
            inline const Target *widget_ptrcast(const void *src)
            {
                const Widget *w = (src != NULL) ? static_cast<const Widget *>(src) : NULL;
                return ((w != NULL) && (w->instance_of(&Target::metadata))) ? static_cast<const Target *>(w) : NULL;
            }

        inline bool instance_of(Widget *w, const w_class_t *wc)
        {
            return (w != NULL) && (w->instance_of(wc));
        }

        template <class Target>
            inline bool instance_of(Widget *w)
            {
                return (w != NULL) && (w->instance_of(&Target::metadata));
            }
    }
}


#endif /* LSP_PLUG_IN_TK_BASE_CAST_H_ */
