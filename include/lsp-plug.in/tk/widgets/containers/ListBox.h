/*
 * ListBox.h
 *
 *  Created on: 30 июл. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_LISTBOX_H_
#define LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_LISTBOX_H_

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
                typedef struct alloc_t
                {
                } alloc_t;

            protected:
                ScrollBar                       sHBar;
                ScrollBar                       sVBar;
                ws::rectangle_t                 sArea;

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
                prop::Integer                   sBorderRadius;
                prop::Color                     sBorderColor;
                prop::Integer                   sSpacing;

            protected:
                void                    do_destroy();
                void                    estimate_size(alloc_t *a, const ws::rectangle_t *xr);
                void                    realize_children(const ws::rectangle_t *r);

                static status_t         slot_on_scroll_change(Widget *sender, void *ptr, void *data);
                static void             on_add_item(void *obj, Property *prop, Widget *w);
                static void             on_remove_item(void *obj, Property *prop, Widget *w);

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
                LSP_TK_PROPERTY(Integer,            border_radius,              &sBorderRadius)
                LSP_TK_PROPERTY(Color,              border_color,               &sBorderColor)

            public:
                virtual Widget             *find_widget(ssize_t x, ssize_t y);

                virtual status_t            add(Widget *child);

                virtual status_t            remove(Widget *child);

                virtual status_t            remove_all();
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_LISTBOX_H_ */
