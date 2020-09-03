/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 30 июл. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LISTBOXITEM_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LISTBOXITEM_H_

namespace lsp
{
    namespace tk
    {
        
        class ListBoxItem: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                prop::String                sText;
                prop::Color                 sBgSelectedColor;
                prop::Color                 sTextColor;
                prop::Color                 sTextSelectedColor;

            protected:
                virtual void                property_changed(Property *prop);

            public:
                explicit ListBoxItem(Display *dpy);
                virtual ~ListBoxItem();

            public:
                LSP_TK_PROPERTY(String,     text,                       &sText)
                LSP_TK_PROPERTY(Color,      bg_selected_color,          &sBgSelectedColor)
                LSP_TK_PROPERTY(Color,      text_color,                 &sTextColor)
                LSP_TK_PROPERTY(Color,      text_selected_color,        &sTextSelectedColor)

            public:
                virtual status_t            init();
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LISTBOXITEM_H_ */
