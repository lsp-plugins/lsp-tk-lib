/*
 * Copyright (C) 2022 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2022 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 9 нояб. 2022 г.
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
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(Tab, WidgetContainer)
                // Bind
                sLayout.bind("layout", this);
                sTextAdjust.bind("text.adjust", this);
                sFont.bind("font", this);
                sColor.bind("color", this);
                sSelectedColor.bind("selected.color", this);
                sHoverColor.bind("hover.color", this);
                sTextColor.bind("text.color", this);
                sTextSelectedColor.bind("text.selected.color", this);
                sTextHoverColor.bind("text.hover.color", this);

                // Configure
                sLayout.set(0.0f, 1.0f);
                sTextAdjust.set(TA_NONE);
                sFont.set_size(12.0f);
                sColor.set("#cccccc");
                sSelectedColor.set("#ffffff");
                sHoverColor.set("#00ccff");
                sTextColor.set("#888888");
                sTextSelectedColor.set("#000000");
                sTextHoverColor.set("#eeeeee");
            LSP_TK_STYLE_IMPL_END

            LSP_TK_BUILTIN_STYLE(Tab, "Tab", "root");
        } /* namespace style */

        //-----------------------------------------------------------------------------
        // Tab implementation
        const w_class_t Tab::metadata           = { "Tab", &WidgetContainer::metadata };

        Tab::Tab(Display *dpy):
            WidgetContainer(dpy),
            sLayout(&sProperties),
            sText(&sProperties),
            sTextAdjust(&sProperties),
            sFont(&sProperties),
            sColor(&sProperties),
            sSelectedColor(&sProperties),
            sHoverColor(&sProperties),
            sTextColor(&sProperties),
            sTextSelectedColor(&sProperties),
            sTextHoverColor(&sProperties)
        {
            pWidget     = NULL;

            pClass      = &metadata;
        }

        Tab::~Tab()
        {
            nFlags     |= FINALIZED;
            do_destroy();
        }

        void Tab::do_destroy()
        {
            if (pWidget != NULL)
            {
                unlink_widget(pWidget);
                pWidget = NULL;
            }
        }

        void Tab::destroy()
        {
            nFlags     |= FINALIZED;
            do_destroy();
            WidgetContainer::destroy();
        }

        status_t Tab::init()
        {
            status_t result = WidgetContainer::init();
            if (result != STATUS_OK)
                return result;

            sLayout.bind("layout", &sStyle);
            sText.bind(&sStyle, pDisplay->dictionary());
            sTextAdjust.bind("text.adjust", &sStyle);
            sFont.bind("font", &sStyle);
            sColor.bind("color", &sStyle);
            sSelectedColor.bind("selected.color", &sStyle);
            sHoverColor.bind("hover.color", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sTextSelectedColor.bind("text.selected.color", &sStyle);
            sTextHoverColor.bind("text.hover.color", &sStyle);

            return STATUS_OK;
        }

        void Tab::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);

            if (sLayout.is(prop))
                query_resize();
            if (sText.is(prop))
                query_resize();
            if (sTextAdjust.is(prop))
                query_resize();
            if (sFont.is(prop))
                query_resize();
            if (prop->one_of(sColor, sSelectedColor, sHoverColor,
                sTextColor, sTextSelectedColor, sTextHoverColor))
                query_draw();
        }

        void Tab::size_request(ws::size_limit_t *r)
        {
            float scaling   = lsp_max(0.0f, sScaling.get());

            if ((pWidget == NULL) || (!pWidget->is_visible_child_of(this)))
            {
                r->nMinWidth    = -1;
                r->nMinHeight   = -1;
                r->nMaxWidth    = -1;
                r->nMaxHeight   = -1;
            }
            else
            {
                pWidget->get_padded_size_limits(r);
                r->nMaxWidth    = -1;
                r->nMaxHeight   = -1;
            }

            r->nPreWidth    = -1;
            r->nPreHeight   = -1;
        }

        void Tab::realize(const ws::rectangle_t *r)
        {
            WidgetContainer::realize(r);

            if ((pWidget == NULL) || (!pWidget->is_visible_child_of(this)))
                return;

            // Realize child widget
            ws::rectangle_t xr;
            ws::size_limit_t sr;

            pWidget->get_padded_size_limits(&sr);
            sLayout.apply(&xr, r, &sr);
            pWidget->padding()->enter(&xr, pWidget->scaling()->get());
            pWidget->realize_widget(&xr);
        }

        void Tab::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            if (nFlags & REDRAW_SURFACE)
                force = true;

            // Initialize palette
            lsp::Color bg_color;
            get_actual_bg_color(bg_color);

            // Draw background if child is invisible or not present
            if ((pWidget == NULL) || (!pWidget->visibility()->get()))
            {
                s->clip_begin(area);
                s->fill_rect(bg_color, SURFMASK_NONE, 0.0f, &sSize);
                s->clip_end();
                return;
            }

            if ((force) || (pWidget->redraw_pending()))
            {
                // Draw the child only if it is visible in the area
                ws::rectangle_t xr;
                pWidget->get_rectangle(&xr);
                if (Size::intersection(&xr, area))
                    pWidget->render(s, &xr, force);

                pWidget->commit_redraw();
            }

            if (force)
            {
                ws::rectangle_t cr;

                pWidget->get_rectangle(&cr);
                if (Size::overlap(area, &sSize))
                {
                    s->clip_begin(area);
                    {
                        pWidget->get_actual_bg_color(bg_color);
                        s->fill_frame(bg_color, SURFMASK_NONE, 0.0f, &sSize, &cr);
                    }
                    s->clip_end();
                }
            }
        }

        status_t Tab::add(Widget *widget)
        {
            if ((widget == NULL) || (widget == this))
                return STATUS_BAD_ARGUMENTS;
            if (pWidget != NULL)
                return STATUS_ALREADY_EXISTS;

            widget->set_parent(this);
            pWidget = widget;
            query_resize();

            return STATUS_OK;
        }

        status_t Tab::remove(Widget *widget)
        {
            if (pWidget != widget)
                return STATUS_NOT_FOUND;

            unlink_widget(pWidget);
            pWidget  = NULL;
            query_resize();

            return STATUS_OK;
        }

        Widget *Tab::find_widget(ssize_t x, ssize_t y)
        {
            if ((pWidget == NULL) || (!pWidget->is_visible_child_of(this)))
                return NULL;

            return (pWidget->inside(x, y)) ? pWidget : NULL;
        }

    } /* namespace tk */
} /* namespace lsp */


