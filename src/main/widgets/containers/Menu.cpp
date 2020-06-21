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
            sScrolling(&sProperties),
            sBorderSize(&sProperties),
            sBorderColor(&sProperties),
            sCheckSize(&sProperties),
            sCheckBorder(&sProperties),
            sCheckBorderGap(&sProperties),
            sCheckBorderRadius(&sProperties),
            sSeparatorWidth(&sProperties),
            sSpacing(&sProperties),
            sSubmenu(&Menu::metadata, &sProperties)
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

            nSelected               = -1;

            sIStats.full_w          = 0;
            sIStats.full_h          = 0;
            sIStats.item_w          = 0;
            sIStats.item_h          = 0;
            sIStats.ckbox           = false;
            sIStats.shortcut        = false;
            sIStats.submenu         = false;

            sUp.sPos.nLeft          = 0;
            sUp.sPos.nTop           = 0;
            sUp.sPos.nWidth         = 0;
            sUp.sPos.nHeight        = 0;
            sUp.bEnabled            = false;

            sDown.sPos.nLeft        = 0;
            sDown.sPos.nTop         = 0;
            sDown.sPos.nWidth       = 0;
            sDown.sPos.nHeight      = 0;
            sDown.bEnabled          = false;

            pClass                  = &metadata;
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
            sWindow.layout()->set(-1.0f, -1.0f, 1.0f, 1.0f);

            // Bind properties
            sFont.bind("font", &sStyle);
            sScrolling.bind("scrolling", &sStyle);
            sBorderSize.bind("border.size", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sCheckSize.bind("check.size", &sStyle);
            sCheckBorder.bind("check.border", &sStyle);
            sCheckBorderGap.bind("check.border.gap", &sStyle);
            sCheckBorderRadius.bind("check.border.radius", &sStyle);
            sSeparatorWidth.bind("separator.width", &sStyle);
            sSpacing.bind("spacing", &sStyle);
            sSubmenu.bind(NULL);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sFont.init(sclass);
                sScrolling.init(sclass, 0.0f);
                sBorderSize.init(sclass, 1);
                sBorderColor.init(sclass, "#000000");
                sCheckSize.init(sclass, 12);
                sCheckBorder.init(sclass, 1);
                sCheckBorderGap.init(sclass, 1);
                sCheckBorderRadius.init(sclass, 4);
                sSeparatorWidth.init(sclass, 1);
                sSpacing.init(sclass, 2);

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
            r->nMinWidth    = st.full_w + border*2;
            r->nMinHeight   = st.item_h + border*2;
            r->nMaxWidth    = r->nMinWidth;
            r->nMaxHeight   = st.full_h + border*2;
        }

        void Menu::allocate_items(lltl::darray<item_t> *out, istats_t *st)
        {
            float scaling   = lsp_max(0.0f, sScaling.get());
            ssize_t spacing = lsp_max(0.0f, scaling * sSpacing.get());

            st->full_w      = 0;
            st->full_h      = 0;
            st->item_w      = 0;
            st->item_h      = 0;
            st->ckbox       = false;
            st->shortcut    = false;
            st->submenu     = false;

            // Size of check box/radio
            ssize_t cb_br       = lsp_max(0, sCheckBorderRadius.get() * scaling) * 3;
            ssize_t cb_size     = (sCheckBorder.get() > 0) ? lsp_min(1.0f, sCheckBorder.get() * scaling) : 0;
            if (cb_size > 0)
                cb_size            += (sCheckBorderGap.get() > 0) ? lsp_min(1.0f, sCheckBorderGap.get() * scaling) : 0;
            cb_size            += lsp_min(2.0f, sCheckSize.get() * scaling);
            cb_size             = lsp_max(cb_size, cb_br);

            // Size of text, shortcut and reference
            LSPString s;
            ws::font_parameters_t fp;
            ws::text_parameters_t tp;
            sFont.get_parameters(pDisplay, scaling, &fp);

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
                menu_item_type_t mt = pi->item->type()->get();
                bool xsep           = !(mt == MI_SEPARATOR);
                bool check          = (mt == MI_CHECK) || (mt == MI_RADIO);

                // Reduce padding
                if (xsep)
                {
                    pi->pad.nLeft     >>= 2;
                    pi->pad.nRight    >>= 2;
                }

                // Estimate size of caption
                pi->text.nLeft      = 0;
                pi->text.nTop       = 0;
                pi->area.nLeft      = 0;
                pi->area.nTop       = 0;

                if (xsep)
                {
                    mi->text()->format(&s);
                    sFont.get_text_parameters(pDisplay, &tp, scaling, &s);

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
                if (check)
                {
                    st->ckbox           = true;
                    pi->check.nWidth    = cb_size;
                    pi->check.nHeight   = cb_size;
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
                if ((xsep) && (mi->shortcut()->valid()))
                {
                    mi->shortcut()->format(&s);
                    sFont.get_text_parameters(pDisplay, &tp, scaling, &s);

                    st->shortcut        = true;
                    pi->scut.nWidth     = tp.Width;
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
                if ((xsep) && (mi->menu()->is_set()))
                {
                    st->submenu         = true;
                    pi->ref.nHeight     = fp.Height;
                    pi->ref.nWidth      = lsp_max(2.0f, M_SQRT1_2 * fp.Height);

                    pi->area.nWidth    += pi->ref.nWidth + spacing;
                    pi->area.nHeight    = lsp_max(pi->area.nHeight, pi->ref.nHeight);
                }
                else
                {
                    pi->ref.nWidth      = 0;
                    pi->ref.nHeight     = 0;
                }

                // Apply padding
                pi->area.nWidth        += pi->pad.nLeft + pi->pad.nTop;
                pi->area.nHeight       += pi->pad.nTop  + pi->pad.nBottom;
            }
        }

        void Menu::realize(const ws::rectangle_t *r)
        {
            WidgetContainer::realize(r);

            padding_t pad;
            ws::rectangle_t rr;
            isizes_t sz;
            estimate_sizes(&sz);

            // Estimate scrolling position
            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t scroll      = lsp_max(0, sScrolling.get() * scaling);
            ssize_t border      = lsp_max(0.0f, scaling * sBorderSize.get());
            sPadding.compute(&pad, scaling);

            rr.nLeft            = border;
            rr.nTop             = border;
            rr.nWidth           = r->nWidth  - border * 2;
            rr.nHeight          = r->nHeight - border * 2;

            // Re-compute size limits
            ssize_t spaces      = lsp_max(0, ssize_t(sz.items + sz.separators) - 1);
            ssize_t max_height  = sz.m_height * sz.items + sz.s_height * sz.separators + spaces * sz.vspacing;
            max_height         += pad.nTop + pad.nBottom;
            sz.max_scroll       = lsp_max(0, max_height - rr.nHeight);

            if ((scroll > sz.max_scroll) && (scaling > 0.0f))
            {
                scroll              = sz.max_scroll;
                sScrolling.commit(scroll / scaling);
            }

            // Allocate space for buttons
            sUp.sPos.nLeft      = rr.nLeft;
            sUp.sPos.nTop       = rr.nTop;
            sUp.sPos.nWidth     = rr.nWidth;
            sUp.sPos.nHeight    = lsp_min(4, sz.m_height >> 1);
            sUp.bEnabled        = (scroll > 0);

            sDown.sPos.nWidth   = rr.nWidth;
            sDown.sPos.nHeight  = lsp_min(4, sz.m_height >> 1);
            sDown.sPos.nLeft    = rr.nLeft;
            sDown.sPos.nTop     = rr.nTop + rr.nHeight - sDown.sPos.nHeight;
            sDown.bEnabled      = (scroll < sz.max_scroll);

            // Now allocate position of each visible widget
            rr.nTop            += pad.nTop - scroll;

            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                MenuItem *mi        = vItems.uget(i);
                if ((mi == NULL) || (!mi->visibility()->get()))
                    continue;

                if (mi->type()->separator())
                    rr.nHeight      = sz.s_height;
                else
                    rr.nHeight      = sz.m_height;

                // Realize the widget
                mi->realize(&rr);

                // Update position
                rr.nTop        += rr.nHeight + sz.vspacing;
            }

            // Remember drawing parameters
            sISizes             = sz;
        }

        void Menu::draw(ws::ISurface *s)
        {
            padding_t pad;
            ws::rectangle_t xr, r;
            float scaling       = lsp_max(0.0f, sScaling.get());
            float bright        = sBrightness.get();
            ssize_t border      = lsp_max(0, sBorderSize.get() * scaling);
            sPadding.compute(&pad, scaling);

            xr.nLeft            = border;
            xr.nTop             = border;
            xr.nWidth           = sSize.nWidth  - border * 2;
            xr.nHeight          = sSize.nHeight - border * 2;

            // Draw background
            lsp::Color color(sBgColor);
            s->clear(color);

            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                MenuItem *mi    = vItems.uget(i);
                if ((mi == NULL) || (!mi->visibility()->get()))
                    continue;

                // Do not draw invisible items
                mi->get_rectangle(&r);
                if (!Size::overlap(&xr, &r))
                    continue;

                // Just separator?
                if (mi->type()->separator())
                {
                    r.nLeft        += pad.nLeft;
                    r.nWidth       -= pad.nLeft + pad.nRight;
                    r.nTop         += 2 * scaling;
                    r.nHeight       = sISizes.sp_thick;

                    color.copy(mi->text_color()->color());
                    color.scale_lightness(bright);
                    s->fill_rect(color, &r);
                    continue;
                }

                // More complicated element

            }

            /*
            // Prepare palette
            Color bg_color(sBgColor);
            Color border(sBorderColor);
            Color font(sFont.raw_color());
            Color sel(sSelColor);
            Color tmp;

            border.scale_lightness(brightness());
            font.scale_lightness(brightness());
            sel.scale_lightness(brightness());



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

            */

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

        void Menu::hide_widget()
        {
            sWindow.hide();
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
