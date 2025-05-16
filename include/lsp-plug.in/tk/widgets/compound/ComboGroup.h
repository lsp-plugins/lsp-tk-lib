/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 14 авг. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_COMPOUND_COMBOGROUP_H_
#define LSP_PLUG_IN_TK_WIDGETS_COMPOUND_COMBOGROUP_H_

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
            LSP_TK_STYLE_DEF_BEGIN(ComboGroup, WidgetContainer)
                prop::Font                  sFont;
                prop::TextAdjust            sTextAdjust;
                prop::Color                 sColor;
                prop::Color                 sTextColor;
                prop::Color                 sSpinColor;
                prop::String                sEmptyText;
                prop::Boolean               sOpened;
                prop::Integer               sBorder;
                prop::Padding               sTextPadding;
                prop::Integer               sRadius;
                prop::Integer               sTextRadius;
                prop::Integer               sSpinSize;
                prop::Integer               sSpinSpacing;
                prop::Embedding             sEmbedding;
                prop::Layout                sLayout;
                prop::SizeConstraints       sSizeConstraints;
                prop::Alignment             sHeading;
                prop::Boolean               sInvertMouseVScroll;
            LSP_TK_STYLE_DEF_END
        }

        /**
         * Widget group, implements a single widget container that aligns the child widget
         * according to the layout settings. The container ignores allocation() property
         * of the child widget.
         */
        class ComboGroup: public WidgetContainer
        {
            public:
                static const w_class_t    metadata;

            private:
                static const tether_t       tether_list[];

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

                        virtual status_t        on_hide() override;
                        virtual status_t        on_show() override;
                };

                class List: public ListBox
                {
                    public:
                        static const w_class_t    metadata;

                    private:
                        ComboGroup *pCGroup;

                    public:
                        explicit List(Display *dpy, ComboGroup *cbox);

                    protected:
                        virtual void        property_changed(Property *prop) override;

                    public:
                        virtual status_t    on_submit() override;
                };

            protected:
                List                        sLBox;          // List box
                Window                      sWindow;        // Popup window

                ws::rectangle_t             sLabel;
                ws::rectangle_t             sText;          // Text parameters
                ws::rectangle_t             sArea;
                size_t                      nMBState;       // Mouse button state
                bool                        bInside;

                prop::Font                  sFont;
                prop::TextAdjust            sTextAdjust;
                prop::Color                 sColor;
                prop::Color                 sTextColor;
                prop::Color                 sSpinColor;
                prop::String                sEmptyText;
                prop::Boolean               sOpened;
                prop::Integer               sBorder;
                prop::Padding               sTextPadding;
                prop::Integer               sRadius;
                prop::Integer               sTextRadius;
                prop::Integer               sSpinSize;
                prop::Integer               sSpinSpacing;
                prop::Embedding             sEmbedding;
                prop::Layout                sLayout;
                prop::SizeConstraints       sSizeConstraints;
                prop::Alignment             sHeading;
                prop::Boolean               sInvertMouseVScroll;

                prop::WidgetList<Widget>    vWidgets;
                prop::WidgetPtr<ListBoxItem> sSelected;
                prop::WidgetPtr<Widget>     sActiveGroup;
                prop::CollectionListener    sIListener;

            protected:
                void                        allocate(alloc_t *alloc);
                Widget                     *current_widget();
                ListBoxItem                *current_item();
                bool                        scroll_item(ssize_t direction, size_t count);

                static void                 on_add_widget(void *obj, Property *prop, void *w);
                static void                 on_remove_widget(void *obj, Property *prop, void *w);
                static status_t             slot_on_change(Widget *sender, void *ptr, void *data);
                static status_t             slot_on_submit(Widget *sender, void *ptr, void *data);

            protected:
                virtual Widget             *find_widget(ssize_t x, ssize_t y) override;
                virtual void                property_changed(Property *prop) override;
                virtual void                size_request(ws::size_limit_t *r) override;
                virtual void                realize(const ws::rectangle_t *r) override;

            public:
                explicit ComboGroup(Display *dpy);
                ComboGroup(const ComboGroup &) = delete;
                ComboGroup(ComboGroup &&) = delete;
                virtual ~ComboGroup() override;

                ComboGroup & operator = (const ComboGroup &) = delete;
                ComboGroup & operator = (ComboGroup &&) = delete;

                virtual status_t            init() override;

            public:
                LSP_TK_PROPERTY(Font,                       font,                   &sFont)
                LSP_TK_PROPERTY(TextAdjust,                 text_ajdust,            &sTextAdjust)
                LSP_TK_PROPERTY(Color,                      color,                  &sColor)
                LSP_TK_PROPERTY(Color,                      text_color,             &sTextColor)
                LSP_TK_PROPERTY(Color,                      spin_color,             &sSpinColor)
                LSP_TK_PROPERTY(String,                     empty_text,             &sEmptyText)
                LSP_TK_PROPERTY(Boolean,                    opened,                 &sOpened)
                LSP_TK_PROPERTY(Integer,                    border_size,            &sBorder)
                LSP_TK_PROPERTY(Padding,                    text_padding,           &sTextPadding)
                LSP_TK_PROPERTY(Integer,                    border_radius,          &sRadius)
                LSP_TK_PROPERTY(Integer,                    text_radius,            &sTextRadius)
                LSP_TK_PROPERTY(Integer,                    spin_size,              &sSpinSize)
                LSP_TK_PROPERTY(Integer,                    spin_spacing,           &sSpinSpacing)
                LSP_TK_PROPERTY(Embedding,                  embedding,              &sEmbedding)
                LSP_TK_PROPERTY(WidgetList<Widget>,         children,               &vWidgets)
                LSP_TK_PROPERTY(Layout,                     layout,                 &sLayout)
                LSP_TK_PROPERTY(SizeConstraints,            constraints,            &sSizeConstraints)
                LSP_TK_PROPERTY(Alignment,                  heading,                &sHeading)
                LSP_TK_PROPERTY(Boolean,                    invert_mouse_vscroll,   &sInvertMouseVScroll);
                LSP_TK_PROPERTY(WidgetPtr<ListBoxItem>,     selected,               &sSelected)
                LSP_TK_PROPERTY(WidgetPtr<Widget>,          active_group,           &sActiveGroup)

                LSP_TK_PROPERTY(Font,                       list_font,              sLBox.font())
                LSP_TK_PROPERTY(WidgetList<ListBoxItem>,    items,                  sLBox.items())
                LSP_TK_PROPERTY(WidgetList<Widget>,         widgets,                &vWidgets)

            public:
                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force) override;
                virtual status_t            add(Widget *child) override;
                virtual status_t            remove(Widget *child) override;
                virtual status_t            remove_all() override;

                virtual status_t            on_mouse_down(const ws::event_t *e) override;
                virtual status_t            on_mouse_up(const ws::event_t *e) override;
                virtual status_t            on_mouse_move(const ws::event_t *e) override;
                virtual status_t            on_mouse_scroll(const ws::event_t *e) override;
                virtual status_t            on_key_down(const ws::event_t *e) override;

            public:
                virtual status_t            add_item(ListBoxItem *child);
                virtual status_t            remove_item(ListBoxItem *child);
                virtual status_t            remove_all_items();

                virtual status_t            on_change();
                virtual status_t            on_submit();

        };

    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_WIDGETS_COMPOUND_COMBOGROUP_H_ */
