/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef LSP_PLUG_IN_TK_BASE_WIDGETCONTAINER_H_
#define LSP_PLUG_IN_TK_BASE_WIDGETCONTAINER_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        // Style definition
        namespace style
        {
            LSP_TK_STYLE_DEF_BEGIN(WidgetContainer, Widget)
            LSP_TK_STYLE_DEF_END
        }

        /** This is a basic widget class for any widget that contains sub-widgets
         *
         */
        class WidgetContainer: public Widget
        {
            public:
                static const w_class_t    metadata;

            //---------------------------------------------------------------------------------
            // Construction and destruction
            public:
                explicit WidgetContainer(Display *dpy);
                WidgetContainer(const WidgetContainer &) = delete;
                WidgetContainer(WidgetContainer &&) = delete;
                virtual ~WidgetContainer() override;

                WidgetContainer & operator = (const WidgetContainer &) = delete;
                WidgetContainer & operator = (WidgetContainer &&) = delete;

            //---------------------------------------------------------------------------------
            // Manipulation
            public:
                /**
                 * Get the actual background color of the child widget
                 * @param color actual background color of the child widget
                 */
                virtual void        get_child_bg_color(lsp::Color *color) const;
                virtual void        get_child_bg_color(lsp::Color &color) const;

            public:
                /** Add child to widget container
                 *
                 * @param child child widget to add
                 * @return status of operation
                 */
                virtual status_t    add(Widget *child);

                /** Remove child from widget container
                 *
                 * @param child child widget to remove
                 * @return status of operation
                 */
                virtual status_t    remove(Widget *child);

                /** Remove all widgets
                 *
                 * @return status of operation
                 */
                virtual status_t    remove_all();
        };

    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_BASE_WIDGETCONTAINER_H_ */
