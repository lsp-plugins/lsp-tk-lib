/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 28 мар. 2025 г.
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
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(Overlay, WidgetContainer)
                // Bind
                sTransparency.bind("transparency", this);
                sPriority.bind("priority", this);
                sLayout.bind("layout", this);
                sPosition.bind("position", this);
                sConstraints.bind("size.constraints", this);
                sAutoClose.bind("close.auto", this);
                sBorderRounding.bind("border.rounding", this);
                sBorderRadius.bind("border.radius", this);
                sBorderSize.bind("border.size", this);
                sBorderColor.bind("border.color", this);
                sShadowSize.bind("shadow.size", this);
                sShadowStart.bind("shadow.start", this);
                sShadowEnd.bind("shadow.end", this);

                // Configure
                sTransparency.set(0.25f);
                sPriority.set(0);
                sLayout.set(0.0f, 0.0f, 1.0f, 1.0f);
                sPosition.set(0, 0);
                sConstraints.set_all(-1);
                sAutoClose.set(false);
                sBorderRounding.set_none();
                sBorderRadius.set(12);
                sBorderSize.set(0);
                sBorderColor.set_rgb24(0x000000);
                sShadowSize.set(0);
                sShadowStart.set_rgba32(0x00000000);
                sShadowEnd.set_rgba32(0xff000000);

                // Override
                sLayout.override();
                sConstraints.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(Overlay, "Overlay", "root");
        }

        const w_class_t Overlay::metadata         = { "Overlay", &WidgetContainer::metadata };

        Overlay::Overlay(Display *dpy):
            WidgetContainer(dpy),
            sTransparency(&sProperties),
            sPriority(&sProperties),
            sLayout(&sProperties),
            sPosition(&sProperties),
            sConstraints(&sProperties),
            sAutoClose(&sProperties),
            sBorderRounding(&sProperties),
            sBorderRadius(&sProperties),
            sBorderSize(&sProperties),
            sBorderColor(&sProperties),
            sShadowSize(&sProperties),
            sShadowStart(&sProperties),
            sShadowEnd(&sProperties)
        {
            pClass          = &metadata;

            wWidget         = NULL;
            pPosFunc        = NULL;
            pPosData        = NULL;
        }

        Overlay::~Overlay()
        {
            nFlags     |= FINALIZED;
            do_destroy();
        }

        status_t Overlay::init()
        {
            status_t result = WidgetContainer::init();
            if (result != STATUS_OK)
                return result;

            sTransparency.bind("transparency", &sStyle);
            sPriority.bind("priority", &sStyle);
            sLayout.bind("layout", &sStyle);
            sPosition.bind("position", &sStyle);
            sConstraints.bind("size.constraints", &sStyle);
            sAutoClose.bind("close.auto", &sStyle);
            sBorderRounding.bind("border.rounding", &sStyle);
            sBorderRadius.bind("border.radius", &sStyle);
            sBorderSize.bind("border.size", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sShadowSize.bind("shadow.size", &sStyle);
            sShadowStart.bind("shadow.start", &sStyle);
            sShadowEnd.bind("shadow.end", &sStyle);

            return STATUS_OK;
        }

        void Overlay::destroy()
        {
            nFlags     |= FINALIZED;
            do_destroy();
            WidgetContainer::destroy();
        }

        void Overlay::do_destroy()
        {
            if (wWidget != NULL)
            {
                unlink_widget(wWidget);
                wWidget = NULL;
            }
        }

        Widget *Overlay::find_widget(ssize_t x, ssize_t y)
        {
            if ((wWidget == NULL) || (!wWidget->is_visible_child_of(this)))
                return NULL;

            return (wWidget->inside(x, y)) ? wWidget : NULL;
        }

        void Overlay::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);

            if (prop->one_of(sTransparency, sPriority, sBorderColor))
                query_draw();
            if (prop->one_of(sLayout, sConstraints, sPosition, sBorderRadius, sBorderRounding, sBorderSize))
                query_resize();
        }

        void Overlay::size_request(ws::size_limit_t *r)
        {
            const float scaling = lsp_max(0.0f, sScaling.get());
            const size_t bw     = lsp_max(0.0f, sBorderSize.get() * scaling);

            if ((wWidget == NULL) || (!wWidget->is_visible_child_of(this)))
            {
                r->nMinWidth    = -1;
                r->nMinHeight   = -1;
                r->nMaxWidth    = -1;
                r->nMaxHeight   = -1;
            }
            else
            {
                wWidget->get_padded_size_limits(r);
                r->nMaxWidth    = -1;
                r->nMaxHeight   = -1;
            }

            sConstraints.apply(r, scaling);
            
            // Add border
            r->nMinWidth    = lsp_max(r->nMinWidth, 0)  + bw * 2;
            r->nMinHeight   = lsp_max(r->nMinHeight, 0) + bw * 2;

            r->nPreWidth    = r->nMinWidth;
            r->nPreHeight   = r->nMinHeight;
        }

        void Overlay::realize(const ws::rectangle_t *r)
        {
            const float scaling = lsp_max(0.0f, sScaling.get());
            const size_t bw     = lsp_max(0.0f, sBorderSize.get() * scaling);

//            lsp_trace("width=%d, height=%d", int(r->nWidth), int(r->nHeight));
            WidgetContainer::realize(r);

            if ((wWidget == NULL) || (!wWidget->is_visible_child_of(this)))
                return;

            // Realize child widget
            ws::rectangle_t wr;
            ws::rectangle_t xr;
            ws::size_limit_t sr;

            wr.nLeft        = r->nLeft + bw;
            wr.nTop         = r->nTop  + bw;
            wr.nWidth       = r->nWidth  - bw * 2;
            wr.nHeight      = r->nHeight - bw * 2;

            wWidget->get_padded_size_limits(&sr);
            sLayout.apply(&xr, &wr, &sr);
            wWidget->padding()->enter(&xr, wWidget->scaling()->get());
            wWidget->realize_widget(&xr);
        }

        void Overlay::draw(ws::ISurface *s, bool force)
        {
            const float scaling = lsp_max(0.0f, sScaling.get());
            const ssize_t bw    = lsp_max(0.0f, sBorderSize.get() * scaling);

            // Initialize palette
            lsp::Color bg_color;
            get_actual_bg_color(bg_color);

            // Draw background if child is invisible or not present
            if ((wWidget == NULL) || (!wWidget->visibility()->get()))
            {
                if (force)
                    s->fill_rect(bg_color, SURFMASK_NONE, 0.0f, &sSize);
            }
            else
            {
                // Set up draw translation from window coordinates to surface coordinates
                ws::rectangle_t xr;
                wWidget->get_rectangle(&xr);
                const ws::point_t origin = s->set_origin(bw - xr.nLeft, bw - xr.nTop);
                lsp_finally { s->set_origin(origin); };

                // Draw the child widget
                wWidget->render(s, &xr, false);
                wWidget->commit_redraw();

                if (force)
                {
                    // Draw rectangle around widget
                    ws::rectangle_t sr  = sSize;
                    sr.nLeft           -= xr.nLeft;
                    sr.nTop            -= xr.nTop;

                    wWidget->get_actual_bg_color(bg_color);
                    s->fill_frame(bg_color, SURFMASK_NONE, 0.0f, &sSize, &xr);
                }
            }

            // Draw border
            if (bw > 0)
            {
                ws::rectangle_t br;
                const size_t flags  = sBorderRounding.corners();
                const size_t radius = lsp_max(0.0f, sBorderRadius.get() * scaling);
                lsp::Color bcolor(sBorderColor);

                br.nLeft        = 0;
                br.nTop         = 0;
                br.nWidth       = sSize.nWidth;
                br.nHeight      = sSize.nHeight;

                s->wire_rect(bcolor, flags, radius, &br, bw);
            }
        }

        void Overlay::draw_shadow_ring(
            ws::ISurface *s,
            float xc, float yc,
            float x1, float y1,
            float x2, float y2,
            float angle)
        {
            float vx1           = x1 - xc;
            float vy1           = y1 - yc;
            float vx2           = x2 - xc;
            float vy2           = y2 - yc;
            const float r1      = sqrtf(vx1*vx1 + vy1*vy1);

            const size_t count  = lsp_max(fabsf(r1 * angle * 0.5f), 2.0f);
            const float phi     = angle / float(count);

            const float dx      = cosf(phi);
            const float dy      = sinf(phi);

            for (size_t i=0; i < count; ++i)
            {
                float nvx1          = vx1*dx - vy1*dy;
                float nvy1          = vx1*dy + vy1*dx;
                float nvx2          = vx2*dx - vy2*dy;
                float nvy2          = vx2*dy + vy2*dx;
                vx1                 = nvx1;
                vy1                 = nvy1;
                vx2                 = nvx2;
                vy2                 = nvy2;

                const float x3      = xc + vx1;
                const float y3      = yc + vy1;
                const float x4      = xc + vx2;
                const float y4      = yc + vy2;

                // Fill main part
                ws::IGradient *g    = s->linear_gradient(x1, y1, x2, y2);
                if (g == NULL)
                    return;
                lsp_finally { delete g; };

                // Prepare colors
                g->set_start(sShadowStart);
                g->set_stop(sShadowEnd, 1.0f);

                s->fill_triangle(g, x1, y1, x2, y2, x4, y4);
                s->fill_triangle(g, x1, y1, x4, y4, x3, y3);

                x1                  = x3;
                y1                  = y3;
                x2                  = x4;
                y2                  = y4;
            }
        }

        void Overlay::draw_shadow_segment(
            ws::ISurface *s,
            float x1, float y1,
            float x2, float y2,
            bool start, bool end,
            float width, float radius)
        {
            // Compute direction
            float dx            = (x2 - x1);
            float dy            = (y2 - y1);
            const float dw      = sqrtf(dx*dx + dy*dy);
            if (dw <= 0.05f)
                return;
            const float rdw     = 1.0f / dw;

            dx                 *= rdw;
            dy                 *= rdw;
            const float rdx     = radius * dx;
            const float rdy     = radius * dy;

            dx                 *= width;
            dy                 *= width;

            // Draw start segment
            float x0, y0, x3, y3;

            if ((start) && (radius >= 1.0f))
            {
                x1     += rdx;
                y1     += rdy;
                x0      = x1 + dy;
                y0      = y1 - dx;

                draw_shadow_ring(s, x1 - rdy, y1 + rdx, x1, y1, x0, y0, -0.25f * M_PI);
            }
            else
            {
                x0      = x1 + dy - dx;
                y0      = y1 - dx - dy;
            }

            if ((end) && (radius >= 1.0f))
            {
                x2     -= rdx;
                y2     -= rdy;
                x3      = x2 + dy;
                y3      = y2 - dx;

                draw_shadow_ring(s, x2 - rdy, y2 + rdx, x2, y2, x3, y3, 0.25f * M_PI);
            }
            else
            {
                x3      = x2 + dy + dx;
                y3      = y2 - dx + dy;
            }

            // Fill main part
            ws::IGradient *g    = s->linear_gradient(x1, y1, x1 + dy, y1 - dx);
            if (g != NULL)
            {
                lsp_finally { delete g; };

                // Prepare colors
                g->set_start(sShadowStart);
                g->set_stop(sShadowEnd, 1.0f);

                s->fill_triangle(g, x1, y1, x0, y0, x3, y3);
                s->fill_triangle(g, x1, y1, x3, y3, x2, y2);
            }
        }

        void Overlay::draw_shadow(ws::ISurface *s)
        {
            // Check that we need to draw shadow
            const float scaling = lsp_max(0.0f, sScaling.get());
            const size_t shsize = lsp_max(0.0f, sShadowSize.get() * scaling);
            if (shsize < 1)
                return;

            // Compute shadow parameters
            const size_t bround = sBorderRounding.corners();
            const size_t bsize  = lsp_max(0, sBorderSize.get());
            const size_t radius = lsp_max(0.0f, sBorderRadius.get() * scaling) + bsize;

            const float xb      = sSize.nLeft;
            const float xe      = sSize.nLeft + sSize.nWidth;
            const float yb      = sSize.nTop;
            const float ye      = sSize.nTop + sSize.nHeight;

            draw_shadow_segment(
                s, xb, yb, xe, yb,
                bround & ws::CORNER_LEFT_TOP, bround & ws::CORNER_RIGHT_TOP,
                shsize, radius);
            draw_shadow_segment(
                s, xe, yb, xe, ye,
                bround & ws::CORNER_RIGHT_TOP, bround & ws::CORNER_RIGHT_BOTTOM,
                shsize, radius);
            draw_shadow_segment(
                s, xe, ye, xb, ye,
                bround & ws::CORNER_RIGHT_BOTTOM, bround & ws::CORNER_LEFT_BOTTOM,
                shsize, radius);
            draw_shadow_segment(
                s, xb, ye, xb, yb,
                bround & ws::CORNER_LEFT_BOTTOM, bround & ws::CORNER_LEFT_TOP,
                shsize, radius);
        }

        status_t Overlay::add(Widget *widget)
        {
            if ((widget == NULL) || (widget == this))
                return STATUS_BAD_ARGUMENTS;
            if (wWidget != NULL)
                return STATUS_ALREADY_EXISTS;

            widget->set_parent(this);
            wWidget = widget;
            query_resize();
            return STATUS_OK;
        }

        status_t Overlay::remove(Widget *widget)
        {
            if (wWidget != widget)
                return STATUS_NOT_FOUND;

            unlink_widget(wWidget);
            wWidget  = NULL;
            query_resize();

            return STATUS_OK;
        }

        void Overlay::set_position_function(overlay_position_t func, void *data)
        {
            pPosFunc        = func;
            pPosData        = data;

            query_resize();
        }

        bool Overlay::calculate_position(ws::rectangle_t *rect)
        {
            if (rect == NULL)
                return false;

            if (pPosFunc != NULL)
                return pPosFunc(rect, this, pPosData);

            sPosition.get(&rect->nLeft, &rect->nTop);
            return true;
        }

    } /* namespace tk */
} /* namespace lsp */



