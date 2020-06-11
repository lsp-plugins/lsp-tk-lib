/*
 * Edit.cpp
 *
 *  Created on: 29 авг. 2017 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/common/debug.h>
#include <lsp-plug.in/stdlib/math.h>
#include <wctype.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t Edit::metadata      = { "Edit", &Widget::metadata };

        //-----------------------------------------------------------------------------
        // Edit::TextCursor implementation
        Edit::EditCursor::EditCursor(Edit *widget): TextCursor(widget->display())
        {
            pEdit   = widget;
        }

        Edit::EditCursor::~EditCursor()
        {
        }

        ssize_t Edit::EditCursor::limit(ssize_t value)
        {
            LSPString *text = pEdit->sText.format();
            ssize_t max = (text != NULL) ? 0 : text->length();
            return lsp_limit(value, 0, max);
        }

        void Edit::EditCursor::on_change()
        {
            set_shining(true);
            pEdit->query_draw();
        }

        void Edit::EditCursor::on_blink()
        {
            pEdit->query_draw();
        }

        //-----------------------------------------------------------------------------
        // Edit::KeyboardInput implementation
        Edit::KeyboardInput::KeyboardInput(Edit *widget)
        {
            pEdit   = widget;
        }

        Edit::KeyboardInput::~KeyboardInput()
        {
        }

        status_t Edit::KeyboardInput::on_key_press(const ws::event_t *e)
        {
            LSPString s;
            s.set(lsp_wchar_t(e->nCode));
            lsp_trace("Key code pressed=%x, symbol received=%s", int(e->nCode), s.get_native());

            return STATUS_OK;
        }

        //-----------------------------------------------------------------------------
        // Edit::DataSink implementation
        Edit::DataSink::DataSink(Edit *widget)
        {
            pEdit   = widget;
        }

        Edit::DataSink::~DataSink()
        {
            unbind();
        }

        void Edit::DataSink::unbind()
        {
            if (pEdit != NULL)
            {
                if (pEdit->pDataSink == this)
                    pEdit->pDataSink = NULL;
                pEdit       = NULL;
            }
        }

        status_t Edit::DataSink::receive(const LSPString *text, const char *mime)
        {
            if (pEdit != NULL)
                pEdit->paste_clipboard(text);
            return STATUS_OK;
        }

        status_t Edit::DataSink::close(status_t code)
        {
            status_t res = TextDataSink::close(code);
            unbind();

            return res;
        }

        //-----------------------------------------------------------------------------
        // Edit implementation
        Edit::Edit(Display *dpy):
            Widget(dpy),
            sInput(this),
            sCursor(this),
            sText(&sProperties),
            sSelection(&sProperties),
            sFont(&sProperties),
            sColor(&sProperties),
            sBorderColor(&sProperties),
            sBorderGapColor(&sProperties),
            sCursorColor(&sProperties),
            sTextColor(&sProperties),
            sTextSelectedColor(&sProperties),
            sSelectionColor(&sProperties),
            sBorderSize(&sProperties),
            sBorderGapSize(&sProperties),
            sBorderRadius(&sProperties),
            sConstraints(&sProperties)
        {
            sTextPos            = 0;
            nMBState            = 0;
            nScrDirection       = 0;
//            pPopup          = &sStdPopup;
            pDataSink           = NULL;

            sTextArea.nLeft     = -1;
            sTextArea.nTop      = -1;
            sTextArea.nWidth    = 0;
            sTextArea.nHeight   = 0;

//            vStdItems[0]    = NULL;
//            vStdItems[1]    = NULL;
//            vStdItems[2]    = NULL;
            pClass          = &metadata;
        }

        Edit::~Edit()
        {
        }

        status_t Edit::init()
        {
            handler_id_t id;
            status_t result = Widget::init();
            if (result != STATUS_OK)
                return result;

            // Initialize keyboard and scroll timer
            if (pDisplay != NULL)
            {
                sInput.init(pDisplay);
                sScroll.bind(pDisplay);
                sScroll.set_handler(timer_handler, self());
            }

            sSelection.bind("selection", &sStyle);
            sFont.bind("font", &sStyle);
            sColor.bind("color", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sBorderGapColor.bind("border.gap.color", &sStyle);
            sCursorColor.bind("cursor.color", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sTextSelectedColor.bind("text.selected.color", &sStyle);
            sSelectionColor.bind("selection.color", &sStyle);
            sBorderSize.bind("border.size", &sStyle);
            sBorderGapSize.bind("border.gap.size", &sStyle);
            sBorderRadius.bind("border.radius", &sStyle);
            sConstraints.bind("size.constraints", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sSelection.init(sclass);
                sFont.init(sclass);
                sColor.init(sclass, "#ffffff");
                sBorderColor.init(sclass, "0x000000");
                sBorderGapColor.init(sclass, "0xcccccc");
                sCursorColor.init(sclass, "#0x000000");
                sTextColor.init(sclass, "#000000");
                sTextSelectedColor.init(sclass, "#ffffff");
                sSelectionColor.init(sclass, "#0000cc");
                sBorderSize.init(sclass, 1);
                sBorderGapSize.init(sclass, 1);
                sBorderRadius.init(sclass, 4);
                sConstraints.init(sclass, -1, -1, -1, 8);

                // Overrides
                sPointer.override(sclass, ws::MP_IBEAM);
            }

            // TODO: Initialize standard menu
//            ui_handler_id_t id = 0;
//            LSP_STATUS_ASSERT(sStdPopup.init());
//            LSPMenuItem *mi = new LSPMenuItem(pDisplay);
//            if (mi == NULL)
//                return STATUS_NO_MEM;
//            vStdItems[0] = mi;
//            LSP_STATUS_ASSERT(mi->init());
//            LSP_STATUS_ASSERT(sStdPopup.add(mi));
//            LSP_STATUS_ASSERT(mi->text()->set("actions.edit.cut"));
//            id = mi->slots()->bind(LSPSLOT_SUBMIT, slot_popup_cut_action, self());
//            if (id < 0)
//                return -id;
//
//            mi = new LSPMenuItem(pDisplay);
//            if (mi == NULL)
//                return STATUS_NO_MEM;
//            vStdItems[1] = mi;
//            LSP_STATUS_ASSERT(mi->init());
//            LSP_STATUS_ASSERT(sStdPopup.add(mi));
//            LSP_STATUS_ASSERT(mi->text()->set("actions.edit.copy"));
//            id = mi->slots()->bind(LSPSLOT_SUBMIT, slot_popup_copy_action, self());
//            if (id < 0)
//                return -id;
//
//            mi = new LSPMenuItem(pDisplay);
//            if (mi == NULL)
//                return STATUS_NO_MEM;
//            vStdItems[2] = mi;
//            LSP_STATUS_ASSERT(mi->init());
//            LSP_STATUS_ASSERT(sStdPopup.add(mi));
//            LSP_STATUS_ASSERT(mi->text()->set("actions.edit.paste"));
//            id = mi->slots()->bind(LSPSLOT_SUBMIT, slot_popup_paste_action, self());
//            if (id < 0)
//                return -id;

            // Bind slots
            id = sSlots.add(SLOT_CHANGE, slot_on_change, self());
            if (id < 0)
                return -id;

            // Initialize pointer
//            set_cursor(MP_IBEAM);

            return STATUS_OK;
        }

        void Edit::destroy()
        {
//            for (size_t i=0; i<3; ++i)
//                if (vStdItems[i] != NULL)
//                {
//                    vStdItems[i]->destroy();
//                    delete vStdItems[i];
//                    vStdItems[i] = NULL;
//                }

            if (pDataSink != NULL)
            {
                pDataSink->unbind();
                pDataSink   = NULL;
            }

            Widget::destroy();
        }

        void Edit::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (sSelection.is(prop))
            {
                // TODO: sync state
                query_draw();
            }

            if (sText.is(prop))
            {
                // TODO: sync parameters
                query_draw();
            }

            if (sFont.is(prop))
                query_resize();
            if (sColor.is(prop))
                query_draw();
            if (sBorderColor.is(prop))
                query_draw();
            if (sBorderGapColor.is(prop))
                query_draw();
            if (sCursorColor.is(prop))
                query_draw();
            if (sTextColor.is(prop))
                query_draw();
            if (sTextSelectedColor.is(prop))
                query_draw();
            if (sSelectionColor.is(prop))
                query_draw();
            if (sBorderSize.is(prop))
                query_resize();
            if (sBorderGapSize.is(prop))
                query_resize();
            if (sBorderRadius.is(prop))
                query_resize();
            if (sConstraints.is(prop))
                query_resize();
        }

        void Edit::size_request(ws::size_limit_t *r)
        {
            float scaling   = lsp_max(0.0f, sScaling.get());
            size_t radius   = (sBorderRadius.get() > 0) ? lsp_max(1.0f, sBorderRadius.get() * scaling) : 0;
            size_t border   = (sBorderSize.get() > 0) ? lsp_max(1.0f, sBorderSize.get() * scaling) : 0;
            if (border > 0)
                border         += (sBorderGapSize.get() > 0) ? lsp_max(1.0f, sBorderGapSize.get()) : 0;

            size_t extra    = lsp_max(radius, border);

            r->nMinWidth    = extra*2;
            r->nMinHeight   = extra*2;

            // Compute text parameters
            size_t rgap     = radius - lsp_max(0.0f, truncf(M_SQRT1_2 * (radius - border)));

            // Estimate sizes
            ws::font_parameters_t fp;
            sFont.get_parameters(pDisplay, scaling, &fp);
            r->nMinHeight   = lsp_max(r->nMinHeight, rgap*2 + fp.Height);

            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;

            // Apply size constraints
            sConstraints.apply(r, scaling);
        }

        void Edit::realize(const ws::rectangle_t *r)
        {
            Widget::realize(r);

            float scaling       = lsp_max(0.0f, sScaling.get());
            ssize_t radius      = (sBorderRadius.get() > 0) ? lsp_max(1.0f, sBorderRadius.get() * scaling) : 0;
            ssize_t border      = (sBorderSize.get() > 0) ? lsp_max(1.0f, sBorderSize.get() * scaling) : 0;
            if (border > 0)
                border             += (sBorderGapSize.get() > 0) ? lsp_max(1.0f, sBorderGapSize.get()) : 0;

            border              = lsp_max(border, radius - truncf(M_SQRT1_2 * (radius - border)));

            sTextArea.nLeft     = r->nLeft  + border;
            sTextArea.nTop      = r->nTop   + border;
            sTextArea.nWidth    = r->nWidth - border*2;
            sTextArea.nHeight   = r->nHeight- border*2;
        }

        status_t Edit::timer_handler(ws::timestamp_t time, void *arg)
        {
            Edit *_this = widget_ptrcast<Edit>(arg);
            if (_this == NULL)
                return STATUS_BAD_ARGUMENTS;
            _this->update_scroll();
            return STATUS_OK;
        }

        void Edit::run_scroll(ssize_t dir)
        {
            nScrDirection = dir;
            if (dir == 0)
                sScroll.cancel();
            else if (!sScroll.is_launched())
                sScroll.launch(0, 25);
        }

        void Edit::update_scroll()
        {
            sCursor.move(nScrDirection);
            if (sSelection.valid())
                sSelection.set_last(sCursor.location());

            LSPString *text = sText.format();
            ssize_t len = (text != NULL) ? text->length() : 0;
            if ((sCursor.position() <= 0) || (sCursor.position() >= len))
                sScroll.cancel();
        }

        void Edit::update_clipboard(size_t bufid)
        {
            if (sSelection.valid() && sSelection.non_empty())
            {
                TextDataSource *src = new TextDataSource();
                if (src == NULL)
                    return;
                src->acquire();

                // Set the selection
                LSPString *text = sText.format();
                if (text != NULL)
                {
                    ssize_t first = sSelection.first(), last = sSelection.last();
                    status_t result = src->set_text(text, first, last);
                    if (result == STATUS_OK)
                        pDisplay->set_clipboard(bufid, src);
                }

                src->release();
            }
        }

//        status_t Edit::set_text(const LSPString *text)
//        {
//            if (!sText.set(text))
//                return STATUS_NO_MEM;
//
//            query_draw();
//
//            ssize_t len = sText.length();
//            if (sCursor.location() > len)
//                sCursor.set(len);
//            if (sSelection.valid())
//            {
//                if (sSelection.first() > len)
//                    sSelection.set_first(len);
//                if (sSelection.last() > len)
//                    sSelection.set_last(len);
//            }
//            return STATUS_OK;
//        }

        void Edit::draw(ws::ISurface *s)
        {
            ws::font_parameters_t fp;
            ws::text_parameters_t tp;
            ws::rectangle_t xr;

            xr.nLeft        = 0;
            xr.nTop         = 0;
            xr.nWidth       = sSize.nWidth;
            xr.nHeight      = sSize.nHeight;

            // Clear
            lsp::Color color(sBgColor);
            s->clear(color);

            // Draw border
            float scaling   = lsp_max(0.0f, sScaling.get());
            float lightness = sBrightness.get();
            ssize_t radius  = (sBorderRadius.get() > 0) ? lsp_max(1.0f, sBorderRadius.get() * scaling) : 0;
            ssize_t border  = (sBorderSize.get() > 0) ? lsp_max(1.0f, sBorderSize.get() * scaling) : 0;
            float aa        = s->set_antialiasing(true);

            // Draw border
            if (border > 0)
            {
                color.copy(sBorderColor);
                color.scale_lightness(lightness);
                s->fill_round_rect(color, SURFMASK_ALL_CORNER, radius, &xr);

                xr.nLeft       += border;
                xr.nTop        += border;
                xr.nWidth      -= border * 2;
                xr.nHeight     -= border * 2;
                radius          = lsp_max(0, radius - border);

                ssize_t gap     = (sBorderGapSize.get() > 0) ? lsp_max(1.0f, sBorderGapSize.get()) : 0;
                if (gap > 0)
                {
                    color.copy(sBorderGapColor);
                    color.scale_lightness(lightness);
                    s->fill_round_rect(color, SURFMASK_ALL_CORNER, radius, &xr);

                    xr.nLeft       += gap;
                    xr.nTop        += gap;
                    xr.nWidth      -= gap * 2;
                    xr.nHeight     -= gap * 2;
                    radius          = lsp_max(0, radius - gap);
                }
            }

//            font_parameters_t fp;
//            text_parameters_t tp;
//            ssize_t pad  = 3;
//
//            // Prepare palette
//            Color bg_color(sBgColor);
//            Color color(sColor);
//            Color fcol(sFont.raw_color());
//            Color sel_col(sSelColor);
//
//            color.scale_lightness(brightness());
//            fcol.scale_lightness(brightness());
//            sel_col.scale_lightness(brightness());
//
//            // Draw background
//            s->clear(bg_color);
//
//            // Draw
//            bool aa = s->set_antialiasing(true);
//            s->fill_round_rect(0.5f, 0.5f, sSize.nWidth - 1, sSize.nHeight - 1, 4, SURFMASK_ALL_CORNER, color);
//
//            s->set_antialiasing(aa);
//            ssize_t fw = sSize.nWidth - pad *2;
//
//            sFont.get_parameters(s, &fp);
//            sFont.get_text_parameters(s, &tp, &sText, 0, sCursor.location());
//            ssize_t tw    = /*tp.XBearing +*/ tp.XAdvance;
//
//            if (sCursor.visible() && sCursor.replacing() && (sCursor.position() >= ssize_t(sText.length())))
//            {
//                sFont.get_text_parameters(s, &tp, "_");
//                fw         -= tp.Width;
//            }
//
//            ssize_t xleft = sTextPos + tw;
//
//            // Adjust cursor position
//            if (sCursor.visible())
//            {
//                // Make cursor visible within the range of edit
//                if (xleft < 0)
//                {
//                    sTextPos    = -tw;
//                    xleft       = 0;
//                }
//                else if (xleft >= ssize_t(fw))
//                {
//                    sTextPos    = fw - tw;
//                    xleft       = fw;
//                }
//                else if ((sTextPos < 0) && (sCursor.location() == ssize_t(sText.length())))
//                {
//                    sTextPos   += fw - xleft;
//                    xleft       = fw;
//                }
//            }
//
//            // Adjust text position
//            if (sTextPos > 0)
//            {
//                xleft          -= sTextPos;
//                sTextPos        = 0;
//            }
//
//            if ((sSelection.valid()) && (!sSelection.is_empty()))
//            {
//                ssize_t first   = sSelection.starting();
//                ssize_t last    = sSelection.ending();
//
//                ssize_t xpos    = sTextPos + pad;
//
//                if (first > 0)
//                {
//                    sFont.get_text_parameters(s, &tp, &sText, 0, first);
//                    sFont.draw(s, xpos, pad + (sSize.nHeight - pad * 2 - fp.Height)*0.5f + fp.Ascent, fcol, &sText, 0, first);
//                    xpos           += /*tp.XBearing + */ tp.XAdvance;
//                }
//                sFont.get_text_parameters(s, &tp, &sText, first, last);
//                s->fill_rect(xpos, pad, tp.XBearing + tp.XAdvance, sSize.nHeight - pad*2, sel_col);
//                sFont.draw(s, xpos, pad + (sSize.nHeight - pad * 2 - fp.Height)*0.5f + fp.Ascent, color, &sText, first, last);
//                xpos           += /*tp.XBearing + */ tp.XAdvance;
//
//                if (last < ssize_t(sText.length()))
//                {
//                    sFont.get_text_parameters(s, &tp, &sText, last);
//                    sFont.draw(s, xpos, pad + (sSize.nHeight - pad * 2 - fp.Height)*0.5f + fp.Ascent, fcol, &sText, last);
//                    xpos           += /*tp.XBearing + */ tp.XAdvance;
//                }
//            }
//            else
//                sFont.draw(s, sTextPos + pad, pad + (sSize.nHeight - pad * 2 - fp.Height)*0.5f + fp.Ascent, fcol, &sText);
//
//            // Draw cursor if required
//            if (sCursor.visible() && sCursor.shining())
//            {
//                float cleft = xleft + pad ; // + tp.XAdvance + tp.XBearing;
//                float ctop  = pad + (sSize.nHeight - pad * 2 - fp.Height)*0.5f;
//
//                if (sCursor.inserting())
//                {
//                    if ((sSelection.valid()) && (!sSelection.is_empty()))
//                        s->line(cleft + 0.5f, ctop, cleft, ctop + fp.Height, 1.0f, bg_color);
//                    else
//                        s->line(cleft + 0.5f, ctop, cleft, ctop + fp.Height, 1.0f, fcol);
//                }
//                else // replacing
//                {
//                    if (sCursor.position() >= ssize_t(sText.length()))
//                    {
//                        sFont.get_text_parameters(s, &tp, "_");
//                        s->fill_rect(cleft, pad, tp.XAdvance, sSize.nHeight - pad * 2, bg_color);
//                    }
//                    else
//                    {
//                        sFont.get_text_parameters(s, &tp, &sText, sCursor.position(), sCursor.position() + 1);
//                        ssize_t xw = (tp.XAdvance > tp.Width) ? tp.XAdvance : tp.Width + 1;
//                        s->fill_rect(cleft + tp.XBearing - 1, pad, xw, sSize.nHeight - pad * 2, bg_color);
//                        sFont.draw(s, cleft, ctop + fp.Ascent, color, &sText, sCursor.position(), sCursor.position() + 1);
//                    }
//                }
//            }
//
//            s->set_antialiasing(true);
//            s->wire_round_rect(0.5f, 0.5f, sSize.nWidth - 1, sSize.nHeight - 1, 4, SURFMASK_ALL_CORNER, 1, color);
//
            s->set_antialiasing(aa);
        }

        status_t Edit::on_change()
        {
            return STATUS_OK;
        }

        status_t Edit::slot_on_change(Widget *sender, void *ptr, void *data)
        {
            Edit *_this = widget_ptrcast<Edit>(ptr);
            return (_this != NULL) ? _this->on_change() : STATUS_BAD_ARGUMENTS;
        }

        status_t Edit::on_mouse_down(const ws::event_t *e)
        {
            size_t state = nMBState;
            nMBState    |= (1 << e->nCode);
            if (state == 0)
                take_focus();

            if ((e->nCode == ws::MCB_LEFT) && (state == 0))
            {
                ssize_t first = mouse_to_cursor_pos(e->nLeft, e->nTop);
                if (first >= 0)
                {
                    sSelection.set(first);
                    sCursor.set_position(first);
                }
            }

            return STATUS_OK;
        }

        ssize_t Edit::mouse_to_cursor_pos(ssize_t x, ssize_t y)
        {
            x -= sSize.nLeft;
            if ((x < 0) || (x >= sSize.nWidth))
                return -1;

            LSPString *text = sText.format();
            if (text == NULL)
                return -1;

            float scaling = lsp_max(0.0f, sScaling.get());
            ssize_t left = 0, right = text->length(), pad = 3 * scaling;
            ws::text_parameters_t tp;
            if (sFont.get_text_parameters(pDisplay, &tp, scaling, text))
            {
                if (x > (sTextPos + pad + tp.XAdvance))
                    return right;
            }

            while ((right - left) > 1)
            {
                ssize_t middle = (left + right) >> 1;
                if (!sFont.get_text_parameters(pDisplay, &tp, scaling, text, 0, middle))
                    return -1;

                ssize_t tx = sTextPos + pad + tp.XAdvance;
                if (tx > x)
                    right = middle;
                else if (tx < x)
                    left = middle;
                else // tx == x
                {
                    left    = middle;
                    break;
                }
            }

            return left;
        }

        status_t Edit::on_mouse_dbl_click(const ws::event_t *e)
        {
            if (e->nCode == ws::MCB_LEFT)
            {
                const LSPString *text = sText.format();
                if (text == NULL)
                    return STATUS_OK;

                ssize_t first = mouse_to_cursor_pos(e->nLeft, e->nTop);
                if (!iswalnum(text->at(first)))
                    return STATUS_OK;

                ssize_t last = first, len = text->length();
                while (first > 0)
                {
                    if (!iswalnum(text->at(first-1)))
                        break;
                    first --;
                }

                while ((++last) < len)
                {
                    if (!iswalnum(text->at(last)))
                        break;
                }

                sSelection.set(first, last);
                update_clipboard(ws::CBUF_PRIMARY);
                sCursor.set(last);
            }
            return STATUS_OK;
        }

        status_t Edit::on_mouse_tri_click(const ws::event_t *e)
        {
            if (e->nCode == ws::MCB_LEFT)
            {
                sSelection.set_all();
                update_clipboard(ws::CBUF_PRIMARY);
            }
            return STATUS_OK;
        }

        status_t Edit::on_mouse_up(const ws::event_t *e)
        {
            lsp_trace("mouse up");
            if ((nMBState == (1 << ws::MCB_RIGHT)) && (e->nCode == ws::MCB_RIGHT))
            {
//                if (pPopup != NULL)
//                    pPopup->show(this, e);
            }
            else if ((nMBState == (1 << ws::MCB_LEFT)) && (e->nCode == ws::MCB_LEFT))
            {
                update_clipboard(ws::CBUF_PRIMARY);
                if (sSelection.length() <= 0)
                    sSelection.clear();
            }
            else if ((nMBState == (1 << ws::MCB_MIDDLE)) && (e->nCode == ws::MCB_MIDDLE))
            {
                ssize_t first = mouse_to_cursor_pos(e->nLeft, e->nTop);
                sSelection.set(first);
                sCursor.set(first);
                request_clipboard(ws::CBUF_PRIMARY);
            }

            nMBState    &= ~(1 << e->nCode);
            return STATUS_OK;
        }

        status_t Edit::on_mouse_move(const ws::event_t *e)
        {
            if (nMBState == (1 << ws::MCB_LEFT))
            {
                if (e->nLeft < sSize.nLeft)
                    run_scroll(-1);
                else if (e->nLeft > (sSize.nLeft + sSize.nWidth))
                    run_scroll(+1);
                else
                {
                    run_scroll(0);

                    ssize_t last = mouse_to_cursor_pos(e->nLeft, e->nTop);
                    if (last >= 0)
                    {
                        sSelection.set_last(last);
                        sCursor.set_position(last);
                    }
                }
            }

            return STATUS_OK;
        }

        status_t Edit::on_focus_in(const ws::event_t *e)
        {
            sCursor.show();
            return STATUS_OK;
        }

        status_t Edit::on_focus_out(const ws::event_t *e)
        {
            sCursor.hide();
            return STATUS_OK;
        }

        void Edit::paste_clipboard(const LSPString *s)
        {
            LSPString *text = sText.format();
            if (text == NULL)
                return;

            size_t changes = 0;
            if (sSelection.valid() && sSelection.non_empty())
            {
                text->remove(sSelection.starting(), sSelection.ending());
                sCursor.set_location(sSelection.starting());
                sSelection.clear();
                ++changes;
            }

            if (s->length() > 0)
            {
                size_t pos = sCursor.location();
                if (text->insert(pos, s))
                {
                    pos += s->length();
                    sCursor.set_location(pos);
                    sSelection.set(pos);
                    ++changes;
                }
            }

            // Invalidate text after changes
            if (changes > 0)
                sText.invalidate();
        }

        status_t Edit::on_key_down(const ws::event_t *e)
        {
// TODO
//            LSPString s;
//            s.set(lsp_wchar_t(e->nCode));
//            lsp_trace("Key code pressed=%x, symbol received=%s, modifiers=%x", int(e->nCode), s.get_native(), int(e->nState));
//
//            ws::code_t key = KeyboardHandler::translate_keypad(e->nCode);
//
//            if (ws::is_character_key(key))
//            {
//                if (!(e->nState & (ws::MCF_CONTROL | ws::MCF_ALT)))
//                {
//                    if (sSelection.valid() && sSelection.length() > 0)
//                    {
//                        sText.remove(sSelection.starting(), sSelection.ending());
//                        sCursor.set_location(sSelection.starting());
//                        sSelection.clear();
//                        update_clipboard(CBUF_PRIMARY);
//                    }
//
//                    ssize_t loc = sCursor.location();
//                    if ((sCursor.replacing()) && (loc < ssize_t(sText.length())))
//                        sText.set(loc, lsp_wchar_t(key));
//                    else
//                        sText.insert(sCursor.location(), lsp_wchar_t(key));
//
//                    sCursor.move(1);
//                    return STATUS_OK;
//                }
//                else if (e->nState & ws::MCF_CONTROL)
//                {
//                    switch (e->nCode)
//                    {
//                        case 'C': case 'c':
//                            if (sSelection.valid() && sSelection.non_empty())
//                                update_clipboard(CBUF_CLIPBOARD);
//                            break;
//                        case 'V': case 'v':
//                            request_clipboard(CBUF_CLIPBOARD);
//                            break;
//                        case 'A': case 'a':
//                            sSelection.set(0, sText.length());
//                            update_clipboard(CBUF_PRIMARY);
//                            break;
//                        case 'X': case 'x':
//                            cut_data(CBUF_CLIPBOARD);
//                            break;
//                    }
//                    return STATUS_OK;
//                }
//            }
//
//            if (e->nState & ws::MCF_SHIFT)
//            {
//                if (sSelection.first() < 0)
//                    sSelection.set_first(sCursor.location());
//            }
//
//            switch (key)
//            {
//                case ws::WSK_HOME:
//                    if (e->nState & ws::MCF_SHIFT)
//                        sSelection.set_last(0);
//                    else
//                        sSelection.clear();
//                    sCursor.set_location(0);
//                    break;
//                case ws::WSK_END:
//                    if (e->nState & ws::MCF_SHIFT)
//                        sSelection.set_last(sText.length());
//                    else
//                        sSelection.clear();
//                    sCursor.set_location(sText.length());
//                    break;
//                case ws::WSK_LEFT:
//                    sCursor.move(-1);
//                    if (e->nState & ws::MCF_SHIFT)
//                        sSelection.set_last(sCursor.location());
//                    else
//                        sSelection.clear();
//                    break;
//                case ws::WSK_RIGHT:
//                    sCursor.move(1);
//                    if (e->nState & ws::MCF_SHIFT)
//                        sSelection.set_last(sCursor.location());
//                    else
//                        sSelection.clear();
//                    break;
//                case ws::WSK_BACKSPACE:
//                {
//                    if (sSelection.valid() && sSelection.length() > 0)
//                    {
//                        sText.remove(sSelection.starting(), sSelection.ending());
//                        sCursor.set_location(sSelection.starting());
//                        sSelection.clear();
//                    }
//                    else
//                    {
//                        ssize_t pos = sCursor.location();
//                        if (pos <= 0)
//                            break;
//                        sText.remove(pos - 1, pos);
//                        sCursor.set_location(pos-1);
//                    }
//                    query_draw();
//                    sSlots.execute(ws::LSPSLOT_CHANGE, this);
//                    break;
//                }
//                case ws::WSK_DELETE:
//                {
//                    if (sSelection.valid() && sSelection.length() > 0)
//                    {
//                        sText.remove(sSelection.starting(), sSelection.ending());
//                        sCursor.set_location(sSelection.starting());
//                        sSelection.clear();
//                    }
//                    else
//                    {
//                        ssize_t pos = sCursor.location();
//                        if (pos >= ssize_t(sText.length()))
//                            break;
//                        sText.remove(pos, pos + 1);
//                        sCursor.set_location(pos);
//                    }
//                    query_draw();
//                    sSlots.execute(LSPSLOT_CHANGE, this);
//                    break;
//                }
//                case ws::WSK_INSERT:
//                {
//                    size_t flags = (e->nState & (ws::MCF_CONTROL | ws::MCF_SHIFT | ws::MCF_ALT));
//                    if (flags == ws::MCF_SHIFT)
//                        request_clipboard(ws::CBUF_CLIPBOARD);
//                    else if (flags == ws::MCF_CONTROL)
//                    {
//                        if (sSelection.valid() && sSelection.non_empty())
//                            update_clipboard(ws::CBUF_CLIPBOARD);
//                    }
//                    else
//                        sCursor.toggle_mode();
//                    break;
//                }
//            }

            return STATUS_OK;
        }

        void Edit::request_clipboard(size_t bufid)
        {
            // Unbind previous data sink
            if (pDataSink != NULL)
            {
                pDataSink->unbind();
                pDataSink = NULL;
            }

            // Create new data sink and run
            DataSink *sink  = new DataSink(this);
            if (sink == NULL)
                return;
            pDataSink       = sink;

            // Request clipboard contents in async mode
            pDisplay->get_clipboard(bufid, sink);

//            pDisplay->get_clipboard(bufid, sink);
//
//            if (sSelection.valid() && sSelection.non_empty())
//            {
//                sText.remove(sSelection.starting(), sSelection.ending());
//                sCursor.set_location(sSelection.starting());
//                sSelection.clear();
//            }
//            pDisplay->fetch_clipboard(bufid, "UTF8_STRING", clipboard_handler, self());
        }

        status_t Edit::on_key_up(const ws::event_t *e)
        {
            lsp_trace("Key code released=%x, modifiers=%x", int(e->nCode), int(e->nState));
            ws::code_t key = KeyboardHandler::translate_keypad(e->nCode);
            if (((key == ws::WSK_SHIFT_L) || (key == ws::WSK_SHIFT_R)) && (e->nState & ws::MCF_SHIFT))
                update_clipboard(ws::CBUF_PRIMARY);

            return STATUS_OK;
        }

        status_t Edit::cut_data(size_t bufid)
        {
// TODO
//            if (sSelection.valid() && sSelection.non_empty())
//            {
//                update_clipboard(bufid);
//                sText.remove(sSelection.starting(), sSelection.ending());
//                sCursor.set_location(sSelection.starting());
//                sSelection.clear();
//            }
            return STATUS_OK;
        }

        status_t Edit::copy_data(size_t bufid)
        {
            if (sSelection.valid() && sSelection.non_empty())
                update_clipboard(bufid);
            return STATUS_OK;
        }

        status_t Edit::paste_data(size_t bufid)
        {
            request_clipboard(bufid);
            return STATUS_OK;
        }

        status_t Edit::slot_popup_cut_action(Widget *sender, void *ptr, void *data)
        {
            Edit *_this = widget_ptrcast<Edit>(ptr);
            return (_this != NULL) ? _this->cut_data(ws::CBUF_CLIPBOARD) : STATUS_BAD_ARGUMENTS;
        }

        status_t Edit::slot_popup_copy_action(Widget *sender, void *ptr, void *data)
        {
            Edit *_this = widget_ptrcast<Edit>(ptr);
            return (_this != NULL) ? _this->copy_data(ws::CBUF_CLIPBOARD) : STATUS_BAD_ARGUMENTS;
        }

        status_t Edit::slot_popup_paste_action(Widget *sender, void *ptr, void *data)
        {
            Edit *_this = widget_ptrcast<Edit>(ptr);
            return (_this != NULL) ? _this->paste_data(ws::CBUF_CLIPBOARD) : STATUS_BAD_ARGUMENTS;
        }

    } /* namespace tk */
} /* namespace lsp */
