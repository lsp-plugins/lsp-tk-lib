/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 3 дек. 2024 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_TABGROUP_H_
#define LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_TABGROUP_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/tk/widgets/containers/TabControl.h>

namespace lsp
{
    namespace tk
    {
        // Style definition
        namespace style
        {
            LSP_TK_STYLE_DEF_BEGIN(TabGroup, TabControl)
            LSP_TK_STYLE_DEF_END
        } /* namespae style */

        /**
         * Tab control, implements a single widget container that aligns the child widget
         * according to the layout settings. The container ignores allocation() property
         * of the child widget.
         */
        class TabGroup: public WidgetContainer
        {
            public:
                static const w_class_t      metadata;

            protected:
                typedef struct tab_t
                {
                    ws::rectangle_t         bounds;         // Bounding box area
                    ws::rectangle_t         text;           // Text area
                    tk::TabItem            *widget;
                    size_t                  border;         // Border size
                } tab_t;

                enum tab_mode_t
                {
                    TM_NORMAL,
                    TM_SELECTED,
                    TM_HOVER
                };

            protected:
                tk::Widget                 *pWidget;        // Wrapped widget
                lltl::darray<tab_t>         vVisible;       // List of visible tab headings
                ws::rectangle_t             sArea;          // Internal area for drawing the widget
                ws::rectangle_t             sBounds;        // External area for drawing widget
                ws::rectangle_t             sTabArea;       // Location of tab area
                ws::rectangle_t             sHead[2];       // Heading rectangles
                ws::rectangle_t             sHeadSpacing;   // Heading spacing
                ws::rectangle_t             sHeadGap;       // Heading gap
                ssize_t                     nTabShift;      // Tab shift
                size_t                      nMBState;       // Mouse button state
                tk::TabItem                *pEventTab;      // Current event tab

                prop::Color                 sBorderColor;
                prop::Color                 sHeadingColor;
                prop::Color                 sHeadingSpacingColor;
                prop::Color                 sHeadingGapColor;
                prop::Integer               sBorderSize;
                prop::Integer               sBorderRadius;
                prop::Integer               sTabSpacing;
                prop::Integer               sHeadingSpacing;
                prop::Integer               sHeadingGap;
                prop::Float                 sHeadingGapBrightness;
                prop::Embedding             sEmbedding;
                prop::Layout                sHeading;
                prop::SizeConstraints       sSizeConstraints;
                prop::Boolean               sTabJoint;
                prop::Boolean               sHeadingFill;
                prop::Boolean               sHeadingSpacingFill;
                prop::Pointer               sTabPointer;            // Tab pointer

                prop::WidgetList<TabItem>   vItems;
                prop::WidgetPtr<TabItem>    sSelected;
                prop::CollectionListener    sIListener;

            protected:
                void                        allocate_tabs(size_t *max_tab_border, ws::rectangle_t *area, lltl::darray<tab_t> *tabs);
                tk::TabItem                *current_tab();
                bool                        scroll_item(ssize_t increment);
                tk::TabItem                *find_tab(ssize_t x, ssize_t y);
                void                        draw_tab(ws::ISurface *s, const tab_t *tab, tab_mode_t mode, const ws::rectangle_t *area);
                void                        do_destroy();

            protected:
                static void                 on_add_widget(void *obj, Property *prop, void *w);
                static void                 on_remove_widget(void *obj, Property *prop, void *w);
                static status_t             slot_on_change(Widget *sender, void *ptr, void *data);
                static status_t             slot_on_submit(Widget *sender, void *ptr, void *data);
                static const lsp::Color    *select_color(tab_mode_t mode, const tk::Color *normal, const tk::Color * selected, const tk::Color *hover);

            protected:
                virtual void                property_changed(Property *prop) override;
                virtual void                size_request(ws::size_limit_t *r) override;
                virtual void                realize(const ws::rectangle_t *r) override;

            public:
                explicit TabGroup(Display *dpy);
                TabGroup(const TabGroup &) = delete;
                TabGroup(TabGroup &&) = delete;
                virtual ~TabGroup() override;

                TabGroup & operator = (const TabGroup &) = delete;
                TabGroup & operator = (TabGroup &&) = delete;

                virtual status_t            init() override;
                virtual void                destroy() override;

            public:
                LSP_TK_PROPERTY(Color,                      border_color,               &sBorderColor)
                LSP_TK_PROPERTY(Color,                      heading_color,              &sHeadingColor)
                LSP_TK_PROPERTY(Color,                      heading_spacing_color,      &sHeadingSpacingColor)
                LSP_TK_PROPERTY(Color,                      heading_gap_color,          &sHeadingGapColor)
                LSP_TK_PROPERTY(Integer,                    border_size,                &sBorderSize)
                LSP_TK_PROPERTY(Integer,                    border_radius,              &sBorderRadius)
                LSP_TK_PROPERTY(Integer,                    tab_spacing,                &sTabSpacing)
                LSP_TK_PROPERTY(Integer,                    heading_spacing,            &sHeadingSpacing)
                LSP_TK_PROPERTY(Integer,                    heading_gap,                &sHeadingGap)
                LSP_TK_PROPERTY(Float,                      heading_gap_brightness,     &sHeadingGapBrightness)
                LSP_TK_PROPERTY(Embedding,                  embedding,                  &sEmbedding)
                LSP_TK_PROPERTY(Layout,                     heading,                    &sHeading)
                LSP_TK_PROPERTY(SizeConstraints,            constraints,                &sSizeConstraints)
                LSP_TK_PROPERTY(Boolean,                    tab_joint,                  &sTabJoint)
                LSP_TK_PROPERTY(Boolean,                    heading_fill,               &sHeadingFill)
                LSP_TK_PROPERTY(Boolean,                    heading_spacing_fill,       &sHeadingSpacingFill)
                LSP_TK_PROPERTY(Pointer,                    mouse_pointer,              &sTabPointer)
                LSP_TK_PROPERTY(WidgetPtr<TabItem>,         selected,                   &sSelected)
                LSP_TK_PROPERTY(WidgetList<TabItem>,        widgets,                    &vItems)

            public:
                virtual Widget             *find_widget(ssize_t x, ssize_t y) override;

                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force) override;

                virtual status_t            add(Widget *child) override;
                virtual status_t            remove(Widget *child) override;
                virtual status_t            remove_all() override;

                virtual status_t            on_mouse_down(const ws::event_t *e) override;
                virtual status_t            on_mouse_up(const ws::event_t *e) override;
                virtual status_t            on_mouse_move(const ws::event_t *e) override;
                virtual status_t            on_mouse_scroll(const ws::event_t *e) override;
                virtual status_t            on_mouse_out(const ws::event_t *e) override;
                virtual status_t            on_key_down(const ws::event_t *e) override;
                virtual status_t            on_mouse_pointer(pointer_event_t *e) override;

            public:
                virtual status_t            add_item(TabItem *child);
                virtual status_t            remove_item(TabItem *child);
                virtual status_t            remove_all_items();

                virtual status_t            on_change();
                virtual status_t            on_submit();
        };

    } /* namespace tk */
} /* namespace lsp */


#endif /* LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_TABGROUP_H_ */
