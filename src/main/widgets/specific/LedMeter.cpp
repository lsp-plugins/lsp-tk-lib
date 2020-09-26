/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 25 сент. 2020 г.
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
#include <lsp-plug.in/stdlib/math.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LedMeter::metadata              = { "LedMeter", &Widget::metadata };

        LedMeter::LedMeter(Display *dpy):
            Widget(dpy),
            vItems(&sProperties),
            sConstraints(&sProperties),
            sFont(&sProperties),
            sBorder(&sProperties),
            sAngle(&sProperties),
            sEstText(&sProperties),
            sSGroups(&sProperties),
            sTextVisible(&sProperties)
        {
            sAAll.nLeft     = 0;
            sAAll.nTop      = 0;
            sAAll.nWidth    = 0;
            sAAll.nHeight   = 0;

            pClass          = &metadata;
        }

        LedMeter::~LedMeter()
        {
            do_destroy();
        }

        void LedMeter::destroy()
        {
            Widget::destroy();
            do_destroy();
        }

        void LedMeter::do_destroy()
        {
            // Unlink all items
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                LedMeterChannel *item   = vItems.uget(i);
                if (item == NULL)
                    continue;

                unlink_widget(item);
            }

            // Flush containers
            vItems.flush();
            vVisible.flush();
        }

        status_t LedMeter::init()
        {
            status_t res = Widget::init();
            if (res != STATUS_OK)
                return res;

            // Init listener
            sIListener.bind_all(this, on_add_item, on_remove_item);

            // Bind properties
            sConstraints.bind("constraints", &sStyle);
            sFont.bind("font", &sStyle);
            sBorder.bind("border", &sStyle);
            sAngle.bind("angle", &sStyle);
            sEstText.bind(&sStyle, pDisplay->dictionary());
            sSGroups.bind("stereo_groups", &sStyle);
            sTextVisible.bind("text.visible", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sConstraints.init(sclass, 20, -1, 20, -1);
                sFont.init(sclass, 9);
                sBorder.init(sclass, 2);
                sAngle.init(sclass, 0);
                sSGroups.init(sclass, true);
                sTextVisible.init(sclass, false);

                sEstText.set_raw("+99.9");
            }

            return STATUS_OK;
        }

        void LedMeter::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (sConstraints.is(prop))
                query_resize();
            if (sFont.is(prop) && (sTextVisible.get()))
                query_resize();
            if (sBorder.is(prop))
                query_resize();
            if (sAngle.is(prop))
                query_resize();
            if (sEstText.is(prop) && (sTextVisible.get()))
                query_resize();
            if (sTextVisible.is(prop))
                query_draw();
        }

        void LedMeter::get_visible_items(lltl::parray<LedMeterChannel> *dst)
        {
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                LedMeterChannel *c = vItems.get(i);
                if ((c == NULL) || (!c->visibility()->get()))
                    continue;
                if (!dst->add(c))
                    return;
            }
        }

        void LedMeter::size_request(ws::size_limit_t *r)
        {
            lltl::parray<LedMeterChannel> list;
            get_visible_items(&list);

            float scaling   = lsp_max(0.0f, sScaling.get());
            float seg_size  = 4.0f * scaling;
            ssize_t border  = (sBorder.get() > 0) ? lsp_max(1.0f, sBorder.get() * scaling) : 0;
            ssize_t angle   = sAngle.get();
            bool has_text   = sTextVisible.get();
            bool pack       = (sSGroups.get()) && (list.size() >= 2);

            ws::text_parameters_t tp;
            ws::font_parameters_t fp;

            if (has_text)
            {
                LSPString text;
                sEstText.format(&text);
                sFont.get_parameters(pDisplay, scaling, &fp);
                sFont.get_text_parameters(pDisplay, &tp, scaling, &text);
                tp.Height           = lsp_max(tp.Height, fp.Height);
            }

            if (angle & 1)
            {
                // Vertical
                r->nMinWidth        = ceilf(seg_size);
                r->nMinHeight       = 0;

                // Estimate the minimum height
                for (size_t i=0, n=list.size(); i<n; ++i)
                {
                    LedMeterChannel *c  = list.uget(i);
                    r->nMinHeight       = lsp_max(r->nMinHeight, ceilf(seg_size * lsp_min(0, c->min_segments()->get())));
                }

                // Estimate place for text
                if (has_text)
                {
                    r->nMinHeight      += border + tp.Height;
                    r->nMinWidth        = lsp_max(r->nMinWidth, tp.Width);
                    if (pack)
                    {
                        r->nMinHeight      += tp.Height;
                        r->nMinWidth        = lsp_min(r->nMinWidth, seg_size * 2);
                    }
                }
            }
            else
            {
                // Horizontal
                r->nMinWidth        = 0;
                r->nMinHeight       = ceilf(seg_size);

                // Estimate the minimum width
                for (size_t i=0, n=list.size(); i<n; ++i)
                {
                    LedMeterChannel *c  = list.uget(i);
                    r->nMinWidth        = lsp_max(r->nMinWidth, ceilf(seg_size * lsp_min(0, c->min_segments()->get())));
                }

                // Estimate place for text
                if (has_text)
                {
                    r->nMinWidth       += border + tp.Width;
                    r->nMinHeight       = lsp_max(r->nMinHeight, tp.Height);

                    if (pack)
                    {
                        r->nMinHeight       = lsp_max(r->nMinHeight, tp.Height * 2);
                        r->nMinHeight       = lsp_max(r->nMinHeight, seg_size  * 2);
                    }
                }
            }

            r->nMinWidth       += border * 2;
            r->nMinHeight      += border * 2;
            r->nMaxWidth        = -1;
            r->nMaxHeight       = -1;
            r->nPreWidth        = -1;
            r->nPreHeight       = -1;

            // Apply size constraints
            if (angle & 1)
                sConstraints.apply(r, scaling);  // Apply non-transposed size constraints
            else
                sConstraints.tapply(r, scaling); // Apply transposed size constraints
        }

        void LedMeter::realize(const ws::rectangle_t *r)
        {
            // TODO
        }

        void LedMeter::draw(ws::ISurface *s)
        {
            // TODO
        }

        void LedMeter::on_add_item(void *obj, Property *prop, Widget *w)
        {
            LedMeterChannel *item = widget_cast<LedMeterChannel>(w);
            if (item == NULL)
                return;

            LedMeter *_this = widget_ptrcast<LedMeter>(obj);
            if (_this == NULL)
                return;

            item->set_parent(_this);
            _this->query_resize();
        }

        void LedMeter::on_remove_item(void *obj, Property *prop, Widget *w)
        {
            LedMeterChannel *item = widget_cast<LedMeterChannel>(w);
            if (item == NULL)
                return;

            LedMeter *_this = widget_ptrcast<LedMeter>(obj);
            if (_this == NULL)
                return;

            // Remove widget from supplementary structures
            _this->unlink_widget(item);
            _this->query_resize();
        }

        Widget *LedMeter::find_widget(ssize_t x, ssize_t y)
        {
            // Update coordinates
            x -= sSize.nLeft;
            y -= sSize.nTop;
            bool has_text = sTextVisible.get();

            // Find widget
            for (size_t i=0, n=vVisible.size(); i<n; ++i)
            {
                LedMeterChannel *c = vVisible.uget(i);
                if (c == NULL)
                    continue;

                if (Position::inside(&c->sAMeter, x, y))
                    return c;
                if ((has_text) && (Position::inside(&c->sAText, x, y)))
                    return c;
            }

            return NULL;
        }

        void LedMeter::query_draw(size_t flags)
        {
            Widget::query_draw(flags | REDRAW_SURFACE);
        }

        status_t LedMeter::add(Widget *widget)
        {
            LedMeterChannel *item   = widget_cast<LedMeterChannel>(widget);
            return (item != NULL) ? vItems.add(item) : STATUS_BAD_TYPE;
        }

        status_t LedMeter::remove(Widget *child)
        {
            LedMeterChannel *item   = widget_cast<LedMeterChannel>(child);
            return (item != NULL) ? vItems.premove(item) : STATUS_BAD_TYPE;
        }

        status_t LedMeter::remove_all()
        {
            vItems.clear();
            return STATUS_OK;
        }

    }
}


