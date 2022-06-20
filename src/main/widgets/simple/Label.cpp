/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 6 июл. 2017 г.
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
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace tk
    {

        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(Label, Widget)
                // Bind
                sTextLayout.bind("text.layout", this);
                sTextAdjust.bind("text.adjust", this);
                sFont.bind("font", this);
                sColor.bind("text.color", this);
                sHoverColor.bind("text.hover.color", this);
                sHover.bind("text.hover", this);
                sConstraints.bind("size.constraints", this);
                sIPadding.bind("ipadding", this);
                // Configure
                sTextLayout.set(0.0f, 0.0f);
                sTextAdjust.set(TA_NONE);
                sFont.set_size(12.0f);
                sColor.set("#000000");
                sHoverColor.set("#ff0000");
                sHover.set(false);
                sConstraints.set(-1, -1, -1, -1);
                sIPadding.set(0, 0, 0, 0);
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(Label, "Label", "root");
        }

        const w_class_t Label::metadata =       { "Label", &Widget::metadata };

        Label::Label(Display *dpy):
            Widget(dpy),
            sTextLayout(&sProperties),
            sTextAdjust(&sProperties),
            sFont(&sProperties),
            sColor(&sProperties),
            sHoverColor(&sProperties),
            sHover(&sProperties),
            sText(&sProperties),
            sConstraints(&sProperties),
            sPopup(&sProperties)
        {
            nMFlags     = 0;
            nState      = 0;

            pClass      = &metadata;
        }

        Label::~Label()
        {
            nFlags     |= FINALIZED;
        }

        status_t Label::init()
        {
            status_t result = Widget::init();
            if (result != STATUS_OK)
                return result;

            sTextLayout.bind("text.layout", &sStyle);
            sTextAdjust.bind("text.adjust", &sStyle);
            sFont.bind("font", &sStyle);
            sColor.bind("text.color", &sStyle);
            sHoverColor.bind("text.hover.color", &sStyle);
            sHover.bind("text.hover", &sStyle);
            sText.bind(&sStyle, pDisplay->dictionary());
            sConstraints.bind("size.constraints", &sStyle);
            sIPadding.bind("ipadding", &sStyle);
            sPopup.bind(NULL);

            handler_id_t id = sSlots.add(SLOT_SUBMIT, slot_on_submit, self());
            if (id >= 0) id = sSlots.add(SLOT_BEFORE_POPUP, slot_on_before_popup, self());
            if (id >= 0) id = sSlots.add(SLOT_POPUP, slot_on_popup, self());
            if (id < 0)
                return -id;

            return STATUS_OK;
        }

        void Label::property_changed(Property *prop)
        {
            Widget::property_changed(prop);
            if (sTextLayout.is(prop))
                query_draw();
            if (sTextAdjust.is(prop))
                query_resize();
            if (sFont.is(prop))
                query_resize();
            if (sColor.is(prop))
                query_draw();
            if (sHoverColor.is(prop))
                query_draw();
            if (sHover.is(prop))
                query_draw();
            if (sText.is(prop))
                query_resize();
            if (sConstraints.is(prop))
                query_resize();
            if (sIPadding.is(prop))
                query_resize();
        }

        void Label::draw(ws::ISurface *s)
        {
            // Form the text string
            LSPString text;
            sText.format(&text);
            sTextAdjust.apply(&text);

            // Estimate sizes
            float scaling   = lsp_max(0.0f, sScaling.get());
            float fscaling  = lsp_max(0.0f, scaling * sFontScaling.get());
            bool hover      = (nState & F_MOUSE_IN) && (sHover.get());
            ws::font_parameters_t fp;
            ws::text_parameters_t tp;
            ws::rectangle_t r, size;

            sFont.get_parameters(s, fscaling, &fp);
            sFont.get_multitext_parameters(s, &tp, fscaling, &text);
            sIPadding.sub(&size, &sSize, scaling);

            // Estimate drawing area
            tp.Height       = lsp_max(tp.Height, fp.Height);
            if (tp.Width <= size.nWidth)
            {
                r.nLeft         = 0;
                r.nWidth        = size.nWidth;
            }
            else
            {
                r.nLeft         = -0.5f * (tp.Width - size.nWidth);
                r.nWidth        = ceil(tp.Width);
            }

            if (tp.Height <= size.nHeight)
            {
                r.nTop          = 0;
                r.nHeight       = size.nHeight;
            }
            else
            {
                r.nTop          = -0.5f * (tp.Height - size.nHeight);
                r.nHeight       = ceil(tp.Height);
            }

            // Initialize palette
            lsp::Color bg_color;
            lsp::Color f_color((hover) ? sHoverColor : sColor);

            get_actual_bg_color(bg_color);
            f_color.scale_lch_luminance(sBrightness.get());

            // Draw background
            s->clear(bg_color);

            float halign    = lsp_limit(sTextLayout.halign() + 1.0f, 0.0f, 2.0f);
            float valign    = lsp_limit(sTextLayout.valign() + 1.0f, 0.0f, 2.0f);
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
                sFont.get_text_parameters(s, &tp, fscaling, &text, last, tail);
                float dx    = (r.nWidth - tp.Width) * 0.5f;
                ssize_t x   = r.nLeft   + dx * halign - tp.XBearing;
                y          += fp.Height;

                sFont.draw(s, f_color, x, y, fscaling, &text, last, tail);
                last    = curr + 1;
            }
        }

        void Label::size_request(ws::size_limit_t *r)
        {
            r->nMinWidth    = 0;
            r->nMinHeight   = 0;
            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;
            r->nPreWidth    = -1;
            r->nPreHeight   = -1;

            // Form the text string
            LSPString text;
            sText.format(&text);
            sTextAdjust.apply(&text);

            // Estimate sizes
            float scaling   = lsp_max(0.0f, sScaling.get());
            float fscaling  = lsp_max(0.0f, scaling * sFontScaling.get());
            ws::font_parameters_t fp;
            ws::text_parameters_t tp;

            sFont.get_parameters(pDisplay, fscaling, &fp);
            sFont.get_multitext_parameters(pDisplay, &tp, fscaling, &text);

            r->nMinWidth    = ceil(tp.Width);
            r->nMinHeight   = ceil(lsp_max(tp.Height, fp.Height));

            // Apply size constraints
            sConstraints.apply(r, scaling);
            sIPadding.add(r, scaling);
        }

        status_t Label::on_mouse_in(const ws::event_t *e)
        {
            Widget::on_mouse_in(e);

            size_t flags = nState;
            nState |= F_MOUSE_IN;
            if (flags != nState)
                query_draw();

            return STATUS_OK;
        }

        status_t Label::on_mouse_out(const ws::event_t *e)
        {
            Widget::on_mouse_out(e);

            size_t flags = nState;
            nState &= ~F_MOUSE_IN;
            if (flags != nState)
                query_draw();

            return STATUS_OK;
        }

        status_t Label::on_mouse_move(const ws::event_t *e)
        {
            size_t flags = nState;
            nState = lsp_setflag(nState, F_MOUSE_IN, inside(e->nLeft, e->nTop));
            if (flags != nState)
                query_draw();
            return STATUS_OK;
        }

        status_t Label::on_mouse_down(const ws::event_t *e)
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

        status_t Label::on_mouse_up(const ws::event_t *e)
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

        status_t Label::on_before_popup(Menu *menu)
        {
            return STATUS_OK;
        }

        status_t Label::on_popup(Menu *menu)
        {
            return STATUS_OK;
        }

        status_t Label::on_submit()
        {
            return STATUS_OK;
        }

        status_t Label::slot_on_submit(Widget *sender, void *ptr, void *data)
        {
            Label *_this = widget_ptrcast<Label>(ptr);
            return (_this != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }

        status_t Label::slot_on_before_popup(Widget *sender, void *ptr, void *data)
        {
            Label *_this = widget_ptrcast<Label>(ptr);
            Menu *_menu = widget_ptrcast<Menu>(sender);
            return (_this != NULL) ? _this->on_before_popup(_menu) : STATUS_BAD_ARGUMENTS;
        }

        status_t Label::slot_on_popup(Widget *sender, void *ptr, void *data)
        {
            Label *_this = widget_ptrcast<Label>(ptr);
            Menu *_menu = widget_ptrcast<Menu>(sender);
            return (_this != NULL) ? _this->on_popup(_menu) : STATUS_BAD_ARGUMENTS;
        }
    } /* namespace tk */
} /* namespace lsp */
