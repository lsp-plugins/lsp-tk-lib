/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 18 сент. 2017 г.
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
#include <lsp-plug.in/common/debug.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(Menu, WidgetContainer)
                // Bind
                sFont.bind("font", this);
                sScrolling.bind("scrolling", this);
                sBorderSize.bind("border.size", this);
                sBorderColor.bind("border.color", this);
                sScrollColor.bind("scroll.color", this);
                sScrollTextColor.bind("scroll.text.color", this);
                sScrollSelectedColor.bind("scroll.selected.color", this);
                sScrollTextSelectedColor.bind("scroll.text.selected.color", this);
                sCheckSize.bind("check.size", this);
                sCheckBorder.bind("check.border", this);
                sCheckBorderGap.bind("check.border.gap", this);
                sCheckBorderRadius.bind("check.border.radius", this);
                sSeparatorWidth.bind("separator.width", this);
                sSpacing.bind("spacing", this);
                sIPadding.bind("ipadding", this);
                // Configure
                sFont.set_size(12.0f);
                sScrolling.set(0.0f);
                sBorderSize.set(1);
                sBorderColor.set("#000000");
                sScrollColor.set("#cccccc");
                sScrollTextColor.set("#000000");
                sScrollSelectedColor.set("#000088");
                sScrollTextSelectedColor.set("#ffffff");
                sCheckSize.set(12);
                sCheckBorder.set(1);
                sCheckBorderGap.set(1);
                sCheckBorderRadius.set(3);
                sSeparatorWidth.set(1);
                sSpacing.set(4);
                sIPadding.set_all(0);
                sPadding.set_all(0);
                // Override
                sVisibility.set(false);
                sBgColor.set("#cccccc");
                // Commit
                sVisibility.override();
                sBgColor.override();
                sIPadding.override();
                sPadding.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(Menu, "Menu");

            // Menu::Window style
            LSP_TK_BUILTIN_STYLE(PopupWindow, "Menu::Window");

            // Menu::Scroll style
            LSP_TK_STYLE_DEF_BEGIN(Menu__MenuScroll, Widget)
            LSP_TK_STYLE_DEF_END
            LSP_TK_STYLE_IMPL_BEGIN(Menu__MenuScroll, Widget)
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(Menu__MenuScroll, "Menu::MenuScroll");
        }

        //-----------------------------------------------------------------------------
        // Menu window implementation
        const w_class_t Menu::Window::metadata      = { "Menu::Window", &PopupWindow::metadata };

        Menu::Window::Window(Display *dpy, Menu *menu): PopupWindow(dpy)
        {
            pMenu = menu;
            pClass = &metadata;
        }

        Widget *Menu::Window::sync_mouse_handler(const ws::event_t *e)
        {
            Widget *old     = hMouse.pWidget;
            Widget *curr    = PopupWindow::sync_mouse_handler(e);

            if (curr != old)
                curr->take_focus();

            return curr;
        }

        Widget *Menu::Window::acquire_mouse_handler(const ws::event_t *e)
        {
            return sync_mouse_handler(e);
        }

        Widget *Menu::Window::release_mouse_handler(const ws::event_t *e)
        {
            return sync_mouse_handler(e);
        }

        status_t Menu::Window::handle_event(const ws::event_t *e)
        {
            ws::rectangle_t xr;

            status_t result = STATUS_OK;
            ws::event_t xe  = *e;

            switch (e->nType)
            {
                //-------------------------------------------------------------
                // Keyboard handling
                case ws::UIE_KEY_DOWN:
                case ws::UIE_KEY_UP:
                {
                    // Just bypass events for non-root menu
                    if (pMenu->pParentMenu != NULL)
                    {
                        result = PopupWindow::handle_event(&xe);
                        break;
                    }

                    // We're root menu
                    if (get_screen_rectangle(&xr) != STATUS_OK)
                        break;
                    lsp_trace("root coords: {%d, %d, %d, %d}", int(xr.nLeft), int(xr.nTop), int(xr.nWidth), int(xr.nHeight));
                    xe.nLeft       += xr.nLeft;
                    xe.nTop        += xr.nTop;

                    // Obtain keyboard handler
                    Menu *kbd       = pMenu->pKeyboardMenu;
                    if (kbd != NULL)
                    {
                        kbd->sWindow.get_screen_rectangle(&xr);
                        xe.nLeft       -= xr.nLeft;
                        xe.nTop        -= xr.nTop;
                        result          = kbd->handle_event(&xe);
                    }
                    break;
                }

                case ws::UIE_MOUSE_CLICK:
                case ws::UIE_MOUSE_DBL_CLICK:
                case ws::UIE_MOUSE_TRI_CLICK:
                case ws::UIE_MOUSE_DOWN:
                case ws::UIE_MOUSE_UP:
                case ws::UIE_MOUSE_MOVE:
                {
                    // Just bypass events for non-root menu
                    if (pMenu->pParentMenu != NULL)
                    {
                        result = PopupWindow::handle_event(&xe);
                        break;
                    }

                    // We're root menu
                    if (get_screen_rectangle(&xr) != STATUS_OK)
                        break;
                    lsp_trace("root coords: {%d, %d, %d, %d}", int(xr.nLeft), int(xr.nTop), int(xr.nWidth), int(xr.nHeight));
                    xe.nLeft       += xr.nLeft;
                    xe.nTop        += xr.nTop;

                    // Find mouse handler
                    Menu *m         = pMenu->find_menu(&xe, &xr);
                    if (m == NULL)
                    {
                        if (e->nType != ws::UIE_MOUSE_MOVE)
                            pMenu->hide();
                        break;
                    }

                    // Bypass event
                    xe.nLeft       -= xr.nLeft;
                    xe.nTop        -= xr.nTop;

                    lsp_trace("handler=%p, coords: {%d, %d}", m,
                            int(xe.nLeft), int(xe.nTop));
                    result          = (m != pMenu) ?
                            m->sWindow.handle_event(&xe) :
                            PopupWindow::handle_event(&xe);
                    break;
                }

                //-------------------------------------------------------------
                // Other events
                default:
                    result      = PopupWindow::handle_event(&xe);
                    break;
            }

            // Update pointer
            update_pointer();

            return result;
        }

        bool Menu::Window::take_focus()
        {
            bool res = PopupWindow::take_focus();
            if (res)
            {
                Menu *root              = pMenu->root_menu();
                root->pKeyboardMenu     = pMenu;
            }

            return res;
        }

        //-----------------------------------------------------------------------------
        // Menu scroll implementation
        const w_class_t Menu::MenuScroll::metadata      = { "Menu::MenuScroll", &Widget::metadata };

        Menu::MenuScroll::MenuScroll(Display *dpy, Menu *menu, ssize_t dir): Widget(dpy)
        {
            pMenu       = menu;
            nDirection  = dir;
            bActive     = false;

            pClass      = &metadata;
        }

        status_t Menu::MenuScroll::on_mouse_in(const ws::event_t *e)
        {
            bActive     = true;
            return pMenu->start_mouse_scroll(nDirection);
        }

        status_t Menu::MenuScroll::on_mouse_out(const ws::event_t *e)
        {
            bActive     = false;
            return pMenu->end_mouse_scroll();
        }

        status_t Menu::MenuScroll::on_focus_out(const ws::event_t *e)
        {
            bActive     = false;
            return pMenu->end_mouse_scroll();
        }

        bool Menu::MenuScroll::active() const
        {
            return (sVisibility.get()) && (bActive);
        }

        //-----------------------------------------------------------------------------
        // Menu implementation
        const w_class_t Menu::metadata      = { "Menu", &WidgetContainer::metadata };

        Menu::Menu(Display *dpy):
            WidgetContainer(dpy),
            sWindow(dpy, this),
            sUp(dpy, this, -1),
            sDown(dpy, this, 1),
            sFont(&sProperties),
            sScrolling(&sProperties),
            sBorderSize(&sProperties),
            sBorderColor(&sProperties),
            sScrollColor(&sProperties),
            sScrollSelectedColor(&sProperties),
            sScrollTextColor(&sProperties),
            sScrollTextSelectedColor(&sProperties),
            sCheckSize(&sProperties),
            sCheckBorder(&sProperties),
            sCheckBorderGap(&sProperties),
            sCheckBorderRadius(&sProperties),
            sSeparatorWidth(&sProperties),
            sSpacing(&sProperties),
            sIPadding(&sProperties),
            sSubmenu(&sProperties)
        {
            nSelected               = -1;
            nKeyScroll              = 0;
            nMouseScroll            = 0;
            pParentMenu             = NULL;
            pChildMenu              = NULL;
            pKeyboardMenu           = NULL;

            sIStats.full_w          = 0;
            sIStats.full_h          = 0;
            sIStats.item_w          = 0;
            sIStats.item_h          = 0;
            sIStats.check_w         = 0;
            sIStats.check_h         = 0;
            sIStats.scut_w          = 0;
            sIStats.scut_h          = 0;
            sIStats.link_w          = 0;
            sIStats.link_h          = 0;
            sIStats.items           = 0;
            sIStats.separators      = 0;
            sIStats.max_scroll      = 0;
            sIStats.ckbox           = false;
            sIStats.shortcut        = false;
            sIStats.submenu         = false;

            pClass                  = &metadata;
        }
        
        Menu::~Menu()
        {
            nFlags     |= FINALIZED;
            do_destroy();
        }

        status_t Menu::init()
        {
            // Initialize parent widget
            status_t result = Widget::init();
            if (result != STATUS_OK)
                return result;

            // Initialize window
            if ((result = sWindow.init()) != STATUS_OK)
            {
                sWindow.destroy();
                return result;
            }
            sWindow.add_arrangement(A_RIGHT, 0.0f, false);
            sWindow.add_arrangement(A_LEFT, 0.0f, false);
            sWindow.layout()->set(-1.0f, -1.0f, 1.0f, 1.0f);
            sWindow.auto_close()->set(false);

            if ((result = sUp.init()) != STATUS_OK)
                return result;
            sUp.set_parent(this);
            sUp.visibility()->set(false);

            if ((result = sDown.init()) != STATUS_OK)
                return result;
            sDown.set_parent(this);
            sDown.visibility()->set(false);

            // Initialize timers
            sKeyTimer.bind(pDisplay);
            sKeyTimer.set_handler(key_scroll_handler, self());

            sMouseTimer.bind(pDisplay);
            sMouseTimer.set_handler(mouse_scroll_handler, self());

            // Bind properties
            sFont.bind("font", &sStyle);
            sScrolling.bind("scrolling", &sStyle);
            sBorderSize.bind("border.size", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sScrollColor.bind("scroll.color", &sStyle);
            sScrollTextColor.bind("scroll.text.color", &sStyle);
            sScrollSelectedColor.bind("scroll.selected.color", &sStyle);
            sScrollTextSelectedColor.bind("scroll.text.selected.color", &sStyle);
            sCheckSize.bind("check.size", &sStyle);
            sCheckBorder.bind("check.border", &sStyle);
            sCheckBorderGap.bind("check.border.gap", &sStyle);
            sCheckBorderRadius.bind("check.border.radius", &sStyle);
            sSeparatorWidth.bind("separator.width", &sStyle);
            sSpacing.bind("spacing", &sStyle);
            sIPadding.bind("ipadding", &sStyle);
            sSubmenu.bind(NULL);

            return STATUS_OK;
        }

        void Menu::destroy()
        {
            nFlags     |= FINALIZED;
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

            // Cancel timers
            sKeyTimer.cancel();
            sMouseTimer.cancel();

            vVisible.flush();
            vItems.flush();
            sWindow.destroy();
        }

        void Menu::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);

            if (sFont.is(prop))
                query_resize();
            if (sScrolling.is(prop))
                query_resize();
            if (sBorderSize.is(prop))
                query_resize();
            if (sBorderColor.is(prop))
                query_draw();
            if (sScrollColor.is(prop))
                query_draw();
            if (sScrollTextColor.is(prop))
                query_draw();
            if (sScrollSelectedColor.is(prop))
                query_draw();
            if (sScrollTextSelectedColor.is(prop))
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
            if (sSpacing.is(prop))
                query_resize();
        }

        void Menu::size_request(ws::size_limit_t *r)
        {
            istats_t st;
            lltl::darray<item_t> items;

            float scaling   = lsp_max(0.0f, sScaling.get());
            ssize_t border  = lsp_max(0.0f, scaling * sBorderSize.get()) * 2;

            allocate_items(&items, &st);

            // Compute minimum and maximum size
            r->nMinWidth    = st.full_w + border;
            r->nMinHeight   = st.item_h + border;
            r->nMaxWidth    = r->nMinWidth;
            r->nMaxHeight   = st.full_h + border;
            r->nPreWidth    = -1;
            r->nPreHeight   = -1;

            // Apply internal padding
            sIPadding.add(r, scaling);
        }

        void Menu::allocate_items(lltl::darray<item_t> *out, istats_t *st)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());
            float fscaling      = lsp_max(0.0f, scaling * sFontScaling.get());
            ssize_t spacing     = lsp_max(0.0f, scaling * sSpacing.get());

            st->full_w          = 0;
            st->full_h          = 0;
            st->item_w          = 0;
            st->item_h          = 0;
            st->check_w         = 0;
            st->check_h         = 0;
            st->scut_w          = 0;
            st->scut_h          = 0;
            st->link_w          = 0;
            st->link_h          = 0;
            st->items           = 0;
            st->separators      = 0;
            st->ckbox           = false;
            st->shortcut        = false;
            st->submenu         = false;

            // Size of check box/radio
            ssize_t cb_br       = lsp_max(0, sCheckBorderRadius.get() * scaling) * 3;
            st->check_w         = (sCheckBorder.get() > 0) ? lsp_min(1.0f, sCheckBorder.get() * scaling) : 0;
            if (st->check_w > 0)
                st->check_w        += (sCheckBorderGap.get() > 0) ? lsp_min(1.0f, sCheckBorderGap.get() * scaling) : 0;
            st->check_w        += lsp_max(2.0f, sCheckSize.get() * scaling);
            st->check_w         = lsp_max(st->check_w, cb_br);
            st->check_h         = st->check_w;

            // Size of text, shortcut and reference
            LSPString caption, shortcut;
            ws::font_parameters_t fp;
            ws::text_parameters_t tp;
            sFont.get_parameters(pDisplay, fscaling, &fp);

            // First pass: estimate that there are certain elements present
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                // Keep only visible items
                MenuItem *mi        = vItems.get(i);
                if ((mi == NULL) || (!mi->visibility()->get()))
                    continue;

                // Estimate type of item
                menu_item_type_t mt = mi->type()->get();
                if ((mt == MI_CHECK) || (mt == MI_RADIO))
                    st->ckbox           = true;
                else if (mt != MI_SEPARATOR)
                {
                    if (mi->shortcut()->valid())
                    {
                        mi->shortcut()->format(&shortcut);
                        sFont.get_text_parameters(pDisplay, &tp, fscaling, &shortcut);

                        st->shortcut        = true;
                        st->scut_w          = lsp_max(st->scut_w, ceilf(tp.Width));
                        st->scut_h          = lsp_max(st->scut_h, ceilf(lsp_max(fp.Height, tp.Height)));
                    }
                    if (mi->menu()->is_set())
                    {
                        st->submenu         = true;
                        st->link_w          = lsp_max(st->link_w, ssize_t(lsp_max(2.0f, M_SQRT1_2 * fp.Height)));
                        st->link_h          = lsp_max(st->link_h, fp.Height);
                    }
                }
            }

            // Second pass: estimate parameters for each item
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                // Keep only visible items
                MenuItem *mi        = vItems.get(i);
                if ((mi == NULL) || (!mi->visibility()->get()))
                    continue;

                // Add item to list
                item_t *pi          = out->add();
                if (pi == NULL)
                    return;

                // Compute padding
                pi->item            = mi;
                mi->padding()->compute(&pi->pad, scaling);

                // Estimate type of item
                menu_item_type_t mt = mi->type()->get();
                bool xsep           = (mt != MI_SEPARATOR);

                // Reduce padding
                if (xsep)
                    ++st->items;
                else
                {
                    pi->pad.nLeft     >>= 2;
                    pi->pad.nRight    >>= 2;
                    ++st->separators;
                }

                // Estimate size of caption
                pi->text.nLeft      = 0;
                pi->text.nTop       = 0;
                pi->area.nLeft      = 0;
                pi->area.nTop       = 0;

                if (xsep)
                {
                    mi->text()->format(&caption);
                    mi->text_adjust()->apply(&caption);
                    sFont.get_text_parameters(pDisplay, &tp, fscaling, &caption);

                    pi->text.nWidth     = tp.Width;
                    pi->text.nHeight    = lsp_max(fp.Height, tp.Height);
                    pi->area.nWidth     = pi->text.nWidth;
                    pi->area.nHeight    = pi->text.nHeight;
                }
                else
                {
                    pi->text.nWidth     = 0;
                    pi->text.nHeight    = 0;
                    pi->area.nWidth     = lsp_max(8, 8 * scaling);
                    pi->area.nHeight    = lsp_max(0.0f, sSeparatorWidth.get() * scaling);
                }

                // Check box/radio
                pi->check.nLeft     = 0;
                pi->check.nTop      = 0;
                if ((xsep) && (st->ckbox))
                {
                    pi->check.nWidth    = st->check_w;
                    pi->check.nHeight   = st->check_h;
                    pi->area.nWidth    += pi->check.nWidth + spacing;
                    pi->area.nHeight    = lsp_max(pi->area.nHeight, pi->check.nHeight);
                }
                else
                {
                    pi->check.nWidth    = 0;
                    pi->check.nHeight   = 0;
                }

                // Estimate size of shortcut
                pi->scut.nLeft      = 0;
                pi->scut.nTop       = 0;
                if ((xsep) && (st->shortcut))
                {
                    mi->shortcut()->format(&shortcut);
                    sFont.get_text_parameters(pDisplay, &tp, fscaling, &shortcut);

                    st->shortcut        = true;
                    pi->scut.nWidth     = st->scut_w;
                    pi->scut.nHeight    = lsp_max(fp.Height, tp.Height);

                    pi->area.nWidth    += pi->scut.nWidth + spacing;
                    pi->area.nHeight    = lsp_max(pi->area.nHeight, pi->scut.nHeight);
                }
                else
                {
                    pi->scut.nWidth     = 0;
                    pi->scut.nHeight    = 0;
                }

                // Estimate submenu reference size
                pi->ref.nLeft       = 0;
                pi->ref.nTop        = 0;
                if ((xsep) && (st->submenu))
                {
                    st->submenu         = true;
                    pi->ref.nHeight     = fp.Height;
                    pi->ref.nWidth      = lsp_max(2.0f, M_SQRT1_2 * fp.Height);

                    pi->pad.nRight      = lsp_max(ssize_t(pi->pad.nRight), st->link_w + spacing);
                    pi->area.nHeight    = lsp_max(pi->area.nHeight, pi->ref.nHeight);
                }
                else
                {
                    pi->ref.nWidth      = 0;
                    pi->ref.nHeight     = 0;
                }

                // Apply padding
                pi->area.nWidth        += pi->pad.nLeft + pi->pad.nRight;
                pi->area.nHeight       += pi->pad.nTop  + pi->pad.nBottom;

                // Compute statistics
                st->full_h             += pi->area.nHeight;
                st->full_w              = lsp_max(st->full_w, pi->area.nWidth);
                st->item_w              = lsp_max(st->item_w, st->full_w);
                st->item_h              = lsp_max(st->item_h, pi->area.nHeight);

