/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 16 июн. 2020 г.
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
#include <lsp-plug.in/common/debug.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t PopupWindow::metadata       = { "PopupWindow", &Window::metadata };

        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(PopupWindow, Window)
                // Bind
                sTrgArea.bind("trigger.area", this);
                sTrgScreen.bind("trigger.screen", this);
                sAutoClose.bind("close.auto", this);
                // Configure
                sTrgArea.set(0, 0, 0, 0);
                sTrgScreen.set(-1);
                sAutoClose.set(true);
                // Override
                sBorderStyle.set(ws::BS_POPUP);
                sActions.set_actions(ws::WA_POPUP);
                // Commit
                sBorderStyle.override();
                sActions.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(PopupWindow, "PopupWindow", "root");
        }

        PopupWindow::PopupWindow(Display *dpy):
            Window(dpy, NULL, -1),
            sTrgArea(&sProperties),
            sTrgWidget(&sProperties),
            sTrgScreen(&sProperties),
            sAutoClose(&sProperties)
        {
            bInitialized    = false;
            pClass          = &metadata;
        }

        PopupWindow::~PopupWindow()
        {
            nFlags     |= FINALIZED;
        }

        status_t PopupWindow::init()
        {
            status_t res = Window::init();
            if (res != STATUS_OK)
                return res;

            sTrgArea.bind("trigger.area", &sStyle);
            sTrgWidget.bind(NULL);
            sTrgScreen.bind("trigger.screen", &sStyle);
            sAutoClose.bind("close.auto", &sStyle);

            // Mark as initialized
            bInitialized = true;
            property_changed(&sVisibility); // Emit property change event

            return STATUS_OK;
        }

        bool PopupWindow::set_arrangements(const lltl::darray<arrangement_t> *list)
        {
            return vArrangements.set(list);
        }

        bool PopupWindow::set_arrangements(const arrangement_t *list, size_t count)
        {
            return vArrangements.set_n(count, list);
        }

        bool PopupWindow::add_arrangement(const arrangement_t *item)
        {
            return vArrangements.add(item);
        }

        bool PopupWindow::add_arrangement(arrangement_pos_t pos, float align, bool stretch)
        {
            arrangement_t item;
            item.enPosition     = pos;
            item.fAlign         = align;
            item.bStretch       = stretch;

            return add_arrangement(&item);
        }

        status_t PopupWindow::post_init()
        {
            // Bind redraw handler
            sRedraw.bind(pDisplay->display());
            sRedraw.set_handler(tmr_redraw_request, self());

            // Don't create native window
            return STATUS_OK;
        }

        status_t PopupWindow::sync_size()
        {
            ws::size_limit_t sr;
            ws::rectangle_t xr;
            int changes = 0;

            get_padded_size_limits(&sr);
            get_rectangle(&xr);

            if ((sr.nMinWidth >= 0) && (sr.nMinWidth > xr.nWidth))
            {
                ++changes;
                xr.nWidth   = sr.nMinWidth;
            }
            if ((sr.nMinHeight >= 0) && (sr.nMinHeight > xr.nHeight))
            {
                ++changes;
                xr.nHeight  = sr.nMinHeight;
            }

            if (changes > 0)
                pWindow->resize(xr.nWidth, xr.nHeight);

            WidgetContainer::realize_widget(&xr);

            return STATUS_OK;
        }

        void PopupWindow::hide_widget()
        {
            if (!bInitialized)
                return;

            Window::hide_widget();

            // Destroy window
            if (pWindow != NULL)
            {
                pWindow->destroy();
                delete pWindow;
                pWindow = NULL;
            }
        }

        void PopupWindow::show_widget()
        {
            if (!bInitialized)
                return;

            // Window wont'be shown since it's NULL
            Window::show_widget();

            if (!init_window())
                sVisibility.set(false);
        }

        bool PopupWindow::init_window()
        {
            // Passed argument
            ws::rectangle_t trg;
            Widget *w           = sTrgWidget.get();
            ssize_t screen      = sTrgScreen.get();

            sTrgWidget.set(NULL);
            sTrgScreen.set(-1);
            sTrgArea.get(&trg);

            ws::IDisplay *dpy   = pDisplay->display();

            // Estimate the target screen to show the window
            Window *rwnd    = (w != NULL) ? widget_cast<Window>(w->toplevel()) : NULL;
            if ((rwnd != NULL) && (screen < 0))
                screen          = rwnd->screen();
            if ((screen < 0) || (screen >= ssize_t(dpy->screens())))
                screen      = dpy->default_screen();

            // Destroy the window if it does not match requirements
            if ((pWindow != NULL) && (pWindow->screen() != size_t(screen)))
            {
                pWindow->destroy();
                delete pWindow;
                pWindow = NULL;
            }

            // Now we are ready to create window
            ws::IWindow *wnd = pWindow;
            if (wnd == NULL)
            {
                // Create window
                wnd = dpy->create_window(screen);
                if (wnd == NULL)
                    return false;

                // Initialize window
                status_t result = wnd->init();
                if (result != STATUS_OK)
                {
                    wnd->destroy();
                    delete wnd;
                    return false;
                }

                wnd->set_handler(this);
                wnd->set_border_style(sBorderStyle.get());
                wnd->set_window_actions(sActions.get_all());
                pWindow = wnd;
            }

            // Window has been created, adjust position
            bool arranged = false;
            ws::rectangle_t wrect;

            // Perform forced preferred arrange
            for (size_t i=0, n=vArrangements.size(); i<n; ++i)
            {
                arrangement_t *ar = vArrangements.uget(i);
                if ((arranged = arrange_preferred(&wrect, &trg, ar, true)))
                    break;
            }

            // Perform non-forced preferred arrange
            for (size_t i=0, n=vArrangements.size(); i<n; ++i)
            {
                arrangement_t *ar = vArrangements.uget(i);
                if ((arranged = arrange_preferred(&wrect, &trg, ar, false)))
                    break;
            }

            // Perform flexible arrange
            if (!arranged)
            {
                for (size_t i=0, n=vArrangements.size(); i<n; ++i)
                {
                    arrangement_t *ar = vArrangements.uget(i);
                    if ((arranged = arrange_window(&wrect, &trg, ar, false)))
                        break;
                }
            }

            // Perform flexible arrange with force flag
            if (!arranged)
            {
                for (size_t i=0, n=vArrangements.size(); i<n; ++i)
                {
                    arrangement_t *ar = vArrangements.uget(i);
                    if ((arranged = arrange_window(&wrect, &trg, ar, true)))
                        break;
                }
            }

            // Perform forced arrange
            if ((!arranged) && (vArrangements.size() > 0))
            {
                arrangement_t *ar = vArrangements.uget(0);
                forced_arrange(&wrect, &trg, ar);
                arranged = true;
            }

            // Check arrange result
            if (!arranged)
            {
                arrangement_t ar;
                ar.enPosition   = A_BOTTOM;
                ar.fAlign       = 0.0f;
                ar.bStretch     = true;

                trg.nWidth      = 0;
                trg.nHeight     = 0;

                forced_arrange(&wrect, &trg, &ar);
            }

            lsp_trace("window geometry: {%d %d %d %d}", int(wrect.nLeft), int(wrect.nTop), int(wrect.nWidth), int(wrect.nHeight));
            pWindow->set_geometry(&wrect);
            realize(&wrect);
            pWindow->show((rwnd != NULL) ? rwnd->native() : NULL);
            return true;
        }

        void PopupWindow::size_request(ws::size_limit_t *r)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());
            size_t border       = lsp_max(0, sBorderSize.get()) * scaling;

            r->nMinWidth        = border * 2;
            r->nMinHeight       = r->nMinWidth;
            r->nMaxWidth        = r->nMinWidth;
            r->nMaxHeight       = r->nMinWidth;
            r->nPreWidth        = r->nMinWidth;
            r->nPreHeight       = r->nMinWidth;

            if ((pChild != NULL) && (pChild->visibility()->get()))
            {
                ws::size_limit_t cr;
                pChild->get_padded_size_limits(&cr);

                if ((cr.nMaxWidth < 0) || (cr.nMaxWidth < cr.nMinWidth))
                    cr.nMaxWidth    = cr.nMinWidth;
                if ((cr.nMaxHeight < 0) || (cr.nMaxHeight < cr.nMinHeight))
                    cr.nMaxHeight   = cr.nMinHeight;

                r->nMinWidth       += lsp_max(0, cr.nMinWidth);
                r->nMinHeight      += lsp_max(0, cr.nMinHeight);
                r->nMaxWidth       += lsp_max(0, cr.nMaxWidth);
                r->nMaxHeight      += lsp_max(0, cr.nMaxHeight);
                r->nPreWidth       += lsp_max(0, cr.nPreWidth);
                r->nPreHeight      += lsp_max(0, cr.nPreHeight);
            }

            // Window should be at least of 1x1 pixel size
            r->nMinWidth        = lsp_max(1, r->nMinWidth);
            r->nMinHeight       = lsp_max(1, r->nMinHeight);
            r->nMaxWidth        = lsp_max(1, r->nMaxWidth);
            r->nMaxHeight       = lsp_max(1, r->nMaxHeight);
            r->nPreWidth        = lsp_max(1, r->nPreWidth);
            r->nPreHeight       = lsp_max(1, r->nPreHeight);

            // Add self padding
            sPadding.add(r, scaling);

