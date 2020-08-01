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
        const w_class_t ListBox::metadata               = { "ListBox", &WidgetContainer::metadata };

        ListBox::ListBox(Display *dpy):
            WidgetContainer(dpy),
            sHBar(dpy),
            sVBar(dpy),
            vItems(&sProperties, &sIListener),
            vSelected(&sProperties, &sIListener),
            sSizeConstraints(&sProperties),
            sHScrollMode(&sProperties),
            sVScrollMode(&sProperties),
            sHScroll(&sProperties),
            sVScroll(&sProperties),
            sFont(&sProperties),
            sBorderSize(&sProperties),
            sBorderRadius(&sProperties),
            sBorderColor(&sProperties),
            sSpacing(&sProperties)
        {
            sArea.nLeft     = 0;
            sArea.nTop      = 0;
            sArea.nWidth    = 0;
            sArea.nHeight   = 0;

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
            sHBar.set_parent(NULL);
            sVBar.set_parent(NULL);

            sHBar.destroy();
            sVBar.destroy();
        }

        status_t ListBox::init()
        {
            // Initialize widgets
            status_t result = WidgetContainer::init();
            if (result == STATUS_OK)
                result  = sHBar.init();
            if (result == STATUS_OK)
                result  = sVBar.init();
            if (result != STATUS_OK)
                return result;

            sIListener.bind_all(this, on_add_item, on_remove_item);

            // Configure scroll bars
            sHBar.orientation()->set(O_HORIZONTAL);
            sHBar.step()->set(1.0f, 8.0f, 0.5f);
            sHBar.accel_step()->set(1.0f, 8.0f, 0.5f);
            sHBar.set_parent(this);
            sHBar.slots()->bind(SLOT_CHANGE, slot_on_scroll_change, self());

            sVBar.orientation()->set(O_VERTICAL);
            sVBar.step()->set(1.0f, 8.0f, 0.5f);
            sVBar.accel_step()->set(1.0f, 8.0f, 0.5f);
            sVBar.set_parent(this);
            sVBar.slots()->bind(SLOT_CHANGE, slot_on_scroll_change, self());

            // Init style
            sSizeConstraints.bind("size.constraints", &sStyle);
            sHScrollMode.bind("hscroll.mode", &sStyle);
            sVScrollMode.bind("vscroll.mode", &sStyle);
            sHScroll.bind("hscroll", &sStyle);
            sVScroll.bind("vscroll", &sStyle);
            sFont.bind("font", &sStyle);
            sBorderSize.bind("border.size", &sStyle);
            sBorderRadius.bind("border.radius", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sSpacing.bind("spacing", &sStyle);

            sHScroll.lock_range();
            sVScroll.lock_range();

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sSizeConstraints.init(sclass);
                sHScrollMode.init(sclass, SCROLL_OPTIONAL);
                sVScrollMode.init(sclass, SCROLL_OPTIONAL);
                sHScroll.init(sclass, 0.0f, 0.0f, 0.0f);
                sVScroll.init(sclass, 0.0f, 0.0f, 0.0f);
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

        void ListBox::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);
            if (sSizeConstraints.is(prop))
                query_resize();
            if (sHScrollMode.is(prop))
                query_resize();
            if (sVScrollMode.is(prop))
                query_resize();
            if (sHScroll.is(prop))
                sHBar.value()->set(sHScroll.get());
            if (sVScroll.is(prop))
                sVBar.value()->set(sVScroll.get());
            if (sFont.is(prop))
                query_resize();
            if (sBorderSize.is(prop))
                query_resize();
            if (sBorderRadius.is(prop))
                query_resize();
            if (sBorderColor.is(prop))
                query_draw();
            if (sSpacing.is(prop))
                query_resize();

            if (vItems.is(prop))
                query_resize();
            if (vSelected.is(prop))
                query_draw();
        }

        void ListBox::size_request(ws::size_limit_t *r)
        {
        }

        void ListBox::realize(const ws::rectangle_t *r)
        {
        }

        void ListBox::realize_children(const ws::rectangle_t *r)
        {
        }

        Widget *ListBox::find_widget(ssize_t x, ssize_t y)
        {
            if ((sHBar.visibility()->get()) && (sHBar.inside(x, y)))
                return &sHBar;
            if ((sVBar.visibility()->get()) && (sVBar.inside(x, y)))
                return &sVBar;

            return NULL;
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

        status_t ListBox::slot_on_scroll_change(Widget *sender, void *ptr, void *data)
        {
            ListBox *_this = widget_ptrcast<ListBox>(ptr);
            if (_this == NULL)
                return STATUS_OK;

            if ((&_this->sHBar != sender) && (&_this->sVBar != sender))
                return STATUS_OK;

            ws::rectangle_t xr = _this->sArea;
            if (_this->sHBar.visibility()->get())
                xr.nLeft   -= _this->sHBar.value()->get();
            if (_this->sVBar.visibility()->get())
                xr.nTop    -= _this->sVBar.value()->get();

            _this->realize_children(&xr);
            _this->query_draw();

            return STATUS_OK;
        }

        void ListBox::on_add_item(void *obj, Property *prop, Widget *w)
        {
            ListBoxItem *item = widget_cast<ListBoxItem>(w);
            if (item == NULL)
                return;

            ListBox *_this = widget_ptrcast<ListBox>(obj);
            if (_this == NULL)
                return;
        }

        void ListBox::on_remove_item(void *obj, Property *prop, Widget *w)
        {
            ListBoxItem *item = widget_cast<ListBoxItem>(w);
            if (item == NULL)
                return;

            ListBox *_this = widget_ptrcast<ListBox>(obj);
            if (_this == NULL)
                return;

            // Remove widget from selection list
            if (_this->vItems.is(prop))
                _this->vSelected.remove(item);
        }

    } /* namespace tk */
} /* namespace lsp */
