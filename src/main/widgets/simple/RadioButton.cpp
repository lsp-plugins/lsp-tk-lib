/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 10 нояб. 2020 г.
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
            LSP_TK_STYLE_IMPL_BEGIN(RadioButton, Widget)
                // Bind
                style::RadioButtonColors *c = &vColors[style::RADIOBUTTON_NORMAL];
                c->sColor.bind("color", this);
                c->sFillColor.bind("fill.color", this);
                c->sBorderColor.bind("border.color", this);
                c->sBorderGapColor.bind("border.gap.color", this);

                c = &vColors[style::RADIOBUTTON_HOVER];
                c->sColor.bind("hover.color", this);
                c->sFillColor.bind("fill.hover.color", this);
                c->sBorderColor.bind("border.hover.color", this);
                c->sBorderGapColor.bind("border.gap.hover.color", this);

                c = &vColors[style::RADIOBUTTON_INACTIVE];
                c->sColor.bind("inactive.color", this);
                c->sFillColor.bind("inactive.fill.color", this);
                c->sBorderColor.bind("inactive.border.color", this);
                c->sBorderGapColor.bind("inactive.border.gap.color", this);

                c = &vColors[style::RADIOBUTTON_HOVER | style::RADIOBUTTON_INACTIVE];
                c->sColor.bind("inactive.hover.color", this);
                c->sFillColor.bind("inactive.fill.hover.color", this);
                c->sBorderColor.bind("inactive.border.hover.color", this);
                c->sBorderGapColor.bind("inactive.border.gap.hover.color", this);

                sConstraints.bind("size.constraints", this);
                sBorderSize.bind("border.size", this);
                sBorderGapSize.bind("border.gap.size", this);
                sCheckGapSize.bind("check.gap.size", this);
                sCheckMinSize.bind("check.min.size", this);
                sChecked.bind("checked", this);

                // Configure
                c = &vColors[style::RADIOBUTTON_NORMAL];
                c->sColor.set("#00ccff");
                c->sFillColor.set("#ffffff");
                c->sBorderColor.set("#000000");
                c->sBorderGapColor.set("#cccccc");

                c = &vColors[style::RADIOBUTTON_HOVER];
                c->sColor.set("#ff8800");
                c->sFillColor.set("#ffeeee");
                c->sBorderColor.set("#000000");
                c->sBorderGapColor.set("#cccccc");

                c = &vColors[style::RADIOBUTTON_INACTIVE];
                c->sColor.set("#cccccc");
                c->sFillColor.set("#888888");
                c->sBorderColor.set("#000000");
                c->sBorderGapColor.set("#888888");

                c = &vColors[style::RADIOBUTTON_HOVER | style::RADIOBUTTON_INACTIVE];
                c->sColor.set("#cccccc");
                c->sFillColor.set("#888888");
                c->sBorderColor.set("#000000");
                c->sBorderGapColor.set("#888888");

                sConstraints.set_all(16);
                sBorderSize.set(1);
                sBorderGapSize.set(1);
                sCheckGapSize.set(2);
                sCheckMinSize.set(4);
                sChecked.set(false);

                // Commit
                sConstraints.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(RadioButton, "RadioButton", "root");

            void RadioButtonColors::listener(tk::prop::Listener *listener)
            {
                sColor.listener(listener);
                sFillColor.listener(listener);
                sBorderColor.listener(listener);
                sBorderGapColor.listener(listener);
            }

            bool RadioButtonColors::property_changed(Property *prop)
            {
                return prop->one_of(sColor, sFillColor, sBorderColor, sBorderGapColor);
            }
        }

        const w_class_t RadioButton::metadata      = { "RadioButton", &Widget::metadata };

        RadioButton::RadioButton(Display *dpy):
            Widget(dpy),
            sConstraints(&sProperties),
            sBorderSize(&sProperties),
            sBorderGapSize(&sProperties),
            sCheckGapSize(&sProperties),
            sCheckMinSize(&sProperties),
            sChecked(&sProperties)
        {
            pClass          = &metadata;

            for (size_t i=0; i<RBTN_TOTAL; ++i)
                vColors[i].listener(&sProperties);

            nState          = 0;
            nBMask          = 0;

            sArea.nLeft     = 0;
            sArea.nTop      = 0;
            sArea.nWidth    = 0;
            sArea.nHeight   = 0;
        }

        RadioButton::~RadioButton()
        {
        }

        status_t RadioButton::init()
        {
            status_t res    = Widget::init();
            if (res != STATUS_OK)
                return res;

            // Bind properties
            style::RadioButtonColors *c = &vColors[style::RADIOBUTTON_NORMAL];
            c->sColor.bind("color", &sStyle);
            c->sFillColor.bind("fill.color", &sStyle);
            c->sBorderColor.bind("border.color", &sStyle);
            c->sBorderGapColor.bind("border.gap.color", &sStyle);

            c = &vColors[style::RADIOBUTTON_HOVER];
            c->sColor.bind("hover.color", &sStyle);
            c->sFillColor.bind("fill.hover.color", &sStyle);
            c->sBorderColor.bind("border.hover.color", &sStyle);
            c->sBorderGapColor.bind("border.gap.hover.color", &sStyle);

            c = &vColors[style::RADIOBUTTON_INACTIVE];
            c->sColor.bind("inactive.color", &sStyle);
            c->sFillColor.bind("inactive.fill.color", &sStyle);
            c->sBorderColor.bind("inactive.border.color", &sStyle);
            c->sBorderGapColor.bind("inactive.border.gap.color", &sStyle);

            c = &vColors[style::RADIOBUTTON_HOVER | style::RADIOBUTTON_INACTIVE];
            c->sColor.bind("inactive.hover.color", &sStyle);
            c->sFillColor.bind("inactive.fill.hover.color", &sStyle);
            c->sBorderColor.bind("inactive.border.hover.color", &sStyle);
            c->sBorderGapColor.bind("inactive.border.gap.hover.color", &sStyle);

            sConstraints.bind("size.constraints", &sStyle);
            sBorderSize.bind("border.size", &sStyle);
            sBorderGapSize.bind("border.gap.size", &sStyle);
            sCheckGapSize.bind("check.gap.size", &sStyle);
            sCheckMinSize.bind("check.min.size", &sStyle);
            sChecked.bind("checked", &sStyle);

            sConstraints.bind("size.constraints", &sStyle);
            sBorderSize.bind("border.size", &sStyle);
            sBorderGapSize.bind("border.gap.size", &sStyle);
            sCheckGapSize.bind("check.gap.size", &sStyle);
            sCheckMinSize.bind("check.min.size", &sStyle);
            sChecked.bind("checked", &sStyle);

            // Additional slots
            handler_id_t id = 0;
            id = sSlots.add(SLOT_SUBMIT, slot_on_submit, self());

            return (id >= 0) ? STATUS_OK : -id;
        }

        style::RadioButtonColors *RadioButton::select_colors()
        {
            size_t flags = (sActive.get()) ? style::CHECKBOX_NORMAL : style::CHECKBOX_INACTIVE;
            if (nState & XF_HOVER)
                flags          |= style::CHECKBOX_HOVER;

            return &vColors[flags];
        }

        void RadioButton::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            style::RadioButtonColors *colors = select_colors();
            if (colors->property_changed(prop))
                query_draw();

            if (sChecked.is(prop))
            {
                nState  = lsp_setflag(nState, XF_CHECKED, sChecked.get());
                query_draw();
            }

            if (prop->one_of(sConstraints, sBorderSize, sBorderGapSize, sCheckGapSize, sCheckMinSize))
                query_resize();
        }

        void RadioButton::size_request(ws::size_limit_t *r)
        {
            float scaling       = sScaling.get();
            ssize_t border      = (sBorderSize.get() > 0) ? lsp_max(1.0f, sBorderSize.get() * scaling) : 0;
            ssize_t bgap        = (sBorderGapSize.get() > 0) ? lsp_max(1.0f, sBorderGapSize.get() * scaling) : 0;
            ssize_t ckgap       = (sCheckGapSize.get() > 0) ? lsp_max(1.0f, sCheckGapSize.get() * scaling) : 0;
            ssize_t ckmin       = lsp_max(1.0f, sCheckMinSize.get() * scaling);

            // Estimate the size of check box
            border             += lsp_max(ckgap, bgap);

            // Return the size
            r->nMinWidth        = ckmin + border;
            r->nMinHeight       = r->nMinWidth;
            r->nMaxWidth        = -1;
            r->nMaxHeight       = -1;
            r->nPreWidth        = -1;
            r->nPreHeight       = -1;

            // Apply size constraints
            sConstraints.apply(r, scaling);
        }

        void RadioButton::realize(const ws::rectangle_t *r)
        {
            // Call parent widget for realize
            Widget::realize(r);

            sArea.nWidth        = lsp_min(r->nWidth, r->nHeight);
            sArea.nHeight       = sArea.nWidth;
            sArea.nLeft         = r->nLeft  + (r->nWidth  - sArea.nWidth ) / 2;
            sArea.nTop          = r->nTop   + (r->nHeight - sArea.nHeight) / 2;
        }

        void RadioButton::draw(ws::ISurface *s, bool force)
        {
            lsp::Color c;

            float scaling       = sScaling.get();
            float bright        = select_brightness();
            ssize_t border      = (sBorderSize.get() > 0) ? lsp_max(1.0f, sBorderSize.get() * scaling) : 0;
            ssize_t bgap        = (sBorderGapSize.get() > 0) ? lsp_max(1.0f, sBorderGapSize.get() * scaling) : 0;
            ssize_t ckgap       = (sCheckGapSize.get() > 0) ? lsp_max(1.0f, sCheckGapSize.get() * scaling) : 0;
            float r             = sArea.nWidth * 0.5f;
            float cx            = sArea.nLeft - sSize.nLeft + r;
            float cy            = sArea.nTop  - sSize.nTop  + r;
            size_t state        = nState;
            const style::RadioButtonColors *colors = select_colors();

            ckgap               = lsp_max(ckgap, bgap);

            // Draw background
            bool aa     = s->set_antialiasing(false);
            get_actual_bg_color(c);
            s->fill_rect(c, SURFMASK_NONE, 0.0f, 0, 0, sSize.nWidth, sSize.nHeight);
            s->set_antialiasing(true);

            // Draw border
            if (border > 0)
            {
                c.copy(colors->sBorderColor);
                c.scale_lch_luminance(bright);
                s->fill_circle(c, cx, cy, r);
                r                  -= border;
            }

            // Draw border gap
            float frad          = r;
            if (bgap > 0)
            {
                c.copy(colors->sBorderGapColor);
                c.scale_lch_luminance(bright);
                s->fill_circle(c, cx, cy, frad);
                frad               -= bgap;
            }

            // Draw fill
            c.copy(colors->sFillColor);
            c.scale_lch_luminance(bright);
            s->fill_circle(c, cx, cy, frad);

            // Draw check
            if (state & XF_CHECKED)
            {
                r                  -= ckgap;
                c.copy(colors->sColor);
                c.scale_lch_luminance(bright);
                s->fill_circle(c, cx, cy, r);
            }

            // Restore antialiasing
            s->set_antialiasing(aa);
        }

        bool RadioButton::rinside(const ws::rectangle_t *r, float x, float y)
        {
            float rad   = r->nWidth * 0.5f;
            x          -= r->nLeft + rad;
            y          -= r->nTop  + rad;

            return (x*x + y*y) <= rad*rad;
        }

        status_t RadioButton::on_mouse_down(const ws::event_t *e)
        {
            if (nState & XF_OUT)
                return STATUS_OK;

            if (nBMask == 0)
            {
                bool inside     = rinside(&sArea, e->nLeft, e->nTop);
                if ((e->nCode == ws::MCB_LEFT) && (inside))
                    nState     |= XF_ACTIVE;
                else
                    nState     |= XF_OUT;
            }

            nBMask     |= (size_t(1) << e->nCode);

            return on_mouse_move(e);
        }

        status_t RadioButton::on_mouse_up(const ws::event_t *e)
        {
            on_mouse_move(e);

            size_t state    = nState;
            nBMask         &= ~(size_t(1) << e->nCode);
            if (nBMask == 0)
            {
                bool checked = state & XF_CHECKED;
                if (checked != sChecked.get())
                {
                    sChecked.commit_value(checked);
                    sSlots.execute(SLOT_SUBMIT, this);
                }
                nState         &= ~XF_OUT;
            }

            if (state != nState)
                query_draw();

            return STATUS_OK;
        }

        status_t RadioButton::on_mouse_move(const ws::event_t *e)
        {
            if (nState & XF_OUT)
                return STATUS_OK;

            size_t state    = nState;
            bool checked    = sChecked.get();
            bool inside     = rinside(&sArea, e->nLeft, e->nTop);

            nState          = lsp_setflag(nState, XF_HOVER, inside);
            if ((nBMask == ws::MCF_LEFT) && (inside))
                nState          = lsp_setflag(nState, XF_CHECKED, !checked);
            else
                nState          = lsp_setflag(nState, XF_CHECKED, checked);

            if (state != nState)
                query_draw();

            return STATUS_OK;
        }

        status_t RadioButton::on_mouse_out(const ws::event_t *e)
        {
            size_t state    = nState;
            nState          = (sChecked.get()) ? XF_CHECKED : 0;
            nBMask          = 0;

            if (state != nState)
                query_draw();

            return STATUS_OK;
        }

        status_t RadioButton::on_key_down(const ws::event_t *e)
        {
            size_t state    = nState;
            if (e->nCode == ' ')
            {
                bool checked    = !sChecked.get();
                nState          = lsp_setflag(nState, XF_CHECKED, checked);
                sChecked.commit_value(checked);
                sSlots.execute(SLOT_SUBMIT, this);
            }

            if (state != nState)
                query_draw();

            return STATUS_OK;
        }

        status_t RadioButton::slot_on_submit(Widget *sender, void *ptr, void *data)
        {
            RadioButton *_this = widget_ptrcast<RadioButton>(ptr);
            return (_this != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }

        status_t RadioButton::on_submit()
        {
            return STATUS_OK;
        }
    } /* namespace tk */
} /* namespace lsp */


