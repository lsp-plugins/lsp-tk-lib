/*
 * ListBox.cpp
 *
 *  Created on: 30 июл. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        //---------------------------------------------------------------------
        const w_class_t ListBox::ItemList::metadata     = { "ListBox::ItemList", &WidgetContainer::metadata };

        ListBox::ItemList::ItemList(Display *dpy, ListBox *box):
            WidgetContainer(dpy)
        {
            pListBox    = box;

            pClass      = &metadata;
        }
        
        ListBox::ItemList::~ItemList()
        {
            pListBox    = NULL;
        }

        //---------------------------------------------------------------------
        const w_class_t ListBox::ItemArea::metadata     = { "ListBox::ItemArea", &ScrollArea::metadata };

        ListBox::ItemArea::ItemArea(Display *dpy, ListBox *box):
            ScrollArea(dpy)
        {
            pListBox    = box;

            pClass      = &metadata;
        }

        ListBox::ItemArea::~ItemArea()
        {
            pListBox    = NULL;
        }

        //---------------------------------------------------------------------
        const w_class_t ListBox::metadata               = { "ListBox", &WidgetContainer::metadata };

        ListBox::ListBox(Display *dpy):
            WidgetContainer(dpy),
            sList(dpy, this),
            sArea(dpy, this),
            vItems(&sProperties, &sIListener),
            vSelected(&sProperties),
            sFont(&sProperties),
            sBorderSize(&sProperties),
            sBorderRadius(&sProperties),
            sBorderColor(&sProperties),
            sSpacing(&sProperties)
        {

            pClass      = &metadata;
        }
        
        ListBox::~ListBox()
        {
            do_destroy();
        }

        void ListBox::destroy()
        {
            do_destroy();
            WidgetContainer::destroy();
        }

        void ListBox::do_destroy()
        {
            // Unlink all items
            size_t n            = vItems.size();
            for (size_t i=0; i<n; ++i)
            {
                ListBoxItem *item   = vItems.uget(i);
                if (item == NULL)
                    continue;

                unlink_widget(item);
            }

            vItems.flush();

            // Cleanup relations
            sArea.set_parent(NULL);
            sList.set_parent(NULL);

            sArea.destroy();
            sList.destroy();
        }

        status_t ListBox::init()
        {
            // Initialize parent class
            status_t result = WidgetContainer::init();
            if (result != STATUS_OK)
                return result;

            if ((result = sArea.init()) != STATUS_OK)
                return result;
            if ((result = sList.init()) != STATUS_OK)
                return result;

            // Set relations
            sList.set_parent(&sArea);
            sArea.set_parent(this);

            // Init styles
            sFont.bind("font", &sStyle);
            sBorderSize.bind("border.size", &sStyle);
            sBorderRadius.bind("border.radius", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sSpacing.bind("spacing", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sFont.init(sclass);
                sBorderSize.init(sclass, 1);
                sBorderRadius.init(sclass, 2);
                sBorderColor.init(sclass, "#000000");
                sSpacing.init(sclass, 0);

                // Overrides
                sBgColor.override(sclass, "#ffffff");
            }

            return STATUS_OK;
        }

        Widget *ListBox::find_widget(ssize_t x, ssize_t y)
        {
            return sArea.find_widget(x, y);
        }

        status_t ListBox::add(Widget *child)
        {
            ListBoxItem *item = widget_cast<ListBoxItem>(child);
            if (item == NULL)
                return STATUS_BAD_TYPE;

            return vItems.add(item);
        }

        status_t ListBox::remove(Widget *child)
        {
            ListBoxItem *item = widget_cast<ListBoxItem>(child);
            if (item == NULL)
                return STATUS_BAD_TYPE;

            return vItems.premove(item);
        }

        status_t ListBox::remove_all()
        {
            vItems.clear();
            return STATUS_OK;
        }
    } /* namespace tk */
} /* namespace lsp */
