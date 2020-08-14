/*
 * ComboGroup.h
 *
 *  Created on: 14 авг. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_COMPOUND_COMBOGROUP_H_
#define LSP_PLUG_IN_TK_WIDGETS_COMPOUND_COMBOGROUP_H_

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
        class ComboGroup: public WidgetContainer
        {
            private:
                ComboGroup & operator = (const ComboGroup &);

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

                class Window: public PopupWindow
                {
                    public:
                        static const w_class_t    metadata;

                    private:
                        ComboGroup  *pCGroup;

                    public:
                        explicit Window(Display *dpy, ComboGroup *cbox);

                        virtual status_t        on_hide();

                        virtual status_t        on_show();
                };

                class List: public ListBox
                {
                    public:
                        static const w_class_t    metadata;

                    private:
                        ComboGroup *pCGroup;

                    public:
                        explicit List(Display *dpy, ComboGroup *cbox);

                    public:
                        virtual status_t    on_submit();

                        virtual status_t    on_change();
                };

            protected:
                List                        sLBox;          // List box
                Window                      sWindow;        // Popup window

                ws::rectangle_t             sLabel;
                ws::rectangle_t             sArea;

                prop::Font                  sFont;
                prop::Color                 sColor;
                prop::Color                 sTextColor;
                prop::Color                 sSpinColor;
                prop::String                sText;
                prop::Boolean               sOpened;
                prop::Integer               sBorder;
                prop::Integer               sTextBorder;
                prop::Integer               sRadius;
                prop::Integer               sTextRadius;
                prop::Integer               sSpinSize;
                prop::Embedding             sEmbedding;
                prop::Layout                sLayout;
                prop::SizeConstraints       sSizeConstraints;
                prop::WidgetList<Widget>    vWidgets;
                prop::WidgetPtr<ListBoxItem> sSelected;

            protected:
                void                        allocate(alloc_t *alloc);
                void                        do_destroy();
                Widget                     *current_widget();

            protected:
                virtual Widget             *find_widget(ssize_t x, ssize_t y);
                virtual void                property_changed(Property *prop);
                virtual void                size_request(ws::size_limit_t *r);
                virtual void                realize(const ws::rectangle_t *r);

            public:
                explicit ComboGroup(Display *dpy);
                virtual ~ComboGroup();

                virtual status_t            init();

            public:
                LSP_TK_PROPERTY(Font,       font,               &sFont)
                LSP_TK_PROPERTY(Color,      color,              &sColor)
                LSP_TK_PROPERTY(Color,      text_color,         &sTextColor)
                LSP_TK_PROPERTY(Color,      spin_color,         &sSpinColor)
                LSP_TK_PROPERTY(String,     text,               &sText)
                LSP_TK_PROPERTY(String,     opened,             &sOpened)
                LSP_TK_PROPERTY(Integer,    border_size,        &sBorder)
                LSP_TK_PROPERTY(Integer,    text_border,        &sTextBorder)
                LSP_TK_PROPERTY(Integer,    border_radius,      &sRadius)
                LSP_TK_PROPERTY(Integer,    text_radius,        &sTextRadius)
                LSP_TK_PROPERTY(Integer,    spin_size,          &sSpinSize)
                LSP_TK_PROPERTY(Embedding,  embedding,          &sEmbedding)
                LSP_TK_PROPERTY(WidgetList<Widget>,         children,               &vWidgets)
                LSP_TK_PROPERTY(Layout,                     layout,                 &sLayout)
                LSP_TK_PROPERTY(SizeConstraints,            constraints,            &sSizeConstraints)
                LSP_TK_PROPERTY(WidgetPtr<ListBoxItem>,     selected,               &sSelected)

                LSP_TK_PROPERTY(Font,                       list_font,              sLBox.font())
                LSP_TK_PROPERTY(WidgetList<ListBoxItem>,    items,                  sLBox.items())

            public:
                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force);

                virtual status_t            add(Widget *child);

                virtual status_t            remove(Widget *child);

                virtual status_t            remove_all();
        };

    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_WIDGETS_COMPOUND_COMBOGROUP_H_ */
