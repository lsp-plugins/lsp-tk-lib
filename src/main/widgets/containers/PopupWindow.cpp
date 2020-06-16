/*
 * PopupWindow.cpp
 *
 *  Created on: 16 июн. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace tk
    {

        PopupWindow::PopupWindow(Display *dpy):
            Window(dpy, NULL, -1),
            sTrgArea(&sProperties),
            sTrgWidget(&Widget::metadata, &sProperties),
            sTrgScreen(&sProperties)
        {
        }

        PopupWindow::~PopupWindow()
        {
        }

        status_t PopupWindow::init()
        {
            status_t res = Window::init();
            if (res != STATUS_OK)
                return res;

            sTrgArea.bind("trigger.area", &sStyle);
            sTrgWidget.bind(NULL);
            sTrgScreen.bind("trigger.screen", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sTrgArea.init(sclass);
                sTrgScreen.set(-1);

                // Overrides
                sBorderStyle.override(sclass, ws::BS_POPUP);
                sActions.override(sclass, ws::WA_POPUP);
            }

            return STATUS_OK;
        }

        bool PopupWindow::set_arrangements(const lltl::darray<arrangement_t> *list)
        {
            return vArrangements.set(list);
        }

        status_t PopupWindow::post_init()
        {
            // Don't create native window
            return STATUS_OK;
        }

        void PopupWindow::hide_widget()
        {
            Window::hide_widget();

            // Clear restricted area
            sTrgArea.clear();

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
            // Window wont'be shown since it's NULL
            Window *actor = pActor;
            Window::show_widget();

            if (!init_window(actor))
                sVisibility.set(false);
        }

        bool PopupWindow::init_window(Window *actor)
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
            Window *rwnd    = widget_cast<Window>(w->toplevel());
            if ((rwnd != NULL) && (screen < 0))
                screen          = rwnd->screen();
            if ((screen < 0) || (screen >= ssize_t(dpy->screens())))
                screen      = dpy->default_screen();

            // Destroy the window if it does not match requirements
            if ((pWindow != NULL) && (pWindow->screen() != screen))
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
                status_t result = pWindow->init();
                if (result != STATUS_OK)
                {
                    wnd->destroy();
                    delete wnd;
                    return false;
                }

                wnd->set_border_style(sBorderStyle.get());
                wnd->set_window_actions(sActions.get_all());
                pWindow = wnd;
            }

            // Window has been created, adjust position
            bool arranged = false;

            for (size_t i=0, n=vArrangements.size(); i<n; ++i)
            {
                arrangement_t *ar = vArrangements.uget(i);
                if (arranged = arrange_window(&trg, ar))
                    break;
            }

            if (!arranged)
                arrange_forced(&trg);

            pWindow->show((actor != NULL) ? actor->native() : NULL);
            return true;
        }

        bool PopupWindow::arrange_window(const ws::rectangle_t *trg, const arrangement_t *ar)
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
//            ws.nWidth   = sr.nMinWidth;
//            ws.nHeight  = sr.nMinHeight;
            float align = lsp_limit(ar->fAlign + 1.0f, 0.0f, 2.0f) * 0.5f;

            switch (ar->enPosition)
            {
                case A_BOTTOM:
                    gap         = sh - trg->nTop;

                    ws.nTop    += trg->nHeight;
                    ws.nWidth   = sr.nMinWidth;
                    ws.nHeight  = (sr.nMaxHeight >= 0) ? lsp_min(gap, sr.nMaxHeight) : gap;
                    if (ws.nHeight < sr.nMinHeight)
                        return false;
                    if ((ar->bStretch) && (ws.nWidth < trg->nWidth))
                        ws.nWidth   = trg->nWidth;

                    if (ws.nWidth < sr.nMinWidth)
                        return false;

                    break;

                case A_TOP:
                case A_LEFT:
                case A_RIGHT:

                default:
                    return false;
            }

            switch (ar->enPosition)
            {
                case A_BOTTOM:
                case A_TOP:
                    // Adjust horizontal positoin
                    ws.nLeft    = (trg->nWidth - ws.nWidth) * align;
                    end         = ws.nLeft + ws.nWidth;
                    if (ws.nLeft < 0)
                    {
                        if (!ar->bFlexible)
                            return false;
                        ws.nLeft    = 0;
                        end         = ws.nLeft + ws.nWidth;
                        if (end > sw)
                            ws.nWidth  -= (end - sw);
                    }
                    else if (end > sw)
                    {
                        if (!ar->bFlexible)
                            return false;
                        ws.nLeft   -= (end - sw);
                        end         = sw;
                        if (ws.nLeft < 0)
                        {
                            ws.nWidth  += ws.nLeft;
                            ws.nLeft    = 0;
                        }
                    }
                    break;

                default:
                    return false;
            }

            return true;
        }

        void PopupWindow::arrange_forced(const ws::rectangle_t *trg)
        {
        }
    }
}


