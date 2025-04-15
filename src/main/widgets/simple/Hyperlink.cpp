/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 23 окт. 2017 г.
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
#include <lsp-plug.in/common/debug.h>
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/ipc/Process.h>
#include <lsp-plug.in/runtime/system.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(Hyperlink, Widget)
                // Bind
                HyperlinkColors *c = &vColors[HYPERLINK_NORMAL];
                c->sColor.bind("text.color", this);

                c = &vColors[HYPERLINK_HOVER];
                c->sColor.bind("text.hover.color", this);

                c = &vColors[HYPERLINK_INACTIVE];
                c->sColor.bind("inactive.text.color", this);

                c = &vColors[HYPERLINK_INACTIVE| HYPERLINK_HOVER];
                c->sColor.bind("inactive.text.hover.color", this);

                sTextLayout.bind("text.layout", this);
                sTextAdjust.bind("text.adjust", this);
                sFont.bind("font", this);

                sConstraints.bind("size.constraints", this);
                sFollow.bind("follow", this);

                // Configure
                c = &vColors[HYPERLINK_NORMAL];
                c->sColor.set_rgb24(0x0000cc);

                c = &vColors[HYPERLINK_HOVER];
                c->sColor.set_rgb24(0xff0000);

                c = &vColors[HYPERLINK_INACTIVE];
                c->sColor.set_rgb24(0x000088);

                c = &vColors[HYPERLINK_INACTIVE| HYPERLINK_HOVER];
                c->sColor.set_rgb24(0xcc0000);

                sTextLayout.set(0.0f, 0.0f);
                sTextAdjust.set(TA_NONE);
                sFont.set_underline();
                sConstraints.set(-1, -1, -1, -1);
                sFollow.set(true);
                // Override
                sPointer.set(ws::MP_HAND);
                // Commit
                sPointer.override();
                sFont.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(Hyperlink, "Hyperlink", "root");

            void HyperlinkColors::listener(tk::prop::Listener *listener)
            {
                sColor.listener(listener);
            }

            bool HyperlinkColors::property_changed(Property *prop) const
            {
                return sColor.is(prop);
            }
        }

        const w_class_t Hyperlink::metadata =        { "Hyperlink", &Widget::metadata };

        Hyperlink::Hyperlink(Display *dpy):
            Widget(dpy),
            sTextLayout(&sProperties),
            sTextAdjust(&sProperties),
            sFont(&sProperties),
            sText(&sProperties),
            sConstraints(&sProperties),
            sFollow(&sProperties),
            sUrl(&sProperties),
            sPopup(&sProperties)
        {
            nMFlags         = 0;
            nState          = 0;
            vMenus[0]       = NULL;
            vMenus[1]       = NULL;
            vMenus[2]       = NULL;

            pClass          = &metadata;
        }

        Hyperlink::~Hyperlink()
        {
            nFlags     |= FINALIZED;
            do_destroy();
        }

        status_t Hyperlink::init()
        {
            status_t res = Widget::init();
            if (res != STATUS_SUCCESS)
                return res;

            if ((res = create_default_menu()) != STATUS_OK)
                return res;

            style::HyperlinkColors *c = &vColors[style::HYPERLINK_NORMAL];
            c->sColor.bind("text.color", &sStyle);

            c = &vColors[style::HYPERLINK_HOVER];
            c->sColor.bind("text.hover.color", &sStyle);

            c = &vColors[style::HYPERLINK_INACTIVE];
            c->sColor.bind("inactive.text.color", &sStyle);

            c = &vColors[style::HYPERLINK_INACTIVE | style::HYPERLINK_HOVER];
            c->sColor.bind("inactive.text.hover.color", &sStyle);

            sTextLayout.bind("text.layout", &sStyle);
            sTextAdjust.bind("text.adjust", &sStyle);
            sFont.bind("font", &sStyle);
            sText.bind(&sStyle, pDisplay->dictionary());
            sConstraints.bind("size.constraints", &sStyle);
            sFollow.bind("follow", &sStyle);
            sUrl.bind(&sStyle, pDisplay->dictionary());
            sPopup.bind(widget_ptrcast<Menu>(vMenus[0]));

            handler_id_t id = sSlots.add(SLOT_SUBMIT, slot_on_submit, self());
            if (id >= 0) id = sSlots.add(SLOT_BEFORE_POPUP, slot_on_before_popup, self());
            if (id >= 0) id = sSlots.add(SLOT_POPUP, slot_on_popup, self());
            if (id < 0)
                return -id;

            return STATUS_OK;
        }

        void Hyperlink::destroy()
        {
            nFlags     |= FINALIZED;
            Widget::destroy();
            do_destroy();
        }

        void Hyperlink::do_destroy()
        {
            // Destroy all nested widgets
            for (size_t i=0; i<sizeof(vMenus)/sizeof(Menu *); ++i)
            {
                if (vMenus[i] != NULL)
                {
                    vMenus[i]->destroy();
                    delete vMenus[i];
                    vMenus[i] = NULL;
                }
            }
        }

        void Hyperlink::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            const style::HyperlinkColors *colors = select_colors();
            if (colors->property_changed(prop))
                query_draw();

            if (sTextLayout.is(prop))
                query_draw();
            if (sTextAdjust.is(prop))
                query_resize();
            if (sFont.is(prop))
                query_resize();
            if (sText.is(prop))
                query_resize();
            if (sConstraints.is(prop))
                query_resize();
        }

        void Hyperlink::size_request(ws::size_limit_t *r)
        {
            r->nMinWidth    = 0;
            r->nMinHeight   = 0;
            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;

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
            r->nPreWidth    = -1;
            r->nPreHeight   = -1;

            // Apply size constraints
            sConstraints.apply(r, scaling);
        }

        const style::HyperlinkColors *Hyperlink::select_colors() const
        {
            size_t index = (sActive.get()) ? style::HYPERLINK_NORMAL : style::HYPERLINK_INACTIVE;
            index = lsp_setflag(index, style::HYPERLINK_HOVER, nState & F_MOUSE_IN);
            return &vColors[index];
        }

        void Hyperlink::draw(ws::ISurface *s, bool force)
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
            ws::rectangle_t r;

            sFont.get_parameters(pDisplay, fscaling, &fp);
            sFont.get_multitext_parameters(pDisplay, &tp, fscaling, &text);

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

            // Initialize palette
            const style::HyperlinkColors *colors = select_colors();

            lsp::Color bg_color;
            lsp::Color f_color(colors->sColor);

            get_actual_bg_color(bg_color);
            f_color.scale_lch_luminance(select_brightness());

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

        status_t Hyperlink::create_default_menu()
        {
            // Add default menu
            handler_id_t id = 0;
            Menu *menu      = new Menu(pDisplay);
            if (menu == NULL)
                return STATUS_NO_MEM;
            vMenus[0]       = menu;
            LSP_STATUS_ASSERT(menu->init());

            // Create 'copy' menu item
            MenuItem *mi    = new MenuItem(pDisplay);
            if (mi == NULL)
                return STATUS_NO_MEM;
            vMenus[1]       = mi;
            LSP_STATUS_ASSERT(mi->init());
            LSP_STATUS_ASSERT(menu->add(mi));
            LSP_STATUS_ASSERT(mi->text()->set("actions.link.copy"));
            id = mi->slots()->bind(SLOT_SUBMIT, slot_copy_link_action, self());
            if (id < 0)
                return -id;

            mi    = new MenuItem(pDisplay);
                        if (mi == NULL)
                            return STATUS_NO_MEM;
            vMenus[2]       = mi;
            LSP_STATUS_ASSERT(mi->init());
            LSP_STATUS_ASSERT(menu->add(mi));
            LSP_STATUS_ASSERT(mi->text()->set("actions.link.follow"));
            id = mi->slots()->bind(SLOT_SUBMIT, slot_on_submit, self());
            if (id < 0)
                return -id;

            return STATUS_OK;
        }

        status_t Hyperlink::slot_on_submit(Widget *sender, void *ptr, void *data)
        {
            Hyperlink *_this = widget_ptrcast<Hyperlink>(ptr);
            return (_this != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }

        status_t Hyperlink::slot_on_before_popup(Widget *sender, void *ptr, void *data)
        {
            Hyperlink *_this = widget_ptrcast<Hyperlink>(ptr);
            Menu *_menu = widget_ptrcast<Menu>(sender);
            return (_this != NULL) ? _this->on_before_popup(_menu) : STATUS_BAD_ARGUMENTS;
        }

        status_t Hyperlink::slot_on_popup(Widget *sender, void *ptr, void *data)
        {
            Hyperlink *_this = widget_ptrcast<Hyperlink>(ptr);
            Menu *_menu = widget_ptrcast<Menu>(sender);
            return (_this != NULL) ? _this->on_popup(_menu) : STATUS_BAD_ARGUMENTS;
        }

        status_t Hyperlink::slot_copy_link_action(Widget *sender, void *ptr, void *data)
        {
            Hyperlink *_this = widget_ptrcast<Hyperlink>(ptr);
            if (_this == NULL)
                return STATUS_BAD_ARGUMENTS;

            return _this->copy_url(ws::CBUF_CLIPBOARD);
        }

        status_t Hyperlink::follow_url() const
        {
            LSPString url;
            status_t res = sUrl.format(&url);
            if (res != STATUS_OK)
                return res;

            return system::follow_url(&url);
        }

        status_t Hyperlink::copy_url(ws::clipboard_id_t cb)
        {
            // Prepare URL to copy
            LSPString url;
            status_t res = sUrl.format(&url);
            if (res != STATUS_OK)
                return res;

            // Copy data to clipboard
            TextDataSource *src = new TextDataSource();
            if (src == NULL)
                return STATUS_NO_MEM;
            src->acquire();

            status_t result = src->set_text(&url);
            if (result == STATUS_OK)
                pDisplay->set_clipboard(cb, src);
            src->release();

            return result;
        }

        status_t Hyperlink::on_submit()
        {
            lsp_trace("hyperlink submitted");
            return (sFollow.get()) ? follow_url() : STATUS_OK;
        }

        status_t Hyperlink::on_mouse_in(const ws::event_t *e)
        {
            Widget::on_mouse_in(e);

            if (nState & F_MOUSE_IGN)
                return STATUS_OK;
            size_t flags = nState;
            if ((nState & F_MOUSE_DOWN) && (nMFlags == (size_t(1) << ws::MCB_LEFT)))
                nState |= F_MOUSE_IN;
            else if (nMFlags == 0)
                nState |= F_MOUSE_IN;
            else
                nState &= ~F_MOUSE_IN;
            if (flags != nState)
                query_draw();
            return STATUS_OK;
        }

        status_t Hyperlink::on_mouse_out(const ws::event_t *e)
        {
            Widget::on_mouse_out(e);

            if (nState & F_MOUSE_IGN)
                return STATUS_OK;
            size_t flags = nState;
            if ((nState & F_MOUSE_DOWN) && (nMFlags == (size_t(1) << ws::MCB_LEFT)))
                nState |= F_MOUSE_IN;
            else
                nState &= ~F_MOUSE_IN;
            if (flags != nState)
                query_draw();
            return STATUS_OK;
        }

        status_t Hyperlink::on_mouse_move(const ws::event_t *e)
        {
            if (nState & F_MOUSE_IGN)
                return STATUS_OK;
            size_t flags = nState;
            if ((nState & F_MOUSE_DOWN) && (nMFlags == (size_t(1) << ws::MCB_LEFT)) && (inside(e->nLeft, e->nTop)))
                nState |= F_MOUSE_IN;
            else if (nMFlags == 0)
                nState |= F_MOUSE_IN;
            else
                nState &= ~F_MOUSE_IN;
            if (flags != nState)
                query_draw();
            return STATUS_OK;
        }

        status_t Hyperlink::on_mouse_down(const ws::event_t *e)
        {
            size_t flags = nState;

            if (nMFlags == 0)
            {
                if (e->nCode == ws::MCB_LEFT)
                    nState |= F_MOUSE_DOWN | F_MOUSE_IN;
                else
                    nState |= F_MOUSE_IGN;
            }

            nMFlags |= size_t(1) << e->nCode;

            if ((nState & F_MOUSE_DOWN) && (nMFlags == (size_t(1) << ws::MCB_LEFT)) && (inside(e->nLeft, e->nTop)))
                nState |= F_MOUSE_IN;
            else if (nMFlags == 0)
                nState |= F_MOUSE_IN;
            else
                nState &= ~F_MOUSE_IN;

            if (flags != nState)
                query_draw();
            return STATUS_OK;
        }

        status_t Hyperlink::on_mouse_up(const ws::event_t *e)
        {
            size_t flags = nMFlags;
            nMFlags &= ~ (size_t(1) << e->nCode);
            if (nMFlags == 0)
                nState      = 0;

            if ((nState & F_MOUSE_DOWN) && (nMFlags == (size_t(1) << ws::MCB_LEFT)) && (inside(e->nLeft, e->nTop)))
                nState |= F_MOUSE_IN;
            else if (nMFlags == 0)
                nState |= F_MOUSE_IN;
            else
                nState &= ~F_MOUSE_IN;

            if (flags != nState)
                query_draw();

            // Trigger submit action
            if (inside(e->nLeft, e->nTop))
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

        status_t Hyperlink::on_before_popup(Menu *menu)
        {
            return STATUS_OK;
        }

        status_t Hyperlink::on_popup(Menu *menu)
        {
            return STATUS_OK;
        }

    } /* namespace tk */
} /* namespace lsp */
