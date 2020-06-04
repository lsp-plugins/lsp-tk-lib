/*
 * Hyperlink.cpp
 *
 *  Created on: 23 окт. 2017 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/common/debug.h>
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/ipc/Process.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t Hyperlink::metadata =        { "Hyperlink", &Widget::metadata };

        Hyperlink::Hyperlink(Display *dpy):
            Widget(dpy),
            sTextLayout(&sProperties),
            sFont(&sProperties),
            sColor(&sProperties),
            sHoverColor(&sProperties),
            sText(&sProperties),
            sConstraints(&sProperties),
            sFollow(&sProperties),
            sUrl(&sProperties)
        {
            pClass      = &metadata;
            nMFlags     = 0;
            nState      = 0;
        }

        Hyperlink::~Hyperlink()
        {
        }

        status_t Hyperlink::init()
        {
            status_t res = Widget::init();
            if (res != STATUS_SUCCESS)
                return res;

            sTextLayout.bind("text.layout", &sStyle);
            sFont.bind("font", &sStyle);
            sColor.bind("text.color", &sStyle);
            sHoverColor.bind("text.hover.color", &sStyle);
            sText.bind(&sStyle, pDisplay->dictionary());
            sConstraints.bind("size.constraints", &sStyle);
            sFollow.bind("follow", &sStyle);
            sUrl.bind(&sStyle, pDisplay->dictionary());

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sTextLayout.init(sclass, 0.0f, 0.0f);
                sFont.init(sclass, 12.0f, ws::FF_UNDERLINE);
                sColor.init(sclass, "#0000cc");
                sHoverColor.init(sclass, "#ff0000");
                sConstraints.init(sclass, -1, -1, -1, -1);
                sFollow.init(sclass, true);

                // Overrides
                sPointer.override(sclass, ws::MP_HAND);
            }

            handler_id_t id = sSlots.add(SLOT_SUBMIT, slot_on_submit, self());
            if (id >= 0) id = sSlots.add(SLOT_BEFORE_POPUP, slot_on_before_popup, self());
            if (id >= 0) id = sSlots.add(SLOT_POPUP, slot_on_popup, self());
            if (id < 0)
                return -id;

// TODO: add default menu
//            ui_handler_id_t id = 0;
//
//            LSP_STATUS_ASSERT(sStdMenu.init());
//            LSPMenuItem *mi = new LSPMenuItem(pDisplay);
//            if (mi == NULL)
//                return STATUS_NO_MEM;
//            vStdItems[0] = mi;
//            LSP_STATUS_ASSERT(mi->init());
//            LSP_STATUS_ASSERT(sStdMenu.add(mi));
//            LSP_STATUS_ASSERT(mi->text()->set("actions.link.copy"));
//            id = mi->slots()->bind(LSPSLOT_SUBMIT, slot_copy_link_action, self());
//            if (id < 0)
//                return -id;
//
//            mi = new LSPMenuItem(pDisplay);
//            if (mi == NULL)
//                return STATUS_NO_MEM;
//            vStdItems[1] = mi;
//            LSP_STATUS_ASSERT(mi->init());
//            LSP_STATUS_ASSERT(sStdMenu.add(mi));
//            LSP_STATUS_ASSERT(mi->text()->set("actions.link.follow"));
//            id = mi->slots()->bind(LSPSLOT_SUBMIT, slot_on_submit, self());
//            if (id < 0)
//                return -id;
//

            return STATUS_OK;
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

            // Estimate sizes
            float scaling   = sScaling.get();
            ws::font_parameters_t fp;
            ws::text_parameters_t tp;

            sFont.get_parameters(pDisplay, scaling, &fp);
            sFont.get_multitext_parameters(pDisplay, &tp, scaling, &text);

            r->nMinWidth    = ceil(tp.Width);
            r->nMinHeight   = ceil(lsp_max(tp.Height, fp.Height));

            // Apply size constraints
            sConstraints.apply(r, scaling);
        }

        void Hyperlink::property_changed(Property *prop)
        {
            Widget::property_changed(prop);
            if (sTextLayout.is(prop))
                query_draw();
            if (sFont.is(prop))
                query_resize();
            if (sColor.is(prop))
                query_draw();
            if (sHoverColor.is(prop))
                query_draw();
            if (sText.is(prop))
                query_resize();
            if (sConstraints.is(prop))
                query_resize();
        }


        status_t Hyperlink::slot_on_submit(Widget *sender, void *ptr, void *data)
        {
            Hyperlink *_this = widget_ptrcast<Hyperlink>(ptr);
            return (_this != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }

        status_t Hyperlink::slot_on_before_popup(Widget *sender, void *ptr, void *data)
        {
            Hyperlink *_this = widget_ptrcast<Hyperlink>(ptr);
            /* Menu *menu = widget_ptrcast<Menu>(data); */
            return (_this != NULL) ? _this->on_before_popup(/* menu */) : STATUS_BAD_ARGUMENTS;
        }

        status_t Hyperlink::slot_on_popup(Widget *sender, void *ptr, void *data)
        {
            Hyperlink *_this = widget_ptrcast<Hyperlink>(ptr);
            /* Menu *menu = widget_ptrcast<Menu>(data); */
            return (_this != NULL) ? _this->on_popup(/* menu */) : STATUS_BAD_ARGUMENTS;
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

            #ifdef PLATFORM_WINDOWS
                ::ShellExecuteW(
                    NULL,               // Not associated with window
                    L"open",            // Open hyperlink
                    url.get_utf16(),    // The file to execute
                    NULL,               // Parameters
                    NULL,               // Directory
                    SW_SHOWNORMAL       // Show command
                );
            #else

                ipc::Process p;

                if ((res = p.set_command("xdg-open")) != STATUS_OK)
                    return STATUS_OK;
                if ((res = p.add_arg(&url)) != STATUS_OK)
                    return STATUS_OK;
                if ((res = p.launch()) != STATUS_OK)
                    return STATUS_OK;
                p.wait();
            #endif /* PLATFORM_WINDOWS */

            return STATUS_OK;
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

        void Hyperlink::draw(ws::ISurface *s)
        {
            // Form the text string
            LSPString text;
            sText.format(&text);

            // Estimate sizes
            float scaling   = sScaling.get();
            ws::font_parameters_t fp;
            ws::text_parameters_t tp;
            ws::rectangle_t r;

            sFont.get_parameters(pDisplay, scaling, &fp);
            sFont.get_multitext_parameters(pDisplay, &tp, scaling, &text);

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
            lsp::Color bg_color(sBgColor);
            lsp::Color f_color((nState & F_MOUSE_IN) ? sHoverColor : sColor);
            f_color.scale_lightness(sBrightness.get());

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
                sFont.get_text_parameters(s, &tp, scaling, &text, last, tail);
                float dx    = (r.nWidth - tp.Width) * 0.5f;
                ssize_t x   = r.nLeft   + dx * halign - tp.XBearing;
                y          += fp.Height;

                sFont.draw(s, f_color, x, y, scaling, &text, last, tail);
                last    = curr + 1;
            }
        }

        status_t Hyperlink::on_mouse_in(const ws::event_t *e)
        {
            Widget::on_mouse_in(e);

            if (nState & F_MOUSE_IGN)
                return STATUS_OK;
            size_t flags = nState;
            if ((nState & F_MOUSE_DOWN) && (nMFlags == (1 << ws::MCB_LEFT)))
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
            if ((nState & F_MOUSE_DOWN) && (nMFlags == (1 << ws::MCB_LEFT)))
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
            if ((nState & F_MOUSE_DOWN) && (nMFlags == (1 << ws::MCB_LEFT)) && (inside(e->nLeft, e->nTop)))
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

            nMFlags |= 1 << e->nCode;

            if ((nState & F_MOUSE_DOWN) && (nMFlags == (1 << ws::MCB_LEFT)) && (inside(e->nLeft, e->nTop)))
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
            nMFlags &= ~ (1 << e->nCode);
            if (nMFlags == 0)
                nState      = 0;

            if ((nState & F_MOUSE_DOWN) && (nMFlags == (1 << ws::MCB_LEFT)) && (inside(e->nLeft, e->nTop)))
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
                if ((flags == (1 << ws::MCB_LEFT)) && (e->nCode == ws::MCB_LEFT))
                    sSlots.execute(SLOT_SUBMIT, this);
                // TODO
//                else if ((flags == (1 << ws::MCB_RIGHT)) && (e->nCode == ws::MCB_RIGHT) && (pMenu != NULL))
//                {
//                    sSlots.execute(SLOT_BEFORE_POPUP, this, pMenu->self());
//                    pPopup->show(this, e);
//                    sSlots.execute(SLOT_POPUP, this, pMenu->self());
//                }
            }

            return STATUS_OK;
        }

        status_t Hyperlink::on_before_popup(/* Menu *menu */)
        {
            return STATUS_OK;
        }

        status_t Hyperlink::on_popup(/* Menu *menu */)
        {
            return STATUS_OK;
        }

    } /* namespace tk */
} /* namespace lsp */
