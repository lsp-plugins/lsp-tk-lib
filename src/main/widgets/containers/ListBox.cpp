/*
 * ListBox.cpp
 *
 *  Created on: 30 июл. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/stdlib/math.h>

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
            sBorderGap(&sProperties),
            sBorderRadius(&sProperties),
            sBorderColor(&sProperties),
            sSpacing(&sProperties),
            sMultiSelect(&sProperties)
        {
            sArea.nLeft     = 0;
            sArea.nTop      = 0;
            sArea.nWidth    = 0;
            sArea.nHeight   = 0;

            sList.nLeft     = 0;
            sList.nTop      = 0;
            sList.nWidth    = 0;
            sList.nHeight   = 0;

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
            vSelected.flush();
            vVisible.flush();

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
            sBorderGap.bind("border.gap", &sStyle);
            sBorderRadius.bind("border.radius", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sSpacing.bind("spacing", &sStyle);
            sMultiSelect.bind("multiselect", &sStyle);

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
                sBorderGap.init(sclass, 1);
                sBorderRadius.init(sclass, 2);
                sBorderColor.init(sclass, "#000000");
                sSpacing.init(sclass, 0);
                sMultiSelect.init(sclass, false);

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
            if (sMultiSelect.is(prop))
            {
                if (!sMultiSelect.get())
                    keep_single_selection();
            }

            if (vItems.is(prop))
                query_resize();
            if (vSelected.is(prop))
                query_draw();
        }

        void ListBox::allocate_items(alloc_t *alloc)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t spacing     = lsp_max(0.0f, scaling * sSpacing.get());

            lltl::darray<alloc_t> *v    = &alloc->vItems;

            alloc->wMinW        = 0;
            alloc->wMinH        = 0;

            LSPString s;
            ws::font_parameters_t fp;
            ws::text_parameters_t tp;
            sFont.get_parameters(pDisplay, scaling, &fp);

            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                // Skip invisible items
                ListBoxItem *li = vItems.uget(i);
                if ((li == NULL) || (!li->visibility()->get()))
                    continue;

                // Add item to list
                item_t *ai      = v->add();
                if  (!ai)
                    return;
                ai->item        = li;

                // Obtain the text of item and it's parameters
                s.clear();
                li->text()->format(&s);
                sFont.get_text_parameters(pDisplay, &tp, scaling, &s);

                // Estimate size
                ai->a.nLeft     = 0;
                ai->a.nTop      = 0;
                ai->a.nWidth    = tp.Width;
                ai->a.nHeight   = lsp_max(tp.Height, fp.Height);

                ai->r.nLeft     = 0;
                ai->r.nTop      = 0;
                ai->r.nWidth    = 0;
                ai->r.nHeight   = 0;

                // Apply padding
                li->padding()->add(&ai->a, scaling);

                // Update overall width and height
                alloc->wMinW    = lsp_max(alloc->wMinW, ai->a.nWidth);
                alloc->wMinH   += ai->a.nHeight + spacing;
            }
        }

        void ListBox::size_request(ws::size_limit_t *r)
        {
            alloc_t a;
            ws::rectangle_t xr;

            allocate_items(&a);

            xr.nLeft    = 0;
            xr.nTop     = 0;
            xr.nWidth   = -1;
            xr.nHeight  = -1;

            estimate_size(&a, &xr);

            *r          = a.sSize;
        }

        void ListBox::estimate_size(alloc_t *a, const ws::rectangle_t *xr)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());
            scrolling_t hscroll = sHScrollMode.get();
            scrolling_t vscroll = sVScrollMode.get();
            ssize_t border      = (sBorderSize.get() > 0) ? lsp_max(1.0f, sBorderSize.get() * scaling) : 0;
            if (border > 0)
                border             += (sBorderGap.get() > 0) ? lsp_max(0.0f, sBorderGap.get() * scaling) : 0;
            ssize_t radius      = lsp_max(0.0f, sBorderRadius.get() * scaling);
            size_t rgap         = radius - lsp_max(0.0f, truncf(M_SQRT1_2 * (radius - border)));

            // Estimate size of each scroll bar
            ws::size_limit_t hb, vb;
            sHBar.get_padded_size_limits(&hb);
            sVBar.get_padded_size_limits(&vb);

            hb.nMinWidth    = lsp_max(rgap * 2, hb.nMinWidth);
            hb.nMinHeight   = lsp_max(rgap * 2, hb.nMinHeight);
            vb.nMinWidth    = lsp_max(rgap * 2, vb.nMinWidth);
            vb.nMinHeight   = lsp_max(rgap * 2, vb.nMinHeight);

            a->sArea        = *xr;

            a->bHBar        = false;
            a->bVBar        = false;

            ssize_t minw    = (sHScrollMode.clip()) ? 0 : a->wMinW;
            ssize_t minh    = (sVScrollMode.clip()) ? 0 : a->wMinH;

            if ((hscroll == SCROLL_ALWAYS) || (hscroll == SCROLL_OPTIONAL))
            {
                if ((vscroll == SCROLL_ALWAYS) || (vscroll == SCROLL_OPTIONAL))
                {
                    a->sSize.nMinWidth  = hb.nMinWidth  + vb.nMinWidth;
                    a->sSize.nMinHeight = hb.nMinHeight + vb.nMinHeight;
                }
                else
                {
                    a->sSize.nMinWidth  = hb.nMinWidth;
                    a->sSize.nMinHeight = hb.nMinHeight + minh;
                }
            }
            else if ((vscroll == SCROLL_ALWAYS) || (vscroll == SCROLL_OPTIONAL))
            {
                a->sSize.nMinWidth  = vb.nMinWidth  + minw;
                a->sSize.nMinHeight = vb.nMinHeight;
            }
            else
            {
                a->sSize.nMinWidth  = minw;
                a->sSize.nMinHeight = minh;
            }

            a->sSize.nMaxWidth  = (sAllocation.hembed()) ? lsp_max(minw, a->sSize.nMinWidth) : -1;
            a->sSize.nMaxHeight = (sAllocation.vembed()) ? lsp_max(minh, a->sSize.nMinHeight) : -1;

            // Apply size constraints
            sSizeConstraints.apply(&a->sSize, scaling);

            if ((xr->nWidth < 0) || (xr->nHeight < 0))
                return;

            a->sArea            = *xr;
            a->sHBar.nLeft      = xr->nLeft;
            a->sHBar.nTop       = xr->nTop  + xr->nHeight - hb.nMinHeight;
            a->sHBar.nWidth     = xr->nWidth;
            a->sHBar.nHeight    = hb.nMinHeight;
            a->sVBar.nLeft      = xr->nLeft + xr->nWidth  - vb.nMinWidth;
            a->sVBar.nTop       = xr->nTop;
            a->sVBar.nWidth     = vb.nMinWidth;
            a->sVBar.nHeight    = xr->nHeight;

            if ((hscroll == SCROLL_ALWAYS) || ((hscroll == SCROLL_OPTIONAL) && (xr->nWidth < minw)))
            {
                a->bHBar            = true;
                a->sArea.nHeight   -= hb.nMinHeight;

                if ((vscroll == SCROLL_ALWAYS) || ((vscroll == SCROLL_OPTIONAL) && (xr->nHeight < minh)))
                {
                    a->bVBar            = true;
                    a->sArea.nWidth    -= vb.nMinWidth;

                    a->sHBar.nWidth    -= vb.nMinWidth;
                    a->sVBar.nHeight   -= hb.nMinHeight;
                }
            }
            else if ((vscroll == SCROLL_ALWAYS) || ((vscroll == SCROLL_OPTIONAL) && (xr->nHeight < minh)))
            {
                a->bVBar            = true;
                a->sArea.nWidth    -= vb.nMinWidth;
            }

            a->sList.nLeft      = a->sArea.nLeft    + rgap;
            a->sList.nTop       = a->sArea.nTop     + rgap;
            a->sList.nWidth     = a->sArea.nWidth   - rgap * 2;
            a->sList.nHeight    = a->sArea.nHeight  - rgap * 2;
        }

        void ListBox::realize(const ws::rectangle_t *r)
        {
            alloc_t a;
            estimate_size(&a, r);

            // Tune scroll bars
            sHBar.visibility()->set(a.bHBar);
            sVBar.visibility()->set(a.bVBar);

            if (a.bHBar)
            {
                sHBar.realize_widget(&a.sHBar);
                sHScroll.set_range(0, lsp_max(0, a.wMinW - a.sArea.nWidth));
                sHBar.value()->set_range(sHScroll.min(), sHScroll.max());
            }
            if (a.bVBar)
            {
                sVBar.realize_widget(&a.sVBar);
                sVScroll.set_range(0, lsp_max(0, a.wMinH - a.sArea.nHeight));
                sVBar.value()->set_range(sVScroll.min(), sVScroll.max());
            }

            // Realize child widget if present
            sArea   = a.sArea;
            sList   = a.sList;
            vVisible.swap(&a.vItems);

            // Call parent for realize
            WidgetContainer::realize(r);
        }

        void ListBox::realize_children(const ws::rectangle_t *r)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t spacing     = lsp_max(0.0f, scaling * sSpacing.get());
            ssize_t max_w       = sList.nWidth;

            // Estimate maximum width
            for (size_t i=0, n=vVisible.size(); i<n; ++i)
            {
                item_t *it  = vVisible.uget(i);
                max_w       = lsp_max(max_w, it->a.nWidth);
            }

            // Realize widgets
            ws::rectangle_t xr  = *r;

            for (size_t i=0, n=vVisible.size(); i<n; ++i)
            {
                item_t *it  = vVisible.uget(i);

                it->r.nWidth        = max_w;
                it->r.nHeight       = it->a.nHeight;
                it->r.nLeft         = xr.nLeft;
                it->r.nTop          = xr.nTop + (spacing >> 1);

                it->item->realize_widget(&it->r);

                // Update position
                xr.nTop            += it->a.nHeight + spacing;
            }

            // Mark for redraw
            query_draw();
        }

        void ListBox::keep_single_selection()
        {
            lltl::parray<ListBoxItem> si;
            if (!vSelected.values(&si))
                return;

            // Remove all items except last one
            for (ssize_t i=0, n=si.size()-1; i<n; ++i)
            {
                ListBoxItem *li = si.uget(i);
                if (li != NULL)
                    vSelected.remove(li);
            }
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

            ws::rectangle_t xr = _this->sList;
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
