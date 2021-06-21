/*
 * Copyright (C) 2021 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2021 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 16 июн. 2021 г.
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
            LSP_TK_STYLE_IMPL_BEGIN(MultiLabel, WidgetContainer)
                // Bind
                sConstraints.bind("size.constraints", this);
                sBearing.bind("bearing", this);
                sHover.bind("hover", this);
                // Configure
                sConstraints.set(-1, -1, -1, -1);
                sBearing.set(true);
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(MultiLabel, "MultiLabel");
        }

        const w_class_t MultiLabel::metadata        = { "MultiLabel", &WidgetContainer::metadata };

        MultiLabel::MultiLabel(Display *dpy):
            WidgetContainer(dpy),
            sConstraints(&sProperties),
            sBearing(&sProperties),
            sHover(&sProperties),
            vItems(&sProperties, &sIListener)
        {
            nMFlags     = 0;
            nState      = 0;

            pClass      = &metadata;
        }

        MultiLabel::~MultiLabel()
        {
            nFlags     |= FINALIZED;
            do_destroy();
        }

        void MultiLabel::destroy()
        {
            do_destroy();
            WidgetContainer::destroy();
        }

        void MultiLabel::do_destroy()
        {
            // Unlink all items
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                Label *item   = vItems.get(i);
                if (item == NULL)
                    continue;

                unlink_widget(item);
            }

            vItems.flush();
        }

        status_t MultiLabel::init()
        {
            status_t res = WidgetContainer::init();
            if (res != STATUS_OK)
                return res;

            // Init listener
            sIListener.bind_all(this, on_add_item, on_remove_item);

            // Init style
            sConstraints.bind("size.constraints", &sStyle);
            sBearing.bind("bearing", &sStyle);
            sHover.bind("hover", &sStyle);
            sPopup.bind(NULL);

            return STATUS_OK;
        }

        void MultiLabel::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);

            if (sConstraints.is(prop))
                query_resize();
            if (sBearing.is(prop))
                query_resize();
            if (sHover.is(prop))
                query_draw();
            if (vItems.is(prop))
                query_resize();
        }

        void MultiLabel::size_request(ws::size_limit_t *r)
        {
            // Estimate sizes
            LSPString text;
            float scaling   = lsp_max(0.0f, sScaling.get());
            float fscaling  = lsp_max(0.0f, scaling * sFontScaling.get());
            bool bearing    = sBearing.get();

            WidgetContainer::size_request(r);
            ws::font_parameters_t fp;
            ws::text_parameters_t tp;

            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                Label *l    = vItems.get(i);
                if ((l == NULL) || (!l->visibility()->get()))
                    continue;

                // Estimate the label size
                l->sText.format(&text);
                l->sTextAdjust.apply(&text);
                l->sFont.get_parameters(pDisplay, fscaling, &fp);
                l->sFont.get_multitext_parameters(pDisplay, &tp, fscaling, &text);

                if (bearing)
                {
                    r->nMinWidth    = lsp_max(r->nMinWidth,  tp.Width);
                    r->nMinHeight   = lsp_max(r->nMinHeight, lsp_max(tp.Height, fp.Height));
                }
                else
                {
                    r->nMinWidth    = lsp_max(r->nMinWidth,  tp.XAdvance);
                    r->nMinHeight   = lsp_max(r->nMinHeight, lsp_max(tp.Height, fp.Height));
                }
            }

            r->nMaxWidth    = (r->nMaxWidth >= 0) ? lsp_max(r->nMaxWidth, r->nMaxWidth) : -1;
            r->nMaxHeight   = (r->nMaxHeight >= 0) ? lsp_max(r->nMaxHeight, r->nMaxHeight) : -1;

            sConstraints.apply(r, scaling);
        }

        void MultiLabel::realize(const ws::rectangle_t *r)
        {
            WidgetContainer::realize(r);

            // Just realize all child widgets as of the same size
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                Label *l    = vItems.get(i);
                if ((l == NULL) || (!l->visibility()->get()))
                    continue;

                l->realize(r);
            }
        }

        void MultiLabel::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
            // Estimate sizes
            float scaling   = lsp_max(0.0f, sScaling.get());
            float fscaling  = lsp_max(0.0f, scaling * sFontScaling.get());
            bool hover      = (nState & F_MOUSE_IN) && (sHover.get());

            LSPString text;
            lsp::Color bg_color, color;
            ws::font_parameters_t fp;
            ws::text_parameters_t tp;
            ws::rectangle_t r;

            get_actual_bg_color(bg_color);

            s->clip_begin(area);
            {
                s->fill_rect(bg_color, &sSize);

                if (sBearing.get())
                {
                    r.nLeft     = 0;
                    r.nTop      = 0;
                    r.nWidth    = 0;
                    r.nHeight   = 0;

                    for (size_t i=0, n=vItems.size(); i<n; ++i)
                    {
                        Label *l    = vItems.get(i);
                        if ((l == NULL) || (!l->visibility()->get()))
                            continue;

                        // Estimate the label size
                        l->sText.format(&text);
                        l->sFont.get_parameters(pDisplay, fscaling, &fp);
                        l->sFont.get_multitext_parameters(pDisplay, &tp, fscaling, &text);

                        r.nWidth        = lsp_max(r.nWidth,  ssize_t(tp.Width));
                        r.nHeight       = lsp_max(r.nHeight, ssize_t(lsp_max(tp.Height, fp.Height)));
                    }

                    // Just realize all child widgets as of the same size
                    for (size_t i=0, n=vItems.size(); i<n; ++i)
                    {
                        Label *l    = vItems.get(i);
                        if ((l == NULL) || (!l->visibility()->get()))
                            continue;

                        // Draw the label
                        l->sText.format(&text);
                        l->sFont.get_parameters(s, fscaling, &fp);
                        l->sFont.get_multitext_parameters(s, &tp, fscaling, &text);

                        // Copy color preferencies
                        color.copy((hover) ? l->sHoverColor : l->sColor);
                        color.scale_lightness(sBrightness.get());

                        float halign    = lsp_limit(l->sTextLayout.halign() + 1.0f, 0.0f, 2.0f);
                        float valign    = lsp_limit(l->sTextLayout.valign() + 1.0f, 0.0f, 2.0f);
                        float dy        = (sSize.nHeight - r.nHeight) * 0.5f;
                        ssize_t y       = r.nTop + dy * valign - fp.Descent;

                        // Estimate text size
                        ssize_t last = 0, curr = 0, tail = 0, len = text.length();

                        while (curr < len)
                        {
                            // Get next line indexes
                            curr    = text.index_of(last, '\n');
                            if (curr < 0)
                            {
                                curr        = len;
                                tail        = len;
                            }
                            else
                            {
                                tail        = curr;
                                if ((tail > last) && (text.at(tail-1) == '\r'))
                                    --tail;
                            }

                            // Calculate text location
                            l->sFont.get_text_parameters(s, &tp, fscaling, &text, last, tail);
                            float dx    = (sSize.nWidth - r.nWidth) * 0.5f;
                            ssize_t x   = r.nLeft   + dx * halign;
                            y          += fp.Height;

                            l->sFont.draw(s, color, sSize.nLeft + x, sSize.nTop + y, fscaling, &text, last, tail);
                            last    = curr + 1;
                        }

                        // Commit the redraw flag
                        l->commit_redraw();
                    }
                }
                else
                {
                    // Just realize all child widgets as of the same size
                    for (size_t i=0, n=vItems.size(); i<n; ++i)
                    {
                        Label *l    = vItems.get(i);
                        if ((l == NULL) || (!l->visibility()->get()))
                            continue;

                        // Draw the label
                        l->sText.format(&text);
                        l->sFont.get_parameters(s, fscaling, &fp);
                        l->sFont.get_multitext_parameters(s, &tp, fscaling, &text);

                        // Estimate drawing area
                        tp.Height       = lsp_max(tp.Height, fp.Height);
                        if (tp.Width <= sSize.nWidth)
                        {
                            r.nLeft         = 0;
                            r.nWidth        = sSize.nWidth;
                        }
                        else
                        {
                            r.nLeft         = -0.5f * (tp.Width - sSize.nWidth);
                            r.nWidth        = ceil(tp.Width);
                        }

                        if (tp.Height <= sSize.nHeight)
                        {
                            r.nTop          = 0;
                            r.nHeight       = sSize.nHeight;
                        }
                        else
                        {
                            r.nTop          = -0.5f * (tp.Height - sSize.nHeight);
                            r.nHeight       = ceil(tp.Height);
                        }

                        // Copy color preferencies
                        color.copy(l->sColor);
                        color.scale_lightness(sBrightness.get());

                        float halign    = lsp_limit(l->sTextLayout.halign() + 1.0f, 0.0f, 2.0f);
                        float valign    = lsp_limit(l->sTextLayout.valign() + 1.0f, 0.0f, 2.0f);
                        float dy        = (r.nHeight - tp.Height) * 0.5f;
                        ssize_t y       = r.nTop + dy * valign - fp.Descent;

                        // Estimate text size
                        ssize_t last = 0, curr = 0, tail = 0, len = text.length();

                        while (curr < len)
                        {
                            // Get next line indexes
                            curr    = text.index_of(last, '\n');
                            if (curr < 0)
                            {
                                curr        = len;
                                tail        = len;
                            }
                            else
                            {
                                tail        = curr;
                                if ((tail > last) && (text.at(tail-1) == '\r'))
                                    --tail;
                            }

                            // Calculate text location
                            l->sFont.get_text_parameters(s, &tp, fscaling, &text, last, tail);
                            float dx    = (r.nWidth - tp.Width) * 0.5f;
                            ssize_t x   = r.nLeft   + dx * halign - tp.XBearing;
                            y          += fp.Height;

                            l->sFont.draw(s, color, sSize.nLeft + x, sSize.nTop + y, fscaling, &text, last, tail);
                            last    = curr + 1;
                        }

                        // Commit the redraw flag
                        l->commit_redraw();
                    }
                }
            }
            s->clip_end();
        }


        status_t MultiLabel::on_mouse_in(const ws::event_t *e)
        {
            WidgetContainer::on_mouse_in(e);

            size_t flags = nState;
            nState |= F_MOUSE_IN;
            if (flags != nState)
                query_draw();

            return STATUS_OK;
        }


        status_t MultiLabel::on_mouse_out(const ws::event_t *e)
        {
            Widget::on_mouse_out(e);

            size_t flags = nState;
            nState &= ~F_MOUSE_IN;
            if (flags != nState)
                query_draw();

            return STATUS_OK;
        }

        status_t MultiLabel::on_mouse_move(const ws::event_t *e)
        {
            size_t flags = nState;
            nState = lsp_setflag(nState, F_MOUSE_IN, inside(e->nLeft, e->nTop));
            if (flags != nState)
                query_draw();
            return STATUS_OK;
        }

        status_t MultiLabel::on_mouse_down(const ws::event_t *e)
        {
            size_t flags = nState;

            if (nMFlags == 0)
            {
                if (e->nCode == ws::MCB_LEFT)
                    nState |= F_MOUSE_DOWN;
                else
                    nState |= F_MOUSE_IGN;
            }

            nMFlags |= 1 << e->nCode;
            nState = lsp_setflag(nState, F_MOUSE_IN, inside(e->nLeft, e->nTop));

            if (flags != nState)
                query_draw();
            return STATUS_OK;
        }

        status_t MultiLabel::on_mouse_up(const ws::event_t *e)
        {
            size_t flags = nMFlags;
            nMFlags &= ~ (1 << e->nCode);
            if (nMFlags == 0)
                nState      = 0;

            bool xinside = inside(e->nLeft, e->nTop);
            nState = lsp_setflag(nState, F_MOUSE_IN, xinside);
            if (flags != nState)
                query_draw();

            // Trigger submit action
            if (xinside)
            {
                if ((flags == (1 << ws::MCB_LEFT)) && (e->nCode == ws::MCB_LEFT))
                    sSlots.execute(SLOT_SUBMIT, this);
                else if ((flags == (1 << ws::MCB_RIGHT)) && (e->nCode == ws::MCB_RIGHT) && (sPopup.is_set()))
                {
                    Menu *popup = sPopup.get();
                    sSlots.execute(SLOT_BEFORE_POPUP, popup, self());
                    popup->show();
                    sSlots.execute(SLOT_POPUP, popup, self());
                }
            }

            return STATUS_OK;
        }

        status_t MultiLabel::add(Widget *widget)
        {
            Label *item     = widget_cast<Label>(widget);
            return (item != NULL) ? vItems.add(item) : STATUS_BAD_TYPE;
        }

        status_t MultiLabel::remove(Widget *child)
        {
            Label *item     = widget_cast<Label>(child);
            return (item != NULL) ? vItems.premove(item) : STATUS_BAD_TYPE;
        }

        void MultiLabel::on_add_item(void *obj, Property *prop, void *w)
        {
            Label *item = widget_ptrcast<Label>(w);
            if (item == NULL)
                return;

            MultiLabel *_this = widget_ptrcast<MultiLabel>(obj);
            if (_this == NULL)
                return;

            item->set_parent(_this);
            _this->query_resize();
        }

        void MultiLabel::on_remove_item(void *obj, Property *prop, void *w)
        {
            Label *item = widget_ptrcast<Label>(w);
            if (item == NULL)
                return;

            MultiLabel *_this = widget_ptrcast<MultiLabel>(obj);
            if (_this == NULL)
                return;

            // Remove widget from supplementary structures
            _this->unlink_widget(item);
            _this->query_resize();
        }
    }
}


