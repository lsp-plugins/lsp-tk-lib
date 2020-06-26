/*
 * Group.cpp
 *
 *  Created on: 26 июн. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t Group::metadata         = { "Group", &WidgetContainer::metadata };

        Group::Group(Display *dpy):
            WidgetContainer(dpy),
            sFont(&sProperties),
            sColor(&sProperties),
            sTextColor(&sProperties),
            sText(&sProperties),
            sShowText(&sProperties),
            sLayout(&sProperties),
            sBorder(&sProperties),
            sTextBorder(&sProperties),
            sRadius(&sProperties),
            sTextRadius(&sProperties),
            sEmbedding(&sProperties)
        {
            pWidget             = NULL;

            sLabel.nLeft        = 0;
            sLabel.nTop         = 0;
            sLabel.nWidth       = 0;
            sLabel.nHeight      = 0;

            sArea.nLeft         = 0;
            sArea.nTop          = 0;
            sArea.nWidth        = 0;
            sArea.nHeight       = 0;

            pClass              = &metadata;
        }

        Group::~Group()
        {
            do_destroy();
        }

        void Group::destroy()
        {
            do_destroy();
            WidgetContainer::destroy();
        }

        void Group::do_destroy()
        {
            if (pWidget != NULL)
            {
                unlink_widget(pWidget);
                pWidget = NULL;
            }
        }

        status_t Group::init()
        {
            status_t result = WidgetContainer::init();
            if (result != STATUS_OK)
                return result;

            sFont.bind("font", &sStyle);
            sColor.bind("color", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sText.bind(&sStyle, pDisplay->dictionary());
            sShowText.bind("text.show", &sStyle);
            sLayout.bind("layout", &sStyle);
            sBorder.bind("border.size", &sStyle);
            sTextBorder.bind("text.border", &sStyle);
            sRadius.bind("border.radius", &sStyle);
            sTextRadius.bind("text.radius", &sStyle);
            sEmbedding.bind("embed", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sFont.init(sclass);
                sColor.init(sclass, "#000000");
                sTextColor.init(sclass, "#ffffff");
                sShowText.init(sclass, true);
                sLayout.init(sclass, 0.0f, 0.0f, 1.0f, 1.0f);
                sBorder.init(sclass, 2);
                sTextBorder.init(sclass, 2);
                sRadius.init(sclass, 8);
                sTextRadius.init(sclass, 8);
                sEmbedding.init(sclass, false);
            }

            return STATUS_OK;
        }

        Widget *Group::find_widget(ssize_t x, ssize_t y)
        {
            if (pWidget == NULL)
                return NULL;

            return (pWidget->inside(x, y)) ? pWidget : NULL;
        }

        void Group::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);
            if (sFont.is(prop))
                query_resize();
            if (sColor.is(prop))
                query_draw();
            if (sTextColor.is(prop))
                query_draw();
            if (sText.is(prop))
                query_resize();
            if (sShowText.is(prop))
                query_resize();
            if (sLayout.is(prop))
                query_resize();
            if (sBorder.is(prop))
                query_resize();
            if (sRadius.is(prop))
                query_resize();
            if (sTextRadius.is(prop))
                query_resize();
            if (sEmbedding.is(prop))
                query_resize();
        }

        void Group::allocate(ws::rectangle_t *text, padding_t *pad)
        {
            float scaling   = lsp_max(0.0f, sScaling.get());
            ssize_t border  = (sBorder.get() > 0) ? lsp_max(1.0f, sBorder.get() * scaling) : 0;
            ssize_t radius  = lsp_max(0.0f, sRadius.get() * scaling);

            // Text allocation
            ws::rectangle_t xr;
            text->nLeft     = 0;
            text->nTop      = 0;

            if (sShowText.get())
            {
                LSPString s;
                ws::text_parameters_t tp;
                ws::font_parameters_t fp;

                ssize_t tborder     = lsp_max(0.0f, sTextBorder.get() * scaling);
                ssize_t tradius     = lsp_max(0.0f, sTextRadius.get() * scaling);
                ssize_t rgap        = lsp_max(tborder, M_SQRT1_2 * tradius);
                sText.format(&s);

                sFont.get_parameters(pDisplay, scaling, &fp);
                text->nLeft         = tp.Width;
                text->nTop          = lsp_max(fp.Height, tp.Height);

                xr                  = *text;
                xr.nWidth          += tborder + rgap + radius * 3;
                xr.nHeight         += tborder + rgap;
            }
            else
            {
                text->nWidth        = 0;
                text->nHeight       = 0;
                xr                  = *text;
            }

            // Compute padding
            ssize_t xborder = lsp_max(0.0f, (radius-border) * M_SQRT1_2);

            pad->nLeft      = (sEmbedding.left())   ? border : xborder;
            pad->nRight     = (sEmbedding.right())  ? border : xborder;
            pad->nTop       = (sEmbedding.top())    ? border : xborder;
            pad->nBottom    = (sEmbedding.bottom()) ? border : xborder;

            pad->nTop       = lsp_max(xr.nHeight, ssize_t(pad->nTop));
        }

        void Group::size_request(ws::size_limit_t *r)
        {
            ws::rectangle_t text;
            padding_t pad;

            allocate(&text, &pad);

            if (pWidget == NULL)
            {
                r->nMinWidth    = -1;
                r->nMinHeight   = -1;
                r->nMaxWidth    = -1;
                r->nMaxHeight   = -1;
            }
            else
                pWidget->get_padded_size_limits(r);

            Padding::add(r, &pad);
        }

        void Group::realize(const ws::rectangle_t *r)
        {
            lsp_trace("width=%d, height=%d", int(r->nWidth), int(r->nHeight));
            WidgetContainer::realize(r);

            if ((pWidget == NULL) || (!pWidget->visibility()->get()))
                return;

            // Realize child widget
            ws::rectangle_t xr;
            ws::size_limit_t sr;

            pWidget->get_padded_size_limits(&sr);
            sLayout.apply(&xr, r, &sr);
            pWidget->padding()->enter(&xr, pWidget->scaling()->get());
            pWidget->realize_widget(&xr);
        }

        void Group::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            if (nFlags & REDRAW_SURFACE)
                force = true;

            // Clear the space
            lsp::Color bg_color(sBgColor);

            // Draw background if child is invisible or not present
            if ((pWidget == NULL) || (!pWidget->visibility()->get()))
            {
                s->fill_rect(bg_color, sSize.nLeft, sSize.nTop, sSize.nWidth, sSize.nHeight);
                return;
            }

            if ((force) || (pWidget->redraw_pending()))
            {
                // Draw the child only if it is visible in the area
                ws::rectangle_t xr;
                pWidget->get_rectangle(&xr);
                if (Size::intersection(&xr, &sSize))
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
                        bg_color.copy(pWidget->bg_color()->color());
                        s->fill_frame(bg_color, &sSize, &cr);
                    }
                    s->clip_end();
                }
            }
        }

        status_t Group::add(Widget *widget)
        {
            if (widget == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (pWidget != NULL)
                return STATUS_ALREADY_EXISTS;

            widget->set_parent(this);
            pWidget = widget;
            query_resize();
            return STATUS_OK;
        }

        status_t Group::remove(Widget *widget)
        {
            if (pWidget != widget)
                return STATUS_NOT_FOUND;

            unlink_widget(pWidget);
            pWidget  = NULL;

            return STATUS_OK;
        }
    } /* namespace tk */
} /* namespace lsp */


