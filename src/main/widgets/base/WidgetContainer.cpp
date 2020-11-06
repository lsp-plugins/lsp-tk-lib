/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 16 июн. 2017 г.
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
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(WidgetContainer, Widget)
                // Bind
                // Configure
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(WidgetContainer, "WidgetContainer");
        }

        const w_class_t WidgetContainer::metadata = { "WidgetContainer", &Widget::metadata };

        WidgetContainer::WidgetContainer(Display *dpy):
            Widget(dpy)
        {
            pClass          = &metadata;
        }

        WidgetContainer::~WidgetContainer()
        {
            nFlags     |= FINALIZED;
        }

        status_t WidgetContainer::add(Widget *child)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t WidgetContainer::remove(Widget *child)
        {
            return STATUS_NOT_FOUND;
        }

        status_t WidgetContainer::remove_all()
        {
            return STATUS_NOT_IMPLEMENTED;
        }
    } /* namespace tk */
} /* namespace lsp */
