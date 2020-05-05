/*
 * display.cpp
 *
 *  Created on: 5 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/ws/factory.h>
#include <lsp-plug.in/ws/IEventHandler.h>
#include <lsp-plug.in/test-fw/mtest.h>

MTEST_BEGIN("ws", display)

    class Handler: public ws::IEventHandler
    {
        private:
            ws::IWindow    *pWnd;

        public:
            inline Handler(ws::IWindow *wnd)
            {
                pWnd        = wnd;
            }

            virtual status_t handle_event(const ws::event_t *ev)
            {
                if (ev->nType == ws::UIE_REDRAW)
                {
                    Color c(0.0f, 0.5f, 0.75f);
                    ws::ISurface *s = pWnd->get_surface();
                    if (s != NULL)
                        s->clear(&c);

                    return STATUS_OK;
                }
                if (ev->nType == ws::UIE_CLOSE)
                {
                    pWnd->hide();
                    pWnd->display()->quit_main();
                }
                return IEventHandler::handle_event(ev);
            }
    };

    MTEST_MAIN
    {
        ws::IDisplay *dpy = ws::lsp_create_display(0, NULL);
        MTEST_ASSERT(dpy != NULL);

        ws::IWindow *wnd = dpy->create_window();
        MTEST_ASSERT(wnd->init() == STATUS_OK);
        MTEST_ASSERT(wnd->set_caption("Test window", "Test window") == STATUS_OK);
        MTEST_ASSERT(wnd->set_border_style(ws::BS_DIALOG) == STATUS_OK);
        MTEST_ASSERT(wnd->set_window_actions(ws::WA_MOVE | ws::WA_RESIZE | ws::WA_CLOSE) == STATUS_OK);
        MTEST_ASSERT(wnd->resize(320, 200) == STATUS_OK);
        MTEST_ASSERT(wnd->set_size_constraints(160, 100, 640, 400) == STATUS_OK);

        size_t screen = wnd->screen();
        ssize_t sw, sh;
        MTEST_ASSERT(dpy->screen_size(screen, &sw, &sh) == STATUS_OK);
        wnd->move((sw - wnd->width()) >> 1, (sh - wnd->height()) >> 1);

        MTEST_ASSERT(wnd->show() == STATUS_OK);

        Handler h(wnd);
        wnd->set_handler(&h);

        MTEST_ASSERT(dpy->main() == STATUS_OK);

        wnd->destroy();
        delete wnd;
        ws::lsp_free_display(dpy);
    }

MTEST_END


