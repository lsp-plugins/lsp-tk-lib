/*
 * Menu.cpp
 *
 *  Created on: 18 сент. 2017 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t Menu::metadata      = { "Menu", &WidgetContainer::metadata };

        //-----------------------------------------------------------------------------
        // Menu::MenuWindow implementation
//        Menu::MenuWindow::MenuWindow(LSPDisplay *dpy, Menu *menu, size_t screen):
//            LSPWindow(dpy, NULL, screen)
//        {
//            pMenu       = menu;
//        }
//
//        Menu::MenuWindow::~MenuWindow()
//        {
//        }
//
//        LSPWidget *Menu::MenuWindow::find_widget(ssize_t x, ssize_t y)
//        {
//            return (pMenu != NULL) ? pMenu->find_widget(x, y) : NULL;
//        }
//
//        void Menu::MenuWindow::render(ISurface *s, bool force)
//        {
//            if (pMenu != NULL)
//                pMenu->render(s, force);
//            else
//                LSPWindow::render(s, force);
//        }
//
//        Menu *Menu::MenuWindow::get_handler(ws_event_t *e)
//        {
//            Menu *handler = (pMenu != NULL) ? pMenu->check_inside_submenu(e) : NULL;
//            if (handler == NULL)
//                handler = pMenu;
//            return handler;
//        }
//
//        status_t Menu::MenuWindow::on_mouse_down(const ws_event_t *e)
//        {
//            ws_event_t xev = *e;
//            Menu *handler = get_handler(&xev);
//            return (handler != NULL) ? handler->on_mouse_down(&xev) : LSPWindow::on_mouse_down(&xev);
//        }
//
//        status_t Menu::MenuWindow::on_mouse_up(const ws_event_t *e)
//        {
//            ws_event_t xev = *e;
//            Menu *handler = get_handler(&xev);
//            return (handler != NULL) ? handler->on_mouse_up(&xev) : LSPWindow::on_mouse_up(&xev);
//        }
//
//        status_t Menu::MenuWindow::on_mouse_scroll(const ws_event_t *e)
//        {
//            ws_event_t xev = *e;
//            Menu *handler = get_handler(&xev);
//            return (handler != NULL) ? handler->on_mouse_scroll(e) : LSPWindow::on_mouse_scroll(e);
//        }
//
//        status_t Menu::MenuWindow::on_mouse_move(const ws_event_t *e)
//        {
//            ws_event_t xev = *e;
//            Menu *handler = get_handler(&xev);
//            return (handler != NULL) ? handler->on_mouse_move(&xev) : LSPWindow::on_mouse_move(&xev);
//        }
//
//        void Menu::MenuWindow::size_request(size_request_t *r)
//        {
//            if (pMenu != NULL)
//                pMenu->size_request(r);
//
//            // Limit the size of window with the screen size
//            pDisplay->display()->screen_size(screen(), &r->nMaxWidth, &r->nMaxHeight);
//            if ((r->nMinWidth > 0) && (r->nMinWidth > r->nMaxWidth))
//                r->nMinWidth    = r->nMaxWidth;
//            if ((r->nMinHeight > 0) && (r->nMinHeight > r->nMaxHeight))
//                r->nMinHeight   = r->nMaxHeight;
//        }

        //-----------------------------------------------------------------------------
        // Menu implementation
        Menu::Menu(Display *dpy):
            WidgetContainer(dpy),
            sFont(&sProperties),
            sPosition(&sProperties),
            sSpacing(&sProperties),
            sScrolling(&sProperties),
            sBorder(&sProperties),
            sBorderColor(&sProperties),
            sCheckSize(&sProperties),
            sCheckBorder(&sProperties),
            sCheckBorderGap(&sProperties),
            sCheckBorderRadius(&sProperties)
        {
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
            status_t result = Widget::init();
            if (result != STATUS_OK)
                return result;

            sFont.bind("font", &sStyle);
            sPosition.bind("position", &sStyle);
            sSpacing.bind("spacing", &sStyle);
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
                sPosition.init(sclass, 0, 0);
                sSpacing.init(sclass, 0);
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

//            if (pWindow != NULL)
//            {
//                pWindow->destroy();
//                delete pWindow;
//                pWindow = NULL;
//            }
        }

        void Menu::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);

            if (sFont.is(prop))
                query_resize();
            if (sPosition.is(prop))
            {
                // TODO: sync with window
            }
            if (sSpacing.is(prop))
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
        }

        void Menu::realize(const ws::rectangle_t *r)
        {
            WidgetContainer::realize(r);

            size_request_t sr;
            size_request(&sr);

            nScrollMax      = sr.nMinHeight - r->nHeight;
            set_scroll(nScroll);
//            lsp_trace("scroll_max = %d, scroll = %d", int(nScrollMax), int(nScroll));

            query_draw();
            if (pWindow != NULL)
                pWindow->query_draw();
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

        void Menu::query_resize()
        {
            WidgetContainer::query_resize();
//            if (pWindow != NULL)
//                pWindow->query_resize();
        }

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

        ssize_t Menu::find_item(ssize_t mx, ssize_t my, ssize_t *ry)
        {
//            // Are we inside of submenu?
//            if (check_inside_submenu(mx, my))
//                return nSelected;

            if ((mx < 0) || (mx >= sSize.nWidth))
                return SEL_NONE;
            if ((my < 0) || (my >= sSize.nHeight))
                return SEL_NONE;

            font_parameters_t fp;
            sFont.get_parameters(&fp);

            ssize_t separator = fp.Height * 0.5f + nSpacing;
            fp.Height      += nSpacing;

            if (nScrollMax > 0)
            {
                if ((nScroll > 0) && (my < ssize_t(nBorder + separator))) // Top button
                    return SEL_TOP_SCROLL;
                else if ((nScroll < nScrollMax) && (my > ssize_t(sSize.nHeight - nBorder - separator))) // Bottom button
                    return SEL_BOTTOM_SCROLL;
            }

            // Iterate over all menu items
            ssize_t y       = sPadding.top() + nBorder - nScroll;
            size_t n        = vItems.size();

            for (size_t i=0; i < n; ++i)
            {
                MenuItem *item = vItems.get(i);
                if ((item == NULL) || (!item->visible()))
                    continue;

                if (item->is_separator())
                    y += separator;
                else
                {
                    if ((my >= y) && (my < (y + fp.Height)))
                    {
                        if (ry != NULL)
                            *ry     = y;
                        return i;
                    }

                    y += fp.Height;
                }
            }

            return SEL_NONE;
        }

        void Menu::draw(ws::ISurface *s)
        {
            // Prepare palette
            Color bg_color(sBgColor);
            Color border(sBorderColor);
            Color font(sFont.raw_color());
            Color sel(sSelColor);
            Color tmp;

            border.scale_lightness(brightness());
            font.scale_lightness(brightness());
            sel.scale_lightness(brightness());

            // Draw background
            s->clear(bg_color);

            font_parameters_t fp;
            text_parameters_t tp;
            sFont.get_parameters(s, &fp);

            ssize_t separator = fp.Height * 0.5f + nSpacing;
            ssize_t sep_len = sSize.nWidth - (nBorder + nSpacing) * 2;
            ssize_t hspace  = nSpacing >> 1;

            fp.Height      += nSpacing;
            ssize_t y       = sPadding.top() + nBorder - nScroll;
            ssize_t x       = sPadding.left() + nBorder;
            size_t n        = vItems.size();

            LSPString text;

            for (size_t i=0; i < n; ++i)
            {
                MenuItem *item = vItems.get(i);
                if ((item == NULL) || (!item->visible()))
                    continue;

//                lsp_trace("x,y = %d, %d", int(x), int(y));

                if (y >= sSize.nHeight)
                    break;

                if (item->is_separator())
                {
                    if (y > (-separator))
                    {
                        if (sep_len > 0)
                            s->fill_rect(x - sPadding.left() + nSpacing, y + (separator >> 1), sep_len, 1, border);
                    }

                    y += separator;
                }
                else
                {
                    if (y > (-fp.Height))
                    {
                        item->text()->format(&text);
                        if (nSelected == ssize_t(i))
                        {
                            s->fill_rect(nBorder, y, sSize.nWidth - nBorder*2, fp.Height, sel);
                            tmp.copy(bg_color);
                        }
                        else
                            tmp.copy(font);

                        if (!text.is_empty())
                            sFont.draw(s, x, y + fp.Ascent + hspace, tmp, &text);

                        if (item->has_submenu())
                        {
                            sFont.get_text_parameters(s, &tp, "►");
                            sFont.draw(s, sSize.nWidth - nBorder - nSpacing - tp.XAdvance - 2, y + fp.Ascent + hspace, tmp, "►");
                        }
                    }

                    y += fp.Height;
                }
            }

            if (nScrollMax > 0)
            {
                float cx = sSize.nWidth * 0.5f;
                float aa = s->set_antialiasing(true);

                // Top button
                if (nScroll > 0)
                {
                    s->fill_rect(nBorder, nBorder, sSize.nWidth - nBorder * 2, separator, bg_color);
                    if (nSelected == SEL_TOP_SCROLL)
                    {
                        tmp.copy(bg_color);
                        s->fill_rect(nBorder + 1, nBorder + 1, sSize.nWidth - (nBorder + 1)* 2, separator - 1, border);
                    }
                    else
                        tmp.copy(font);

                    // Draw arrow up
                    s->fill_triangle(
                        cx, nBorder + 3,
                        cx + separator, nBorder + separator - 2,
                        cx - separator, nBorder + separator - 2,
                        tmp);
                }
                else if (sPadding.top() > 0)
                    s->fill_rect(nBorder, nBorder, sSize.nWidth - nBorder * 2, sPadding.top(), bg_color);

                // Bottom button
                if (nScroll < nScrollMax)
                {
                    s->fill_rect(nBorder, sSize.nHeight - nBorder - separator,
                        sSize.nWidth - nBorder * 2, separator, bg_color);

                    if (nSelected == SEL_BOTTOM_SCROLL)
                    {
                        tmp.copy(bg_color);
                        s->fill_rect(nBorder + 1, sSize.nHeight - nBorder - separator,
                            sSize.nWidth - (nBorder + 1) * 2, separator - 1, border);
                    }
                    else
                        tmp.copy(font);

                    // Draw arrow down
                    s->fill_triangle(
                        cx, sSize.nHeight - nBorder - 3,
                        cx + separator, sSize.nHeight - nBorder - separator + 2,
                        cx - separator, sSize.nHeight - nBorder - separator + 2,
                        tmp);
                }
                else if (sPadding.bottom() > 0)
                    s->fill_rect(nBorder, sSize.nHeight - nBorder - sPadding.bottom(),
                        sSize.nWidth - nBorder * 2, sPadding.bottom(), bg_color);

                // Restore anti-aliasing
                s->set_antialiasing(aa);
            }

            if (nBorder > 0)
                s->fill_frame(0, 0, sSize.nWidth, sSize.nHeight,
                    nBorder, nBorder, sSize.nWidth - nBorder * 2, sSize.nHeight - nBorder * 2, border);
        }

        bool Menu::hide()
        {
            // Forget the parent menu
            pParentMenu = NULL;

            // Hide active submenu if present
            if (pActiveMenu != NULL)
            {
                pActiveMenu->hide();
                pActiveMenu = NULL;
            }

            // Hide window showing menu
            if (pWindow != NULL)
                pWindow->hide();

            if (!is_visible())
                return false;

            return LSPWidgetContainer::hide();
        }

        bool Menu::show()
        {
            if (is_visible())
                return false;

            size_t screen = pDisplay->display()->default_screen();
            LSPWindow *top = widget_cast<LSPWindow>(toplevel());
            if (top != NULL)
                screen = top->screen();

            return show(screen, nPopupLeft, nPopupTop);
        }

        bool Menu::show(size_t screen)
        {
            return show(screen, nPopupLeft, nPopupTop);
        }

        bool Menu::show(Widget *w)
        {
            return show(w, nPopupLeft, nPopupTop);
        }

        bool Menu::show(Widget *w, ssize_t x, ssize_t y)
        {
            if (is_visible())
                return false;

            size_t screen = pDisplay->display()->default_screen();
            LSPWindow *top = widget_cast<LSPWindow>(toplevel());
            if (top != NULL)
                screen = top->screen();

            return show(w, screen, x, y);
        }

        bool Menu::show(Widget *w, const ws::event_t *ev)
        {
            if (ev == NULL)
                return show(w, nPopupLeft, nPopupTop);

            ws::rectangle_t r;
            r.nLeft     = 0;
            r.nTop      = 0;
            r.nWidth    = 0;
            r.nHeight   = 0;

            Window *parent = widget_cast<Window>(w->toplevel());
            if (parent != NULL)
                parent->get_absolute_geometry(&r);

            return show(w, r.nLeft + ev->nLeft, r.nTop + ev->nTop);
        }

        bool Menu::show(size_t screen, ssize_t left, ssize_t top)
        {
            return show(NULL, screen, left, top);
        }

        bool Menu::show(Widget *w, size_t screen, ssize_t left, ssize_t top)
        {
            if (is_visible())
                return false;

            // Determine what screen to use
            ws::IDisplay *dpy = pDisplay->display();
            if (screen >= dpy->screens())
                screen = dpy->default_screen();

            // Now we are ready to create window
            if (pWindow == NULL)
            {
                // Create window
                pWindow = new MenuWindow(pDisplay, this, screen);
                if (pWindow == NULL)
                    return false;

                // Initialize window
                status_t result = pWindow->init();
                if (result != STATUS_OK)
                {
                    pWindow->destroy();
                    delete pWindow;
                    pWindow = NULL;
                    return false;
                }

                pWindow->set_border_style(BS_POPUP);
                pWindow->actions()->set_actions(WA_POPUP);
            }

            // Get initial window geometry
            realize_t wr;
            pWindow->get_geometry(&wr);
            if (left >= 0)
                wr.nLeft        = left;
            else if (wr.nLeft < 0)
                wr.nLeft        = 0;
            if (top >= 0)
                wr.nTop         = top;
            else if (wr.nTop < 0)
                wr.nTop         = 0;


            // Now request size and adjust location
            size_request_t sr;
            pWindow->size_request(&sr);

            ssize_t sw = 0, sh = 0;
            dpy->screen_size(pWindow->screen(), &sw, &sh);
            ssize_t xlast = wr.nLeft + sr.nMinWidth, ylast = wr.nTop + sr.nMinHeight;

            if (xlast >  sw)
                wr.nLeft   -= (xlast - sw);
            if (ylast > sh)
                wr.nTop    -= (ylast - sh);
            wr.nWidth       = sr.nMinWidth;
            wr.nHeight      = sr.nMinHeight;

            // Now we can set the geometry and show window
            pWindow->set_geometry(&wr);
            wr.nLeft        = 0;
            wr.nTop         = 0;
            realize(&wr);
            nSelected       = SEL_NONE;

            pWindow->show(w);

            // Need to perform grabbing?
            pParentMenu = widget_cast<Menu>(w);
            if (pParentMenu == NULL)
                pWindow->grab_events(GRAB_MENU);

            return LSPWidgetContainer::show();
        }

        void Menu::size_request(ws::size_limit_t *r)
        {
            r->nMinWidth    = 0;
            r->nMinHeight   = 0;
            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;

            // Create surface
            ws::ISurface *s = pDisplay->create_surface(1, 1);
            if (s == NULL)
                return;

            // Estimate the size of menu
            font_parameters_t fp;
            text_parameters_t tp;
            sFont.get_parameters(s, &fp);
            size_t n = vItems.size();
            ssize_t separator = fp.Height * 0.5f;
            ssize_t subitem = 0;

            LSPString text;
            for (size_t i=0; i<n; ++i)
            {
                MenuItem *mi = vItems.at(i);
                if ((mi == NULL) || (!mi->visible()))
                    continue;

                if (mi->is_separator())
                {
                    r->nMinHeight += separator + nSpacing;
                    if (r->nMinWidth < fp.Height)
                        r->nMinWidth = fp.Height;
                }
                else
                {
                    r->nMinHeight  += fp.Height + nSpacing;
                    ssize_t width   = (mi->submenu() != NULL) ? separator : 0;

                    mi->text()->format(&text);
                    if (!text.is_empty())
                    {
                        sFont.get_text_parameters(s, &tp, &text);
                        width          += tp.XAdvance;
                    }

                    if ((subitem <= 0) && (mi->has_submenu()))
                    {
                        sFont.get_text_parameters(s, &tp, "►");
                        subitem        += tp.XAdvance + 2;
                    }

                    if (r->nMinWidth < width)
                        r->nMinWidth        = width;
                }
            }

            r->nMinWidth    += nBorder * 2 + subitem + sPadding.horizontal();
            r->nMinHeight   += nBorder * 2 + sPadding.vertical();

            // Destroy surface
            s->destroy();
            delete s;
        }

        status_t Menu::on_mouse_down(const ws::event_t *e)
        {
            if (nMBState == 0)
            {
                if (!inside(e->nLeft, e->nTop))
                {
                    hide();
                    return STATUS_OK;
                }
            }

            nMBState |= (1 << e->nCode);
            ssize_t iy = 0;
            ssize_t sel = find_item(e->nLeft, e->nTop, &iy);
            selection_changed(sel, iy);

            return STATUS_OK;
        }

        status_t Menu::on_mouse_up(const ws::event_t *e)
        {
            if ((nMBState == (1 << ws::MCB_LEFT)) && (e->nCode == ws::MCB_LEFT))
            {
                Menu *parent = this;
                while (parent->pParentMenu != NULL)
                    parent  = parent->pParentMenu;

                // Cleanup mouse button state flag
                nMBState &= ~ (1 << e->nCode);

                // Selection was found ?
                MenuItem *item = NULL;
                ssize_t iy = 0;
                ssize_t sel = find_item(e->nLeft, e->nTop, &iy);

                // Notify that selection has changed
                selection_changed(sel, iy);

                if (sel >= 0)
                {
                    item = vItems.get(sel);
                    if (item == NULL)
                        parent->hide();
                    else if (item->hidden())
                    {
                        item    = NULL;
                        parent->hide();
                    }
                    else if (!item->has_submenu())
                        parent->hide();

                    if (item != NULL)
                    {
                        ws_event_t ev = *e;
                        item->slots()->execute(LSPSLOT_SUBMIT, item, &ev);
                    }
                }
                else
                {
                    if ((sel != SEL_TOP_SCROLL) && (sel != SEL_BOTTOM_SCROLL))
                        parent->hide();
                }
            }
            else
            {
                // Cleanup mouse button state flag
                nMBState &= ~ (1 << e->nCode);
                if (nMBState == 0)
                    hide();
            }

            return STATUS_OK;
        }

        status_t Menu::on_mouse_scroll(const ws::event_t *e)
        {
            ws::font_parameters_t fp;
            sFont.get_parameters(&fp);
            ssize_t amount = fp.Height + nSpacing;
            if (amount < 1)
                amount = 1;

            ssize_t scroll = nScroll;
            if (e->nCode == ws::MCD_UP)
                set_scroll(nScroll - amount);
            else if (e->nCode == ws::MCD_DOWN)
                set_scroll(nScroll + amount);

            if (scroll != nScroll)
            {
                ssize_t sel = nSelected, iy = 0;
                nSelected   = find_item(e->nLeft, e->nTop, &iy);

                if (sel != nSelected)
                {
                    // Notify that selection has changed
                    selection_changed(nSelected, iy);

                    // Query for draw
                    query_draw();
                    if (pWindow != NULL)
                        pWindow->query_draw();
                }
            }

            return STATUS_OK;
        }

        void Menu::selection_changed(ssize_t sel, ssize_t iy)
        {
            // Get menu item
            MenuItem *item = (sel >= 0) ? vItems.get(sel) : NULL;
            if ((item != NULL) && (pActiveMenu == item->submenu()))
                return;

            if (pActiveMenu != NULL)
            {
                pActiveMenu->hide();
                pActiveMenu = NULL;
            }

            if (item == NULL)
                return;

            if ((pActiveMenu = item->submenu()) == NULL)
                return;

            // Get screen size
            IDisplay *dpy = pDisplay->display();
            ssize_t sw = 0, sh = 0;
            dpy->screen_size(pWindow->screen(), &sw, &sh);

            // Get window geometry
            realize_t wr;
            pWindow->get_geometry(&wr);
            ssize_t xlast = wr.nLeft + wr.nWidth;

            // Estimate active window size
            size_request_t sr;
            pActiveMenu->size_request(&sr);
            if (sr.nMinWidth < 0)
                sr.nMinWidth = 0;

            if ((xlast + sr.nMinWidth) < sw)
                pActiveMenu->show(this, xlast, iy + wr.nTop);
            else
                pActiveMenu->show(this, wr.nLeft - sr.nMinWidth, iy + wr.nTop);
        }

        status_t Menu::on_mouse_move(const ws::event_t *e)
        {
            lsp_trace("x=%d, y=%d", int(e->nLeft), int(e->nTop));
            ssize_t sel = nSelected;
            ssize_t iy  = 0;
            nSelected   = find_item(e->nLeft, e->nTop, &iy);

            if (sel != nSelected)
            {
//                lsp_trace("selection changed. Was %d, now %d", int(sel), int(nSelected));
                // Update timer status
                if ((nSelected == SEL_TOP_SCROLL) || (nSelected == SEL_BOTTOM_SCROLL))
                    sScroll.launch(0, 25);
                else
                {
                    sScroll.cancel();
                    selection_changed(nSelected, iy);
                }

                // Query for draw
                query_draw();
                if (pWindow != NULL)
                    pWindow->query_draw();
            }

            return STATUS_OK;
        }

        status_t Menu::timer_handler(ws::timestamp_t time, void *arg)
        {
            Menu *_this = widget_ptrcast<Menu>(arg);
            if (_this == NULL)
                return STATUS_BAD_ARGUMENTS;
            _this->update_scroll();
            return STATUS_OK;
        }

        void Menu::update_scroll()
        {
            ws::font_parameters_t fp;
            sFont.get_parameters(&fp);
            ssize_t amount = fp.Height * 0.5f;
            if (amount < 1)
                amount = 1;

            switch (nSelected)
            {
                case SEL_TOP_SCROLL:
                    set_scroll(nScroll - amount);
                    if (nScroll <= 0)
                        sScroll.cancel();
                    break;

                case SEL_BOTTOM_SCROLL:
                    set_scroll(nScroll + amount);
                    if (nScroll >= nScrollMax)
                        sScroll.cancel();
                    break;

                default:
                    sScroll.cancel();
                    break;
            }
        }
    
    } /* namespace tk */
} /* namespace lsp */
