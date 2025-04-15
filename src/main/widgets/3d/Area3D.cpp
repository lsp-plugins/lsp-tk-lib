/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/tk/helpers/draw.h>
#include <lsp-plug.in/common/debug.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/runtime/system.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(Area3D, Widget)
                // Bind
                sConstraints.bind("size.constraints", this);
                sBorder.bind("border.size", this);
                sBorderRadius.bind("border.radius", this);
                sBorderFlat.bind("border.flat", this);
                sGlass.bind("glass.visibility", this);
                sColor.bind("color", this);
                sBorderColor.bind("border.color", this);
                sGlassColor.bind("glass.color", this);
                // Configure
                sConstraints.set_all(-1);
                sBorder.set(4);
                sBorderRadius.set(12);
                sBorderFlat.set(false);
                sGlass.set(true);
                sColor.set("#000000");
                sBorderColor.set("#000000");
                sGlassColor.set("#ffffff");
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(Area3D, "Area3D", "root");
        }

        const w_class_t Area3D::metadata      = { "Area3D", &Widget::metadata };

        Area3D::Area3D(Display *dpy):
            Widget(dpy),
            sConstraints(&sProperties),
            sBorder(&sProperties),
            sBorderRadius(&sProperties),
            sBorderFlat(&sProperties),
            sGlass(&sProperties),
            sColor(&sProperties),
            sBorderColor(&sProperties),
            sGlassColor(&sProperties)
        {
            pBackend            = NULL;
            pGlass              = NULL;

            sCanvas.nLeft       = 0;
            sCanvas.nTop        = 0;
            sCanvas.nWidth      = 0;
            sCanvas.nHeight     = 0;

            pClass              = &metadata;
        }

        Area3D::~Area3D()
        {
            nFlags     |= FINALIZED;
            do_destroy();
        }

        void Area3D::destroy()
        {
            nFlags     |= FINALIZED;
            Widget::destroy();
            do_destroy();
        }

        void Area3D::do_destroy()
        {
            // Destroy resources
            drop_glass();
            drop_backend();
        }

        void Area3D::drop_glass()
        {
            if (pGlass != NULL)
            {
                pGlass->destroy();
                delete pGlass;
                pGlass = NULL;
            }
        }

        void Area3D::drop_backend()
        {
            if (pBackend != NULL)
            {
                pBackend->destroy();
                delete pBackend;
                pBackend = NULL;
            }
        }

        status_t Area3D::init()
        {
            status_t result = Widget::init();
            if (result != STATUS_OK)
                return result;

            // Init style
            sConstraints.bind("size.constraints", &sStyle);
            sBorder.bind("border.size", &sStyle);
            sBorderRadius.bind("border.radius", &sStyle);
            sBorderFlat.bind("border.flat", &sStyle);
            sGlass.bind("glass.visibility", &sStyle);
            sColor.bind("color", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sGlassColor.bind("glass.color", &sStyle);

            // Add slots
            handler_id_t id = 0;
            id = sSlots.add(SLOT_DRAW3D, slot_draw3d, self());

            return (id >= 0) ? STATUS_OK : -id;
        }

        void Area3D::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (sBorder.is(prop))
                query_resize();
            if (sBorderRadius.is(prop))
                query_resize();
            if (sBorderFlat.is(prop))
                query_draw();
            if (sGlass.is(prop))
                query_draw();
            if (sColor.is(prop))
                query_draw();
            if (sBorderColor.is(prop))
                query_draw();
            if (sGlassColor.is(prop))
                query_draw();
        }

        void Area3D::size_request(ws::size_limit_t *r)
        {
            float scaling   = lsp_max(0.0f, sScaling.get());
            sConstraints.compute(r, scaling);

            float xr        = lsp_max(0.0f, sBorderRadius.get() * scaling); // external radius
            float bw        = lsp_max(0.0f, sBorder.get() * scaling);       // border size
            float ir        = lsp_max(0.0f, xr - bw);                       // internal radius
            float bp        = (1.0f - M_SQRT1_2) * ir;                      // padding to not to cross internal radius
            ssize_t padding = ceilf(bp + bw);
            ssize_t wh      = lsp_max(padding * 2, xr * 2);                 // minimum possible width and height

            // Append padding to the size limit
            r->nMinWidth    = (r->nMinWidth  >= 0) ? r->nMinWidth  + padding * 2 : padding * 2;
            r->nMinHeight   = (r->nMinHeight >= 0) ? r->nMinHeight + padding * 2 : padding * 2;
            r->nMaxWidth    = (r->nMaxWidth  >= 0) ? r->nMaxWidth  + padding * 2 : -1;
            r->nMaxHeight   = (r->nMaxHeight >= 0) ? r->nMaxHeight + padding * 2 : -1;
            r->nPreWidth    = -1;
            r->nPreHeight   = -1;

            // Apply radius settings
            r->nMinWidth    = (r->nMinWidth  >= 0) ? lsp_max(r->nMinWidth,  wh)  : wh;
            r->nMinHeight   = (r->nMinHeight >= 0) ? lsp_max(r->nMinHeight, wh)  : wh;
            if ((r->nMaxWidth  >= 0) && (r->nMaxWidth  < r->nMinWidth))
                r->nMaxWidth    = r->nMinWidth;
            if ((r->nMaxHeight >= 0) && (r->nMaxHeight < r->nMinHeight))
                r->nMaxHeight   = r->nMinHeight;
        }

        void Area3D::realize(const ws::rectangle_t *r)
        {
            // Call parent class to realize
            Widget::realize(r);

            // Compute the size of area
            float scaling   = lsp_max(0.0f, sScaling.get());
            float xr        = lsp_max(0.0f, ceilf(sBorderRadius.get() * scaling));  // external radius
            float bw        = lsp_max(0.0f, ceilf(sBorder.get() * scaling));        // border size
            float ir        = lsp_max(0.0f, xr - bw);                               // internal radius
            ssize_t padding = ceilf((1.0f - M_SQRT1_2) * ir + bw);                  // padding of internal area

            sCanvas.nLeft   = r->nLeft   + padding;
            sCanvas.nTop    = r->nTop    + padding;
            sCanvas.nWidth  = r->nWidth  - padding*2;
            sCanvas.nHeight = r->nHeight - padding*2;
        }

        void Area3D::hide_widget()
        {
            Widget::hide_widget();
            drop_glass();
            drop_backend();
        }

        ws::IR3DBackend *Area3D::get_backend()
        {
            // Check that we have valid backend
            if ((pBackend != NULL) && (pBackend->valid()))
                return pBackend;

            // Drop backend and create new one
            lsp_trace("Creating 3D backend");
            drop_backend();

            // Obtain the necessary information
            ws::IDisplay *dpy = pDisplay->display();
            if (dpy == NULL)
                return NULL;

            // Obtain the native window handle
            Window *wnd = widget_cast<Window>(toplevel());
            ws::IWindow *native = (wnd != NULL) ? wnd->native() : NULL;
            if (native == NULL)
                return NULL;

            // Try to create backend
            pBackend = dpy->create_r3d_backend(native);
            if (pBackend == NULL)
                return NULL;

            // Sync display and return
            pDisplay->sync();

            return pBackend;
        }

        status_t Area3D::slot_draw3d(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Area3D *_this       = widget_ptrcast<Area3D>(ptr);
            return (_this != NULL) ? _this->on_draw3d(static_cast<ws::IR3DBackend *>(data)) : STATUS_BAD_ARGUMENTS;
        }

        void Area3D::draw(ws::ISurface *s, bool force)
        {
            // Obtain a 3D backend and draw it if it is valid
            ws::IR3DBackend *r3d    = get_backend();
            if ((r3d == NULL) || (!r3d->valid()))
                return;

            // Update backend color
            r3d::color_t c;
            c.r     = sColor.red();
            c.g     = sColor.green();
            c.b     = sColor.blue();
            c.a     = 0.0f;
            r3d->set_bg_color(&c);

            // Perform a draw call
            size_t count        = sCanvas.nWidth * sCanvas.nHeight;
            uint8_t *buf        = static_cast<uint8_t *>(malloc(count * sizeof(uint32_t)));
            if (buf == NULL)
                return;
            lsp_finally{ free(buf); };

            r3d->locate(sCanvas.nLeft, sCanvas.nTop, sCanvas.nWidth, sCanvas.nHeight);
            pDisplay->sync();

            r3d->begin_draw();
                sSlots.execute(SLOT_DRAW3D, this, r3d);
                r3d->sync();
                r3d->read_pixels(buf, r3d::PIXEL_BGRA);
            r3d->end_draw();

            dsp::pbgra32_set_alpha(buf, buf, 0xff, count);
            s->draw_raw(buf, sCanvas.nWidth, sCanvas.nHeight, sCanvas.nWidth * 4,
                sCanvas.nLeft, sCanvas.nTop, 1.0f, 1.0f, 0.0f);
        }

        void Area3D::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
        #ifdef LSP_TRACE
            system::time_t start, end;
            system::get_time(&start);
        #endif /* LSP_TRACE */

            if (nFlags & REDRAW_SURFACE)
                force = true;

            float scaling   = lsp_max(0.0f, sScaling.get());
            float xr        = lsp_max(0.0f, sBorderRadius.get() * scaling); // external radius
            float bw        = lsp_max(0.0f, sBorder.get() * scaling);       // border size
            float bright    = select_brightness();

            // Prepare palette
            ws::ISurface *cv;
            lsp::Color color(sColor);
            lsp::Color bg_color;
            get_actual_bg_color(bg_color);
            color.scale_lch_luminance(bright);

            s->clip_begin(area);
            {
                // Draw widget background
                s->fill_rect(bg_color, SURFMASK_NONE, 0.0f, &sSize);

                bool aa = s->set_antialiasing(true);
                s->fill_rect(color, SURFMASK_ALL_CORNER, xr, &sSize);

                // Draw the contents
                draw(s, force);

                // Draw the glass and the border
                color.copy(sGlassColor);
                bg_color.copy(sBorderColor);
                color.scale_lch_luminance(bright);
                bg_color.scale_lch_luminance(bright);

                bool flat = sBorderFlat.get();

                if (sGlass.get())
                {
                    cv = create_border_glass(&pGlass, s,
                            color, bg_color,
                            SURFMASK_ALL_CORNER, bw, xr,
                            sSize.nWidth, sSize.nHeight, flat
                        );
                    if (cv != NULL)
                        s->draw(cv, sSize.nLeft, sSize.nTop, 1.0f, 1.0f, 0.0f);
                }
                else
                {
                    drop_glass();
                    if (bw > 0)
                        draw_border(s, bg_color, SURFMASK_ALL_CORNER, bw, xr, &sSize, flat);
                }

                s->set_antialiasing(aa);
            }
            s->clip_end();

        #ifdef LSP_TRACE
            system::get_time(&end);
            float time = float(end.seconds - start.seconds) * 1000.0f + (end.nanos - start.nanos) * 1e-6f;
            lsp_trace("render time: %.3f ms", time);
        #endif /* LSP_TRACE */
        }

        status_t Area3D::on_draw3d(ws::IR3DBackend *r3d)
        {
            return STATUS_OK;
        }

    } /* namespace tk */
} /* namespace lsp */
