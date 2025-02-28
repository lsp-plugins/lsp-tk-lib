/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(LedMeter, WidgetContainer)
                // Bind
                sConstraints.bind("constraints", this);
                sFont.bind("font", this);
                sBorder.bind("border", this);
                sAngle.bind("angle", this);
                sSGroups.bind("stereo_groups", this);
                sTextVisible.bind("text.visible", this);
                sHeaderVisible.bind("header.visible", this);
                sColor.bind("color", this);
                sMinChannelWidth.bind("channel.width.min", this);
                // Configure
                sConstraints.set(20, -1, 20, -1);
                sFont.set_size(9.0f);
                sBorder.set(2);
                sAngle.set(0);
                sSGroups.set(true);
                sTextVisible.set(false);
                sHeaderVisible.set(false);
                sColor.set("#000000");
                sMinChannelWidth.set(16);
                // Override
                sFont.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(LedMeter, "LedMeter", "root");
        }

        const w_class_t LedMeter::metadata              = { "LedMeter", &WidgetContainer::metadata };

        LedMeter::LedMeter(Display *dpy):
            WidgetContainer(dpy),
            vItems(&sProperties, &sIListener),
            sConstraints(&sProperties),
            sFont(&sProperties),
            sBorder(&sProperties),
            sAngle(&sProperties),
            sEstText(&sProperties),
            sEstHeader(&sProperties),
            sSGroups(&sProperties),
            sTextVisible(&sProperties),
            sHeaderVisible(&sProperties),
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
            sEstHeader.bind(&sStyle, pDisplay->dictionary());
            sSGroups.bind("stereo_groups", &sStyle);
            sTextVisible.bind("text.visible", &sStyle);
            sHeaderVisible.bind("header.visible", &sStyle);
            sColor.bind("color", &sStyle);
            sMinChannelWidth.bind("channel.width.min", &sStyle);

            sEstText.set_raw("+99.9");
            sEstHeader.set_raw("+99.9");

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
            if (sEstHeader.is(prop) && (sHeaderVisible.get()))
                query_resize();
            if (sTextVisible.is(prop))
                query_resize();
            if (sHeaderVisible.is(prop))
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
            float fscaling  = lsp_max(0.0f, scaling * sFontScaling.get());
            float seg_size  = 4.0f * scaling;
            ssize_t border  = (sBorder.get() > 0) ? lsp_max(1.0f, sBorder.get() * scaling) : 0;
            ssize_t angle   = sAngle.get();
            ssize_t minw    = lsp_max(ceilf(seg_size)*2, sMinChannelWidth.get() * scaling);
            bool has_text   = sTextVisible.get();
            bool has_header = sHeaderVisible.get();

            ws::text_parameters_t tp_text, tp_header;
            ws::font_parameters_t fp;

            LSPString text;

            if (has_text)
            {
                sEstText.format(&text);
                sFont.get_parameters(pDisplay, fscaling, &fp);
                sFont.get_text_parameters(pDisplay, &tp_text, fscaling, &text);
                tp_text.Height      = lsp_max(tp_text.Height, fp.Height);
            }

            if (has_header)
            {
                sEstHeader.format(&text);
                sFont.get_parameters(pDisplay, fscaling, &fp);
                sFont.get_text_parameters(pDisplay, &tp_header, fscaling, &text);
                tp_header.Height    = lsp_max(tp_header.Height, fp.Height);
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
                    r->nMinHeight      += border + tp_text.Height;
                    r->nMinWidth        = lsp_max(r->nMinWidth, tp_text.Width);
                    if ((pack) && (list.size() >= 2))
                    {
                        r->nMinHeight      += tp_text.Height;
                        r->nMinWidth        = lsp_max(r->nMinWidth, seg_size * 2);
                    }
                }

                // Estimate place for header
                if (has_header)
                {
                    r->nMinHeight      += border + tp_header.Height;
                    r->nMinWidth        = lsp_max(r->nMinWidth, tp_header.Width);
                    if ((pack) && (list.size() >= 2))
                    {
                        r->nMinHeight      += tp_header.Height;
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
                    r->nMinWidth       += border + tp_text.Width;
                    r->nMinHeight       = lsp_max(r->nMinHeight, tp_text.Height);

                    if ((pack) && (list.size() >= 2))
                    {
                        r->nMinHeight       = lsp_max(r->nMinHeight, tp_text.Height * 2);
                        r->nMinHeight       = lsp_max(r->nMinHeight, seg_size  * 2);
                    }
                }

                // Estimate place for header
                if (has_header)
                {
                    r->nMinWidth       += border + tp_header.Width;
                    r->nMinHeight       = lsp_max(r->nMinHeight, tp_header.Height);

                    if ((pack) && (list.size() >= 2))
                    {
                        r->nMinHeight       = lsp_max(r->nMinHeight, tp_header.Height * 2);
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
            float fscaling      = lsp_max(0.0f, scaling * sFontScaling.get());
            float seg_size      = 4.0f * scaling;
            ssize_t border      = (sBorder.get() > 0) ? lsp_max(1.0f, sBorder.get() * scaling) : 0;
            ssize_t angle       = sAngle.get();
            bool has_text       = sTextVisible.get();
            bool has_header     = sHeaderVisible.get();
            bool pack           = (sSGroups.get()) && (list.size() >= 2);

            ws::text_parameters_t tp_text, tp_header;
            ws::font_parameters_t fp;
            ws::rectangle_t xr, xtext, xheader;

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

            xheader.nLeft       = 0;
            xheader.nTop        = 0;
            xheader.nWidth      = 0;
            xheader.nHeight     = 0;

            // Compute the amount of space used for text
            ssize_t led_size    = (angle & 1) ? xr.nHeight : xr.nWidth;

            LSPString text;
            if (has_text)
            {
                sEstText.format(&text);
                sFont.get_parameters(pDisplay, fscaling, &fp);
                sFont.get_text_parameters(pDisplay, &tp_text, fscaling, &text);
                tp_text.Height      = lsp_max(tp_text.Height, fp.Height);

                if (angle & 1) // Vertical
                {
                    xtext.nHeight       = tp_text.Height;
                    if ((pack) && (list.size() >= 2))
                        xtext.nHeight      += tp_text.Height;

                    led_size           -= (border + xtext.nHeight);
                }
                else // Horizontal
                {
                    xtext.nWidth        = tp_text.Width;
                    led_size           -= (border + xtext.nWidth);
                }
            }
            else
                tp_text.Height      = 0;

            if (has_header)
            {
                sEstHeader.format(&text);
                sFont.get_parameters(pDisplay, fscaling, &fp);
                sFont.get_text_parameters(pDisplay, &tp_header, fscaling, &text);
                tp_header.Height    = lsp_max(tp_header.Height, fp.Height);

                if (angle & 1) // Vertical
                {
                    xheader.nHeight     = tp_header.Height;
                    if ((pack) && (list.size() >= 2))
                        xheader.nHeight    += tp_header.Height;

                    led_size           -= (border + xheader.nHeight);
                }
                else // Horizontal
                {
                    xheader.nWidth      = tp_header.Width;
                    led_size           -= (border + xheader.nWidth);
                }
            }
            else
                tp_header.Height    = 0;

            // Compute overall areas
            ssize_t segments    = led_size / seg_size;
            ssize_t vgap        = led_size - ceilf(segments * seg_size);
            led_size            = led_size - vgap;
            ssize_t hitems      = (pack && has_text) ? ((list.size() + 1) >> 1) : list.size();
            size_t hlimit       = (pack && has_text) ? list.size() & (~1) : hitems;
            ssize_t payload     = (pack && has_text) ? (hitems << 1) : hitems;
            ssize_t hsegsize    = ((angle & 1) ? xr.nWidth : xr.nHeight) / lsp_max(1, payload);
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

                    xheader.nLeft   = sAAll.nLeft + border;
                    xheader.nTop    = sAAll.nTop  + border;
                    xheader.nWidth  = (pack) ? hsegsize * 2 : hsegsize;

                    xr.nLeft        = xheader.nLeft;
                    xr.nTop         = xheader.nTop;
                    xr.nWidth       = hsegsize;
                    xr.nHeight      = led_size;
                    if (has_header)
                        xr.nTop        += xheader.nHeight + border;

                    xtext.nTop      = xr.nTop + xr.nHeight + border;
                    xtext.nLeft     = xr.nLeft;
                    xtext.nWidth    = xheader.nWidth;

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
                    xr.nTop         = xtext.nTop;
                    xr.nWidth       = hsegsize;
                    xr.nHeight      = led_size;
                    if (has_text)
                        xr.nTop        += xtext.nHeight + border;

                    xheader.nLeft   = xr.nLeft;
                    xheader.nTop    = xr.nTop + xr.nHeight + border;
                    xheader.nWidth  = xtext.nWidth;

                    break;
                }

                case 2: // Right to left
                {
                    sAAll.nLeft    += (vgap >> 1);
                    sAAll.nTop     += (hgap >> 1);
                    sAAll.nWidth   -= vgap;
                    sAAll.nHeight  -= hgap;

                    xheader.nLeft   = sAAll.nLeft + border;
                    xheader.nTop    = sAAll.nTop  + border;
                    xheader.nHeight = hsegsize;

                    xr.nLeft        = xheader.nLeft;
                    xr.nTop         = xheader.nTop;
                    xr.nWidth       = led_size;
                    xr.nHeight      = hsegsize;
                    if (has_header)
                        xr.nLeft       += xheader.nWidth + border;

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

                    xr.nLeft        = xtext.nLeft;
                    xr.nTop         = xtext.nTop;
                    xr.nWidth       = led_size;
                    xr.nHeight      = hsegsize;
                    if (has_text)
                        xr.nLeft       += xtext.nWidth + border;

                    xheader.nLeft   = xr.nLeft + xr.nWidth + border;
                    xheader.nTop    = xr.nTop;
                    xheader.nHeight = hsegsize;

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
                        {
                            if (has_text)
                                xr.nWidth       = lsp_max(xr.nWidth, xtext.nWidth);
                            if (has_header)
                                xr.nWidth       = lsp_max(xr.nWidth, xheader.nWidth);
                        }
                        xtext.nHeight   = tp_text.Height;
                        xheader.nHeight = tp_header.Height;

                        // Update position of meter and text
                        c->sAMeter      = xr;
                        c->sAText       = xtext;
                        c->sAHeader     = xheader;
                        c->sSize        = sSize;

                        xr.nLeft       += hsegsize;
                        if (i & 1)
                        {
                            xtext.nLeft    += hsegsize << 1;
                            xtext.nTop     -= tp_text.Height;

                            xheader.nLeft  += hsegsize << 1;
                            xheader.nTop   -= tp_header.Height;
                        }
                        else
                        {
                            xtext.nTop     += tp_text.Height;
                            xheader.nTop   += tp_header.Height;
                        }
                    }
                }
                else
                {
                    for (size_t i=0, n=list.size(); i<n; ++i)
                    {
                        LedMeterChannel *c = list.uget(i);

                        // Update position of meter and text
                        xtext.nHeight   = tp_text.Height;
                        xheader.nHeight = tp_header.Height;

                        c->sAMeter      = xr;
                        c->sAText       = xtext;
                        c->sAHeader     = xheader;
                        c->sSize        = sSize;

                        xr.nLeft       += hsegsize;
                        xtext.nLeft    += hsegsize;
                        xheader.nLeft  += hsegsize;
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
                        xheader.nHeight = hsegsize << 1;
                        if (has_text)
                            xr.nHeight      = lsp_max(xr.nHeight, xtext.nHeight);
                        if (has_header)
                            xr.nHeight      = lsp_max(xr.nHeight, xheader.nHeight);
                    }

                    // Update position of meter and text
                    c->sAMeter      = xr;
                    c->sAText       = xtext;
                    c->sAHeader     = xheader;
                    c->sSize        = sSize;

                    xr.nTop        += hsegsize;
                    xtext.nTop     += hsegsize;
                    xheader.nTop   += hsegsize;
                }
            }

            // Update visible items
            vVisible.swap(&list);
        }

        void LedMeter::draw(ws::ISurface *s)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());
            float fscaling      = lsp_max(0.0f, scaling * sFontScaling.get());
            float bright        = sBrightness.get();
            bool has_text       = sTextVisible.get();
            bool has_header     = sHeaderVisible.get();
            ssize_t angle       = sAngle.get();

            lsp::Color col;
            get_actual_bg_color(col);
            s->clear(col);
            col.copy(sColor);
            col.scale_lch_luminance(bright);
            s->fill_rect(col, SURFMASK_NONE, 0.0f, &sAAll);

            // Pass 1: Draw meter body
            for (size_t i=0, n=vVisible.size(); i<n; ++i)
            {
                LedMeterChannel *c = vVisible.uget(i);
                const float mbright     = lsp_min(bright, c->brightness()->get());
                c->draw_meter(s, angle, scaling, mbright);
            }

            // Pass 2: Draw related text and commit pending for redraw flag
            for (size_t i=0, n=vVisible.size(); i<n; ++i)
            {
                LedMeterChannel *c = vVisible.uget(i);
                float mbright   = lsp_min(bright, c->brightness()->get());
                if (has_text)
                    c->draw_label(s, &sFont, fscaling, mbright);
                if (has_header)
                    c->draw_header(s, &sFont, fscaling, mbright);

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
            bool has_text   = sTextVisible.get();
            bool has_header = sHeaderVisible.get();

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
                if ((has_header) && (Position::inside(&c->sAHeader, x, y)))
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

    } /* namespace tk */
} /* namespace lsp */


