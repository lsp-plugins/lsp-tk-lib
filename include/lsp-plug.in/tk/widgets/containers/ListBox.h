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

            private:
                class ItemList: public WidgetContainer
                {
                    public:
                        static const w_class_t      metadata;

                    protected:
                        ListBox                    *pListBox;

                    public:
                        explicit ItemList(Display *dpy, ListBox *box);
                        virtual ~ItemList();
                };

                class ItemArea: public ScrollArea
                {
                    public:
                        static const w_class_t      metadata;

                    protected:
                        ListBox                    *pListBox;

                    public:
                        explicit ItemArea(Display *dpy, ListBox *box);
                        virtual ~ItemArea();
                };

            protected:
                ItemList                        sList;
                ItemArea                        sArea;

                prop::WidgetList<ListBoxItem>   vItems;
                prop::WidgetSet<ListBoxItem>    vSelected;
                prop::CollectionListener        sIListener;

                prop::Font                      sFont;
                prop::Integer                   sBorderSize;
                prop::Integer                   sBorderRadius;
                prop::Color                     sBorderColor;
                prop::Integer                   sSpacing;

            protected:
                void                        do_destroy();

            public:
                explicit ListBox(Display *dpy);
                virtual ~ListBox();

                virtual status_t            init();
                virtual void                destroy();

            public:
                LSP_TK_PROPERTY(Layout,             layout,             sArea.layout())
                LSP_TK_PROPERTY(Scrolling,          hscroll_mode,       sArea.hscroll_mode())
                LSP_TK_PROPERTY(Scrolling,          vscroll_mode,       sArea.vscroll_mode())
                LSP_TK_PROPERTY(RangeFloat,         hscroll,            sArea.hscroll())
                LSP_TK_PROPERTY(RangeFloat,         vscroll,            sArea.vscroll())

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
