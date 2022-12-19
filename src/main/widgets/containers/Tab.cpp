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
                sTextLayout.bind("text.layout", this);
                sTextPadding.bind("text.padding", this);
                sFont.bind("font", this);
                sColor.bind("color", this);
                sSelectedColor.bind("selected.color", this);
                sHoverColor.bind("hover.color", this);
                sBorderColor.bind("border.color", this);
                sBorderSelectedColor.bind("border.selected.color", this);
                sBorderHoverColor.bind("border.hover.color", this);
                sTextColor.bind("text.color", this);
                sTextSelectedColor.bind("text.selected.color", this);
                sTextHoverColor.bind("text.hover.color", this);
                sBorderSize.bind("border.size", this);
                sBorderRadius.bind("border.radius", this);

                // Configure
                sLayout.set(0.0f, 1.0f);
                sTextAdjust.set(TA_NONE);
                sTextLayout.set(-1.0f, 0.0f);
                sTextPadding.set_all(2);
                sFont.set_size(12.0f);
                sColor.set("#cccccc");
                sSelectedColor.set("#ffffff");
                sHoverColor.set("#00ccff");
                sBorderColor.set("#888888");
                sBorderSelectedColor.set("#000000");
                sBorderHoverColor.set("#eeeeee");
                sTextColor.set("#888888");
                sTextSelectedColor.set("#000000");
                sTextHoverColor.set("#eeeeee");
                sBorderSize.set(1);
                sBorderRadius.set(4);
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
            sTextLayout(&sProperties),
            sTextPadding(&sProperties),
            sFont(&sProperties),
            sColor(&sProperties),
            sSelectedColor(&sProperties),
            sHoverColor(&sProperties),
            sBorderColor(&sProperties),
            sBorderSelectedColor(&sProperties),
            sBorderHoverColor(&sProperties),
            sTextColor(&sProperties),
            sTextSelectedColor(&sProperties),
            sTextHoverColor(&sProperties),
            sBorderSize(&sProperties),
            sBorderRadius(&sProperties)
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
            sTextLayout.bind("text.layout", &sStyle);
            sTextPadding.bind("text.padding", &sStyle);
            sFont.bind("font", &sStyle);
            sColor.bind("color", &sStyle);
            sSelectedColor.bind("selected.color", &sStyle);
            sHoverColor.bind("hover.color", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sBorderSelectedColor.bind("border.selected.color", &sStyle);
            sBorderHoverColor.bind("border.hover.color", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sTextSelectedColor.bind("text.selected.color", &sStyle);
            sTextHoverColor.bind("text.hover.color", &sStyle);
            sBorderSize.bind("border.size", &sStyle);
            sBorderRadius.bind("border.radius", &sStyle);

            return STATUS_OK;
        }

        void Tab::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);

            if (sLayout.is(prop))
                query_resize();
            if (prop->one_of(sText, sTextAdjust, sTextLayout, sTextPadding, sFont))
                query_resize();
            if (prop->one_of(sColor, sSelectedColor, sHoverColor,
                sBorderColor, sBorderSelectedColor, sBorderHoverColor,
                sTextColor, sTextSelectedColor, sTextHoverColor))
            {
                query_draw();
                tk::TabControl *tc = tk::widget_cast<tk::TabControl>(pParent);
                if (tc != NULL)
                    tc->query_draw(REDRAW_CHILD | REDRAW_SURFACE);
            }
            if (prop->one_of(sBorderSize, sBorderRadius))
                query_resize();
        }

        void Tab::size_request(ws::size_limit_t *r)
        {
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


