/*
 * Menu.cpp
 *
 *  Created on: 18 сент. 2017 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t Menu::metadata      = { "Menu", &WidgetContainer::metadata };

        //-----------------------------------------------------------------------------
        // Menu implementation
        Menu::Menu(Display *dpy):
            WidgetContainer(dpy),
            sWindow(dpy),
            sFont(&sProperties),
            sHSpacing(&sProperties),
            sVSpacing(&sProperties),
            sScrolling(&sProperties),
            sBorder(&sProperties),
            sBorderColor(&sProperties),
            sCheckSize(&sProperties),
            sCheckBorder(&sProperties),
            sCheckBorderGap(&sProperties),
            sCheckBorderRadius(&sProperties)
        {
            pSubmenu    = NULL;
// TODO
//            pWindow     = NULL;
//            pParentMenu = NULL;
//            pActiveMenu = NULL;
//            nPopupLeft  = -1;
//            nPopupTop   = -1;
//            nSelected   = SEL_NONE;
//            nScroll     = 0;
//            nScrollMax  = 0;
//            nBorder     = 1;
//            nSpacing    = 6;
//            nMBState    = 0;
//
//            sPadding.set(16, 16, 0, 0);
//
//            nFlags     &= ~F_VISIBLE;
//
//            sScroll.bind(pDisplay);
//            sScroll.set_handler(timer_handler, this);

            pClass      = &metadata;
        }
        
        Menu::~Menu()
        {
            do_destroy();
        }

        status_t Menu::init()
        {
            // Initialize parent widget
            status_t result = Widget::init();
            if (result != STATUS_OK)
                return result;

            // Initialize window
            result = sWindow.init();
            if (result != STATUS_OK)
            {
                sWindow.destroy();
                return result;
            }
            sWindow.add_arrangement(A_RIGHT, 0.0f, false);
            sWindow.add_arrangement(A_LEFT, 0.0f, false);

            // Bind properties
            sFont.bind("font", &sStyle);
            sHSpacing.bind("hspacing", &sStyle);
            sVSpacing.bind("vspacing", &sStyle);
            sScrolling.bind("scrolling", &sStyle);
            sBorder.bind("border", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sCheckSize.bind("check.size", &sStyle);
            sCheckBorder.bind("check.border", &sStyle);
            sCheckBorderGap.bind("check.border.gap", &sStyle);
            sCheckBorderRadius.bind("check.border.radius", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sFont.init(sclass);
                sHSpacing.init(sclass, 2);
                sVSpacing.init(sclass, 0);
                sScrolling.init(sclass, 0);
                sBorder.init(sclass, 0);
                sBorderColor.init(sclass, "#000000");
                sCheckSize.init(sclass, 12);
                sCheckBorder.init(sclass, 1);
                sCheckBorderGap.init(sclass, 1);
                sCheckBorderRadius.init(sclass, 4);

                // Overrides
                sVisibility.override(sclass, false);
                sBgColor.override(sclass, "#cccccc");
                sPadding.override(sclass, 16, 16, 0, 0);
            }

            return STATUS_OK;
        }

        void Menu::destroy()
        {
            do_destroy();
            WidgetContainer::destroy();
        }

        void Menu::do_destroy()
        {
            size_t n            = vItems.size();
            for (size_t i=0; i<n; ++i)
            {
                MenuItem *item   = vItems.uget(i);
                if (item == NULL)
                    continue;

                unlink_widget(item);
            }

            vItems.flush();
            sWindow.destroy();
        }

        void Menu::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);

            if (sFont.is(prop))
                query_resize();
            if (sHSpacing.is(prop))
                query_resize();
            if (sVSpacing.is(prop))
                query_resize();
            if (sScrolling.is(prop))
                query_draw();
            if (sBorder.is(prop))
                query_resize();
            if (sBorderColor.is(prop))
                query_draw();
            if (sBorderColor.is(prop))
                query_draw();
            if (sCheckSize.is(prop))
                query_resize();
            if (sCheckBorder.is(prop))
                query_resize();
            if (sCheckBorderGap.is(prop))
                query_resize();
            if (sCheckBorderRadius.is(prop))
                query_resize();
        }

        void Menu::size_request(ws::size_limit_t *r)
        {
            float scaling   = lsp_max(0.0f, sScaling.get());
            isizes_t sz;
            estimate_sizes(&sz);

            // Compute minimum and maximum size
            ssize_t spaces  = lsp_max(0, ssize_t(sz.items + sz.separators) - 1);
            r->nMinWidth    = sz.width;
            r->nMinHeight   = lsp_max(sz.m_height, sz.s_height);
            r->nMaxWidth    = sz.width;
            r->nMaxHeight   = sz.m_height * sz.items + sz.s_height * sz.separators + spaces * sz.vspacing;

            // Add padding
            sPadding.add(r, scaling);
        }

        void Menu::estimate_sizes(isizes_t *sz)
        {
            float scaling   = lsp_max(0.0f, sScaling.get());
            sz->hspacing    = lsp_max(0.0f, sHSpacing.get() * scaling);
            sz->vspacing    = lsp_max(0.0f, sVSpacing.get() * scaling);
            sz->items       = 0;
            sz->separators  = 0;
            sz->ckbox       = false;
            sz->shortcut    = false;
            sz->submenu     = false;

            // Lookup for checkbox
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                MenuItem *mi = vItems.get(i);
                if ((mi == NULL) || (!mi->visibility()->get()))
                    continue;

                switch (mi->type()->get())
                {
                    case MI_NORMAL:
                        if (mi->shortcut()->valid())
                            sz->shortcut    = true;
                        if (mi->menu()->is_set())
                            sz->submenu     = true;
                        ++sz->items;
                        break;
                    case MI_CHECK:
                        sz->ckbox       = true;
                        if (mi->shortcut()->valid())
                            sz->shortcut    = true;
                        if (mi->menu()->is_set())
                            sz->submenu     = true;
                        ++sz->items;
                        break;
                    case MI_RADIO:
                        sz->ckbox       = true;
                        if (mi->shortcut()->valid())
                            sz->shortcut    = true;
                        if (mi->menu()->is_set())
                            sz->submenu     = true;
                        ++sz->items;
                        break;
                    case MI_SEPARATOR:
                        ++sz->separators;
                        break;
                }
            }

            // Size of checkbox/radio
            ssize_t br      = lsp_max(0, sCheckBorderRadius.get() * scaling) * 3;

            sz->cb_width    = (sCheckBorder.get() > 0) ? lsp_min(1.0f, sCheckBorder.get() * scaling) : 0;
            if (sz->cb_width > 0)
                sz->cb_height  += (sCheckBorderGap.get() > 0) ? lsp_min(1.0f, sCheckBorderGap.get() * scaling) : 0;
            sz->cb_width   += lsp_min(2.0f, sCheckSize.get() * scaling);
            sz->cb_width    = lsp_max(sz->cb_width, br);
            sz->cb_height   = sz->cb_width;

            // Size of text
            ws::font_parameters_t fp;
            sFont.get_parameters(pDisplay, scaling, &fp);

            sz->mc_width    = 0;
            sz->mc_height   = lsp_max(0, ceil(fp.Height));
            sz->sc_width    = 0;
            sz->sc_height   = sz->mc_height;

            LSPString *text, s;
            ws::text_parameters_t tp;
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                MenuItem *mi    = vItems.get(i);
                if ((mi == NULL) || (!mi->visibility()->get()))
                    continue;
                if (mi->type()->separator())
                    continue;

                // Estimate size of caption
                text            = mi->sText.formatted();
                sFont.get_text_parameters(pDisplay, &tp, scaling, text);
                sz->mc_width    = lsp_max(sz->mc_width, tp.Width);
                sz->mc_height   = lsp_max(sz->mc_height, tp.Height);

                // Estimate size of shortcut
                if (mi->menu()->is_set())
                {
                    mi->sShortcut.format(&s);
                    sFont.get_text_parameters(pDisplay, &tp, scaling, &s);
                    sz->sc_width    = lsp_max(sz->sc_width, tp.Width);
                    sz->sc_height   = lsp_max(sz->sc_height, tp.Height);
                }
            }

            // Submenu reference size, separator size
            sz->mr_height   = lsp_max(sz->mc_height, sz->sc_height);
            sz->mr_width    = lsp_max(1.0f, ceil(sz->sc_height * M_SQRT1_2));
            sz->sp_width    = lsp_max(7, 7 * scaling);
            sz->sp_height   = lsp_max(3, 3 * scaling);

            // Estimate minimum size
            sz->width       = sz->mc_width;
            sz->m_height    = sz->mc_height;

            if (sz->ckbox)
            {
                sz->width      += sz->cb_width + sz->hspacing;
                sz->m_height    = lsp_max(sz->m_height, sz->cb_width);
            }
            if (sz->shortcut)
            {
                sz->width      += sz->sc_width + sz->hspacing;
                sz->m_height    = lsp_max(sz->m_height, sz->sc_width);
            }
            if (sz->submenu)
            {
                sz->width      += sz->mr_width + sz->hspacing;
                sz->m_height    = lsp_max(sz->m_height, sz->mr_width);
            }

            sz->s_height    = sz->sp_height;
        }

        void Menu::realize(const ws::rectangle_t *r)
        {
            WidgetContainer::realize(r);

//            size_request_t sr;
//            size_request(&sr);
//
//            nScrollMax      = sr.nMinHeight - r->nHeight;
//            set_scroll(nScroll);
////            lsp_trace("scroll_max = %d, scroll = %d", int(nScrollMax), int(nScroll));
//
//            query_draw();
//            if (pWindow != NULL)
//                pWindow->query_draw();
        }

//        LSPWidget *Menu::find_widget(ssize_t x, ssize_t y)
//        {
//            size_t items = vItems.size();
//            for (size_t i=0; i<items; ++i)
//            {
//                MenuItem *w = vItems.at(i);
//                if ((w == NULL) || (w->hidden()))
//                    continue;
//                if (w->inside(x, y))
//                    return w;
//            }
//
//            return NULL;
//        }

//        void Menu::query_resize()
//        {
//            WidgetContainer::query_resize();
////            if (pWindow != NULL)
////                pWindow->query_resize();
//        }

        status_t Menu::add(Widget *child)
        {
            if (child == NULL)
                return STATUS_BAD_ARGUMENTS;

            MenuItem *item = widget_cast<MenuItem>(child);
            if (child == NULL)
                return STATUS_BAD_TYPE;

            if (!vItems.add(item))
                return STATUS_NO_MEM;

            item->set_parent(this);

            query_resize();
            return STATUS_SUCCESS;
        }

        status_t Menu::insert(Widget *child, size_t index)
        {
            if ((child == NULL) || (index > vItems.size()))
                return STATUS_BAD_ARGUMENTS;

            MenuItem *item = widget_cast<MenuItem>(child);
            if (child == NULL)
                return STATUS_BAD_TYPE;

            if (!vItems.insert(index, item))
                return STATUS_NO_MEM;

            item->set_parent(this);

            query_resize();
            return STATUS_SUCCESS;
        }

        status_t Menu::remove(Widget *child)
        {
            size_t n            = vItems.size();
            for (size_t i=0; i<n; ++i)
            {
                MenuItem *item   = vItems.uget(i);
                if (item == child)
                {
                    query_resize();
                    return (vItems.remove(i)) ? STATUS_OK : STATUS_UNKNOWN_ERR;
                }
            }

            return STATUS_NOT_FOUND;
        }

        Widget *Menu::get(size_t index)
        {
            return vItems.get(index);
        }

        void Menu::show()
        {
            size_t screen;
            ssize_t x, y;

            if (pDisplay->get_pointer_location(&screen, &x, &y) != STATUS_OK)
                return;

            sWindow.trigger_screen()->set(screen);
            sWindow.trigger_area()->set(x, y, 0, 0);
            sWindow.show();
        }

        void Menu::show(Widget *w)
        {
            if (w == NULL)
                return;

            ws::rectangle_t r;
            w->get_screen_rectangle(&r);
            sWindow.trigger_widget()->set(w);
            sWindow.trigger_area()->set(&r);
            sWindow.show();
        }

        void Menu::show(Widget *w, ssize_t x, ssize_t y)
        {
            sWindow.trigger_widget()->set(w);
            sWindow.trigger_area()->set(x, y, 0, 0);
            sWindow.show();
        }

        void Menu::show(Widget *w, ssize_t x, ssize_t y, ssize_t xw, ssize_t xh)
        {
            sWindow.trigger_widget()->set(w);
            sWindow.trigger_area()->set(x, y, xw, xh);
            sWindow.show();
        }

        void Menu::show(Widget *w, const ws::rectangle_t *r)
        {
            sWindow.trigger_widget()->set(w);
            sWindow.trigger_area()->set(r);
            sWindow.show();
        }

//        Menu *Menu::check_inside_submenu(ws_event_t *ev)
//        {
//            Menu *handler = NULL;
//            if ((pActiveMenu != NULL) &&
//                (pActiveMenu->pWindow != NULL) &&
//                (pActiveMenu->pWindow->visible()))
//            {
//                // Get window geometry
//                realize_t r1, r2;
//                pWindow->get_absolute_geometry(&r1);
//                pActiveMenu->pWindow->get_absolute_geometry(&r2);
//                lsp_trace("wnd=%p, active=%p, ev={%d, %d}, r1={%d, %d}, r2={%d, %d}",
//                        pWindow, pActiveMenu,
//                        int(ev->nLeft), int(ev->nTop),
//                        int(r1.nLeft), int(r1.nTop),
//                        int(r2.nLeft), int(r2.nTop)
//                    );
//
//                ws_event_t xev = *ev;
//                xev.nLeft   = r1.nLeft + ev->nLeft - r2.nLeft;
//                xev.nTop    = r1.nTop  + ev->nTop - r2.nTop;
//
//                if ((handler = pActiveMenu->check_inside_submenu(&xev)) != NULL)
//                {
//                    *ev         = xev;
//                    return handler;
//                }
//            }
//
//            if ((pWindow != NULL) &&
//                (pWindow->visible()))
//            {
//                lsp_trace("check ev {%d, %d} inside of wnd %p, {%d, %d, %d, %d} x { %d, %d }",
//                        int(ev->nLeft), int(ev->nTop),
//                        pWindow,
//                        int(pWindow->left()), int(pWindow->top()),
//                        int(pWindow->right()), int(pWindow->bottom()),
//                        int(pWindow->width()), int(pWindow->height())
//                        );
//
//                if ((ev->nLeft >= 0) &&
//                    (ev->nTop >= 0) &&
//                    (ev->nLeft < pWindow->width()) &&
//                    (ev->nTop < pWindow->height()))
//                    return this;
//            }
//
//            return NULL;
//        }

//        ssize_t Menu::find_item(ssize_t mx, ssize_t my, ssize_t *ry)
//        {
////            // Are we inside of submenu?
////            if (check_inside_submenu(mx, my))
////                return nSelected;
//
//            if ((mx < 0) || (mx >= sSize.nWidth))
//                return SEL_NONE;
//            if ((my < 0) || (my >= sSize.nHeight))
//                return SEL_NONE;
//
//            font_parameters_t fp;
//            sFont.get_parameters(&fp);
//
//            ssize_t separator = fp.Height * 0.5f + nSpacing;
//            fp.Height      += nSpacing;
//
//            if (nScrollMax > 0)
//            {
//                if ((nScroll > 0) && (my < ssize_t(nBorder + separator))) // Top button
//                    return SEL_TOP_SCROLL;
//                else if ((nScroll < nScrollMax) && (my > ssize_t(sSize.nHeight - nBorder - separator))) // Bottom button
//                    return SEL_BOTTOM_SCROLL;
//            }
//
//            // Iterate over all menu items
//            ssize_t y       = sPadding.top() + nBorder - nScroll;
//            size_t n        = vItems.size();
//
//            for (size_t i=0; i < n; ++i)
//            {
//                MenuItem *item = vItems.get(i);
//                if ((item == NULL) || (!item->visible()))
//                    continue;
//
//                if (item->is_separator())
//                    y += separator;
//                else
//                {
//                    if ((my >= y) && (my < (y + fp.Height)))
//                    {
//                        if (ry != NULL)
//                            *ry     = y;
//                        return i;
//                    }
//
//                    y += fp.Height;
//                }
//            }
//
//            return SEL_NONE;
//        }
//
//        void Menu::draw(ws::ISurface *s)
//        {
//            // Prepare palette
//            Color bg_color(sBgColor);
//            Color border(sBorderColor);
//            Color font(sFont.raw_color());
//            Color sel(sSelColor);
//            Color tmp;
//
//            border.scale_lightness(brightness());
//            font.scale_lightness(brightness());
//            sel.scale_lightness(brightness());
//
//            // Draw background
//            s->clear(bg_color);
//
//            font_parameters_t fp;
//            text_parameters_t tp;
//            sFont.get_parameters(s, &fp);
//
//            ssize_t separator = fp.Height * 0.5f + nSpacing;
//            ssize_t sep_len = sSize.nWidth - (nBorder + nSpacing) * 2;
//            ssize_t hspace  = nSpacing >> 1;
//
//            fp.Height      += nSpacing;
//            ssize_t y       = sPadding.top() + nBorder - nScroll;
//            ssize_t x       = sPadding.left() + nBorder;
//            size_t n        = vItems.size();
//
//            LSPString text;
//
//            for (size_t i=0; i < n; ++i)
//            {
//                MenuItem *item = vItems.get(i);
//                if ((item == NULL) || (!item->visible()))
//                    continue;
//
////                lsp_trace("x,y = %d, %d", int(x), int(y));
//
//                if (y >= sSize.nHeight)
//                    break;
//
//                if (item->is_separator())
//                {
//                    if (y > (-separator))
//                    {
//                        if (sep_len > 0)
//                            s->fill_rect(x - sPadding.left() + nSpacing, y + (separator >> 1), sep_len, 1, border);
//                    }
//
//                    y += separator;
//                }
//                else
//                {
//                    if (y > (-fp.Height))
//                    {
//                        item->text()->format(&text);
//                        if (nSelected == ssize_t(i))
//                        {
//                            s->fill_rect(nBorder, y, sSize.nWidth - nBorder*2, fp.Height, sel);
//                            tmp.copy(bg_color);
//                        }
//                        else
//                            tmp.copy(font);
//
//                        if (!text.is_empty())
//                            sFont.draw(s, x, y + fp.Ascent + hspace, tmp, &text);
//
//                        if (item->has_submenu())
//                        {
//                            sFont.get_text_parameters(s, &tp, "►");
//                            sFont.draw(s, sSize.nWidth - nBorder - nSpacing - tp.XAdvance - 2, y + fp.Ascent + hspace, tmp, "►");
//                        }
//                    }
//
//                    y += fp.Height;
//                }
//            }
//
//            if (nScrollMax > 0)
//            {
//                float cx = sSize.nWidth * 0.5f;
//                float aa = s->set_antialiasing(true);
//
//                // Top button
//                if (nScroll > 0)
//                {
//                    s->fill_rect(nBorder, nBorder, sSize.nWidth - nBorder * 2, separator, bg_color);
//                    if (nSelected == SEL_TOP_SCROLL)
//                    {
//                        tmp.copy(bg_color);
//                        s->fill_rect(nBorder + 1, nBorder + 1, sSize.nWidth - (nBorder + 1)* 2, separator - 1, border);
//                    }
//                    else
//                        tmp.copy(font);
//
//                    // Draw arrow up
//                    s->fill_triangle(
//                        cx, nBorder + 3,
//                        cx + separator, nBorder + separator - 2,
//                        cx - separator, nBorder + separator - 2,
//                        tmp);
//                }
//                else if (sPadding.top() > 0)
//                    s->fill_rect(nBorder, nBorder, sSize.nWidth - nBorder * 2, sPadding.top(), bg_color);
//
//                // Bottom button
//                if (nScroll < nScrollMax)
//                {
//                    s->fill_rect(nBorder, sSize.nHeight - nBorder - separator,
//                        sSize.nWidth - nBorder * 2, separator, bg_color);
//
//                    if (nSelected == SEL_BOTTOM_SCROLL)
//                    {
//                        tmp.copy(bg_color);
//                        s->fill_rect(nBorder + 1, sSize.nHeight - nBorder - separator,
//                            sSize.nWidth - (nBorder + 1) * 2, separator - 1, border);
//                    }
//                    else
//                        tmp.copy(font);
//
//                    // Draw arrow down
//                    s->fill_triangle(
//                        cx, sSize.nHeight - nBorder - 3,
//                        cx + separator, sSize.nHeight - nBorder - separator + 2,
//                        cx - separator, sSize.nHeight - nBorder - separator + 2,
//                        tmp);
//                }
//                else if (sPadding.bottom() > 0)
//                    s->fill_rect(nBorder, sSize.nHeight - nBorder - sPadding.bottom(),
//                        sSize.nWidth - nBorder * 2, sPadding.bottom(), bg_color);
//
//                // Restore anti-aliasing
//                s->set_antialiasing(aa);
//            }
//
//            if (nBorder > 0)
//                s->fill_frame(0, 0, sSize.nWidth, sSize.nHeight,
//                    nBorder, nBorder, sSize.nWidth - nBorder * 2, sSize.nHeight - nBorder * 2, border);
//        }
//
//        bool Menu::hide()
//        {
//            // Forget the parent menu
//            pParentMenu = NULL;
//
//            // Hide active submenu if present
//            if (pActiveMenu != NULL)
//            {
//                pActiveMenu->hide();
//                pActiveMenu = NULL;
//            }
//
//            // Hide window showing menu
//            if (pWindow != NULL)
//                pWindow->hide();
//
//            if (!is_visible())
//                return false;
//
//            return LSPWidgetContainer::hide();
//        }
//

        void Menu::show_widget()
        {
            // Call parent class for show
            WidgetContainer::show_widget();

            if (pParent != &sWindow)
            {
                sWindow.remove_all();
                sWindow.add(this);
            }
            sWindow.show();
        }

//        status_t Menu::on_mouse_down(const ws::event_t *e)
//        {
//            if (nMBState == 0)
//            {
//                if (!inside(e->nLeft, e->nTop))
//                {
//                    hide();
//                    return STATUS_OK;
//                }
//            }
//
//            nMBState |= (1 << e->nCode);
//            ssize_t iy = 0;
//            ssize_t sel = find_item(e->nLeft, e->nTop, &iy);
//            selection_changed(sel, iy);
//
//            return STATUS_OK;
//        }
//
//        status_t Menu::on_mouse_up(const ws::event_t *e)
//        {
//            if ((nMBState == (1 << ws::MCB_LEFT)) && (e->nCode == ws::MCB_LEFT))
//            {
//                Menu *parent = this;
//                while (parent->pParentMenu != NULL)
//                    parent  = parent->pParentMenu;
//
//                // Cleanup mouse button state flag
//                nMBState &= ~ (1 << e->nCode);
//
//                // Selection was found ?
//                MenuItem *item = NULL;
//                ssize_t iy = 0;
//                ssize_t sel = find_item(e->nLeft, e->nTop, &iy);
//
//                // Notify that selection has changed
//                selection_changed(sel, iy);
//
//                if (sel >= 0)
//                {
//                    item = vItems.get(sel);
//                    if (item == NULL)
//                        parent->hide();
//                    else if (item->hidden())
//                    {
//                        item    = NULL;
//                        parent->hide();
//                    }
//                    else if (!item->has_submenu())
//                        parent->hide();
//
//                    if (item != NULL)
//                    {
//                        ws_event_t ev = *e;
//                        item->slots()->execute(LSPSLOT_SUBMIT, item, &ev);
//                    }
//                }
//                else
//                {
//                    if ((sel != SEL_TOP_SCROLL) && (sel != SEL_BOTTOM_SCROLL))
//                        parent->hide();
//                }
//            }
//            else
//            {
//                // Cleanup mouse button state flag
//                nMBState &= ~ (1 << e->nCode);
//                if (nMBState == 0)
//                    hide();
//            }
//
//            return STATUS_OK;
//        }
//
//        status_t Menu::on_mouse_scroll(const ws::event_t *e)
//        {
//            ws::font_parameters_t fp;
//            sFont.get_parameters(&fp);
//            ssize_t amount = fp.Height + nSpacing;
//            if (amount < 1)
//                amount = 1;
//
//            ssize_t scroll = nScroll;
//            if (e->nCode == ws::MCD_UP)
//                set_scroll(nScroll - amount);
//            else if (e->nCode == ws::MCD_DOWN)
//                set_scroll(nScroll + amount);
//
//            if (scroll != nScroll)
//            {
//                ssize_t sel = nSelected, iy = 0;
//                nSelected   = find_item(e->nLeft, e->nTop, &iy);
//
//                if (sel != nSelected)
//                {
//                    // Notify that selection has changed
//                    selection_changed(nSelected, iy);
//
//                    // Query for draw
//                    query_draw();
//                    if (pWindow != NULL)
//                        pWindow->query_draw();
//                }
//            }
//
//            return STATUS_OK;
//        }
//
//        void Menu::selection_changed(ssize_t sel, ssize_t iy)
//        {
//            // Get menu item
//            MenuItem *item = (sel >= 0) ? vItems.get(sel) : NULL;
//            if ((item != NULL) && (pActiveMenu == item->submenu()))
//                return;
//
//            if (pActiveMenu != NULL)
//            {
//                pActiveMenu->hide();
//                pActiveMenu = NULL;
//            }
//
//            if (item == NULL)
//                return;
//
//            if ((pActiveMenu = item->submenu()) == NULL)
//                return;
//
//            // Get screen size
//            IDisplay *dpy = pDisplay->display();
//            ssize_t sw = 0, sh = 0;
//            dpy->screen_size(pWindow->screen(), &sw, &sh);
//
//            // Get window geometry
//            realize_t wr;
//            pWindow->get_geometry(&wr);
//            ssize_t xlast = wr.nLeft + wr.nWidth;
//
//            // Estimate active window size
//            size_request_t sr;
//            pActiveMenu->size_request(&sr);
//            if (sr.nMinWidth < 0)
//                sr.nMinWidth = 0;
//
//            if ((xlast + sr.nMinWidth) < sw)
//                pActiveMenu->show(this, xlast, iy + wr.nTop);
//            else
//                pActiveMenu->show(this, wr.nLeft - sr.nMinWidth, iy + wr.nTop);
//        }
//
//        status_t Menu::on_mouse_move(const ws::event_t *e)
//        {
//            lsp_trace("x=%d, y=%d", int(e->nLeft), int(e->nTop));
//            ssize_t sel = nSelected;
//            ssize_t iy  = 0;
//            nSelected   = find_item(e->nLeft, e->nTop, &iy);
//
//            if (sel != nSelected)
//            {
////                lsp_trace("selection changed. Was %d, now %d", int(sel), int(nSelected));
//                // Update timer status
//                if ((nSelected == SEL_TOP_SCROLL) || (nSelected == SEL_BOTTOM_SCROLL))
//                    sScroll.launch(0, 25);
//                else
//                {
//                    sScroll.cancel();
//                    selection_changed(nSelected, iy);
//                }
//
//                // Query for draw
//                query_draw();
//                if (pWindow != NULL)
//                    pWindow->query_draw();
//            }
//
//            return STATUS_OK;
//        }
//
//        status_t Menu::timer_handler(ws::timestamp_t time, void *arg)
//        {
//            Menu *_this = widget_ptrcast<Menu>(arg);
//            if (_this == NULL)
//                return STATUS_BAD_ARGUMENTS;
//            _this->update_scroll();
//            return STATUS_OK;
//        }
//
//        void Menu::update_scroll()
//        {
//            ws::font_parameters_t fp;
//            sFont.get_parameters(&fp);
//            ssize_t amount = fp.Height * 0.5f;
//            if (amount < 1)
//                amount = 1;
//
//            switch (nSelected)
//            {
//                case SEL_TOP_SCROLL:
//                    set_scroll(nScroll - amount);
//                    if (nScroll <= 0)
//                        sScroll.cancel();
//                    break;
//
//                case SEL_BOTTOM_SCROLL:
//                    set_scroll(nScroll + amount);
//                    if (nScroll >= nScrollMax)
//                        sScroll.cancel();
//                    break;
//
//                default:
//                    sScroll.cancel();
//                    break;
//            }
//        }
    
    } /* namespace tk */
} /* namespace lsp */
