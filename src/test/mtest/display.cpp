/*
 * display.cpp
 *
 *  Created on: 5 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/test-fw/mtest.h>
#include <lsp-plug.in/tk/tk.h>

MTEST_BEGIN("tk", display)

//    class Handler: public ws::IEventHandler
//    {
//        private:
//            ws::IWindow    *pWnd;
//
//        public:
//            inline Handler(ws::IWindow *wnd)
//            {
//                pWnd        = wnd;
//            }
//
//            virtual status_t handle_event(const ws::event_t *ev)
//            {
//                if (ev->nType == ws::UIE_REDRAW)
//                {
//                    Color c(0.0f, 0.5f, 0.75f);
//                    ws::ISurface *s = pWnd->get_surface();
//                    if (s != NULL)
//                        s->clear(&c);
//
//                    return STATUS_OK;
//                }
//                if (ev->nType == ws::UIE_CLOSE)
//                {
//                    pWnd->hide();
//                    pWnd->display()->quit_main();
//                }
//                return IEventHandler::handle_event(ev);
//            }
//    };

    MTEST_MAIN
    {
        tk::Display *dpy = new tk::Display();
        MTEST_ASSERT(dpy != NULL);

        MTEST_ASSERT(dpy->init(0, NULL) == STATUS_OK);

        tk::Window *wnd = new tk::Window(dpy);
        MTEST_ASSERT(wnd->init() == STATUS_OK);
        MTEST_ASSERT(wnd->title()->set("Test window") == STATUS_OK);
        MTEST_ASSERT(wnd->role()->set("test") == STATUS_OK);
        wnd->window_actions()->set_actions(ws::WA_MOVE | ws::WA_RESIZE | ws::WA_CLOSE);
        wnd->border_style()->set(ws::BS_DIALOG);
        wnd->size_constraints()->set(160, 100, 640, 400);
        wnd->size()->set(320, 200);
        wnd->show();

        MTEST_ASSERT(dpy->main() == STATUS_OK);

        wnd->destroy();
        delete wnd;
        dpy->destroy();
        delete dpy;
    }

MTEST_END


