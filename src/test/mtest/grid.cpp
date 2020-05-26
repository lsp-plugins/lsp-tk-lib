/*
 * grid.cpp
 *
 *  Created on: 24 мая 2020 г.
 *      Author: sadko
 */


#include <lsp-plug.in/test-fw/mtest.h>
#include <lsp-plug.in/tk/tk.h>

namespace
{
    static const uint32_t colors[] =
    {
        0xff0000,
        0x00ff00,
        0x0000ff,
        0xffff00,
        0xff00ff,
        0x00ffff,

        0xcc0000,
        0x00cc00,
        0x0000cc,
        0xcccc00,
        0xcc00cc,
        0x00cccc,

        0x880000,
        0x008800,
        0x000088,
        0x888800,
        0x880088,
        0x008888,

        0x88cccc,
        0xcc88cc,
        0xcccc88,
        0x8888cc,
        0x88cc88,
        0xcc8888,

        0x88ffff,
        0xff88ff,
        0xffff88,
        0x8888ff,
        0x88ff88,
        0xff8888,

        0xffcccc,
        0xccffcc,
        0xccccff,
        0xffffcc,
        0xffccff,
        0xccffff,
    };
};

MTEST_BEGIN("tk", grid)
    typedef struct handler_t
    {
        test_type_t    *test;
        const char     *label;
    } handler_t;

    static status_t slot_close(tk::Widget *sender, void *ptr, void *data)
    {
        sender->display()->quit_main();
        return STATUS_OK;
    }

    static status_t slot_key_up(tk::Widget *sender, void *ptr, void *data)
    {
        tk::Window *wnd = tk::widget_cast<tk::Window>(sender);
        ws::event_t *ev = static_cast<ws::event_t *>(data);
        test_type_t *_this = static_cast<test_type_t *>(ptr);

        if ((wnd != NULL) && (ev != NULL) && (ev->nType == ws::UIE_KEY_UP))
        {
            _this->printf("Key up: %c (0x%x)\n", (char)ev->nCode, int(ev->nCode));

            if ((ev->nCode == '+') || (ev->nCode == ws::WSK_KEYPAD_ADD))
                wnd->scaling()->set(wnd->scaling()->get() + 0.25f);
            else if ((ev->nCode == '-') || (ev->nCode == ws::WSK_KEYPAD_SUBTRACT))
                wnd->scaling()->set(wnd->scaling()->get() - 0.25f);
        }
        return STATUS_OK;
    }

    static status_t slot_mouse_in(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("MOUSE_IN: %s\n", h->label);

        return STATUS_OK;
    }

    static status_t slot_mouse_out(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("MOUSE_OUT: %s\n", h->label);

        return STATUS_OK;
    }

    static status_t slot_mouse_move(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("MOUSE_MOVE: %s\n", h->label);

        return STATUS_OK;
    }

    static status_t slot_mouse_down(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("MOUSE_DOWN: %s\n", h->label);

        return STATUS_OK;
    }

    static status_t slot_mouse_up(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("MOUSE_UP: %s\n", h->label);

        return STATUS_OK;
    }

    static status_t slot_mouse_click(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("MOUSE_CLICK: %s\n", h->label);

        return STATUS_OK;
    }

    static status_t slot_mouse_dbl_click(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("MOUSE_DBL_CLICK: %s\n", h->label);

        return STATUS_OK;
    }

    static status_t slot_mouse_tri_click(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("MOUSE_TRI_CLICK: %s\n", h->label);

        return STATUS_OK;
    }

    status_t init_widget(tk::Widget *w, lltl::darray<handler_t> &vh, const char *label)
    {
        status_t res = w->init();
        if (res != STATUS_OK)
            return res;

        handler_t *h = vh.add();
        if (h == NULL)
            return STATUS_NO_MEM;
        h->test     = this;
        h->label    = label;

//        tk::handler_id_t hid;
//        hid = w->slots()->bind(tk::SLOT_MOUSE_IN, slot_mouse_in, h);
//        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_DOWN, slot_mouse_down, h);
//        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_MOVE, slot_mouse_move, h);
//        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_UP, slot_mouse_up, h);
//        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_CLICK, slot_mouse_click, h);
//        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_DBL_CLICK, slot_mouse_dbl_click, h);
//        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_TRI_CLICK, slot_mouse_tri_click, h);
//        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_OUT, slot_mouse_out, h);
//
//        if (hid < 0)
//            res = -hid;

        return res;
    }

    static uint32_t next_color(size_t &idx)
    {
        uint32_t res    = colors[idx];
        idx = (idx + 1) % (sizeof(colors) / sizeof(uint32_t));
        return res;
    }

    MTEST_MAIN
    {
        lltl::darray<handler_t> vh;

        tk::Display *dpy = new tk::Display();
        MTEST_ASSERT(dpy != NULL);

        MTEST_ASSERT(dpy->init(0, NULL) == STATUS_OK);

        lltl::parray<tk::Widget> widgets;
        tk::Widget *w = NULL;
        tk::Window *wnd = new tk::Window(dpy);
        tk::Void *wv = NULL;
        tk::Box *box = NULL;
        tk::Grid *grid = NULL;
        tk::WidgetContainer *parent = NULL;

        size_t col = 0;

        // Initialize window
        MTEST_ASSERT(init_widget(wnd, vh, "window") == STATUS_OK);
        MTEST_ASSERT(wnd->title()->set_raw("Test grid") == STATUS_OK);
        MTEST_ASSERT(wnd->role()->set_raw("grid_test") == STATUS_OK);
        wnd->bg_color()->set_rgb(0, 0.75, 1.0);
        wnd->actions()->set_actions(ws::WA_MOVE | ws::WA_RESIZE | ws::WA_CLOSE);
        wnd->border_style()->set(ws::BS_DIALOG);
        wnd->size_constraints()->set(160, 100, 640, 400);
        wnd->size()->set(320, 200);
        wnd->slot(tk::SLOT_CLOSE)->bind(slot_close, this);
        wnd->slot(tk::SLOT_KEY_UP)->bind(slot_key_up, this);
        wnd->pointer()->set(ws::MP_TABLE_CELL);
        wnd->layout()->set(-0.5, 0.5, 0.5, 0.5);
        MTEST_ASSERT(widgets.push(wnd));

        // Create vertical box
        MTEST_ASSERT(box = new tk::Box(dpy));
        MTEST_ASSERT(init_widget(box, vh, "vbox") == STATUS_OK);
        MTEST_ASSERT(widgets.push(box));
        MTEST_ASSERT(wnd->add(box) == STATUS_OK);
        box->bg_color()->set_rgb(1.0f, 1.0f, 1.0f);
        box->orientation()->set_vertical();
        box->spacing()->set(16);
        box->padding()->set_all(8);
        box->pointer()->set(ws::MP_IBEAM);
        parent = box;

        // Create grid (1)
        /*MTEST_ASSERT(grid = new tk::Grid(dpy));
        MTEST_ASSERT(init_widget(grid, vh, "grid1") == STATUS_OK);
        MTEST_ASSERT(widgets.push(grid));
        MTEST_ASSERT(parent->add(grid) == STATUS_OK);
        grid->padding()->set_all(1);
        grid->bg_color()->set_rgb(1.0, 0.75, 1.0);
        grid->pointer()->set(ws::MP_DEFAULT);
//        box->bg_color()->set_rgb(1.0, 1.0, 1.0);
        grid->hspacing()->set(4);
        grid->vspacing()->set(8);
        grid->orientation()->set(tk::O_HORIZONTAL);
        grid->rows()->set(2);
        grid->columns()->set(3);
        {
            // Create widget (1)
            MTEST_ASSERT(wv = new tk::Void(dpy));
            MTEST_ASSERT(init_widget(wv, vh, "void1-1-1") == STATUS_OK);
            MTEST_ASSERT(widgets.push(wv));
            MTEST_ASSERT(grid->add(wv) == STATUS_OK);
            wv->constraints()->set(64, 48, -1, -1);
            wv->bg_color()->set_rgb24(next_color(col));
            wv->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(wv = new tk::Void(dpy));
            MTEST_ASSERT(init_widget(wv, vh, "void1-1-2") == STATUS_OK);
            MTEST_ASSERT(widgets.push(wv));
            MTEST_ASSERT(grid->add(wv) == STATUS_OK);
            wv->constraints()->set(64, 48, 64, 48);
            wv->bg_color()->set_rgb24(next_color(col));
            wv->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(wv = new tk::Void(dpy));
            MTEST_ASSERT(init_widget(wv, vh, "void1-1-3") == STATUS_OK);
            MTEST_ASSERT(widgets.push(wv));
            MTEST_ASSERT(grid->add(wv) == STATUS_OK);
            wv->constraints()->set(32, 32, 64, 64);
            wv->bg_color()->set_rgb24(next_color(col));
            wv->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(wv = new tk::Void(dpy));
            MTEST_ASSERT(init_widget(wv, vh, "void1-2-1") == STATUS_OK);
            MTEST_ASSERT(widgets.push(wv));
            MTEST_ASSERT(grid->add(wv) == STATUS_OK);
            wv->constraints()->set(64, 48, -1, -1);
            wv->bg_color()->set_rgb24(next_color(col));
            wv->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(wv = new tk::Void(dpy));
            MTEST_ASSERT(init_widget(wv, vh, "void1-2-2") == STATUS_OK);
            MTEST_ASSERT(widgets.push(wv));
            MTEST_ASSERT(grid->add(wv) == STATUS_OK);
            wv->constraints()->set(64, 48, 96, 64);
            wv->bg_color()->set_rgb24(next_color(col));
            wv->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(wv = new tk::Void(dpy));
            MTEST_ASSERT(init_widget(wv, vh, "void1-2-3") == STATUS_OK);
            MTEST_ASSERT(widgets.push(wv));
            MTEST_ASSERT(grid->add(wv) == STATUS_OK);
            wv->constraints()->set(48, 48, 96, 96);
            wv->bg_color()->set_rgb24(next_color(col));
            wv->pointer()->set(ws::MP_HAND);
        }*/

        // Create grid (2)
        MTEST_ASSERT(grid = new tk::Grid(dpy));
        MTEST_ASSERT(init_widget(grid, vh, "grid2") == STATUS_OK);
        MTEST_ASSERT(widgets.push(grid));
        MTEST_ASSERT(parent->add(grid) == STATUS_OK);
        grid->padding()->set_all(1);
        grid->bg_color()->set_rgb(1.0, 0.75, 1.0);
        grid->pointer()->set(ws::MP_DEFAULT);
//        box->bg_color()->set_rgb(1.0, 1.0, 1.0);
        grid->hspacing()->set(4);
        grid->vspacing()->set(8);
        grid->orientation()->set(tk::O_HORIZONTAL);
        grid->rows()->set(5);
        grid->columns()->set(5);
        {
            // Create widget (1)
            MTEST_ASSERT(wv = new tk::Void(dpy));
            MTEST_ASSERT(init_widget(wv, vh, "void2-1-1") == STATUS_OK);
            MTEST_ASSERT(widgets.push(wv));
            MTEST_ASSERT(grid->attach(0, 0, wv, 2, 2) == STATUS_OK);
            wv->constraints()->set(64, 48, -1, -1);
            wv->bg_color()->set_rgb24(next_color(col));
            wv->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(wv = new tk::Void(dpy));
            MTEST_ASSERT(init_widget(wv, vh, "void2-1-2") == STATUS_OK);
            MTEST_ASSERT(widgets.push(wv));
            MTEST_ASSERT(grid->attach(3, 0, wv, 2, 1) == STATUS_OK);
            wv->constraints()->set(64, 48, 64, 48);
            wv->bg_color()->set_rgb24(next_color(col));
            wv->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(wv = new tk::Void(dpy));
            MTEST_ASSERT(init_widget(wv, vh, "void2-1-3") == STATUS_OK);
            MTEST_ASSERT(widgets.push(wv));
            MTEST_ASSERT(grid->attach(4, 0, wv, 2, 1) == STATUS_OK);
            wv->constraints()->set(32, 32, 64, 64);
            wv->bg_color()->set_rgb24(next_color(col));
            wv->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(wv = new tk::Void(dpy));
            MTEST_ASSERT(init_widget(wv, vh, "void2-2-1") == STATUS_OK);
            MTEST_ASSERT(widgets.push(wv));
            MTEST_ASSERT(grid->attach(0, 3, wv, 1, 2) == STATUS_OK);
            wv->constraints()->set(64, 48, -1, -1);
            wv->bg_color()->set_rgb24(next_color(col));
            wv->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(wv = new tk::Void(dpy));
            MTEST_ASSERT(init_widget(wv, vh, "void2-2-2") == STATUS_OK);
            MTEST_ASSERT(widgets.push(wv));
            MTEST_ASSERT(grid->attach(0, 4, wv, 1, 2) == STATUS_OK);
            wv->constraints()->set(64, 48, 96, 64);
            wv->bg_color()->set_rgb24(next_color(col));
            wv->pointer()->set(ws::MP_HAND);

            MTEST_ASSERT(wv = new tk::Void(dpy));
            MTEST_ASSERT(init_widget(wv, vh, "void2-2-3") == STATUS_OK);
            MTEST_ASSERT(widgets.push(wv));
            MTEST_ASSERT(grid->attach(3, 3, wv, 2, 2) == STATUS_OK);
            wv->constraints()->set(48, 48, 96, 96);
            wv->bg_color()->set_rgb24(next_color(col));
            wv->pointer()->set(ws::MP_HAND);
        }

        // Show window
        wnd->visibility()->set(true);

        MTEST_ASSERT(dpy->main() == STATUS_OK);

        while ((w = widgets.pop()) != NULL)
        {
            w->destroy();
            delete w;
        }

        dpy->destroy();
        delete dpy;
    }

MTEST_END






