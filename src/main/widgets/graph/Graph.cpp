/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 20 авг. 2020 г.
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
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/common/debug.h>
#include <lsp-plug.in/lltl/ptrset.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(Graph, WidgetContainer)
                // Bind
                sConstraints.bind("size.constraints", this);
                sBorder.bind("border.size", this);
                sBorderRadius.bind("border.radius", this);
                sBorderFlat.bind("border.flat", this);
                sGlass.bind("glass.visibility", this);
                sColor.bind("color", this);
                sBorderColor.bind("border.color", this);
                sGlassColor.bind("glass.color", this);
                sIPadding.bind("ipadding", this);

                // Configure
                sConstraints.set_all(-1);
                sBorder.set(4);
                sBorderRadius.set(12);
                sBorderFlat.set(false);
                sGlass.set(true);
                sColor.set("#000000");
                sBorderColor.set("#000000");
                sGlassColor.set("#ffffff");
                sIPadding.set(1);
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(Graph, "Graph", "root");
        }

        const w_class_t Graph::metadata         =   { "Graph", &WidgetContainer::metadata };

        Graph::Graph(Display *dpy):
            WidgetContainer(dpy),
            vItems(&sProperties, &sIListener),
            sConstraints(&sProperties),
            sBorder(&sProperties),
            sBorderRadius(&sProperties),
            sBorderFlat(&sProperties),
            sGlass(&sProperties),
            sColor(&sProperties),
            sBorderColor(&sProperties),
            sGlassColor(&sProperties),
            sIPadding(&sProperties)
        {
            pGlass              = NULL;

            sCanvas.nLeft       = 0;
            sCanvas.nTop        = 0;
            sCanvas.nWidth      = 0;
            sCanvas.nHeight     = 0;
            sICanvas.nLeft      = 0;
            sICanvas.nTop       = 0;
            sICanvas.nWidth     = 0;
            sICanvas.nHeight    = 0;

            pClass              = &metadata;
        }

        Graph::~Graph()
        {
            nFlags     |= FINALIZED;
            do_destroy();
        }

        void Graph::destroy()
        {
            nFlags     |= FINALIZED;
            WidgetContainer::destroy();
            do_destroy();
        }

        void Graph::do_destroy()
        {
            // Unlink all items
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                GraphItem *item     = vItems.get(i);
                if (item == NULL)
                    continue;

                unlink_widget(item);
            }

            // Destroy glass
            drop_glass();

            vItems.flush();
            vAxis.flush();
            vBasis.flush();
            vOrigins.flush();
        }

        void Graph::drop_glass()
        {
            if (pGlass != NULL)
            {
                pGlass->destroy();
                delete pGlass;
                pGlass = NULL;
            }
        }

        status_t Graph::init()
        {
            status_t result = WidgetContainer::init();
            if (result != STATUS_OK)
                return result;

            // Init listener
            sIListener.bind_all(this, on_add_item, on_remove_item);

            // Init style
            sConstraints.bind("size.constraints", &sStyle);
            sBorder.bind("border.size", &sStyle);
            sBorderRadius.bind("border.radius", &sStyle);
            sBorderFlat.bind("border.flat", &sStyle);
            sGlass.bind("glass.visibility", &sStyle);
            sColor.bind("color", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sGlassColor.bind("glass.color", &sStyle);
            sIPadding.bind("ipadding", &sStyle);

            return STATUS_OK;
        }

        void Graph::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);
            if (vItems.is(prop))
                query_draw();

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
            {
                drop_glass();
                query_draw();
            }
            if (sGlassColor.is(prop))
                query_draw();
            if (sIPadding.is(prop))
                query_resize();
        }

        void Graph::size_request(ws::size_limit_t *r)
        {
            float scaling   = lsp_max(0.0f, sScaling.get());
            sConstraints.compute(r, scaling);
            sIPadding.add(r, scaling);

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

        void Graph::realize(const ws::rectangle_t *r)
        {
            // Call parent class to realize
            WidgetContainer::realize(r);

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

            sICanvas.nLeft  = 0;
            sICanvas.nTop   = 0;
            sICanvas.nWidth = sCanvas.nWidth;
            sICanvas.nHeight= sCanvas.nHeight;

            sIPadding.enter(&sICanvas, scaling);

            for (size_t i=0, n = vItems.size(); i<n; ++i)
            {
                tk::GraphItem *gi = vItems.get(i);
                if (gi == NULL)
                    continue;
                tk::Slot *slot = gi->slot(SLOT_RESIZE_PARENT);
                if (slot != NULL)
                {
                    ws::rectangle_t xr = *r;
                    slot->execute(this, &xr);
                }
            }
        }

        void Graph::hide_widget()
        {
            WidgetContainer::hide_widget();
            drop_glass();
        }

        void Graph::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            if (nFlags & REDRAW_SURFACE)
                force = true;

            float scaling   = lsp_max(0.0f, sScaling.get());
            float xr        = lsp_max(0.0f, sBorderRadius.get() * scaling); // external radius
            float bw        = lsp_max(0.0f, sBorder.get() * scaling);       // border size
            float bright    = sBrightness.get();

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

                // Get surface of widget
                cv  = get_surface(s, sCanvas.nWidth, sCanvas.nHeight);
                if (cv != NULL)
                    s->draw(cv, sCanvas.nLeft, sCanvas.nTop, 1.0f, 1.0f, 0.0f);

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
        }

        ssize_t Graph::check_collision(const w_alloc_t *a, const w_alloc_t *b)
        {
            if ((a->nGroup != b->nGroup) || (a->nPriority == b->nPriority))
                return 0;
            if (!Size::overlap(&a->sRect, &b->sRect))
                return 0;

            return b->nPriority - a->nPriority;
        }

        ssize_t Graph::compare_walloc(const w_alloc_t *a, const w_alloc_t *b)
        {
            ssize_t diff = a->nGroup - b->nGroup;
            if (diff == 0)
                diff        = a->nPriority - b->nPriority;
            return diff;
        }

        void Graph::draw(ws::ISurface *s, bool force)
        {
            // Clear canvas
            lsp::Color c(sColor);
            // c.set_rgb24(0x888888);
            c.scale_lch_luminance(sBrightness.get());
            s->clear(c);

            // Sync internal lists of axes and origins
            sync_lists();

            // Compute list of discarded widgets
            lltl::ptrset<GraphItem> discarded;
            {
                lltl::darray<w_alloc_t> grouped;

                // Fill all grouped widgets
                for (size_t i=0, n=vItems.size(); i<n; ++i)
                {
                    GraphItem *gi = vItems.get(i);
                    if ((gi == NULL) || (!gi->visibility()->get()))
                        continue;

                    // Fill the widget allocation
                    w_alloc_t wa;
                    wa.nGroup = gi->priority_group()->get();
                    if (wa.nGroup < 0)
                        continue;
                    if (!gi->bound_box(s, &wa.sRect))
                        continue;

                    wa.nPriority = gi->priority()->get();
                    wa.pWidget   = gi;

                    grouped.add(&wa);
                }
                grouped.qsort(compare_walloc);

                // Scan for conflicting widgets and discard some widgets according to priority
                for (size_t i=0, n=grouped.size(); i<n; ++i)
                {
                    w_alloc_t *wa = grouped.uget(i);
                    if (wa == NULL)
                        continue;

                    for (size_t j=0; j<i; ++j)
                    {
                        w_alloc_t *wb = grouped.uget(j);
                        if (wb == NULL)
                            continue;

                        if (check_collision(wb, wa) > 0)
                        {
                            discarded.put(wa->pWidget);
                            break;
                        }
                    }
                }
            }

            // Draw all objects
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                GraphItem *gi = vItems.get(i);
                if ((gi == NULL) || (!gi->visibility()->get()))
                    continue;
                if (discarded.contains(gi))
                    continue;

                gi->render(s, &sICanvas, true);
                gi->commit_redraw();
            }
        }

        void Graph::sync_lists()
        {
            vAxis.clear();
            vBasis.clear();
            vOrigins.clear();

            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                GraphItem *gi = vItems.get(i);
                if (gi == NULL)
                    continue;

                GraphOrigin *go = widget_cast<GraphOrigin>(gi);
                if (go != NULL)
                    vOrigins.add(go);
                GraphAxis *ga = widget_cast<GraphAxis>(gi);
                if (ga != NULL)
                {
                    vAxis.add(ga);
                    if (ga->basis()->get())
                        vBasis.add(ga);
                }
            }
        }

        Widget *Graph::find_widget(ssize_t x, ssize_t y)
        {
            // Check that we are within allowed area
            ssize_t tx = x - (sCanvas.nLeft + sICanvas.nLeft);
            ssize_t ty = y - (sCanvas.nTop  + sICanvas.nTop);
            if ((tx < 0) || (ty < 0))
                return NULL;
            if ((tx > sICanvas.nWidth) || (ty > sICanvas.nHeight))
                return NULL;

            // Sync internal lists of axes and origins
            sync_lists();

            // Lookup widgets
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                GraphItem *gi = vItems.get(i);
                if ((gi == NULL) || (!gi->is_visible_child_of(this)))
                    continue;

                if (gi->inside(x, y))
                    return gi;
            }
            return NULL;
        }

        void Graph::on_add_item(void *obj, Property *prop, void *w)
        {
            GraphItem *item = widget_ptrcast<GraphItem>(w);
            if (item == NULL)
                return;

            Graph *_this = widget_ptrcast<Graph>(obj);
            if (_this == NULL)
                return;

            item->set_parent(_this);
            _this->query_draw();
        }

        void Graph::on_remove_item(void *obj, Property *prop, void *w)
        {
            GraphItem *item = widget_ptrcast<GraphItem>(w);
            if (item == NULL)
                return;

            Graph *_this = widget_ptrcast<Graph>(obj);
            if (_this == NULL)
                return;

            // Remove widget from supplementary structures
            _this->unlink_widget(item);
            _this->query_draw();
        }

        bool Graph::origin(size_t index, float *x, float *y)
        {
            return origin(vOrigins.get(index), x, y);
        }

        bool Graph::origin(GraphOrigin *o, float *x, float *y)
        {
            if (o == NULL)
            {
                *x      = 0.0f;
                *y      = 0.0f;
                return false;
            }

            *x  = sICanvas.nLeft + sICanvas.nWidth  * (1.0f + o->left()->get()) * 0.5f;
            *y  = sICanvas.nTop  + sICanvas.nHeight * (1.0f - o->top()->get() ) * 0.5f;

            return true;
        }

        status_t Graph::add(Widget *child)
        {
            GraphItem *item     = widget_cast<GraphItem>(child);
            if (item == NULL)
                return STATUS_BAD_TYPE;

            status_t res = vItems.add(item);
            if (res == STATUS_OK)
            {
                GraphOrigin *go     = widget_cast<GraphOrigin>(child);
                if (go != NULL)
                    vOrigins.add(go);
                GraphAxis *ga       = widget_cast<GraphAxis>(child);
                if (ga != NULL)
                {
                    vAxis.add(ga);
                    if (ga->basis()->get())
                        vBasis.add(ga);
                }
            }
            return res;
        }

        status_t Graph::remove(Widget *child)
        {
            GraphItem *item     = widget_cast<GraphItem>(child);
            if (item == NULL)
                return STATUS_BAD_TYPE;

            status_t res        = vItems.premove(item);
            if (res == STATUS_OK)
            {
                GraphOrigin *go     = widget_cast<GraphOrigin>(child);
                if (go != NULL)
                    vOrigins.premove(go);
                GraphAxis *ga       = widget_cast<GraphAxis>(child);
                if (ga != NULL)
                {
                    vAxis.premove(ga);
                    if (ga->basis()->get())
                        vBasis.premove(ga);
                }
            }

            return res;
        }

        status_t Graph::remove_all()
        {
            vItems.clear();
            vOrigins.clear();
            vAxis.clear();
            vBasis.clear();
            return STATUS_OK;
        }

        void Graph::canvas_size(ws::rectangle_t *r)
        {
            *r      = sICanvas;
        }

        status_t Graph::xy_to_axis(size_t index, float *out, ssize_t x, ssize_t y)
        {
            // Find the axis
            tk::GraphAxis *ax = axis(index);
            if (ax == NULL)
                return STATUS_NOT_FOUND;

            // Perform computation stuff
            if (out != NULL)
            {
                x      -= canvas_aleft();
                y      -= canvas_atop();
                *out    = ax->project(x, y);
            }

            return STATUS_OK;
        }

        status_t Graph::axis_to_xy(size_t index, ssize_t *x, ssize_t *y, float value)
        {
            // Find the axis and the origin
            tk::GraphAxis *ax = axis(index);
            if (ax == NULL)
                return STATUS_NOT_FOUND;
            tk::GraphOrigin *o = origin(ax->origin()->get());
            if (o == NULL)
                return STATUS_NOT_FOUND;

            // Perform computation stuff
            float xx = 0, yy = 0;
            if (!origin(o, &xx, &yy))
                return STATUS_NOT_FOUND;

            // Translate the point coordinates
            if (!ax->apply(&xx, &yy, &value, 1))
                return STATUS_NOT_FOUND;

            // Return success
            if (x != NULL)
                *x  = xx;
            if (y != NULL)
                *y  = yy;

            return STATUS_OK;
        }
    } /* namespace tk */
} /* namespace lsp */
