/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 26 июн. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_GROUP_H_
#define LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_GROUP_H_

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
            LSP_TK_STYLE_DEF_BEGIN(Group, Align)
                prop::Font              sFont;
                prop::Color             sColor;
                prop::Color             sTextColor;
                prop::String            sText;
                prop::Boolean           sShowText;
                prop::Integer           sBorder;
                prop::Integer           sTextBorder;
                prop::Integer           sRadius;
                prop::Integer           sTextRadius;
                prop::Embedding         sEmbedding;
                prop::Padding           sIPadding;
                prop::Alignment         sHeading;
            LSP_TK_STYLE_DEF_END
        }

        /**
         * Widget group, implements a single widget container that aligns the child widget
         * according to the layout settings. The container ignores allocation() property
         * of the child widget.
         */
        class Group: public Align
        {
            public:
                static const w_class_t    metadata;

            protected:
                typedef struct alloc_t
                {
                    ws::rectangle_t         text;
                    ws::rectangle_t         rtext;
                    padding_t               pad;
                    padding_t               xpad;
                } alloc_t;

            protected:
                ws::rectangle_t         sLabel;
                ws::rectangle_t         sArea;

                prop::Font              sFont;
                prop::Color             sColor;
                prop::Color             sTextColor;
                prop::String            sText;
                prop::Boolean           sShowText;
                prop::Integer           sBorder;
                prop::Integer           sTextBorder;
                prop::Integer           sRadius;
                prop::Integer           sTextRadius;
                prop::Embedding         sEmbedding;
                prop::Padding           sIPadding;
                prop::Alignment         sHeading;

            protected:
                void                    allocate(alloc_t *alloc);

            protected:
                virtual void            property_changed(Property *prop);
                virtual void            size_request(ws::size_limit_t *r);
                virtual void            realize(const ws::rectangle_t *r);

            public:
                explicit Group(Display *dpy);
                virtual ~Group();

                virtual status_t        init();

            public:
                LSP_TK_PROPERTY(Font,       font,           &sFont)
                LSP_TK_PROPERTY(Color,      color,          &sColor)
                LSP_TK_PROPERTY(Color,      text_color,     &sTextColor)
                LSP_TK_PROPERTY(String,     text,           &sText)
                LSP_TK_PROPERTY(Boolean,    show_text,      &sShowText)
                LSP_TK_PROPERTY(Integer,    border_size,    &sBorder)
                LSP_TK_PROPERTY(Integer,    text_border,    &sTextBorder)
                LSP_TK_PROPERTY(Integer,    border_radius,  &sRadius)
                LSP_TK_PROPERTY(Integer,    text_radius,    &sTextRadius)
                LSP_TK_PROPERTY(Embedding,  embedding,      &sEmbedding)
                LSP_TK_PROPERTY(Padding,    ipadding,       &sIPadding)
                LSP_TK_PROPERTY(Alignment,  heading,        &sHeading)

            public:
                virtual void            render(ws::ISurface *s, const ws::rectangle_t *area, bool force);
        };
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_GROUP_H_ */