//                lsp_trace("menu item text=%s, text.w={%d, %d}, check.w={%d, %d}, scut.w=%d, ref.w=%d, area.w={%d, %d}, st.w=%d, it.w=%d",
//                    (xsep) ? caption.get_utf8() : "-",
//                    int(pi->text.nWidth), int(pi->text.nHeight),
//                    int(pi->check.nWidth), int(pi->check.nHeight),
//                    int(pi->scut.nWidth),
//                    int(pi->ref.nWidth),
//                    int(pi->area.nWidth), int(pi->area.nHeight),
//                    int(st->full_w),
//                    int(st->item_w)
//                );
            }
        }

        Menu *Menu::root_menu()
        {
            Menu *curr  = this;
            lsp_trace("this = %p", this);
            while ((curr->pParentMenu != NULL))
                curr        = curr->pParentMenu;

            lsp_trace("root = %p", curr);
            return curr;
        }

        Menu *Menu::find_menu(const ws::event_t *ev, ws::rectangle_t *xr)
        {
            lsp_trace("this = %p", this);

            // Get last child
            Menu *curr = this;
            while (curr->pChildMenu != NULL)
                curr = curr->pChildMenu;

            lsp_trace("last = %p", curr);

            // Seek from last child to parent
            while (curr != NULL)
            {
                curr->sWindow.get_screen_rectangle(xr);
                lsp_trace("pos={%d, %d}, curr = %p, param = {%d, %d, %d, %d}",
                        int(ev->nLeft), int(ev->nTop), curr,
                        int(xr->nLeft), int(xr->nTop), int(xr->nWidth), int(xr->nHeight));
                if (Position::inside(xr, ev->nLeft, ev->nTop))
                {
                    lsp_trace("found handler: %p", curr);
                    return curr;
                }
                curr    = curr->pParentMenu;
            }

            lsp_trace("not found handler");

            return NULL;
        }

        void Menu::realize(const ws::rectangle_t *r)
        {
            WidgetContainer::realize(r);

            ws::rectangle_t rr, xr;
            istats_t st;
            lltl::darray<item_t> items;
            allocate_items(&items, &st);

            // Estimate scrolling position
            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t scroll      = lsp_max(0, sScrolling.get() * scaling);
            ssize_t border      = lsp_max(0.0f, scaling * sBorderSize.get());
            ssize_t spacing     = lsp_max(0.0f, scaling * sSpacing.get());

            rr.nLeft            = border;
            rr.nTop             = border;
            rr.nWidth           = r->nWidth  - border * 2;
            rr.nHeight          = r->nHeight - border * 2;

            // Apply internal padding
            sIPadding.enter(&rr, scaling);

            // Re-compute scrolling parameters
            st.max_scroll       = lsp_max(0, st.full_h - rr.nHeight);
            if ((scroll > st.max_scroll) && (scaling > 0.0f))
            {
                scroll              = st.max_scroll;
                sScrolling.commit_value(scroll / scaling);
            }

            // Allocate space for buttons
            xr.nLeft            = rr.nLeft;
            xr.nTop             = rr.nTop - border;
            xr.nWidth           = rr.nWidth;
            xr.nHeight          = lsp_max(4, st.item_h >> 1) + border;
            sUp.visibility()->set(scroll > 0);
            sUp.realize_widget(&xr);

            xr.nWidth           = rr.nWidth;
            xr.nHeight          = lsp_max(4, st.item_h >> 1) + border;
            xr.nLeft            = rr.nLeft;
            xr.nTop             = rr.nTop + rr.nHeight - xr.nHeight + border;
            sDown.visibility()->set(scroll < st.max_scroll);
            sDown.realize_widget(&xr);

            // Now allocate position of each visible widget
            rr.nTop            -= scroll;

            for (size_t i=0, n=items.size(); i<n; ++i)
            {
                item_t *pi          = items.uget(i);
                MenuItem *mi        = pi->item;

                // Estimate type of item
                menu_item_type_t mt = mi->type()->get();
                bool sep            = (mt == MI_SEPARATOR);
                bool check          = (mt == MI_CHECK) || (mt == MI_RADIO);

                pi->area.nLeft      = rr.nLeft;
                pi->area.nTop       = rr.nTop;
                pi->area.nWidth     = rr.nWidth;

                // Realize menu item
                xr                  = pi->area;
                pi->item->realize_widget(&xr);

                // Apply padding
                xr.nLeft           += pi->pad.nLeft;
                xr.nTop            += pi->pad.nTop;
                xr.nWidth          -= pi->pad.nLeft + pi->pad.nRight;
                xr.nHeight         -= pi->pad.nTop  + pi->pad.nBottom;

                // Just separator?
                if (sep)
                {
                    pi->text            = xr;
                    rr.nTop            += pi->area.nHeight;
                    continue;
                }

                // Do we have a check box/radio ?
                if (st.ckbox)
                {
                    if (check)
                    {
                        pi->check.nLeft     = xr.nLeft;
                        pi->check.nTop      = xr.nTop + ((xr.nHeight - pi->check.nHeight) >> 1);
                    }

                    xr.nLeft           += st.check_w + spacing;
                    xr.nWidth          -= st.check_w + spacing;
                }

                // Do we have a link ?
                if (st.submenu)
                {
                    if (mi->menu()->is_set())
                    {
                        pi->ref.nLeft       = xr.nLeft + xr.nWidth + pi->pad.nRight - st.link_w;
                        pi->ref.nTop        = xr.nTop + ((xr.nHeight - pi->ref.nHeight) >> 1);
                    }

                    xr.nWidth          -= st.link_w + spacing;
                }

                // Do we have a shortcut?
                if (st.shortcut)
                {
                    if (mi->shortcut()->valid())
                    {
                        pi->scut.nLeft      = xr.nLeft + xr.nWidth - st.scut_w;
                        pi->scut.nTop       = xr.nTop + ((xr.nHeight - pi->scut.nHeight) >> 1);
                    }

                    xr.nWidth          -= st.scut_w + spacing;
                }

                // Allocate space for text
                pi->text.nLeft      = xr.nLeft;
                pi->text.nTop       = xr.nTop + ((xr.nHeight - pi->text.nHeight) >> 1);

                // Update position
                rr.nTop            += pi->area.nHeight;
            }

            // Remember drawing parameters
            vVisible.swap(items);
            sIStats             = st;
        }

        void Menu::draw(ws::ISurface *s)
        {
            ws::rectangle_t xr, r;
            float scaling       = lsp_max(0.0f, sScaling.get());
            float fscaling      = lsp_max(0.0f, scaling * sFontScaling.get());
            float bright        = sBrightness.get();
            ssize_t border      = lsp_max(0, sBorderSize.get() * scaling);

            xr.nLeft            = border;
            xr.nTop             = border;
            xr.nWidth           = sSize.nWidth  - border * 2;
            xr.nHeight          = sSize.nHeight - border * 2;

            // Draw background
            lsp::Color color;
            get_actual_bg_color(color);
            s->clear(color);

            // Apply internal padding
            sIPadding.enter(&xr, scaling);

            ws::font_parameters_t fp;
            LSPString text;

            sFont.get_parameters(pDisplay, fscaling, &fp);

            float aa            = s->set_antialiasing(true);

            for (ssize_t i=0, n=vVisible.size(); i<n; ++i)
            {
                item_t *pi          = vVisible.uget(i);

                // Do not draw invisible items
                if (!Size::overlap(&xr, &pi->area))
                    continue;

                // Get child widget
                MenuItem *mi        = pi->item;

                // Commit redraw for child widget
                mi->commit_redraw();

                // Just separator?
                if (mi->type()->separator())
                {
                    color.copy(mi->text_color()->color());
                    color.scale_lightness(bright);
                    s->fill_rect(color, &pi->text);
                    continue;
                }

                // Selected element?
                if (nSelected == i)
                {
                    color.copy(mi->bg_selected_color()->color());
                    color.scale_lightness(bright);
                    s->fill_rect(color, &pi->area);
                }

                // Draw text
                mi->text()->format(&text);
                mi->text_adjust()->apply(&text);
                if (nSelected == i)
                    color.copy(mi->text_selected_color()->color());
                else
                    color.copy(mi->text_color()->color());
                color.scale_lightness(bright);
                sFont.draw(s, color, pi->text.nLeft, pi->text.nTop + fp.Ascent, fscaling, &text);

                // Draw shortcut
                if (mi->shortcut()->valid())
                {
                    mi->shortcut()->format(&text);
                    sFont.draw(s, color, pi->scut.nLeft, pi->scut.nTop + fp.Ascent, fscaling, &text);
                }

                // Draw reference
                if (mi->menu()->is_set())
                {
                    float x = pi->ref.nLeft + pi->ref.nWidth * 0.25f;
                    float y = pi->ref.nTop  + pi->ref.nHeight * 0.25f;
                    s->fill_triangle(
                            x, y,
                            x + pi->ref.nWidth * 0.5f, y + pi->ref.nHeight * 0.25f,
                            x, y + pi->ref.nHeight * 0.5f,
                            color
                        );
                }

                // Need to draw check box/radio?
                if (mi->type()->check())
                {
                    ssize_t br          = lsp_max(0, sCheckBorderRadius.get() * scaling);
                    ssize_t bw          = (sCheckBorder.get() > 0) ? lsp_max(1, sCheckBorder.get() * scaling) : 0;

                    r                   = pi->check;
                    if (bw > 0)
                    {
                        color.copy(mi->check_border_color()->color());
                        color.scale_lightness(bright);
                        s->fill_round_rect(color, SURFMASK_ALL_CORNER, br, &r);
                        r.nLeft            += bw;
                        r.nTop             += bw;
                        r.nWidth           -= bw * 2;
                        r.nHeight          -= bw * 2;
                        br                  = lsp_max(0, br - bw);

                        color.copy(mi->check_bg_color()->color());
                        color.scale_lightness(bright);
                        s->fill_round_rect(color, SURFMASK_ALL_CORNER, br, &r);

                        r.nLeft            += bw;
                        r.nTop             += bw;
                        r.nWidth           -= bw * 2;
                        r.nHeight          -= bw * 2;
                        br                  = lsp_max(0, br - bw);

                        if (mi->checked()->get())
                        {
                            color.copy(mi->check_color()->color());
                            color.scale_lightness(bright);
                            s->fill_round_rect(color, SURFMASK_ALL_CORNER, br, &r);
                        }
                    }
                    else
                    {
                        if (mi->checked()->get())
                            color.copy(mi->check_color()->color());
                        else
                            color.copy(mi->check_bg_color()->color());
                        color.scale_lightness(bright);
                        s->fill_round_rect(color, SURFMASK_ALL_CORNER, br, &r);
                    }
                }
                else if (mi->type()->radio())
                {
                    float br            = pi->check.nWidth * 0.5f;
                    float xc            = pi->check.nLeft + br;
                    float yc            = pi->check.nTop  + br;
                    ssize_t bw          = (sCheckBorder.get() > 0) ? lsp_max(1, sCheckBorder.get() * scaling) : 0;

                    if (bw > 0)
                    {
                        color.copy(mi->check_border_color()->color());
                        color.scale_lightness(bright);
                        s->fill_circle(xc, yc, br, &color);
                        br                  = lsp_max(0.0f, br - bw);

                        color.copy(mi->check_bg_color()->color());
                        color.scale_lightness(bright);
                        s->fill_circle(xc, yc, br, &color);
                        br                  = lsp_max(0, br - bw);

                        if (mi->checked()->get())
                        {
                            color.copy(mi->check_color()->color());
                            color.scale_lightness(bright);
                            s->fill_circle(xc, yc, br, &color);
                        }
                    }
                    else
                    {
                        if (mi->checked()->get())
                            color.copy(mi->check_color()->color());
                        else
                            color.copy(mi->check_bg_color()->color());
                        color.scale_lightness(bright);
                        s->fill_circle(xc, yc, br, &color);
                    }

                }
            }

            // Draw scroll button background
            s->set_antialiasing(false);
            if (sUp.visibility()->get())
            {
                color.copy((sUp.active())   ? sScrollSelectedColor.color() : sScrollColor.color());
                color.scale_lightness(bright);
                sUp.get_rectangle(&xr);
                s->fill_rect(color, &xr);
            }
            if (sDown.visibility()->get())
            {
                color.copy((sDown.active()) ? sScrollSelectedColor.color() : sScrollColor.color());
                color.scale_lightness(bright);
                sDown.get_rectangle(&xr);
                s->fill_rect(color, &xr);
            }

            // Draw scroll button text
            s->set_antialiasing(true);

            // Draw scroll buttons
            if (sUp.visibility()->get())
            {
                color.copy((sUp.active())   ? sScrollTextSelectedColor.color() : sScrollTextColor.color());
                color.scale_lightness(bright);
                sUp.get_rectangle(&xr);

                float x = xr.nLeft + xr.nWidth * 0.5f;
                float y = xr.nTop;

                s->fill_triangle(
                        x, y + xr.nHeight * 0.25f,
                        x + xr.nHeight, y + xr.nHeight * 0.75f,
                        x - xr.nHeight, y + xr.nHeight * 0.75f,
                        color
                    );
            }
            if (sDown.visibility()->get())
            {
                color.copy((sDown.active()) ? sScrollTextSelectedColor.color() : sScrollTextColor.color());
                color.scale_lightness(bright);
                sDown.get_rectangle(&xr);

                float x = xr.nLeft + xr.nWidth * 0.5f;
                float y = xr.nTop;

                s->fill_triangle(
                        x, xr.nTop + xr.nHeight * 0.75f,
                        x - xr.nHeight, y + xr.nHeight * 0.25f,
                        x + xr.nHeight, y + xr.nHeight * 0.25f,
                        color
                    );
            }

            s->set_antialiasing(false);

            // Draw border
            if (border > 0)
            {
                color.copy(sBorderColor);
                color.scale_lightness(bright);
                s->fill_frame(
                    color,
                    0, 0, sSize.nWidth, sSize.nHeight,
                    border, border, sSize.nWidth - border * 2, sSize.nHeight - border * 2
                );
            }

            s->set_antialiasing(aa);
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

                    status_t res = (vItems.remove(i)) ? STATUS_OK : STATUS_UNKNOWN_ERR;
                    if (res == STATUS_OK)
                        unlink_widget(item);

                    return res;
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
            Widget::show();
        }

        void Menu::show(Widget *w)
        {
            if (w == NULL)
                return;

            ws::rectangle_t r;
            w->get_screen_rectangle(&r);
            sWindow.trigger_widget()->set(w);
            sWindow.trigger_area()->set(&r);
            Widget::show();
        }

        void Menu::showxy(Widget *w)
        {
            if (w == NULL)
                return;

            ws::rectangle_t r;
            w->get_screen_rectangle(&r);
            sWindow.trigger_widget()->set(w);
            sWindow.trigger_area()->set(r.nLeft, r.nTop, 0, 0);
            Widget::show();
        }

        void Menu::show(Widget *w, ssize_t x, ssize_t y)
        {
            sWindow.trigger_widget()->set(w);
            sWindow.trigger_area()->set(x, y, 0, 0);
            Widget::show();
        }

        void Menu::show(Widget *w, ssize_t x, ssize_t y, ssize_t xw, ssize_t xh)
        {
            sWindow.trigger_widget()->set(w);
            sWindow.trigger_area()->set(x, y, xw, xh);
            Widget::show();
        }

        void Menu::show(Widget *w, const ws::rectangle_t *r)
        {
            sWindow.trigger_widget()->set(w);
            sWindow.trigger_area()->set(r);
            Widget::show();
        }

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

            // Take focus if there is no parent menu
            if (pParentMenu == NULL)
            {
                sWindow.grab_events(ws::GRAB_MENU);
                sWindow.take_focus();
            }
        }

        void Menu::hide_widget()
        {
            nSelected = -1;

            hide_nested_menus(this);
            sWindow.hide();
        }

        Widget *Menu::find_widget(ssize_t x, ssize_t y)
        {
            // Handle special buttons
            if ((sUp.is_visible_child_of(this)) && (sUp.inside(x, y)))
                return &sUp;
            if ((sDown.is_visible_child_of(this)) && (sDown.inside(x, y)))
                return &sDown;

            for (size_t i=0, n=vVisible.size(); i<n; ++i)
            {
                item_t *pi = vVisible.uget(i);
                if ((pi->item->valid()) && (pi->item->inside(x, y)))
                    return pi->item;
            }
            return NULL;
        }

        void Menu::select_menu_item(MenuItem *item, bool popup)
        {
            ssize_t sel = -1;
            for (size_t i=0, n=vVisible.size(); i<n; ++i)
            {
                item_t *pi = vVisible.uget(i);
                if (pi->item == item)
                {
                    sel = i;
                    break;
                }
            }

            select_menu_item(sel, popup);
        }

        void Menu::select_menu_item(ssize_t sel, bool popup)
        {
            sWindow.take_focus();

            lsp_trace("sel = %d, nSelected = %d", int(sel), int(nSelected));

            if (sel != nSelected)
            {
                // Update state
                nSelected   = sel;
                query_draw();
            }

            // Need to check for submenu?
            if (popup)
            {
                item_t *it = (sel >= 0) ? vVisible.uget(sel) : NULL;
                MenuItem *item = it->item;
                Menu *cmenu = (item != NULL) ? item->menu()->get() : NULL;
                if (cmenu != NULL)
                    show_submenu(cmenu, item);
                else if (pChildMenu != NULL)
                {
                    Menu *child = pChildMenu;
                    pChildMenu = NULL;
                    child->hide();
                }
            }
        }

        void Menu::select_first_item(bool popup)
        {
            ssize_t sel = -1;
            for (size_t i=0, n=vVisible.size(); i<n; ++i)
            {
                item_t *pi = vVisible.uget(i);
                if ((pi->item != NULL) && (!pi->item->type()->separator()))
                {
                    sel = i;
                    break;
                }
            }

            select_menu_item(sel, popup);
        }

        void Menu::hide_nested_menus(Menu *parent)
        {
            for (Menu *pm = parent; pm != NULL; )
            {
                Menu *cm = pm->pChildMenu;
                if (cm == NULL)
                    break;

                // Unlink menu
                cm->pParentMenu = NULL;
                pm->pChildMenu  = NULL;

                // Hide menu and move forward
                cm->hide();
                pm  = cm;
            }
        }

        void Menu::submit_menu_item(MenuItem *item, bool focus)
        {
            // Obtain child menu
            Menu *cmenu = (item != NULL) ? item->menu()->get() : NULL;
            if (cmenu == NULL)
            {
                // No child menu, just hide root menu and finish
                Menu *root = this;
                while (root->pParentMenu != NULL)
                    root = root->pParentMenu;
                root->hide();
                return;
            }

            show_submenu(cmenu, item);
            if (focus)
                cmenu->select_first_item(false);
        }

        void Menu::show_submenu(Menu *menu, Widget *w)
        {
            // Hide all nested menus for cmenu
            if (pChildMenu != menu)
            {
                if (pChildMenu != NULL)
                    pChildMenu->hide();
            }
            hide_nested_menus(menu);

            menu->pParentMenu   = this;
            pChildMenu          = menu;

            // Show the nested menu
            menu->show(w);
        }

        status_t Menu::on_key_down(const ws::event_t *e)
        {
            switch (e->nCode)
            {
                case ws::WSK_KEYPAD_UP:
                case ws::WSK_UP:
                    nKeyScroll      = -1;
                    on_key_scroll(nKeyScroll);
                    break;

                case ws::WSK_KEYPAD_DOWN:
                case ws::WSK_DOWN:
                    nKeyScroll      = 1;
                    on_key_scroll(nKeyScroll);
                    break;

                default:
                    nKeyScroll      = 0;
                    break;
            }

            if (nKeyScroll != 0)
                sKeyTimer.launch(-1, 250, 1000);

            return STATUS_OK;
        }

        status_t Menu::on_key_up(const ws::event_t *e)
        {
            MenuItem *submit = NULL;

            // Analyze key code
            switch (e->nCode)
            {
                case ws::WSK_KEYPAD_UP:
                case ws::WSK_UP:
                case ws::WSK_KEYPAD_DOWN:
                case ws::WSK_DOWN:
                    nKeyScroll      = 0;
                    break;

                case ws::WSK_RETURN:
                case ws::WSK_KEYPAD_ENTER:
                {
                    item_t *pi      = (nSelected >= 0) ? vVisible.get(nSelected) : NULL;
                    submit          = (pi != NULL) ? pi->item : NULL;

                    nKeyScroll      = 0;
                    break;
                }

                case ws::WSK_LEFT:
                case ws::WSK_KEYPAD_LEFT:
                {
                    Menu *parent = pParentMenu;
                    if (parent != NULL)
                    {
                        hide();
                        parent->sWindow.take_focus();
                    }
                    break;
                }

                case ws::WSK_RIGHT:
                case ws::WSK_KEYPAD_RIGHT:
                {
                    item_t *item = (nSelected >= 0) ? vVisible.get(nSelected) : NULL;
                    MenuItem *sel = (item != NULL) ? item->item : NULL;
                    Menu *child = ((sel != NULL) && (!sel->type()->separator())) ? sel->menu()->get() : NULL;
                    if (child != NULL)
                    {
                        show_submenu(child, sel);
                        child->select_first_item(false);
                    }
                    break;
                }

                case ws::WSK_ESCAPE:
                {
                    Menu *root      = root_menu();
                    if (root != NULL)
                        root->hide();
                    break;
                }

                default:
                    nKeyScroll      = 0;
                    break;
            }

            // Cancel scrolling timer
            if (nKeyScroll == 0)
                sKeyTimer.cancel();

            // Check if we need to trigger submit event
            if (submit != NULL)
            {
                submit_menu_item(submit, true);
                submit->slots()->execute(SLOT_SUBMIT, submit);
            }

            return STATUS_OK;
        }

        status_t Menu::key_scroll_handler(ws::timestamp_t sched, ws::timestamp_t time, void *arg)
        {
            Menu *m = widget_ptrcast<Menu>(arg);
            return (m != NULL) ? m->on_key_scroll(m->nKeyScroll) : STATUS_OK;
        }

        status_t Menu::mouse_scroll_handler(ws::timestamp_t sched, ws::timestamp_t time, void *arg)
        {
            Menu *m = widget_ptrcast<Menu>(arg);
            return (m != NULL) ? m->on_mouse_scroll(m->nMouseScroll) : STATUS_OK;
        }

        status_t Menu::on_key_scroll(ssize_t dir)
        {
            // If we have no potential items for selection - return immediately
            ssize_t last = ssize_t(vVisible.size()) - 1;
            if ((last < 0) || (sIStats.items <= 0) || (dir == 0))
                return STATUS_OK;

            // Lookup for next item in list
            ssize_t pos = nSelected;
            item_t *pi  = NULL;
            while (true)
            {
                // Update position
                pos    += dir;
                if (pos < 0)
                    pos     = last;
                else if (pos > last)
                    pos     = 0;

                // Get item
                pi  = vVisible.uget(pos);
                if (!pi->item->type()->separator())
                    break;
            }

            // Now we have updated position
            if ((pos == nSelected) || (pi == NULL))
                return STATUS_OK;

            // Position has altered
            nSelected = pos;
            pi->item->take_focus();
            sync_scroll(pi->item);
            query_draw();

            return STATUS_OK;
        }

        status_t Menu::on_mouse_scroll(ssize_t dir)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t scroll      = sScrolling.get() * scaling;
            ssize_t amount      = lsp_max(1, sIStats.item_h >> 1);

            lsp_trace("scroll=%d, amount=%d, dir=%d, max=%d", int(scroll), int(amount), int(dir), int(sIStats.max_scroll));

            scroll              = lsp_limit(scroll + dir * amount, 0, sIStats.max_scroll);

            if (scaling > 0)
                sScrolling.set(scroll / scaling);

            return STATUS_OK;
        }

        void Menu::sync_scroll(MenuItem *item)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t scroll      = lsp_max(0, sScrolling.get() * scaling);
            ssize_t border      = lsp_max(0.0f, scaling * sBorderSize.get());

            ws::rectangle_t xr, wr;
            xr.nLeft            = sSize.nLeft   + border;
            xr.nTop             = sSize.nTop    + border;
            xr.nWidth           = sSize.nWidth  - border * 2;
            xr.nHeight          = sSize.nHeight - border * 2;

            // The widget is over the visible area?
            ssize_t new_scroll  = scroll;
            ssize_t top         = xr.nTop;
            ssize_t bottom      = xr.nTop + xr.nHeight;

            if (sUp.visibility()->get())
            {
                sUp.get_rectangle(&wr);
                top                 = wr.nTop + wr.nHeight;
            }
            if (sDown.visibility()->get())
            {
                sDown.get_rectangle(&wr);
                bottom              = wr.nTop;
            }

            item->get_rectangle(&wr);

            if (wr.nTop < top)
                new_scroll         -= top - wr.nTop;
            else if ((wr.nTop + wr.nHeight) > bottom)
                new_scroll         += wr.nTop + wr.nHeight - bottom;

            // Limit scrolling and update
            new_scroll          = lsp_limit(new_scroll, 0, sIStats.max_scroll);
            if ((new_scroll != scroll) && (scaling > 0.0f))
                sScrolling.set(new_scroll / scaling);
        }

        status_t Menu::start_mouse_scroll(ssize_t dir)
        {
            nMouseScroll    = dir;
            sMouseTimer.launch(-1, 25);
            return STATUS_OK;
        }

        status_t Menu::end_mouse_scroll()
        {
            nMouseScroll    = 0;
            sMouseTimer.cancel();
            return STATUS_OK;
        }

    } /* namespace tk */
} /* namespace lsp */
