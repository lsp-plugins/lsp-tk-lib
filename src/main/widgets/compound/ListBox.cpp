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
            sListBgColor(&sProperties),
            sSpacing(&sProperties),
            sMultiSelect(&sProperties),
            sHScrollSpacing(&sProperties),
            sVScrollSpacing(&sProperties)
        {
            nBMask          = 0;
            nXFlags         = 0;
            nCurrIndex      = -1;
            nLastIndex      = -1;
            nKeyScroll      = SCR_NONE;

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
            handler_id_t id;

            // Initialize widgets
            status_t result = WidgetContainer::init();
            if (result == STATUS_OK)
                result  = sHBar.init();
            if (result == STATUS_OK)
                result  = sVBar.init();
            if (result != STATUS_OK)
                return result;

            sIListener.bind_all(this, on_add_item, on_remove_item);
            sKeyTimer.bind(pDisplay);
            sKeyTimer.set_handler(key_scroll_handler, self());

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
            sListBgColor.bind("list.bg.color", &sStyle);
            sSpacing.bind("spacing", &sStyle);
            sMultiSelect.bind("selection.multiple", &sStyle);
            sHScrollSpacing.bind("hscroll.spacing", &sStyle);
            sVScrollSpacing.bind("vscroll.spacing", &sStyle);

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
                sListBgColor.init(sclass, "#ffffff");
                sSpacing.init(sclass, 0);
                sMultiSelect.init(sclass, false);
                sHScrollSpacing.init(sclass, 1);
                sVScrollSpacing.init(sclass, 1);
            }

            // Bind slots
            id = sSlots.add(SLOT_CHANGE, slot_on_change, self());
            if (id >= 0) id = sSlots.add(SLOT_SUBMIT, slot_on_submit, self());

            return (id >= 0) ? STATUS_OK : -id;
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
            if (sListBgColor.is(prop))
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

            lltl::darray<item_t> *v    = &alloc->vItems;

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
                ai->index       = i;

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
            ssize_t rgap        = radius - lsp_max(0.0f, truncf(M_SQRT1_2 * (radius - border)));
            ssize_t hsspacing   = lsp_max(0.0f, sHScrollSpacing.get() * scaling);
            ssize_t vsspacing   = lsp_max(0.0f, sVScrollSpacing.get() * scaling);

            // Estimate size of each scroll bar
            ws::size_limit_t hb, vb;
            sHBar.get_padded_size_limits(&hb);
            sVBar.get_padded_size_limits(&vb);

            hb.nMinWidth    = lsp_max(rgap * 2, hb.nMinWidth);
            hb.nMinHeight   = lsp_max(rgap * 2, hb.nMinHeight);
            vb.nMinWidth    = lsp_max(rgap * 2, vb.nMinWidth);
            vb.nMinHeight   = lsp_max(rgap * 2, vb.nMinHeight);

            a->bHBar        = false;
            a->bVBar        = false;

            ssize_t minw    = (sHScrollMode.clip()) ? 0 : a->wMinW + rgap * 2;
            ssize_t minh    = (sVScrollMode.clip()) ? 0 : a->wMinH + rgap * 2;

            if ((hscroll == SCROLL_ALWAYS) || (hscroll == SCROLL_OPTIONAL))
            {
                if ((vscroll == SCROLL_ALWAYS) || (vscroll == SCROLL_OPTIONAL))
                {
                    a->sSize.nMinWidth  = hb.nMinWidth  + vb.nMinWidth  + vsspacing;
                    a->sSize.nMinHeight = hb.nMinHeight + vb.nMinHeight + hsspacing;
                }
                else
                {
                    a->sSize.nMinWidth  = hb.nMinWidth;
                    a->sSize.nMinHeight = hb.nMinHeight + minh + hsspacing;
                }
            }
            else if ((vscroll == SCROLL_ALWAYS) || (vscroll == SCROLL_OPTIONAL))
            {
                a->sSize.nMinWidth  = vb.nMinWidth  + minw + vsspacing;
                a->sSize.nMinHeight = vb.nMinHeight;
            }
            else
            {
                a->sSize.nMinWidth  = minw;
                a->sSize.nMinHeight = minh;
            }

            a->sSize.nMaxWidth  = -1;
            a->sSize.nMaxHeight = -1;
            a->sSize.nPreWidth  = lsp_max(minw, a->sSize.nMinWidth);
            a->sSize.nPreHeight = lsp_max(minh, a->sSize.nMinHeight);

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
                a->sArea.nHeight   -= hb.nMinHeight + hsspacing;

                if ((vscroll == SCROLL_ALWAYS) || ((vscroll == SCROLL_OPTIONAL) && (xr->nHeight < minh)))
                {
                    a->bVBar            = true;
                    a->sArea.nWidth    -= vb.nMinWidth + vsspacing;

                    a->sHBar.nWidth    -= vb.nMinWidth;
                    a->sVBar.nHeight   -= hb.nMinHeight;
                }
            }
            else if ((vscroll == SCROLL_ALWAYS) || ((vscroll == SCROLL_OPTIONAL) && (xr->nHeight < minh)))
            {
                a->bVBar            = true;
                a->sArea.nWidth    -= vb.nMinWidth + vsspacing;
            }

            a->sList.nLeft      = a->sArea.nLeft    + rgap;
            a->sList.nTop       = a->sArea.nTop     + rgap;
            a->sList.nWidth     = a->sArea.nWidth   - rgap * 2;
            a->sList.nHeight    = a->sArea.nHeight  - rgap * 2;
        }

        void ListBox::realize(const ws::rectangle_t *r)
        {
            alloc_t a;
            allocate_items(&a);
            estimate_size(&a, r);

            // Update internal state
            sArea   = a.sArea;
            sList   = a.sList;
            vVisible.swap(&a.vItems);

            // Tune scroll bars
            sHBar.visibility()->set(a.bHBar);
            sVBar.visibility()->set(a.bVBar);

            if (a.bHBar)
            {
                sHBar.realize_widget(&a.sHBar);
                sHScroll.set_range(0, lsp_max(0, a.wMinW - a.sList.nWidth));
                sHBar.value()->set_range(sHScroll.min(), sHScroll.max());
            }
            if (a.bVBar)
            {
                sVBar.realize_widget(&a.sVBar);
                sVScroll.set_range(0, lsp_max(0, a.wMinH - a.sList.nHeight));
                sVBar.value()->set_range(sVScroll.min(), sVScroll.max());
            }

            // Realize children
            realize_children();

            // Update scrolling
            item_t *curr    = find_by_index(nCurrIndex);
            ssize_t start   = vVisible.index_of(curr);
            if (start >= 0)
            {
                if (scroll_to_item(start))
                    realize_children();
            }

            // Call parent for realize
            WidgetContainer::realize(r);
        }

        void ListBox::realize_children()
        {
            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t spacing     = lsp_max(0.0f, scaling * sSpacing.get());
            ssize_t max_w       = sList.nWidth;

            ws::rectangle_t xr  = sList;
            if (sHBar.visibility()->get())
                xr.nLeft   -= sHBar.value()->get();
            if (sVBar.visibility()->get())
                xr.nTop    -= sVBar.value()->get();

            // Estimate maximum width
            for (size_t i=0, n=vVisible.size(); i<n; ++i)
            {
                item_t *it  = vVisible.uget(i);
                max_w       = lsp_max(max_w, it->a.nWidth);
            }

            // Realize widgets
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

        void ListBox::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            if (nFlags & REDRAW_SURFACE)
                force = true;

            bool aa;
            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t border      = (sBorderSize.get() > 0) ? lsp_max(1.0f, sBorderSize.get() * scaling) : 0;
            ssize_t radius      = lsp_max(0.0f, sBorderRadius.get() * scaling);
            ssize_t hsspacing   = lsp_max(0.0f, sHScrollSpacing.get() * scaling);
            ssize_t vsspacing   = lsp_max(0.0f, sVScrollSpacing.get() * scaling);
            lsp::Color col(sBgColor);

            ws::rectangle_t h, v, xa, xr;
            xa  = sSize;

            // Render scroll bars
            if (sHBar.visibility()->get())
            {
                sHBar.get_padded_rectangle(&h);
                xa.nHeight  -= h.nHeight;
                if ((sHBar.redraw_pending()) || (force))
                {
                    sHBar.render(s, area, force);
                    sHBar.commit_redraw();
                }

                if (sVBar.visibility()->get())
                {
                    sVBar.get_padded_rectangle(&v);
                    xa.nWidth   -= v.nWidth;
                    if ((sVBar.redraw_pending()) || (force))
                    {
                        sVBar.render(s, area, force);
                        sVBar.commit_redraw();
                    }

                    // Draw the padding
                    if (force)
                    {
                        s->clip_begin(area);
                        s->fill_rect(col, h.nLeft + h.nWidth, v.nTop + v.nHeight, v.nWidth, h.nHeight);
                        s->fill_rect(col, v.nLeft - vsspacing, v.nTop, vsspacing, v.nHeight + hsspacing);
                        s->fill_rect(col, h.nLeft, h.nTop - hsspacing, h.nWidth, hsspacing);
                        s->clip_end();
                    }
                }
                else
                {
                    // Draw the padding
                    if (force)
                    {
                        s->clip_begin(area);
                        s->fill_rect(col, h.nLeft + h.nWidth, v.nTop + v.nHeight, v.nWidth, h.nHeight);
                        s->fill_rect(col, h.nLeft, h.nTop - hsspacing, h.nWidth, hsspacing);
                        s->clip_end();
                    }
                }
            }
            else if (sVBar.visibility()->get())
            {
                sVBar.get_padded_rectangle(&v);
                xa.nWidth   -= v.nWidth;

                if ((sVBar.redraw_pending()) || (force))
                {
                    sVBar.render(s, area, force);
                    sVBar.commit_redraw();
                }

                if (force)
                {
                    s->clip_begin(area);
                    s->fill_rect(col, v.nLeft - vsspacing, v.nTop, vsspacing, v.nHeight);
                    s->clip_end();
                }
            }

            // Draw the list contents
            if (force)
            {
                if (Size::intersection(&xa, &sArea, area))
                {
                    // Draw the frame around
                    s->clip_begin(area);
                        s->fill_frame(col, &sArea, &sList);

                        aa = s->set_antialiasing(true);
                            col.copy(sBorderColor);
                            xr = sArea;
                            s->fill_round_rect(col, SURFMASK_ALL_CORNER, radius, &xr);

                            col.copy(sListBgColor);
                            xr.nLeft       += border;
                            xr.nTop        += border;
                            xr.nWidth      -= border * 2;
                            xr.nHeight     -= border * 2;
                            s->fill_round_rect(col, SURFMASK_ALL_CORNER, radius, &xr);

                        s->set_antialiasing(aa);
                    s->clip_end();
                }

                if (Size::intersection(&xa, &sList, area))
                {
                    // Perform rendering of list
                    LSPString text;
                    ws::font_parameters_t fp;
                    ws::text_parameters_t tp;
                    sFont.get_parameters(pDisplay, scaling, &fp);

                    s->clip_begin(&xa);
                    for (size_t i=0, n=vVisible.size(); i<n; ++i)
                    {
                        item_t *it = vVisible.get(i);
                        if (it == NULL)
                            continue;
                        ListBoxItem *li = it->item;
                        if (li == NULL)
                            continue;

                        li->commit_redraw();
                        if (!Size::overlap(&xa, &it->r)) // Do not draw invisible items
                            continue;

                        text.clear();
                        li->text()->format(&text);
                        bool selected = vSelected.contains(li);
                        sFont.get_text_parameters(pDisplay, &tp, scaling, &text);

                        if (selected)
                        {
                            col.copy(li->bg_selected_color()->color());
                            s->fill_rect(col, &it->r);
                            col.copy(li->text_selected_color()->color());
                        }
                        else
                        {
                            col.copy(li->bg_color()->color());
                            s->fill_rect(col, &it->r);
                            col.copy(li->text_color()->color());
                        }

                        li->padding()->enter(&xr, &it->r, scaling);
                        sFont.draw(s, col,
                                xr.nLeft,
                                xr.nTop  + ((it->r.nHeight - fp.Height) * 0.5f) + fp.Ascent,
                                scaling, &text);
                    }
                    s->clip_end();
                }
            }
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

            _this->realize_children();
            _this->query_draw();

            return STATUS_OK;
        }

        status_t ListBox::slot_on_change(Widget *sender, void *ptr, void *data)
        {
            ListBox *_this = widget_ptrcast<ListBox>(ptr);
            return (_this != NULL) ? _this->on_change() : STATUS_BAD_ARGUMENTS;
        }

        status_t ListBox::slot_on_submit(Widget *sender, void *ptr, void *data)
        {
            ListBox *_this = widget_ptrcast<ListBox>(ptr);
            return (_this != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }

        void ListBox::on_add_item(void *obj, Property *prop, Widget *w)
        {
            ListBoxItem *item = widget_cast<ListBoxItem>(w);
            if (item == NULL)
                return;

            ListBox *_this = widget_ptrcast<ListBox>(obj);
            if (_this == NULL)
                return;

            item->set_parent(_this);
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

            _this->unlink_widget(item);
        }

        status_t ListBox::on_mouse_down(const ws::event_t *e)
        {
            if (nBMask == 0)
            {
                nXFlags        &= nXFlags &= ~(F_SUBMIT | F_CHANGED);
                if (e->nCode == ws::MCB_LEFT)
                {
                    nLastIndex      = nCurrIndex;
                    nXFlags         = lsp_setflag(nXFlags, F_SEL_ACTIVE, Position::inside(&sList, e->nLeft, e->nTop));
                }
            }
            nBMask         |= 1 << e->nCode;
            nXFlags         = lsp_setflag(nXFlags, F_SUBMIT, nBMask == ws::MCF_LEFT);

            ws::event_t xe  = *e;
            xe.nType        = ws::UIE_MOUSE_MOVE;

            return on_mouse_move(&xe);
        }

        status_t ListBox::on_mouse_up(const ws::event_t *e)
        {
            nBMask     &= ~(1 << e->nCode);
            if (nBMask == 0)
            {
                if ((nXFlags & (F_SUBMIT | F_CHANGED)) == (F_SUBMIT | F_CHANGED))
                {
                    nXFlags &= ~(F_SUBMIT | F_CHANGED);
                    sSlots.execute(SLOT_SUBMIT, this, NULL);
                }
            }
            return STATUS_OK;
        }

        status_t ListBox::on_mouse_move(const ws::event_t *e)
        {
            if (nBMask != ws::MCF_LEFT)
                return STATUS_OK;

            item_t *it  = find_item(e->nLeft, e->nTop);
            if (it != NULL)
            {
                nCurrIndex      = it->index;
                if (e->nState & ws::MCF_SHIFT)
                    select_range(nLastIndex, nCurrIndex, e->nState & ws::MCF_CONTROL);
                else
                    select_single(nCurrIndex, e->nState & ws::MCF_CONTROL);
            }

            return STATUS_OK;
        }

        status_t ListBox::on_mouse_scroll(const ws::event_t *e)
        {
            ws::event_t xe = *e; // Copy the event

            switch (xe.nCode)
            {
                case ws::MCD_UP:
                case ws::MCD_DOWN:
                    if (xe.nState & ws::MCF_SHIFT)
                    {
                        if (!sHBar.visibility()->get())
                            break;
                        xe.nState &= ~ws::MCF_SHIFT; // Shift may be modifier, reset this flag
                        sHBar.handle_event(e);
                    }
                    else
                    {
                        if (!sVBar.visibility()->get())
                            break;
                        sVBar.handle_event(e);
                    }
                    break;

                case ws::MCD_LEFT:
                case ws::MCD_RIGHT:
                    if (xe.nState & ws::MCF_SHIFT)
                    {
                        if (!sVBar.visibility()->get())
                            break;
                        xe.nState &= ~ws::MCF_SHIFT; // Shift may be modifier, reset this flag
                        sVBar.handle_event(e);
                    }
                    else
                    {
                        if (!sHBar.visibility()->get())
                            break;
                        sHBar.handle_event(e);
                    }
                    break;

                default:
                    break;
            }


            return STATUS_OK;
        }

        ListBox::item_t *ListBox::find_item(ssize_t x, ssize_t y)
        {
            if (vVisible.is_empty())
                return NULL;

            // Since all items have sorted order from top to bottom, perform binary search
            ssize_t first = 0, last = vVisible.size() - 1, middle;
            item_t *it;

            while (first <= last)
            {
                middle          = (first + last) >> 1;
                it              = vVisible.uget(middle);

                if (y < it->r.nTop)
                    last  = middle - 1;
                else if (y >= (it->r.nTop + it->r.nHeight))
                    first = middle + 1;
                else
                {
                    first       = middle;
                    break;
                }
            }

            it  = vVisible.uget(first);
            return (Position::inside(&it->r, x, y)) ? it : NULL;
        }

        ListBox::item_t *ListBox::find_by_index(ssize_t index)
        {
            if (vVisible.is_empty())
                return NULL;

            // Since all items have sorted order from top to bottom, perform binary search
            ssize_t first = 0, last = vVisible.size() - 1, middle;
            item_t *it;

            if ((index < first) || (index > last))
                return NULL;

            while (first <= last)
            {
                middle          = (first + last) >> 1;
                it              = vVisible.uget(middle);

                if (index < ssize_t(it->index))
                    last  = middle - 1;
                else if (index > ssize_t(it->index))
                    first = middle + 1;
                else
                {
                    first       = middle;
                    break;
                }
            }

            return vVisible.get(first);
        }

        void ListBox::select_range(ssize_t first, ssize_t last, bool add)
        {
            if (!sMultiSelect.get())
            {
                select_single(last, add);
                return;
            }

            bool changed = false;

            if (!add)
            {
                vSelected.clear();
                changed = true;
            }

            if (last < first)
                swap(first, last);

            for (; first <= last; ++first)
            {
                ListBoxItem *li = vItems.get(first);
                if ((li == NULL) || (!li->visibility()->get()))
                    continue;

                vSelected.add(li);
                changed = true;
            }

            // Execute change
            if (changed)
            {
                nXFlags  |= F_CHANGED;
                sSlots.execute(SLOT_CHANGE, this, NULL);
            }
        }

        void ListBox::select_single(ssize_t index, bool add)
        {
            bool changed = false;
            if ((!add) || (!sMultiSelect.get()))
            {
                vSelected.clear();
                changed = true;
            }
            ListBoxItem *it = vItems.get(index);
            if (it != NULL)
            {
                vSelected.toggle(it);
                changed = true;
            }

            // Execute change
            if (changed)
            {
                nXFlags  |= F_CHANGED;
                sSlots.execute(SLOT_CHANGE, this, NULL);
            }
        }

        status_t ListBox::on_key_down(const ws::event_t *e)
        {
            size_t scroll   = nKeyScroll;

            nKeyScroll  = lsp_setflag(nKeyScroll, SCR_SHIFT, e->nState & ws::MCF_SHIFT);
            nKeyScroll  = lsp_setflag(nKeyScroll, SCR_CTRL, e->nState & ws::MCF_CONTROL);

            switch (e->nCode)
            {
                case ws::WSK_KEYPAD_UP:         nKeyScroll |= SCR_KP_UP;        break;
                case ws::WSK_KEYPAD_PAGE_UP:    nKeyScroll |= SCR_KP_PGUP;      break;
                case ws::WSK_KEYPAD_DOWN:       nKeyScroll |= SCR_KP_DOWN;      break;
                case ws::WSK_KEYPAD_PAGE_DOWN:  nKeyScroll |= SCR_KP_PGDOWN;    break;
                case ws::WSK_KEYPAD_LEFT:       nKeyScroll |= SCR_KP_LEFT;      break;
                case ws::WSK_KEYPAD_RIGHT:      nKeyScroll |= SCR_KP_RIGHT;     break;

                case ws::WSK_UP:                nKeyScroll |= SCR_UP;           break;
                case ws::WSK_PAGE_UP:           nKeyScroll |= SCR_PGUP;         break;
                case ws::WSK_DOWN:              nKeyScroll |= SCR_DOWN;         break;
                case ws::WSK_PAGE_DOWN:         nKeyScroll |= SCR_PGDOWN;       break;
                case ws::WSK_LEFT:              nKeyScroll |= SCR_LEFT;         break;
                case ws::WSK_RIGHT:             nKeyScroll |= SCR_RIGHT;        break;
                    break;

                case ws::WSK_HOME:
                case ws::WSK_KEYPAD_HOME:
                {
                    item_t *it  = vVisible.first();
                    if (it != NULL)
                    {
                        nCurrIndex  = it->index;
                        select_single(nCurrIndex, false);
                        scroll_to_item(vVisible.index_of(it));
                    }
                    break;
                }

                case ws::WSK_END:
                case ws::WSK_KEYPAD_END:
                {
                    item_t *it  = vVisible.last();
                    if (it != NULL)
                    {
                        nCurrIndex  = it->index;
                        select_single(nCurrIndex, false);
                        scroll_to_item(vVisible.index_of(it));
                    }
                    break;
                }

                case ws::WSK_KEYPAD_ENTER:
                case ws::WSK_RETURN:
                    select_single(nCurrIndex, false);
                    sSlots.execute(SLOT_SUBMIT, this, NULL);
                    break;

                default:
                    break;
            }

            if ((scroll ^ nKeyScroll) & SCR_KEYMASK)
            {
                on_key_scroll();
                if (scroll == 0)
                    sKeyTimer.launch(-1, 250, 1000);
            }

            return STATUS_OK;
        }

        status_t ListBox::on_key_up(const ws::event_t *e)
        {
            nKeyScroll  = lsp_setflag(nKeyScroll, SCR_SHIFT, e->nState & ws::MCF_SHIFT);
            nKeyScroll  = lsp_setflag(nKeyScroll, SCR_CTRL, e->nState & ws::MCF_CONTROL);

            switch (e->nCode)
            {
                case ws::WSK_KEYPAD_UP:         nKeyScroll &= ~SCR_KP_UP;       break;
                case ws::WSK_KEYPAD_PAGE_UP:    nKeyScroll &= ~SCR_KP_PGUP;     break;
                case ws::WSK_KEYPAD_DOWN:       nKeyScroll &= ~SCR_KP_DOWN;     break;
                case ws::WSK_KEYPAD_PAGE_DOWN:  nKeyScroll &= ~SCR_KP_PGDOWN;   break;
                case ws::WSK_KEYPAD_LEFT:       nKeyScroll &= ~SCR_KP_LEFT;     break;
                case ws::WSK_KEYPAD_RIGHT:      nKeyScroll &= ~SCR_KP_RIGHT;    break;

                case ws::WSK_UP:                nKeyScroll &= ~SCR_UP;          break;
                case ws::WSK_PAGE_UP:           nKeyScroll &= ~SCR_PGUP;        break;
                case ws::WSK_DOWN:              nKeyScroll &= ~SCR_DOWN;        break;
                case ws::WSK_PAGE_DOWN:         nKeyScroll &= ~SCR_PGDOWN;      break;
                case ws::WSK_LEFT:              nKeyScroll &= ~SCR_LEFT;        break;
                case ws::WSK_RIGHT:             nKeyScroll &= ~SCR_RIGHT;       break;

                default:
                    break;
            }

            if (!(nKeyScroll & SCR_KEYMASK))
                sKeyTimer.cancel();

            return STATUS_OK;
        }

        status_t ListBox::key_scroll_handler(ws::timestamp_t sched, ws::timestamp_t time, void *arg)
        {
            ListBox *_this = widget_ptrcast<ListBox>(arg);
            return (_this != NULL) ? _this->on_key_scroll() : STATUS_OK;
        }

        status_t ListBox::on_key_scroll()
        {
            size_t k1 = nKeyScroll & SCR_KEYMASK1;
            size_t k2 = nKeyScroll & SCR_KEYMASK2;
            size_t mask = k1 ^ (k2 >> 1); // UP key cancels DOWN etc
            if (mask == 0)
                return STATUS_OK;

            float scaling   = lsp_max(0.0f, sScaling.get());
            item_t *curr    = find_by_index(nCurrIndex);
            ssize_t start   = lsp_max(-1, vVisible.index_of(curr));
            ssize_t last    = vVisible.size() - 1;
            ssize_t vindex  = start;

            // Vertical scrolling
            if (mask & (SCR_PGUP | SCR_KP_PGUP))
            {
                ssize_t amount  = sList.nHeight - curr->r.nHeight;

                // Perform PG_UP and PG_DOWN scroll
                if (nKeyScroll & (SCR_PGUP | SCR_KP_PGUP))
                {
                    while (vindex > 0)
                    {
                        curr        = vVisible.get(--vindex);
                        amount     -= curr->r.nHeight;
                        if (amount <= 0)
                            break;
                    }
                }
                else
                {
                    while (vindex < last)
                    {
                        curr        = vVisible.get(++vindex);
                        amount     -= curr->r.nHeight;
                        if (amount <= 0)
                            break;
                    }
                }
            }
            else if ((mask & SCR_UP) | (mask & SCR_KP_UP))
            {
                if (nKeyScroll & (SCR_UP | SCR_KP_UP))
                {
                    if (vindex > 0)
                        --vindex;
                }
                else
                {
                    if (vindex < last)
                        ++vindex;
                }
            }

            if (vindex != start)
            {
                curr        = vVisible.uget(vindex);
                nCurrIndex  = curr->index;
                select_single(nCurrIndex, false);
                scroll_to_item(vindex);
            }

            // Horizontal scrolling
            if ((mask & (SCR_LEFT | SCR_KP_LEFT)) && (sHBar.visibility()->get()))
            {
                float amount    = sHBar.accel_step()->get(nKeyScroll & SCR_CTRL, nKeyScroll & SCR_SHIFT);
                amount          = lsp_max(1.0f, amount * scaling);
                if (nKeyScroll & (SCR_LEFT | SCR_KP_LEFT))
                    amount         = -amount;
                sHBar.value()->add(amount);
            }

            return STATUS_OK;
        }

        bool ListBox::scroll_to_item(ssize_t vindex)
        {
            if (!sVBar.visibility()->get())
                return false;

            item_t *curr    = vVisible.get(vindex);
            if (curr == NULL)
                return false;

            if (curr->r.nTop < sList.nTop)
                sVBar.value()->sub(sList.nTop - curr->r.nTop);
            else if ((curr->r.nTop + curr->r.nHeight) > (sList.nTop + sList.nHeight))
                sVBar.value()->add(curr->r.nTop + curr->r.nHeight - sList.nTop - sList.nHeight);
            else
                return false;

            return true;
        }

        status_t ListBox::on_change()
        {
            return STATUS_OK;
        }

        status_t ListBox::on_submit()
        {
            return STATUS_OK;
        }

    } /* namespace tk */
} /* namespace lsp */
