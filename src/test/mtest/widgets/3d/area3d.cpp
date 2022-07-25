/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 9 окт. 2020 г.
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

#include <lsp-plug.in/test-fw/mtest.h>
#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/runtime/system.h>
#include <lsp-plug.in/common/debug.h>
#include <private/mtest/tk/common.h>

#define FRAME_PERIOD        (1000.0f / 25.0f) /* Launch at 25 Hz rate */

namespace lsp
{
    #define D3(x, y, z)     { x, y, z, 1.0f     }
    #define V3(dx, dy, dz)  { dx, dy, dz, 0.0f  }
    #define C3(r, g, b)     { r, g, b, 0.0f     }

    typedef struct axis_point3d_t
    {
        r3d::dot4_t     v;
        r3d::color_t    c;
    } axis_point3d_t;

    static axis_point3d_t axis_lines[] =
    {
        // X axis (red)
        { { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        { { 2.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        // Y axis (green)
        { { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { 0.0f, 2.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        // Z axis (blue)
        { { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
        { { 0.0f, 0.0f, 2.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
    };

    static axis_point3d_t dot[] = {
        { { 0.0f, 0.0f, 0.0f, 1.0f}, { 1.0f, 1.0f, 1.0f, 1.0f } }
    };

    // 3D mesh data for box: vertex, normal, color
    static r3d::dot4_t box_vertex[] =
    {
        D3(  1.0f,  1.0f,  1.0f ),
        D3( -1.0f,  1.0f,  1.0f ),
        D3( -1.0f, -1.0f,  1.0f ),
        D3(  1.0f, -1.0f,  1.0f ),

        D3(  1.0f,  1.0f, -1.0f ),
        D3( -1.0f,  1.0f, -1.0f ),
        D3( -1.0f, -1.0f, -1.0f ),
        D3(  1.0f, -1.0f, -1.0f )
    };

    static r3d::color_t box_colors[] =
    {
        C3( 1.0f, 0.0f, 0.0f ),
        C3( 0.0f, 1.0f, 0.0f ),
        C3( 0.0f, 0.0f, 1.0f ),
        C3( 1.0f, 1.0f, 0.0f ),

        C3( 1.0f, 0.0f, 1.0f ),
        C3( 0.0f, 1.0f, 1.0f ),
        C3( 1.0f, 1.0f, 1.0f ),
        C3( 0.5f, 0.5f, 0.5f )
    };

    static r3d::vec4_t box_normal[] =
    {
        V3(  1.0f,  0.0f,  0.0f ),
        V3( -1.0f,  0.0f,  0.0f ),
        V3(  0.0f,  1.0f,  0.0f ),
        V3(  0.0f, -1.0f,  0.0f ),
        V3(  0.0f,  0.0f,  1.0f ),
        V3(  0.0f,  0.0f, -1.0f )
    };

    // Indices
    static uint32_t box_vertex_idx[] =
    {
        0, 1, 2,  0, 2, 3,
        0, 4, 5,  0, 5, 1,
        1, 5, 6,  1, 6, 2,
        0, 3, 7,  0, 7, 4,
        3, 2, 6,  3, 6, 7,
        5, 4, 7,  5, 7, 6
    };

    static uint32_t box_normal_idx[] =
    {
        4, 4, 4,  4, 4, 4,
        2, 2, 2,  2, 2, 2,
        1, 1, 1,  1, 1, 1,
        0, 0, 0,  0, 0, 0,
        3, 3, 3,  3, 3, 3,
        5, 5, 5,  5, 5, 5,
    };
}

MTEST_BEGIN("tk.widgets.3d", area3d)

    typedef struct handler_t
    {
        test_type_t    *test;
        char           *label;
    } handler_t;

    class Scene: public tk::Timer
    {
        protected:
            typedef union {
                dsp::matrix3d_t     dspm;
                r3d::mat4_t         r3dm;
            } matrix_t;

            typedef union {
                dsp::point3d_t      dspv;
                r3d::dot4_t         r3dv;
            } vertex_t;

            typedef union {
                dsp::vector3d_t     dspv;
                r3d::vec4_t         r3dv;
            } vector_t;

        protected:
            size_t                  nPhase;
            tk::Area3D             *pArea;
            test_type_t            *pTest;

            matrix_t                sWorld;
            matrix_t                sView;
            matrix_t                sProj;

            size_t                  nPeriod;
            size_t                  nYaw;
            size_t                  nStep;

            float                   fFov;
            vertex_t                sPov;
            vertex_t                sDst;
            vector_t                sTop;

        private:
            static status_t slot_draw(tk::Widget *sender, void *ptr, void *data)
            {
                Scene *_this    = static_cast<Scene *>(ptr);
                return (_this != NULL) ? _this->draw(static_cast<ws::IR3DBackend *>(data)) : STATUS_BAD_ARGUMENTS;
            }

        public:
            explicit Scene(test_type_t *test, tk::Area3D *a3): Timer()
            {
                nPhase          = 0;
                pTest           = test;
                pArea           = a3;

                nPeriod         = 0x100000;
                nYaw            = 0;
                nStep           = (nPeriod * FRAME_PERIOD) / 5000.0f;

                fFov            = 70.0f;

                // Bind rendering slot
                a3->slots()->bind(tk::SLOT_DRAW3D, slot_draw, this);
                dsp::init_point_xyz(&sPov.dspv, 3.0f, 0.6f, 2.1f);
                dsp::init_point_xyz(&sDst.dspv, 0.0f, 0.0f, 0.0f);
                dsp::init_vector_dxyz(&sTop.dspv, 0.0f, 0.0f, -1.0f);
                dsp::init_matrix3d_identity(&sWorld.dspm);
            }

        public:
            virtual status_t run(ws::timestamp_t time, void *args)
            {
                // Update mesh matrix
                nYaw            = (nYaw + nStep) % nPeriod;
                float yaw       = (2.0f * M_PI * nYaw) / float(nPeriod);

                dsp::init_matrix3d_rotate_z(&sWorld.dspm, -yaw);

                // Query area for redraw
                pArea->query_draw();

                return STATUS_OK;
            }

            status_t draw(ws::IR3DBackend *r3d)
            {
                ssize_t vx, vy, vw, vh;
                matrix_t mat;

                r3d->get_location(&vx, &vy, &vw, &vh);

                // Compute frustum matrix
                float aspect    = float(vw)/float(vh);
                float zNear     = 0.1f;
                float zFar      = 1000.0f;

                float fH        = tanf( fFov * M_PI / 360.0f) * zNear;
                float fW        = fH * aspect;
                dsp::init_matrix3d_frustum(&sProj.dspm, -fW, fW, -fH, fH, zNear, zFar);

                // Compute view matrix
                dsp::vector3d_t dir;
                dsp::init_vector_p2(&dir, &sDst.dspv, &sPov.dspv);
                dsp::init_matrix3d_lookat_p1v2(&sView.dspm, &sPov.dspv, &dir, &sTop.dspv);

                // Update matrices
                r3d->set_matrix(r3d::MATRIX_WORLD, &sWorld.r3dm);
                r3d->set_matrix(r3d::MATRIX_PROJECTION, &sProj.r3dm);
                r3d->set_matrix(r3d::MATRIX_VIEW, &sView.r3dm);

                // Now we can perform rendering
                // Set Light parameters
                r3d::light_t light;

                light.type          = r3d::LIGHT_SPOT;
                light.position      = sPov.r3dv;
                light.direction.dx  = -dir.dx;
                light.direction.dy  = -dir.dy;
                light.direction.dz  = -dir.dz;
                light.direction.dw  = 0.0f;

                light.ambient.r     = 0.5f;
                light.ambient.g     = 0.5f;
                light.ambient.b     = 0.5f;
                light.ambient.a     = 1.0f;

                light.diffuse.r     = 0.5f;
                light.diffuse.g     = 0.5f;
                light.diffuse.b     = 0.5f;
                light.diffuse.a     = 1.0f;

                light.specular.r    = 0.5f;
                light.specular.g    = 0.5f;
                light.specular.b    = 0.5f;
                light.specular.a    = 1.0f;

                light.constant      = 1.0f;
                light.linear        = 0.0f;
                light.quadratic     = 0.0f;
                light.cutoff        = 180.0f;

                // Enable/disable lighting
                r3d->set_lights(&light, 1);

                r3d::buffer_t buf;

                // Draw axes
                r3d::init_buffer(&buf);

                buf.type            = r3d::PRIMITIVE_LINES;
                buf.width           = 2.0f;
                buf.count           = sizeof(axis_lines) / (sizeof(axis_point3d_t) * 2);
                buf.flags           = 0;

                buf.vertex.data     = &axis_lines[0].v;
                buf.vertex.stride   = sizeof(axis_point3d_t);

                buf.color.data      = &axis_lines[0].c;
                buf.color.stride    = sizeof(axis_point3d_t);

                r3d->draw_primitives(&buf);

                // Draw dot
                r3d::init_buffer(&buf);

                dsp::init_matrix3d_translate(&mat.dspm, 1.5f, 1.5f, 1.5f);

                buf.model           = mat.r3dm;
                buf.type            = r3d::PRIMITIVE_POINTS;
                buf.width           = 8.0f;
                buf.count           = 1;
                buf.flags           = 0;

                buf.vertex.data     = &dot[0].v;
                buf.vertex.stride   = sizeof(axis_point3d_t);

                buf.color.data      = &dot[0].c;
                buf.color.stride    = sizeof(axis_point3d_t);

                r3d->draw_primitives(&buf);

                // Draw box
                r3d::init_buffer(&buf);

                buf.type            = r3d::PRIMITIVE_TRIANGLES;
                buf.width           = 1.0f;
                buf.count           = sizeof(box_vertex_idx) / (sizeof(uint32_t) * 3);
                buf.flags           = r3d::BUFFER_LIGHTING;

                buf.vertex.data     = box_vertex;
                buf.vertex.stride   = sizeof(r3d::vec4_t);
                buf.vertex.index    = box_vertex_idx;

                buf.normal.data     = box_normal;
                buf.normal.stride   = sizeof(r3d::vec4_t);
                buf.normal.index    = box_normal_idx;

                buf.color.data      = box_colors;
                buf.color.stride    = sizeof(r3d::color_t);
                buf.color.index     = box_vertex_idx;

                r3d->draw_primitives(&buf);

                return STATUS_OK;
            }

    };

    static status_t slot_close(tk::Widget *sender, void *ptr, void *data)
    {
        sender->display()->quit_main();
        return STATUS_OK;
    }

    static status_t slot_key_up(tk::Widget *sender, void *ptr, void *data)
    {
        tk::Window *wnd = tk::widget_cast<tk::Window>(sender->toplevel());
        ws::event_t *ev = static_cast<ws::event_t *>(data);
        test_type_t *_this = static_cast<test_type_t *>(ptr);

        if ((wnd != NULL) && (ev != NULL) && (ev->nType == ws::UIE_KEY_UP))
        {
            _this->printf("Key up: %c (0x%x)\n", (char)ev->nCode, int(ev->nCode));

            switch (ev->nCode)
            {
                case '+':
                case ws::WSK_KEYPAD_ADD:
                    wnd->style()->schema()->scaling()->add(0.25f);
                    break;

                case '-':
                case ws::WSK_KEYPAD_SUBTRACT:
                    wnd->style()->schema()->scaling()->sub(0.25f);
                    break;

                default:
                    break;
            }
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

    static status_t slot_dot_change(tk::Widget *sender, void *ptr, void *data)
    {
        handler_t *h = static_cast<handler_t *>(ptr);
        h->test->printf("DOT changed: %s\n", h->label);

        tk::GraphDot *gd = tk::widget_cast<tk::GraphDot>(sender);
        if (gd != NULL)
        {
            float size = 0.12f * (100.0f + gd->zvalue()->get());
            gd->border_size()->set(size);
            gd->hover_border_size()->set(size);
        }

        return STATUS_OK;
    }

    status_t init_widget(tk::Widget *w, lltl::parray<handler_t> &vh, const char *label)
    {
        status_t res = w->init();
        if (res != STATUS_OK)
            return res;

        handler_t *h = new handler_t;
        if ((h == NULL) || (!vh.add(h)))
            return STATUS_NO_MEM;
        h->test     = this;
        h->label    = ::strdup(label);

        tk::handler_id_t hid = 0;
        hid = w->slots()->bind(tk::SLOT_MOUSE_IN, slot_mouse_in, h);
        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_DOWN, slot_mouse_down, h);
        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_MOVE, slot_mouse_move, h);
        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_UP, slot_mouse_up, h);
        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_CLICK, slot_mouse_click, h);
        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_DBL_CLICK, slot_mouse_dbl_click, h);
        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_TRI_CLICK, slot_mouse_tri_click, h);
        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_OUT, slot_mouse_out, h);
        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_MOUSE_OUT, slot_mouse_out, h);
        if (hid >= 0) hid = w->slots()->bind(tk::SLOT_KEY_UP, slot_key_up, h);

        if (hid < 0)
            res = -hid;

        return res;
    }

    static void destroy_handlers(lltl::parray<handler_t> &vh)
    {
        for (size_t i=0, n=vh.size(); i<n; ++i)
        {
            handler_t *h = vh.uget(i);
            ::free(h->label);
            delete h;
        }
    }

    typedef struct marker_t
    {
        float       value;
        uint32_t    color;
    } marker_t;

    typedef struct text_t
    {
        float       x;
        float       y;
        const char *text;
        float       halign;
        float       valign;
    } text_t;

    MTEST_MAIN
    {
        lltl::parray<handler_t> vh;

        tk::Display *dpy = new tk::Display();
        MTEST_ASSERT(dpy != NULL);

        MTEST_ASSERT(dpy->init(0, NULL) == STATUS_OK);

        lltl::parray<tk::Widget> widgets;
        tk::Widget *w = NULL;
        tk::Window *wnd = new tk::Window(dpy);
        tk::Area3D *a3 = NULL;
        tk::Grid *grid = NULL;

        // Initialize window
        MTEST_ASSERT(init_widget(wnd, vh, "window") == STATUS_OK);
        MTEST_ASSERT(wnd->title()->set_raw("Test area3d") == STATUS_OK);
        MTEST_ASSERT(wnd->role()->set_raw("area3d_test") == STATUS_OK);
        wnd->bg_color()->set_rgb24(0x1b1c22);
        wnd->actions()->set_actions(ws::WA_MOVE | ws::WA_RESIZE | ws::WA_CLOSE);
        wnd->border_style()->set(ws::BS_DIALOG);
        wnd->border_color()->set_rgb(1.0f, 1.0f, 0.0f);
        wnd->constraints()->set(160, 100, 640, 400);
        wnd->size()->set(320, 200);
        wnd->slot(tk::SLOT_CLOSE)->bind(slot_close, this);
        wnd->pointer()->set(ws::MP_TABLE_CELL);
        wnd->layout()->set(-0.5, 0.5, 0.5, 0.5);
        MTEST_ASSERT(widgets.push(wnd));

        // Create Grid
        MTEST_ASSERT(grid = new tk::Grid(dpy));
        MTEST_ASSERT(init_widget(grid, vh, "grid") == STATUS_OK);
        MTEST_ASSERT(widgets.push(grid));
        MTEST_ASSERT(wnd->add(grid) == STATUS_OK);
        grid->bg_color()->set_rgb(1.0f, 1.0f, 1.0f);
        grid->rows()->set(2);
        grid->columns()->set(1);
        grid->orientation()->set_horizontal();

        // Create scene
        {
            // Create Area3D
            MTEST_ASSERT(a3 = new tk::Area3D(dpy));
            MTEST_ASSERT(init_widget(a3, vh, "graph") == STATUS_OK);
            MTEST_ASSERT(widgets.push(a3));
            MTEST_ASSERT(grid->add(a3) == STATUS_OK);

            a3->constraints()->set_min(320, 200);
            a3->bg_color()->set_rgb24(0x1b1c22);
            a3->color()->set_rgb24(0x000000);
        }

        // Show window
        Scene scene(this, a3);
        scene.bind(dpy);
        scene.launch(-1, FRAME_PERIOD);
        wnd->visibility()->set(true);

        MTEST_ASSERT(dpy->main() == STATUS_OK);

        // Cancel timer
        scene.cancel();

        // Destroy all stuff
        while ((w = widgets.pop()) != NULL)
        {
            w->destroy();
            delete w;
        }

        dpy->destroy();
        delete dpy;
        destroy_handlers(vh);
    }

MTEST_END


