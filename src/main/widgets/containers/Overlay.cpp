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
            sBorderColor(&sProperties)
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



