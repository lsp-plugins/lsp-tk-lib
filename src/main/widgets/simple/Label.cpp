/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#include <lsp-plug.in/common/debug.h>
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/stdlib/stdio.h>
#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/tk/helpers/draw.h>

#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {

        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(Label, Widget)
                // Bind
                LabelColors *c = &vColors[LABEL_NORMAL];
                c->sColor.bind("text.color", this);

                c = &vColors[LABEL_HOVER];
                c->sColor.bind("text.hover.color", this);

                c = &vColors[LABEL_INACTIVE];
                c->sColor.bind("inactive.text.color", this);

                c = &vColors[LABEL_INACTIVE| LABEL_HOVER];
                c->sColor.bind("inactive.text.hover.color", this);

                sTextLayout.bind("text.layout", this);
                sTextAdjust.bind("text.adjust", this);
                sTextClip.bind("text.clip", this);
                sFont.bind("font", this);
                sHover.bind("text.hover", this);
                sConstraints.bind("size.constraints", this);
                sIPadding.bind("ipadding", this);

                // Configure
                c = &vColors[LABEL_NORMAL];
                c->sColor.set_rgb24(0x000000);

                c = &vColors[LABEL_HOVER];
                c->sColor.set_rgb24(0xff0000);

                c = &vColors[LABEL_INACTIVE];
                c->sColor.set_rgb24(0x888888);

                c = &vColors[LABEL_INACTIVE| LABEL_HOVER];
                c->sColor.set_rgb24(0xcc0000);

                sTextLayout.set(0.0f, 0.0f);
                sTextAdjust.set(TA_NONE);
                sTextClip.set(false);
                sFont.set_size(12.0f);
                sHover.set(false);
                sConstraints.set(-1, -1, -1, -1);
                sIPadding.set(0, 0, 0, 0);
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(Label, "Label", "root");

            void LabelColors::listener(tk::prop::Listener *listener)
            {
                sColor.listener(listener);
            }

            bool LabelColors::property_changed(Property *prop) const
            {
                return sColor.is(prop);
            }
        }

        const w_class_t Label::metadata =       { "Label", &Widget::metadata };

        Label::Label(Display *dpy):
            Widget(dpy),
            sTextLayout(&sProperties),
            sTextAdjust(&sProperties),
            sFont(&sProperties),
            sHover(&sProperties),
            sText(&sProperties),
            sTextClip(&sProperties),
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
            destroy_text_estimations();
        }

        status_t Label::init()
        {
            status_t result = Widget::init();
            if (result != STATUS_OK)
                return result;

            style::LabelColors *c = &vColors[style::LABEL_NORMAL];
            c->sColor.bind("text.color", &sStyle);

            c = &vColors[style::LABEL_HOVER];
            c->sColor.bind("text.hover.color", &sStyle);

            c = &vColors[style::LABEL_INACTIVE];
            c->sColor.bind("inactive.text.color", &sStyle);

            c = &vColors[style::LABEL_INACTIVE | style::LABEL_HOVER];
            c->sColor.bind("inactive.text.hover.color", &sStyle);

            sTextLayout.bind("text.layout", &sStyle);
            sTextAdjust.bind("text.adjust", &sStyle);
            sFont.bind("font", &sStyle);
            sHover.bind("text.hover", &sStyle);
            sText.bind(&sStyle, pDisplay->dictionary());
            sTextClip.bind("text.clip", &sStyle);
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

            const style::LabelColors *colors = select_colors();
            if (colors->property_changed(prop))
                query_draw();

            if (prop->one_of(sTextLayout, sHover))
                query_draw();

            if (prop->one_of(sTextAdjust, sTextClip, sFont, sText, sConstraints, sIPadding))
                query_resize();
        }

        void Label::draw(ws::ISurface *s, bool force)
        {
            // Form the text string
            LSPString text;
            sText.format(&text);
            sTextAdjust.apply(&text);

            // Estimate sizes
            float scaling   = lsp_max(0.0f, sScaling.get());
            float fscaling  = lsp_max(0.0f, scaling * sFontScaling.get());
            ws::font_parameters_t fp;
            ws::text_parameters_t tp;
            ws::rectangle_t r, size;

            sFont.get_parameters(s, fscaling, &fp);
            sFont.get_multitext_parameters(s, &tp, fscaling, &text);
            sIPadding.sub(&size, &sSize, scaling);

            // Apply text clipping
            const bool clip = sTextClip.get();
            if (clip)
            {
                r.nLeft     = 0;
                r.nTop      = 0;
                r.nWidth    = sSize.nWidth;
                r.nHeight   = sSize.nHeight;
                sIPadding.enter(&r, scaling);
                s->clip_begin(&r);
            }
            lsp_finally {
                if (clip)
                    s->clip_end();
            };

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
            const style::LabelColors *colors = select_colors();

            lsp::Color bg_color;
            lsp::Color f_color(colors->sColor);

            get_actual_bg_color(bg_color);
            f_color.scale_lch_luminance(select_brightness());

            // Draw background
            s->clear(bg_color);

            draw_multiline_text(s,
                &sFont, &r, f_color, &fp, &tp,
                sTextLayout.halign(), sTextLayout.valign(),
                fscaling, &text);
        }

        const style::LabelColors *Label::select_colors(bool active, bool hover) const
        {
            size_t index = (active) ? style::LABEL_NORMAL : style::LABEL_INACTIVE;
            index = lsp_setflag(index, style::LABEL_HOVER, hover);
            return &vColors[index];
        }

        const style::LabelColors *Label::select_colors() const
        {
            return select_colors(
                sActive.get(),
                (nState & F_MOUSE_IN) && (sHover.get()));
        }

        size_t Label::destroy_text_estimations()
        {
            size_t removed = 0;
            for (lltl::iterator<prop::String> it = vEstimations.values(); it; ++it)
            {
                prop::String *s = it.get();
                if (s != NULL)
                {
                    ++removed;
                    delete s;
                }
            }
            vEstimations.clear();

            return removed;
        }

        void Label::clear_text_estimations()
        {
            const size_t removed = destroy_text_estimations();
            if (removed > 0)
                query_resize();
        }

        tk::String *Label::add_text_estimation()
        {
            prop::String *s = new prop::String(&sProperties);
            if (s == NULL)
                return NULL;
            s->bind(&sStyle, pDisplay->dictionary());

            if (vEstimations.add(s))
            {
                query_resize();
                return s;
            }

            delete s;
            return NULL;
        }

        void Label::estimate_string_size(estimation_t *e, const LSPString *s)
        {
            sFont.get_multitext_parameters(pDisplay, &e->tp, e->fscaling, s);

            e->r->nMinWidth     = lsp_max(e->r->nMinWidth, ceilf(e->tp.Width));
            e->r->nMinHeight    = lsp_max(e->r->nMinHeight, ceilf(lsp_max(e->tp.Height, e->fp.Height)));
        }

        bool Label::contains_digit(const LSPString *s)
        {
            const lsp_wchar_t *chars = s->characters();
            for (size_t i=0, n=s->length(); i<n; ++i)
            {
                const lsp_wchar_t ch = chars[i];
                if ((ch >= '0') && (ch <= '9'))
                    return true;
            }
            return false;
        }

        void Label::set_all_digits(LSPString *s, lsp_wchar_t new_ch)
        {
            for (size_t i=0, n=s->length(); i<n; ++i)
            {
                const lsp_wchar_t ch = s->char_at(i);
                if ((ch >= '0') && (ch <= '9'))
                    s->set_at(i, new_ch);
            }
        }

        void Label::estimate_string_size(estimation_t *e, tk::String *s)
        {
            if (s == NULL)
                return;

            LSPString text;

            // Form the text string
            s->format(&text);
            sTextAdjust.apply(&text);
            estimate_string_size(e, &text);

            // Check that text contains digits
            if (contains_digit(&text))
            {
                for (char ch = '0'; ch <= '9'; ++ch)
                {
                    set_all_digits(&text, ch);
                    estimate_string_size(e, &text);
                }
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

            // Form the estimation parameters
            estimation_t e;
            e.scaling       = lsp_max(0.0f, sScaling.get());
            e.fscaling      = lsp_max(0.0f, e.scaling * sFontScaling.get());
            e.r             = r;
            sFont.get_parameters(pDisplay, e.fscaling, &e.fp);

            if (!sTextClip.get())
            {
                // Estimate the size of the label
                for (lltl::iterator<prop::String> it = vEstimations.values(); it; ++it)
                    estimate_string_size(&e, it.get());
                estimate_string_size(&e, &sText);
            }
            else
                r->nMinHeight   = e.fp.Height;
            
            // Apply size constraints
            sConstraints.apply(r, e.scaling);
            sIPadding.add(r, e.scaling);
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

            nMFlags |= size_t(1) << e->nCode;
            nState = lsp_setflag(nState, F_MOUSE_IN, inside(e->nLeft, e->nTop));

            if (flags != nState)
                query_draw();
            return STATUS_OK;
        }

        status_t Label::on_mouse_up(const ws::event_t *e)
        {
            size_t flags = nMFlags;
            nMFlags &= ~ (size_t(1) << e->nCode);
            if (nMFlags == 0)
                nState      = 0;

            bool xinside = inside(e->nLeft, e->nTop);
            nState = lsp_setflag(nState, F_MOUSE_IN, xinside);
            if (flags != nState)
                query_draw();

            // Trigger submit action
            if (xinside)
            {
                if ((flags == (size_t(1) << ws::MCB_LEFT)) && (e->nCode == ws::MCB_LEFT))
                    sSlots.execute(SLOT_SUBMIT, this);
                else if ((flags == (size_t(1) << ws::MCB_RIGHT)) && (e->nCode == ws::MCB_RIGHT) && (sPopup.is_set()))
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
