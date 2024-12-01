/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_COMPOUND_LISTBOX_H_
#define LSP_PLUG_IN_TK_WIDGETS_COMPOUND_LISTBOX_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class ListBoxItem;
        
        // Style definition
        namespace style
        {
            LSP_TK_STYLE_DEF_BEGIN(ListBox, WidgetContainer)
                prop::SizeConstraints           sSizeConstraints;
                prop::Scrolling                 sHScrollMode;
                prop::Scrolling                 sVScrollMode;
                prop::RangeFloat                sHScroll;
                prop::RangeFloat                sVScroll;
                prop::Font                      sFont;
                prop::Integer                   sBorderSize;
                prop::Integer                   sBorderGap;
                prop::Integer                   sBorderRadius;
                prop::Color                     sBorderColor;
                prop::Color                     sListBgColor;
                prop::Integer                   sSpacing;
                prop::Boolean                   sMultiSelect;
                prop::Integer                   sHScrollSpacing;
                prop::Integer                   sVScrollSpacing;
            LSP_TK_STYLE_DEF_END
        }

        class ListBox: public WidgetContainer
        {
            public:
                static const w_class_t      metadata;

            protected:
                typedef struct item_t
                {
                    ws::rectangle_t     a;          // Allocated space for widget
                    ws::rectangle_t     r;          // Realized space for widget
                    size_t              index;      // Index of the widget
                    ListBoxItem        *item;       // Widget item contained in the cell
                } item_t;

                typedef struct alloc_t
                {
                    lltl::darray<item_t>    vItems;     // Items
                    ssize_t                 wMinW;      // Minimum width
                    ssize_t                 wMinH;      // Minimum height
                    ssize_t                 wItemH;     // Maximum item height
                    bool                    bHBar;      // Horizontal scroll enabled
                    bool                    bVBar;      // Vertical scroll enabled
                    ws::size_limit_t        sSize;      // Actual size
                    ws::rectangle_t         sArea;      // Area to display contents
                    ws::rectangle_t         sList;      // List to display contents
                    ws::rectangle_t         sHBar;      // Horizontal scroll bar
                    ws::rectangle_t         sVBar;      // Vertical scroll bar
                } alloc_t;

                enum scroll_t
                {
                    SCR_NONE            = 0,
                    SCR_UP              = 1 << 0,
                    SCR_DOWN            = 1 << 1,
                    SCR_KP_UP           = 1 << 2,
                    SCR_KP_DOWN         = 1 << 3,
                    SCR_PGUP            = 1 << 4,
                    SCR_PGDOWN          = 1 << 5,
                    SCR_KP_PGUP         = 1 << 6,
                    SCR_KP_PGDOWN       = 1 << 7,

                    SCR_LEFT            = 1 << 8,
                    SCR_RIGHT           = 1 << 9,
                    SCR_KP_LEFT         = 1 << 10,
                    SCR_KP_RIGHT        = 1 << 11,

                    SCR_SHIFT           = 1 << 12,
                    SCR_CTRL            = 1 << 13,

                    SCR_KEYMASK         = 0xfff,
                    SCR_KEYMASK1        = 0x555,
                    SCR_KEYMASK2        = 0xaaa
                };

                enum flags_t
                {
                    F_SEL_ACTIVE        = 1 << 0,
                    F_SUBMIT            = 1 << 1,
                    F_CHANGED           = 1 << 2
                };

            protected:
                size_t                          nBMask;
                size_t                          nXFlags;
                ssize_t                         nPendingIndex;  // Pending index
                ssize_t                         nCurrIndex;
                ssize_t                         nLastIndex;
                size_t                          nKeyScroll;     // Key scroll direction
                ListBoxItem                    *pHoverItem;     // Hover item

                ScrollBar                       sHBar;
                ScrollBar                       sVBar;
                ws::rectangle_t                 sArea;
                ws::rectangle_t                 sList;
                lltl::darray<item_t>            vVisible;

                prop::WidgetList<ListBoxItem>   vItems;
                prop::WidgetSet<ListBoxItem>    vSelected;
                prop::CollectionListener        sIListener;
                prop::CollectionListener        sSListener;

                prop::SizeConstraints           sSizeConstraints;
                prop::Scrolling                 sHScrollMode;
                prop::Scrolling                 sVScrollMode;
                prop::RangeFloat                sHScroll;
                prop::RangeFloat                sVScroll;

                prop::Font                      sFont;
                prop::Integer                   sBorderSize;
                prop::Integer                   sBorderGap;
                prop::Integer                   sBorderRadius;
                prop::Color                     sBorderColor;
                prop::Color                     sListBgColor;
                prop::Integer                   sSpacing;
                prop::Boolean                   sMultiSelect;
                prop::Integer                   sHScrollSpacing;
                prop::Integer                   sVScrollSpacing;

            protected:
                void                    do_destroy();
                void                    allocate_items(alloc_t *alloc);
                void                    estimate_size(alloc_t *a, const ws::rectangle_t *xr);
                void                    realize_children();
                void                    keep_single_selection();
                item_t                 *find_item(ssize_t x, ssize_t y);
                item_t                 *find_by_index(ssize_t index);
                void                    select_range(ssize_t first, ssize_t last, bool add);
                void                    select_single(ssize_t index, bool add);
                status_t                on_key_scroll();
                bool                    scroll_to_item(ssize_t vindex);

            protected:
                static status_t         slot_on_scroll_change(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_change(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_submit(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_scroll_key_event(Widget *sender, void *ptr, void *data);

                static void             on_add_item(void *obj, Property *prop, void *w);
                static void             on_remove_item(void *obj, Property *prop, void *w);
                static void             on_select_item(void *obj, Property *prop, void *w);
                static void             on_deselect_item(void *obj, Property *prop, void *w);

            protected:
                virtual void            property_changed(Property *prop) override;
                virtual void            size_request(ws::size_limit_t *r) override;
                virtual void            realize(const ws::rectangle_t *r) override;

            public:
                explicit ListBox(Display *dpy);
                ListBox(const ListBox &) = delete;
                ListBox(ListBox &&) = delete;
                virtual ~ListBox() override;

                ListBox & operator = (const ListBox &) = delete;
                ListBox & operator = (ListBox &&) = delete;

                virtual status_t            init() override;
                virtual void                destroy() override;

            public:
                LSP_TK_PROPERTY(SizeConstraints,    constraints,        &sSizeConstraints)
                LSP_TK_PROPERTY(Scrolling,          hscroll_mode,       &sHScrollMode)
                LSP_TK_PROPERTY(Scrolling,          vscroll_mode,       &sVScrollMode)

                LSP_TK_PROPERTY(RangeFloat,         hscroll,            &sHScroll)
                LSP_TK_PROPERTY(RangeFloat,         vscroll,            &sVScroll)
                LSP_TK_PROPERTY(StepFloat,          hstep,              sHBar.step())
                LSP_TK_PROPERTY(StepFloat,          vstep,              sVBar.step())
                LSP_TK_PROPERTY(StepFloat,          haccel_step,        sHBar.accel_step())
                LSP_TK_PROPERTY(StepFloat,          vaccel_step,        sVBar.accel_step())

                LSP_TK_PROPERTY(WidgetList<ListBoxItem>,    items,      &vItems)
                LSP_TK_PROPERTY(WidgetSet<ListBoxItem>,     selected,   &vSelected)

                LSP_TK_PROPERTY(Font,               font,                       &sFont)
                LSP_TK_PROPERTY(Integer,            spacing,                    &sSpacing)
                LSP_TK_PROPERTY(Integer,            border_size,                &sBorderSize)
                LSP_TK_PROPERTY(Integer,            border_gap,                 &sBorderGap)
                LSP_TK_PROPERTY(Integer,            border_radius,              &sBorderRadius)
                LSP_TK_PROPERTY(Color,              border_color,               &sBorderColor)
                LSP_TK_PROPERTY(Color,              list_bg_color,              &sListBgColor)
                LSP_TK_PROPERTY(Boolean,            multi_select,               &sMultiSelect)

                LSP_TK_PROPERTY(Integer,            hscroll_spacing,            &sHScrollSpacing)
                LSP_TK_PROPERTY(Integer,            vscroll_spacing,            &sVScrollSpacing)

            public:
                virtual Widget             *find_widget(ssize_t x, ssize_t y) override;
                virtual status_t            add(Widget *child) override;
                virtual status_t            remove(Widget *child) override;
                virtual status_t            remove_all() override;
                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force) override;

                virtual status_t            on_mouse_down(const ws::event_t *e) override;
                virtual status_t            on_mouse_up(const ws::event_t *e) override;
                virtual status_t            on_mouse_out(const ws::event_t *e) override;
                virtual status_t            on_mouse_move(const ws::event_t *e) override;
                virtual status_t            on_mouse_scroll(const ws::event_t *e) override;

                virtual status_t            on_key_down(const ws::event_t *e) override;
                virtual status_t            on_key_up(const ws::event_t *e) override;

            public:
                virtual status_t            on_change();
                virtual status_t            on_submit();

                virtual void                scroll_to_current();
                virtual void                scroll_to(size_t index);
        };
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_COMPOUND_LISTBOX_H_ */
