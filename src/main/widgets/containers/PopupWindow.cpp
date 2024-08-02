/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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
#include <lsp-plug.in/tk/helpers/arrange.h>
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
            status_t res = Window::init_internal(false);
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

        bool PopupWindow::set_tether(const lltl::darray<tether_t> *list)
        {
            return vTether.set(list);
        }

        bool PopupWindow::set_tether(const tether_t *list, size_t count)
        {
            return vTether.set_n(count, list);
        }

        bool PopupWindow::add_tether(const tether_t *item)
        {
            return vTether.add(item);
        }

        bool PopupWindow::add_tether(size_t flags, float halign, float valign)
        {
            tether_t *item      = vTether.add();
            if (item == NULL)
                return false;

            item->nFlags        = flags;
            item->fHAlign       = halign;
            item->fVAlign       = valign;

            return true;
        }

        status_t PopupWindow::post_init()
        {
            // Bind redraw handler
            sRedraw.bind(pDisplay->display());
            sRedraw.set_handler(tmr_redraw_request, self());

            // Don't create native window
            return STATUS_OK;
        }

        status_t PopupWindow::sync_size(bool force)
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
            Widget *w           = sTrgWidget.get();
            ssize_t screen      = sTrgScreen.get();

            sTrgWidget.set(NULL);
            sTrgScreen.set(-1);

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
            arrange_window_geometry();
            pWindow->show((rwnd != NULL) ? rwnd->native() : NULL);
            return true;
        }

        void PopupWindow::arrange_window_geometry()
        {
            // Obtain information about possible regions of placing the window
            lltl::darray<ws::rectangle_t> areas;
            size_t n_monitors = 0;
            const ws::MonitorInfo *mi = pWindow->display()->enum_monitors(&n_monitors);
            if (mi != NULL)
            {
                ws::rectangle_t wa;
                const ws::MonitorInfo *primary = NULL;
                bool has_wa     = pWindow->display()->work_area_geometry(&wa) == STATUS_OK;

                // Add primary monitor first
                for (size_t i=0; i<n_monitors; ++i)
                {
                    if (!mi[i].primary)
                        continue;
                    primary = mi;
                    ws::rectangle_t *r = areas.add(&mi[i].rect);
                    if ((has_wa) && (r != NULL))
                        Size::intersection(r, &wa); // Compute intersection if overlap
                }
                // Add secondary monitors last
                for (size_t i=0; i<n_monitors; ++i)
                {
                    if (mi[i].primary)
                        continue;
                    ws::rectangle_t *r = areas.add(&mi[i].rect);
                    // We need to restrict the working area if monitor is over the primary
                    if ((has_wa) && (r != NULL) && (primary != NULL) && (Size::overlap(r, &primary->rect)))
                        Size::intersection(r, &wa); //  // Compute intersection if overlap
                }
            }
            else
            {
                ws::rectangle_t *r = areas.add();
                if (r != NULL)
                {
                    r->nLeft        = 0;
                    r->nTop         = 0;
                    pWindow->display()->screen_size(pWindow->screen(), &r->nWidth, &r->nHeight);
                }
            }

            // Arrange the window
            ws::rectangle_t trg, wrect;
            ws::size_limit_t sr;
            get_padded_size_limits(&sr);
            sTrgArea.get(&trg);

            arrange_rectangle(
                &wrect, &trg, &sr,
                areas.array(), areas.size(),
                vTether.array(), vTether.size());

            wrect.nWidth    = lsp_max(1, wrect.nWidth);
            wrect.nHeight   = lsp_max(1, wrect.nHeight);

            lsp_trace("window geometry: {%d %d %d %d}", int(wrect.nLeft), int(wrect.nTop), int(wrect.nWidth), int(wrect.nHeight));
            pWindow->set_geometry(&wrect);
            realize(&wrect);
        }

        void PopupWindow::size_request(ws::size_limit_t *r)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());
            size_t border       = lsp_max(0, sBorderSize.get()) * scaling;

            r->nMinWidth        = -1;
            r->nMinHeight       = -1;
            r->nMaxWidth        = -1;
            r->nMaxHeight       = -1;
            r->nPreWidth        = -1;
            r->nPreHeight       = -1;

            if ((pChild != NULL) && (pChild->visibility()->get()))
                pChild->get_padded_size_limits(r);

            // Add self padding
            sPadding.add(r, scaling);

            // Add border padding
            padding_t pad;
            pad.nLeft   = border;
            pad.nRight  = border;
            pad.nTop    = border;
            pad.nBottom = border;
            Padding::add(r, &pad);

            // Window should be at least of 1x1 pixel size
            r->nMinWidth        = lsp_max(1, r->nMinWidth);
            r->nMinHeight       = lsp_max(1, r->nMinHeight);
            if (r->nMaxWidth  >= 0)
                r->nMaxWidth        = lsp_max(r->nMinWidth,  r->nMaxWidth);
            if (r->nMaxHeight >= 0)
                r->nMaxHeight       = lsp_max(r->nMinHeight, r->nMaxHeight);

            lsp_trace("this=%p, w={%d, %d, %d}, h={%d, %d, %d}", this,
                    int(r->nMinWidth), int(r->nMaxWidth), int(r->nPreWidth),
                    int(r->nMinHeight), int(r->nMaxHeight), int(r->nPreHeight)
                );
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
    } /* namespace tk */
} /* namespace lsp */