//            lsp_trace("this=%p, w={%d, %d, %d}, h={%d, %d, %d}", this,
//                    int(r->nMinWidth), int(r->nMaxWidth), int(r->nPreWidth),
//                    int(r->nMinHeight), int(r->nMaxHeight), int(r->nPreHeight)
//                );
        }

        bool PopupWindow::arrange_preferred(ws::rectangle_t *dst, const ws::rectangle_t *trg, const arrangement_t *ar, bool force)
        {
            ws::rectangle_t ws;
            ws::size_limit_t sr;
            ssize_t sw, sh, end;
            get_padded_size_limits(&sr);

            // Obtain screen size
            ws::IDisplay *dpy   = pDisplay->display();
            size_t screen       = pWindow->screen();
            dpy->screen_size(screen, &sw, &sh);

            ws.nLeft    = trg->nLeft;
            ws.nTop     = trg->nTop;
            ws.nWidth   = sr.nPreWidth;
            ws.nHeight  = sr.nPreHeight;
            float align = lsp_limit(ar->fAlign, -1.0f, 1.0f);

            switch (ar->enPosition)
            {
                case A_BOTTOM:
                    if (ws.nHeight < 0) // No preferred height?
                        return false;
                    if (ws.nWidth < 0)
                        ws.nWidth   = sr.nMinWidth;

                    ws.nTop    += trg->nHeight;
                    if ((force) && (ar->bStretch) && (ws.nWidth < trg->nWidth))
                        ws.nWidth   = trg->nWidth;
                    break;

                case A_TOP:
                    if (ws.nHeight < 0) // No preferred height?
                        return false;
                    if (ws.nWidth < 0)
                        ws.nWidth   = sr.nMinWidth;

                    ws.nTop     = trg->nTop - ws.nHeight;
                    if ((force) && (ar->bStretch) && (ws.nWidth < trg->nWidth))
                        ws.nWidth   = trg->nWidth;
                    break;

                case A_LEFT:
                    if (ws.nWidth < 0) // No preferred width?
                        return false;
                    if (ws.nHeight < 0)
                        ws.nWidth   = sr.nMinHeight;

                    ws.nLeft    = trg->nLeft - ws.nWidth;
                    if ((force) && (ar->bStretch) && (ws.nHeight < trg->nHeight))
                        ws.nHeight = trg->nHeight;
                    break;

                case A_RIGHT:
                    if (ws.nWidth < 0) // No preferred width?
                        return false;
                    if (ws.nHeight < 0)
                        ws.nWidth   = sr.nMinHeight;

                    ws.nLeft   += trg->nWidth;
                    if ((force) && (ar->bStretch) && (ws.nHeight < trg->nHeight))
                        ws.nHeight = trg->nHeight;
                    break;

                default:
                    return false;
            }

            // Window should be at least of 1x1 size
            ws.nWidth   = lsp_max(1, ws.nWidth);
            ws.nHeight  = lsp_max(1, ws.nHeight);

            // Check limits
            if (ws.nLeft < 0)
                return false;
            if (ws.nTop < 0)
                return false;
            if ((ws.nLeft + ws.nWidth ) > sw)
                return false;
            if ((ws.nTop  + ws.nHeight) > sh)
                return false;

            // Perform alignment
            switch (ar->enPosition)
            {
                case A_TOP:
                case A_BOTTOM:
                    // Adjust horizontal position
                    if ((ar->bStretch) && (ws.nWidth < trg->nWidth))
                        ws.nWidth   = trg->nWidth;
                    if (ws.nWidth < sr.nMinWidth)
                        return false;

                    ws.nLeft   += ws.nWidth * align;
                    end         = ws.nLeft + ws.nWidth;
                    if (ws.nLeft < 0)
                    {
                        ws.nLeft    = 0;
                        end         = ws.nLeft + ws.nWidth;
                        if (end > sw)
                            ws.nWidth  = sw;
                    }
                    else if (end > sw)
                    {
                        ws.nLeft   -= (end - sw);
                        end         = sw;
                        if (ws.nLeft < 0)
                        {
                            ws.nWidth   = sw;
                            ws.nLeft    = 0;
                        }
                    }
                    break;

                case A_LEFT:
                case A_RIGHT:
                    if ((ar->bStretch) && (ws.nHeight < trg->nHeight))
                        ws.nHeight = trg->nHeight;
                    if (ws.nHeight < sr.nMinHeight)
                        return false;

                    ws.nTop    += ws.nHeight * align;
                    end         = ws.nTop + ws.nHeight;
                    if (ws.nTop < 0)
                    {
                        ws.nTop     = 0;
                        end         = ws.nTop + ws.nHeight;
                        if (end > sh)
                            ws.nHeight = sh;
                    }
                    else if (end > sh)
                    {
                        ws.nTop    -= (end - sh);
                        end         = sh;
                        if (ws.nTop < 0)
                        {
                            ws.nHeight  = sh;
                            ws.nTop     = 0;
                        }
                    }
                    break;

                default:
                    return false;
            }

            *dst = ws;
            return true;
        }

        bool PopupWindow::arrange_window(ws::rectangle_t *dst, const ws::rectangle_t *trg, const arrangement_t *ar, bool force)
        {
            ws::rectangle_t ws;
            ws::size_limit_t sr;
            ssize_t sw, sh, gap, end;
            get_padded_size_limits(&sr);

            // Obtain screen size
            ws::IDisplay *dpy   = pDisplay->display();
            size_t screen       = pWindow->screen();
            dpy->screen_size(screen, &sw, &sh);

            ws.nLeft    = trg->nLeft;
            ws.nTop     = trg->nTop;
            ws.nWidth   = (sr.nMaxWidth >= 0) ? sr.nMaxWidth : sr.nMinWidth;
            ws.nHeight  = (sr.nMaxHeight >= 0) ? sr.nMaxHeight : sr.nMinHeight;
            float align = lsp_limit(ar->fAlign, -1.0f, 1.0f);

            switch (ar->enPosition)
            {
                case A_BOTTOM:
                    ws.nTop    += trg->nHeight;
                    gap         = sh - ws.nTop;

                    ws.nWidth   = sr.nMinWidth;
                    if (sr.nMaxHeight >= 0)
                        ws.nHeight  =  lsp_min(gap, sr.nMaxHeight);
                    if (ws.nHeight < sr.nMinHeight)
                    {
                        if (!force)
                            return false;
                        ws.nTop    -= (sr.nMinHeight - ws.nHeight);
                        ws.nHeight  = sr.nMinHeight;
                    }
                    break;

                case A_TOP:
                    gap         = trg->nTop;
                    if (sr.nMaxHeight >= 0)
                        ws.nHeight  =  lsp_min(gap, sr.nMaxHeight);
                    if (ws.nHeight < sr.nMinHeight)
                    {
                        if (!force)
                            return false;
                        ws.nHeight  = sr.nMinHeight;
                    }
                    if ((ar->bStretch) && (ws.nWidth < trg->nWidth))
                        ws.nWidth   = trg->nWidth;
                    ws.nTop     = trg->nTop - ws.nHeight;
                    break;

                case A_LEFT:
                    gap         = trg->nLeft;
                    if (sr.nMaxWidth >= 0)
                        ws.nWidth   =  lsp_min(gap, sr.nMaxWidth);
                    if (ws.nWidth < sr.nMinWidth)
                    {
                        if (!force)
                            return false;
                        ws.nWidth   = sr.nMinWidth;
                    }
                    if ((ar->bStretch) && (ws.nWidth < trg->nWidth))
                        ws.nWidth  = trg->nWidth;
                    ws.nLeft    = trg->nLeft - ws.nWidth;
                    break;

                case A_RIGHT:
                    ws.nLeft   += trg->nWidth;
                    gap         = sw - ws.nLeft;
                    if (sr.nMaxWidth >= 0)
                        ws.nWidth   =  lsp_min(gap, sr.nMaxWidth);
                    if (ws.nWidth < sr.nMinWidth)
                    {
                        if (!force)
                            return false;
                        ws.nLeft   -= (sr.nMinWidth - ws.nWidth);
                        ws.nWidth   = sr.nMinWidth;
                    }
                    if ((ar->bStretch) && (ws.nWidth < trg->nWidth))
                        ws.nWidth  = trg->nWidth;
                    break;

                default:
                    return false;
            }

            // Window should be at least of 1x1 size
            ws.nWidth   = lsp_max(1, ws.nWidth);
            ws.nHeight  = lsp_max(1, ws.nHeight);

            switch (ar->enPosition)
            {
                case A_TOP:
                case A_BOTTOM:
                    // Adjust horizontal position
                    if ((ar->bStretch) && (ws.nWidth < trg->nWidth))
                        ws.nWidth   = trg->nWidth;
                    if (ws.nWidth < sr.nMinWidth)
                        return false;

                    ws.nLeft   += ws.nWidth * align;
                    end         = ws.nLeft + ws.nWidth;
                    if (ws.nLeft < 0)
                    {
                        ws.nLeft    = 0;
                        end         = ws.nLeft + ws.nWidth;
                        if (end > sw)
                            ws.nWidth  = sw;
                    }
                    else if (end > sw)
                    {
                        ws.nLeft   -= (end - sw);
                        end         = sw;
                        if (ws.nLeft < 0)
                        {
                            ws.nWidth   = sw;
                            ws.nLeft    = 0;
                        }
                    }
                    break;

                case A_LEFT:
                case A_RIGHT:
                    if ((ar->bStretch) && (ws.nHeight < trg->nHeight))
                        ws.nHeight = trg->nHeight;
                    if (ws.nHeight < sr.nMinHeight)
                        return false;

                    ws.nTop    += ws.nHeight * align;
                    end         = ws.nTop + ws.nHeight;
                    if (ws.nTop < 0)
                    {
                        ws.nTop     = 0;
                        end         = ws.nTop + ws.nHeight;
                        if (end > sh)
                            ws.nHeight = sh;
                    }
                    else if (end > sh)
                    {
                        ws.nTop    -= (end - sh);
                        end         = sh;
                        if (ws.nTop < 0)
                        {
                            ws.nHeight  = sh;
                            ws.nTop     = 0;
                        }
                    }
                    break;

                default:
                    return false;
            }

            // Return geometry of the window
            *dst = ws;
            return true;
        }

        void PopupWindow::forced_arrange(ws::rectangle_t *dst, const ws::rectangle_t *trg, const arrangement_t *ar)
        {
            ws::rectangle_t ws;
            ws::size_limit_t sr;
            get_padded_size_limits(&sr);

            // Obtain screen size
            ws::IDisplay *dpy   = pDisplay->display();
            size_t screen       = pWindow->screen();
            ssize_t sw, sh;
            dpy->screen_size(screen, &sw, &sh);

            // Window should be at least of 1x1 size but not greater than available screen size
            ws.nWidth   = lsp_limit(sr.nMinWidth,  1, sw);
            ws.nHeight  = lsp_limit(sr.nMinHeight, 1, sh);

            switch (ar->enPosition)
            {
                case A_BOTTOM:
                    ws.nTop     = trg->nTop + trg->nHeight;
                    ws.nLeft    = trg->nLeft;
                    break;

                case A_TOP:
                    ws.nTop     = trg->nTop - ws.nHeight;
                    ws.nLeft    = trg->nLeft;
                    break;

                case A_LEFT:
                    ws.nLeft    = trg->nLeft - ws.nWidth;
                    ws.nTop     = trg->nTop;
                    break;

                case A_RIGHT:
                    ws.nLeft    = trg->nLeft + trg->nWidth;
                    ws.nTop     = trg->nTop;
                    break;

                default:
                    return;
            }

            // Apply position
            if (ws.nLeft < 0)
                ws.nLeft        = 0;
            if ((ws.nLeft + ws.nWidth) > sw)
                ws.nLeft       -= (ws.nLeft + ws.nWidth - sw);
            if (ws.nTop < 0)
                ws.nTop         = 0;
            if ((ws.nTop + ws.nHeight) > sh)
                ws.nTop        -= (ws.nTop + ws.nHeight - sh);

            // Return geometry of the window
            *dst    = ws;
        }

        status_t PopupWindow::handle_event(const ws::event_t *e)
        {
            if ((e->nType == ws::UIE_MOUSE_DOWN) && (!(e->nState & ws::MCF_BTN_MASK)) && (sAutoClose.get()))
            {
                if ((e->nLeft < 0) ||
                    (e->nTop < 0) ||
                    (e->nLeft >= sSize.nWidth) ||
                    (e->nTop >= sSize.nHeight))
                    sVisibility.set(false);
            }

            return Window::handle_event(e);
        }
    }
}


