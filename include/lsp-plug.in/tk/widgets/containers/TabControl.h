/*
 * Copyright (C) 2022 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2022 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 8 нояб. 2022 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_TABCONTROL_H_
#define LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_TABCONTROL_H_

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
            LSP_TK_STYLE_DEF_BEGIN(TabControl, WidgetContainer)
                prop::Font                  sActiveTabFont;
                prop::Font                  sInactiveTabFont;
                prop::TextAdjust            sActiveTabTextAdjust;
                prop::TextAdjust            sInactiveTabTextAdjust;
                prop::Color                 sBorderColor;
                prop::Color                 sActiveTabColor;
                prop::Color                 sInactiveTabColor;
                prop::Color                 sActiveTabBorderColor;
                prop::Color                 sInactiveTabBorderColor;
                prop::Color                 sActiveTabTextColor;
                prop::Color                 sInactiveTabTextColor;
                prop::Padding               sActiveTabTextPadding;
                prop::Padding               sInactiveTabTextPadding;
                prop::Integer               sBorderSize;
                prop::Integer               sActiveTabBorderSize;
                prop::Integer               sInactiveTabBorderSize;
                prop::Integer               sBorderRadius;
                prop::Integer               sActiveTabRadius;
                prop::Integer               sInactiveTabRadius;
                prop::Embedding             sEmbedding;
                prop::Layout                sLayout;
                prop::Layout                sHeading;
                prop::SizeConstraints       sSizeConstraints;
            LSP_TK_STYLE_DEF_END
        } /* namespae style */

        /**
         * Tab control, implements a single widget container that aligns the child widget
         * according to the layout settings. The container ignores allocation() property
         * of the child widget.
         */
        class TabControl: public WidgetContainer
        {
            private:
                TabControl & operator = (const TabControl &);
                TabControl(const TabControl &);

            public:
                static const w_class_t      metadata;

            protected:
                typedef struct alloc_t
                {
                    ws::rectangle_t         text;
                    ws::rectangle_t         rtext;
                    padding_t               pad;
                    padding_t               xpad;
                } alloc_t;

            protected:
                lltl::darray<ws::rectangle_t>   vTabs;      // List of allocated tab headings
                ws::rectangle_t             sArea;
                size_t                      nMBState;       // Mouse button state
                tk::Tab                    *pEventTab;

                prop::Font                  sActiveTabFont;
                prop::Font                  sInactiveTabFont;
                prop::TextAdjust            sActiveTabTextAdjust;
                prop::TextAdjust            sInactiveTabTextAdjust;
                prop::Color                 sBorderColor;
                prop::Color                 sActiveTabColor;
                prop::Color                 sInactiveTabColor;
                prop::Color                 sActiveTabBorderColor;
                prop::Color                 sInactiveTabBorderColor;
                prop::Color                 sActiveTabTextColor;
                prop::Color                 sInactiveTabTextColor;
                prop::Padding               sActiveTabTextPadding;
                prop::Padding               sInactiveTabTextPadding;
                prop::Integer               sBorderSize;
                prop::Integer               sActiveTabBorderSize;
                prop::Integer               sInactiveTabBorderSize;
                prop::Integer               sBorderRadius;
                prop::Integer               sActiveTabRadius;
                prop::Integer               sInactiveTabRadius;
                prop::Embedding             sEmbedding;
                prop::Layout                sLayout;
                prop::Layout                sHeading;
                prop::SizeConstraints       sSizeConstraints;

                prop::WidgetList<Tab>       vWidgets;
                prop::WidgetPtr<Tab>        sSelected;
                prop::CollectionListener    sIListener;

            protected:
                void                        allocate(alloc_t *alloc);
                Widget                     *current_widget();
                bool                        scroll_item(ssize_t increment);
                tk::Tab                    *find_tab(ssize_t x, ssize_t y);

            protected:
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
                explicit TabControl(Display *dpy);
                virtual ~TabControl();

                virtual status_t            init();

            public:
                LSP_TK_PROPERTY(Font,                       active_tab_font,            &sActiveTabFont)
                LSP_TK_PROPERTY(Font,                       inactive_tab_font,          &sInactiveTabFont)
                LSP_TK_PROPERTY(TextAdjust,                 active_tab_text_ajdust,     &sActiveTabTextAdjust)
                LSP_TK_PROPERTY(TextAdjust,                 inactive_tab_text_ajdust,   &sInactiveTabTextAdjust)
                LSP_TK_PROPERTY(Color,                      border_color,               &sBorderColor)
                LSP_TK_PROPERTY(Color,                      active_tab_color,           &sActiveTabColor)
                LSP_TK_PROPERTY(Color,                      inactive_tab_color,         &sInactiveTabColor)
                LSP_TK_PROPERTY(Color,                      active_tab_border_color,    &sActiveTabBorderColor)
                LSP_TK_PROPERTY(Color,                      inactive_tab_border_color,  &sInactiveTabBorderColor)
                LSP_TK_PROPERTY(Padding,                    active_tab_text_padding,    &sActiveTabTextPadding)
                LSP_TK_PROPERTY(Padding,                    inactive_tab_text_padding,  &sInactiveTabTextPadding)
                LSP_TK_PROPERTY(Integer,                    border_size,                &sBorderSize)
                LSP_TK_PROPERTY(Integer,                    active_tab_border_size,     &sActiveTabBorderSize)
                LSP_TK_PROPERTY(Integer,                    inactive_tab_border_size,   &sInactiveTabBorderSize)
                LSP_TK_PROPERTY(Integer,                    border_radius,              &sBorderRadius)
                LSP_TK_PROPERTY(Integer,                    active_tab_radius,          &sActiveTabRadius)
                LSP_TK_PROPERTY(Integer,                    inactive_tab_radius,        &sInactiveTabRadius)
                LSP_TK_PROPERTY(Embedding,                  embedding,                  &sEmbedding)
                LSP_TK_PROPERTY(Layout,                     layout,                     &sLayout)
                LSP_TK_PROPERTY(Layout,                     heading,                    &sHeading)
                LSP_TK_PROPERTY(SizeConstraints,            constraints,                &sSizeConstraints)
                LSP_TK_PROPERTY(WidgetPtr<Tab>,             selected,                   &sSelected)
                LSP_TK_PROPERTY(WidgetList<Tab>,            widgets,                    &vWidgets)

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
                virtual status_t            on_change();

                virtual status_t            on_submit();
        };

    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_TABCONTROL_H_ */
