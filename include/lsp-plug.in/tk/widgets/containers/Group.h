/*
 * Group.h
 *
 *  Created on: 26 июн. 2020 г.
 *      Author: sadko
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

            public:
                virtual void            render(ws::ISurface *s, const ws::rectangle_t *area, bool force);
        };

    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_GROUP_H_ */
