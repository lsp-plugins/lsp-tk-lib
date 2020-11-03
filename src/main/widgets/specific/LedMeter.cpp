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
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        STYLE_INITIALIZER_BEGIN(LedMeter, WidgetContainer);

            prop::SizeConstraints::init("constraints", style, 20, -1, 20, -1);
            prop::Font::init("font", style, 9);
            prop::Integer::init("border", style, 2);
            prop::Integer::init("angle", style, 0);
            prop::String::init("text.est", style);
            prop::Boolean::init("stereo_groups", style, true);
            prop::Boolean::init("text.visible", style, false);
            prop::Color::init("color", style, "#000000");
            prop::Integer::init("channel.width.min", style, 16);

        STYLE_INITIALIZER_END(LedMeter, "LedMeter");
        LSP_BUILTIN_STYLE(LedMeter);

        const w_class_t LedMeter::metadata              = { "LedMeter", &WidgetContainer::metadata };

        LedMeter::LedMeter(Display *dpy):
            WidgetContainer(dpy),
            vItems(&sProperties, &sIListener),
            sConstraints(&sProperties),
            sFont(&sProperties),
            sBorder(&sProperties),
            sAngle(&sProperties),
            sEstText(&sProperties),
            sSGroups(&sProperties),
            sTextVisible(&sProperties),
            sColor(&sProperties),
            sMinChannelWidth(&sProperties)
        {
            sAAll.nLeft     = 0;
            sAAll.nTop      = 0;
            sAAll.nWidth    = 0;
            sAAll.nHeight   = 0;

            pClass          = &metadata;
        }

        LedMeter::~LedMeter()
        {
            nFlags     |= FINALIZED;
            do_destroy();
        }

        void LedMeter::destroy()
        {
            nFlags     |= FINALIZED;
            WidgetContainer::destroy();
            do_destroy();
        }

        void LedMeter::do_destroy()
        {
            // Unlink all items
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                LedMeterChannel *item   = vItems.get(i);
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
            status_t res = WidgetContainer::init();
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
            sColor.bind("color", &sStyle);
            sMinChannelWidth.bind("channel.width.min", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sConstraints.init(sclass, 20, -1, 20, -1);
                sFont.init(sclass, 9);
                sBorder.init(sclass, 2);
                sAngle.init(sclass, 0);
                sSGroups.init(sclass, true);
                sTextVisible.init(sclass, false);
                sColor.init(sclass, "#000000");
                sMinChannelWidth.init(sclass, 16);
            }

            sEstText.set_raw("+99.9");

            return STATUS_OK;
        }

        void LedMeter::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);

            if (vItems.is(prop))
                query_draw();
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
                query_resize();
            if (sMinChannelWidth.is(prop))
                query_resize();
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

            bool pack       = (sSGroups.get()) && (list.size() >= 2);
            float scaling   = lsp_max(0.0f, sScaling.get());
            float seg_size  = 4.0f * scaling;
            ssize_t border  = (sBorder.get() > 0) ? lsp_max(1.0f, sBorder.get() * scaling) : 0;
            ssize_t angle   = sAngle.get();
            ssize_t minw    = lsp_max(ceilf(seg_size)*2, sMinChannelWidth.get() * scaling);
            bool has_text   = sTextVisible.get();

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
                r->nMinWidth        = minw;
                r->nMinHeight       = 0;

                // Estimate the minimum height
                for (size_t i=0, n=list.size(); i<n; ++i)
                {
                    LedMeterChannel *c  = list.uget(i);
                    r->nMinHeight       = lsp_max(r->nMinHeight, ceilf(seg_size * lsp_max(0, c->min_segments()->get())));
                }

                // Estimate place for text
                if (has_text)
                {
                    r->nMinHeight      += border + tp.Height;
                    r->nMinWidth        = lsp_max(r->nMinWidth, tp.Width);
                    if ((pack) && (list.size() >= 2))
                    {
                        r->nMinHeight      += tp.Height;
                        r->nMinWidth        = lsp_max(r->nMinWidth, seg_size * 2);
                    }
                }

                // Multiply the width by number of channels
                if (pack)
                    r->nMinWidth       *= ((list.size() + 1) >> 1);
                else
                    r->nMinWidth       *= list.size();
            }
            else
            {
                // Horizontal
                r->nMinWidth        = 0;
                r->nMinHeight       = minw;

                // Estimate the minimum width
                for (size_t i=0, n=list.size(); i<n; ++i)
                {
                    LedMeterChannel *c  = list.uget(i);
                    r->nMinWidth        = lsp_max(r->nMinWidth, ceilf(seg_size * lsp_max(0, c->min_segments()->get())));
                }

                // Estimate place for text
                if (has_text)
                {
                    r->nMinWidth       += border + tp.Width;
                    r->nMinHeight       = lsp_max(r->nMinHeight, tp.Height);

                    if ((pack) && (list.size() >= 2))
                    {
                        r->nMinHeight       = lsp_max(r->nMinHeight, tp.Height * 2);
                        r->nMinHeight       = lsp_max(r->nMinHeight, seg_size  * 2);
                    }
                }

                // Multiply the height by number of channels
                if (pack)
                    r->nMinHeight      *= ((list.size() + 1) >> 1);
                else
                    r->nMinHeight      *= list.size();
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
            // Realize the parent class
            WidgetContainer::realize(r);

            // Get list of visible items
            lltl::parray<LedMeterChannel> list;
            get_visible_items(&list);

            float scaling       = lsp_max(0.0f, sScaling.get());
            float seg_size      = 4.0f * scaling;
            ssize_t border      = (sBorder.get() > 0) ? lsp_max(1.0f, sBorder.get() * scaling) : 0;
            ssize_t angle       = sAngle.get();
            bool has_text       = sTextVisible.get();
            bool pack           = (sSGroups.get()) && (list.size() >= 2);

            ws::text_parameters_t tp;
            ws::font_parameters_t fp;
            ws::rectangle_t xr, xtext;

            sAAll.nLeft         = 0;
            sAAll.nTop          = 0;
            sAAll.nWidth        = r->nWidth;
            sAAll.nHeight       = r->nHeight;

            xr.nLeft            = border;
            xr.nTop             = border;
            xr.nWidth           = r->nWidth  - border*2;
            xr.nHeight          = r->nHeight - border*2;

            xtext.nLeft         = 0;
            xtext.nTop          = 0;
            xtext.nWidth        = 0;
            xtext.nHeight       = 0;

            // Compute the amount of space used for text
            ssize_t led_size    = (angle & 1) ? xr.nHeight : xr.nWidth;

            if (has_text)
            {
                LSPString text;
                sEstText.format(&text);
                sFont.get_parameters(pDisplay, scaling, &fp);
                sFont.get_text_parameters(pDisplay, &tp, scaling, &text);
                tp.Height           = lsp_max(tp.Height, fp.Height);

                if (angle & 1) // Vertical
                {
                    xtext.nHeight       = tp.Height;
                    if ((pack) && (list.size() >= 2))
                        xtext.nHeight      += tp.Height;

                    led_size           -= (border + xtext.nHeight);
                }
                else // Horizontal
                {
                    xtext.nWidth        = tp.Width;
                    led_size           -= (border + xtext.nWidth);
                }
            }

            // Compute overall areas
            ssize_t segments    = led_size / seg_size;
            ssize_t vgap        = led_size - ceilf(segments * seg_size);
            led_size            = led_size - vgap;
            ssize_t hitems      = (pack && has_text) ? ((list.size() + 1) >> 1) : list.size();
            size_t hlimit       = (pack && has_text) ? list.size() & (~1) : hitems;
            ssize_t payload     = (pack && has_text) ? (hitems << 1) : hitems;
            ssize_t hsegsize    = ((angle & 1) ? xr.nWidth : xr.nHeight) / payload;
            ssize_t hgap        = ((angle & 1) ? xr.nWidth : xr.nHeight) - (hsegsize * payload);

            // Allocate meters
            switch (size_t(angle & 3))
            {
                case 1: // Bottom to top
                {
                    sAAll.nLeft    += (hgap >> 1);
                    sAAll.nTop     += (vgap >> 1);
                    sAAll.nWidth   -= hgap;
                    sAAll.nHeight  -= vgap;

                    xr.nLeft        = sAAll.nLeft + border;
                    xr.nTop         = sAAll.nTop  + border;
                    xr.nWidth       = hsegsize;
                    xr.nHeight      = sAAll.nTop + sAAll.nHeight - xr.nTop - border - ((has_text) ? xtext.nHeight + border : 0);

                    xtext.nTop      = xr.nTop + xr.nHeight + border;
                    xtext.nLeft     = xr.nLeft;
                    xtext.nWidth    = (pack) ? hsegsize * 2 : hsegsize;

                    break;
                }

                case 3: // Top to bottom
                {
                    sAAll.nLeft    += (hgap >> 1);
                    sAAll.nTop     += (vgap >> 1);
                    sAAll.nWidth   -= hgap;
                    sAAll.nHeight  -= vgap;

                    xtext.nLeft     = sAAll.nLeft + border;
                    xtext.nTop      = sAAll.nTop  + border;
                    xtext.nWidth    = (pack) ? hsegsize * 2 : hsegsize;

                    xr.nLeft        = xtext.nLeft;
                    xr.nTop         = xtext.nTop  + ((has_text) ? xtext.nHeight + border : 0);
                    xr.nWidth       = hsegsize;
                    xr.nHeight      = sAAll.nTop + sAAll.nHeight - xr.nTop - border;

                    break;
                }

                case 2: // Right to left
                {
                    sAAll.nLeft    += (vgap >> 1);
                    sAAll.nTop     += (hgap >> 1);
                    sAAll.nWidth   -= vgap;
                    sAAll.nHeight  -= hgap;

                    xr.nLeft        = sAAll.nLeft + border;
                    xr.nTop         = sAAll.nTop  + border;
                    xr.nWidth       = sAAll.nLeft + sAAll.nWidth - xr.nLeft - border - ((has_text) ? xtext.nWidth + border : 0);
                    xr.nHeight      = hsegsize;

                    xtext.nLeft     = xr.nLeft + xr.nWidth + border;
                    xtext.nTop      = xr.nTop;
                    xtext.nHeight   = hsegsize;

                    break;
                }

                case 0: // Left to right
                default:
                {
                    sAAll.nLeft    += (vgap >> 1);
                    sAAll.nTop     += (hgap >> 1);
                    sAAll.nWidth   -= vgap;
                    sAAll.nHeight  -= hgap;

                    xtext.nLeft     = sAAll.nLeft + border;
                    xtext.nTop      = sAAll.nTop  + border;
                    xtext.nHeight   = hsegsize;

                    xr.nLeft        = xtext.nLeft + ((has_text) ? xtext.nWidth + border : 0);
                    xr.nTop         = xtext.nTop;
                    xr.nWidth       = sAAll.nLeft + sAAll.nWidth - xr.nLeft - border;
                    xr.nHeight      = hsegsize;

                    break;
                }
            }

            // Realize each meter channel
            if (angle & 1)
            {
                if (pack)
                {
                    for (size_t i=0, n=list.size(); i<n; ++i)
                    {
                        LedMeterChannel *c = list.uget(i);

                        if (i >= hlimit)
                            xr.nWidth       = xtext.nWidth;
                        xtext.nHeight   = tp.Height;

                        // Update position of meter and text
                        c->sAMeter      = xr;
                        c->sAText       = xtext;

                        xr.nLeft       += hsegsize;
                        if (i & 1)
                        {
                            xtext.nLeft    += hsegsize << 1;
                            xtext.nTop     -= tp.Height;
                        }
                        else
                            xtext.nTop     += tp.Height;
                    }
                }
                else
                {
                    for (size_t i=0, n=list.size(); i<n; ++i)
                    {
                        LedMeterChannel *c = list.uget(i);

                        // Update position of meter and text
                        xtext.nHeight   = tp.Height;

                        c->sAMeter      = xr;
                        c->sAText       = xtext;

                        xr.nLeft       += hsegsize;
                        xtext.nLeft    += hsegsize;
                    }
                }
            }
            else
            {
                for (size_t i=0, n=list.size(); i<n; ++i)
                {
                    LedMeterChannel *c = list.uget(i);
                    if (i >= hlimit)
                    {
                        xtext.nHeight   = hsegsize << 1;
                        xr.nHeight      = xtext.nHeight;
                    }

                    // Update position of meter and text
                    c->sAMeter      = xr;
                    c->sAText       = xtext;

                    xr.nTop        += hsegsize;
                    xtext.nTop     += hsegsize;
                }
            }

            // Update visible items
            vVisible.swap(&list);
        }

        void LedMeter::draw(ws::ISurface *s)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());
            float bright        = sBrightness.get();
            bool has_text       = sTextVisible.get();
            ssize_t angle       = sAngle.get();

            lsp::Color col(sBgColor);
            s->clear(col);
            col.copy(sColor);
            s->fill_rect(col, &sAAll);

            for (size_t i=0, n=vVisible.size(); i<n; ++i)
            {
                LedMeterChannel *c = vVisible.uget(i);

                c->draw_meter(s, angle, scaling, bright);
                if (has_text)
                    c->draw_label(s, &sFont, scaling, bright);

                // Commit pending redraw request
                c->commit_redraw();
            }
        }

        void LedMeter::on_add_item(void *obj, Property *prop, void *w)
        {
            LedMeterChannel *item = widget_ptrcast<LedMeterChannel>(w);
            if (item == NULL)
                return;

            LedMeter *_this = widget_ptrcast<LedMeter>(obj);
            if (_this == NULL)
                return;

            item->set_parent(_this);
            _this->query_resize();
        }

        void LedMeter::on_remove_item(void *obj, Property *prop, void *w)
        {
            LedMeterChannel *item = widget_ptrcast<LedMeterChannel>(w);
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
                if ((c == NULL) || (!c->is_visible_child_of(this)))
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
            WidgetContainer::query_draw(flags | REDRAW_SURFACE);
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


