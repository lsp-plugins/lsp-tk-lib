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
        
        class ListBox: public WidgetContainer
        {
            private:
                ListBox & operator = (const ListBox &);

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
                ssize_t                         nCurrIndex;
                ssize_t                         nLastIndex;
                size_t                          nKeyScroll;     // Key scroll direction

                Timer                           sKeyTimer;      // Key scroll timer
                ScrollBar                       sHBar;
                ScrollBar                       sVBar;
                ws::rectangle_t                 sArea;
                ws::rectangle_t                 sList;
                lltl::darray<item_t>            vVisible;

                prop::WidgetList<ListBoxItem>   vItems;
                prop::WidgetSet<ListBoxItem>    vSelected;
                prop::CollectionListener        sIListener;

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

                static void             on_add_item(void *obj, Property *prop, Widget *w);
                static void             on_remove_item(void *obj, Property *prop, Widget *w);
                static status_t         key_scroll_handler(ws::timestamp_t sched, ws::timestamp_t time, void *arg);

            protected:
                virtual void            property_changed(Property *prop);
                virtual void            size_request(ws::size_limit_t *r);
                virtual void            realize(const ws::rectangle_t *r);

            public:
                explicit ListBox(Display *dpy);
                virtual ~ListBox();

                virtual status_t            init();
                virtual void                destroy();

            public:
                LSP_TK_PROPERTY(SizeConstraints,    size_constraints,   &sSizeConstraints)
                LSP_TK_PROPERTY(Scrolling,          hscroll_mode,       &sHScrollMode)
                LSP_TK_PROPERTY(Scrolling,          vscroll_mode,       &sVScrollMode)

                LSP_TK_PROPERTY(RangeFloat,         hscroll,            &sHScroll)
                LSP_TK_PROPERTY(RangeFloat,         vscroll,            &sVScroll)

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
                virtual Widget             *find_widget(ssize_t x, ssize_t y);

                virtual status_t            add(Widget *child);

                virtual status_t            remove(Widget *child);

                virtual status_t            remove_all();

                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force);

                virtual status_t            on_mouse_down(const ws::event_t *e);

                virtual status_t            on_mouse_up(const ws::event_t *e);

                virtual status_t            on_mouse_move(const ws::event_t *e);

                virtual status_t            on_mouse_scroll(const ws::event_t *e);

                virtual status_t            on_key_down(const ws::event_t *e);

                virtual status_t            on_key_up(const ws::event_t *e);

                virtual status_t            on_change();

                virtual status_t            on_submit();
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_COMPOUND_LISTBOX_H_ */
