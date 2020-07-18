/*
 * ScrollArea.cpp
 *
 *  Created on: 17 июл. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t ScrollArea::metadata    = { "ScrollArea", &WidgetContainer::metadata };
        
        ScrollArea::ScrollArea(Display *dpy):
            WidgetContainer(dpy),
            sHBar(dpy),
            sVBar(dpy),
            sLayout(&sProperties),
            sSizeConstraints(&sProperties),
            sHScroll(&sProperties),
            sVScroll(&sProperties)
        {
            pWidget     = NULL;
            
            pClass      = &metadata;
        }
        
        ScrollArea::~ScrollArea()
        {
            do_destroy();
        }

        status_t ScrollArea::init()
        {
            // Initialize widgets
            status_t result = WidgetContainer::init();
            if (result == STATUS_OK)
                result  = sHBar.init();
            if (result == STATUS_OK)
                result  = sVBar.init();
            if (result != STATUS_OK)
                return result;

            // Configure scroll bars
            sHBar.orientation()->set(O_HORIZONTAL);
            sHBar.orientation()->set(O_VERTICAL);
            sHBar.set_parent(this);
            sVBar.set_parent(this);

            // Initialize style
            sLayout.bind("layout", &sStyle);
            sSizeConstraints.bind("size.constraints", &sStyle);
            sHScroll.bind("hscroll", &sStyle);
            sVScroll.bind("vscroll", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sLayout.init(sclass, -1.0f, -1.0f, 0.0f, 0.0f);
                sSizeConstraints.init(sclass);
                sHScroll.init(sclass, SCROLL_OPTIONAL);
                sVScroll.init(sclass, SCROLL_OPTIONAL);
            }

            return STATUS_OK;
        }

        void ScrollArea::destroy()
        {
            do_destroy();
            WidgetContainer::destroy();
        }

        void ScrollArea::do_destroy()
        {
            if (pWidget != NULL)
            {
                unlink_widget(pWidget);
                pWidget = NULL;
            }
        }

        void ScrollArea::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);
            if (sLayout.is(prop))
                query_resize();
            if (sSizeConstraints.is(prop))
                query_resize();
            if (sHScroll.is(prop))
                query_resize();
            if (sVScroll.is(prop))
                query_resize();
        }

        void ScrollArea::estimate_size(alloc_t *a, const ws::rectangle_t *xr)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());
            scrolling_t hscroll = sHScroll.get();
            scrolling_t vscroll = sVScroll.get();

            // Estimate size of each scroll bar
            ws::size_limit_t hb, vb, wid;
            sHBar.get_padded_size_limits(&hb);
            sVBar.get_padded_size_limits(&vb);

            hb.nMinWidth    = lsp_max(0, hb.nMinWidth);
            hb.nMinHeight   = lsp_max(0, hb.nMinHeight);
            vb.nMinWidth    = lsp_max(0, vb.nMinWidth);
            vb.nMinHeight   = lsp_max(0, vb.nMinHeight);

            a->sArea        = *xr;

            // Estimate child widget properties
            if ((pWidget == NULL) || (!pWidget->visibility()->get()))
            {
                wid.nMinWidth   = -1;
                wid.nMinHeight  = -1;
                wid.nMaxWidth   = -1;
                wid.nMaxHeight  = -1;
            }
            else
                pWidget->get_padded_size_limits(&wid);

            a->wMinW        = lsp_max(0, wid.nMinWidth);
            a->wMinH        = lsp_max(0, wid.nMinHeight);

            ssize_t minw    = (sHScroll.clip()) ? 0 : a->wMinW;
            ssize_t minh    = (sVScroll.clip()) ? 0 : a->wMinH;

            if ((hscroll == SCROLL_ALWAYS) || (hscroll == SCROLL_OPTIONAL))
            {
                if ((vscroll == SCROLL_ALWAYS) || (vscroll == SCROLL_OPTIONAL))
                {
                    a->sSize.nMinWidth  = hb.nMinWidth  + vb.nMinWidth;
                    a->sSize.nMinHeight = hb.nMinHeight + vb.nMinHeight;
                }
                else
                {
                    a->sSize.nMinWidth  = hb.nMinWidth;
                    a->sSize.nMinHeight = hb.nMinHeight + minh;
                }
            }
            else if ((vscroll == SCROLL_ALWAYS) || (vscroll == SCROLL_OPTIONAL))
            {
                a->sSize.nMinWidth  = vb.nMinWidth  + minw;
                a->sSize.nMinHeight = vb.nMinHeight;
            }
            else
            {
                a->sSize.nMinWidth  = minw;
                a->sSize.nMinHeight = minh;
            }

            a->sSize.nMaxWidth  = (sAllocation.hembed()) ? lsp_max(minw, a->sSize.nMinWidth) : -1;
            a->sSize.nMaxHeight = (sAllocation.vembed()) ? lsp_max(minw, a->sSize.nMinHeight) : -1;

            // Apply size constraints
            sSizeConstraints.apply(&a->sSize, scaling);

            if ((xr->nWidth < 0) || (xr->nHeight < 0))
                return;

            a->sArea            = *xr;
            a->sHBar.nLeft      = xr->nLeft;
            a->sHBar.nTop       = xr->nTop  + xr->nHeight - hb.nMinHeight;
            a->sHBar.nWidth     = xr->nWidth;
            a->sHBar.nHeight    = hb.nMinHeight;
            a->sVBar.nLeft      = xr->nLeft + xr->nWidth  - vb.nMinWidth;
            a->sVBar.nTop       = xr->nTop;
            a->sVBar.nWidth     = vb.nMinWidth;
            a->sVBar.nHeight    = xr->nHeight;

            if ((hscroll == SCROLL_ALWAYS) || ((hscroll == SCROLL_OPTIONAL) && (xr->nWidth < minw)))
            {
                a->bHBar            = true;
                a->sArea.nHeight   -= hb.nMinHeight;

                if ((vscroll == SCROLL_ALWAYS) || ((vscroll == SCROLL_OPTIONAL) && (xr->nHeight < minh)))
                {
                    a->bVBar            = true;
                    a->sArea.nWidth    -= vb.nMinWidth;

                    a->sHBar.nWidth    -= vb.nMinWidth;
                    a->sVBar.nHeight   -= vb.nMinHeight;
                }
            }
            else if ((vscroll == SCROLL_ALWAYS) || ((vscroll == SCROLL_OPTIONAL) && (xr->nHeight < minh)))
            {
                a->bVBar            = true;
                a->sArea.nWidth    -= vb.nMinWidth;
            }
        }

        void ScrollArea::size_request(ws::size_limit_t *r)
        {
            alloc_t a;
            ws::rectangle_t xr;

            xr.nLeft    = 0;
            xr.nTop     = 0;
            xr.nWidth   = -1;
            xr.nHeight  = -1;

            estimate_size(&a, &xr);

            *r          = a.sSize;
        }

        void ScrollArea::realize(const ws::rectangle_t *r)
        {
            alloc_t a;
            estimate_size(&a, r);

            // Tune scroll bars
            sHBar.visibility()->set(a.bHBar);
            sVBar.visibility()->set(a.bVBar);

            if (a.bHBar)
            {
                sHBar.realize_widget(&a.sHBar);
                sHBar.value()->set_range(0, lsp_max(0, a.wMinW - a.sArea.nWidth));
            }
            if (a.bVBar)
            {
                sVBar.realize_widget(&a.sVBar);
                sHBar.value()->set_range(0, lsp_max(0, a.wMinH - a.sArea.nHeight));
            }

            // Realize child widget if present
            if ((pWidget != NULL) && (pWidget->visibility()->get()))
            {
                ws::rectangle_t xr, rr;
                ws::size_limit_t sr;

                rr          = a.sArea;
                rr.nWidth   = lsp_max(a.wMinW, rr.nWidth);
                rr.nHeight  = lsp_max(a.wMinH, rr.nHeight);

                pWidget->get_padded_size_limits(&sr);
                sLayout.apply(&xr, &rr, &sr);

                if (a.bHBar)
                    xr.nLeft   -= sHBar.value()->get();
                if (a.bVBar)
                    xr.nTop    -= sVBar.value()->get();

                pWidget->padding()->enter(&xr, pWidget->scaling()->get());
                pWidget->realize_widget(&xr);
            }
        }

        void ScrollArea::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            if (nFlags & REDRAW_SURFACE)
                force = true;

            lsp::Color col(sBgColor);

            ws::rectangle_t h, v, xa;
            xa  = sSize;

            // Render scroll bars
            if (sHBar.visibility()->get())
            {
                sHBar.get_padded_rectangle(&h);
                xa.nHeight  -= h.nHeight;
                if ((sHBar.redraw_pending()) || (force))
                {
                    sHBar.render(s, area, force);
                    sHBar.commit_redraw();
                }

                if (sVBar.visibility()->get())
                {
                    sHBar.get_padded_rectangle(&v);
                    xa.nWidth   -= v.nWidth;
                    if ((sVBar.redraw_pending()) || (force))
                    {
                        sVBar.render(s, area, force);
                        sVBar.commit_redraw();
                    }

                    // Draw the padding
                    if (force)
                    {
                        s->clip_begin(area);
                        s->fill_rect(col, h.nLeft + h.nWidth, v.nTop + v.nHeight, v.nWidth, h.nHeight);
                        s->clip_end();
                    }
                }
            }
            else if (sVBar.visibility()->get())
            {
                sHBar.get_padded_rectangle(&h);
                xa.nWidth   -= v.nWidth;

                if ((sVBar.redraw_pending()) || (force))
                {
                    sVBar.render(s, area, force);
                    sVBar.commit_redraw();
                }
            }

            // Draw background if child is invisible or not present
            if ((pWidget == NULL) || (!pWidget->visibility()->get()))
            {
                s->clip_begin(area);
                    s->fill_rect(col, &xa);
                s->clip_end();
                return;
            }

            if ((force) || (pWidget->redraw_pending()))
            {
                // Draw the child only if it is visible in the area
                ws::rectangle_t xr;
                pWidget->get_rectangle(&xr);
                if (Size::intersection(&xr, &xa))
                    pWidget->render(s, &xr, force);

                pWidget->commit_redraw();
            }

            if (force)
            {
                ws::rectangle_t cr;

                pWidget->get_rectangle(&cr);
                if (Size::overlap(area, &xa))
                {
                    s->clip_begin(area);
                    {
                        col.copy(pWidget->bg_color()->color());
                        s->fill_frame(col, &xa, &cr);
                    }
                    s->clip_end();
                }
            }
        }

        status_t ScrollArea::add(Widget *widget)
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

        status_t ScrollArea::remove(Widget *widget)
        {
            if (pWidget != widget)
                return STATUS_NOT_FOUND;

            unlink_widget(pWidget);
            pWidget  = NULL;

            return STATUS_OK;
        }

        Widget *ScrollArea::find_widget(ssize_t x, ssize_t y)
        {
            if ((sHBar.visibility()->get()) && (sHBar.inside(x, y)))
                return &sHBar;
            if ((sVBar.visibility()->get()) && (sVBar.inside(x, y)))
                return &sVBar;
            if ((pWidget != NULL) && (pWidget->inside(x, y)))
                return pWidget;

            return NULL;
        }

    } /* namespace tk */
} /* namespace lsp */
