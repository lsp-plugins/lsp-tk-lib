/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 29 авг. 2017 г.
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
#include <wctype.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(Edit, Widget)
                // Init
                sSelection.bind("selection", this);
                sFont.bind("font", this);
                sColor.bind("color", this);
                sBorderColor.bind("border.color", this);
                sBorderGapColor.bind("border.gap.color", this);
                sCursorColor.bind("cursor.color", this);
                sTextColor.bind("text.color", this);
                sTextSelectedColor.bind("text.selected.color", this);
                sSelectionColor.bind("selection.color", this);
                sBorderSize.bind("border.size", this);
                sBorderGapSize.bind("border.gap.size", this);
                sBorderRadius.bind("border.radius", this);
                sConstraints.bind("size.constraints", this);
                // Configure
                sSelection.set(-1, -1);
                sFont.set_size(12.0f);
                sColor.set("#ffffff");
                sBorderColor.set("#000000");
                sBorderGapColor.set("#cccccc");
                sCursorColor.set("#000000");
                sTextColor.set("#000000");
                sTextSelectedColor.set("#ffffff");
                sSelectionColor.set("#00c0ff");
                sBorderSize.set(1);
                sBorderGapSize.set(1);
                sBorderRadius.set(4);
                sConstraints.set(-1, -1, -1, 8);
                // Override
                sPointer.set(ws::MP_IBEAM);
                // Commit
                sPointer.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(Edit, "Edit");
        }

        const w_class_t Edit::metadata      = { "Edit", &Widget::metadata };

        //-----------------------------------------------------------------------------
        // Edit::TextCursor implementation
        Edit::EditCursor::EditCursor(Edit *widget): TextCursor(widget->display())
        {
            pEdit   = widget;
        }

        Edit::EditCursor::~EditCursor()
        {
            nFlags     |= FINALIZED;
        }

        ssize_t Edit::EditCursor::limit(ssize_t value)
        {
            LSPString *text = pEdit->sText.formatted();
            return lsp_limit(value, 0, ssize_t(text->length()));
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
            sConstraints(&sProperties),
            sPopup(&sProperties)
        {
            sTextPos            = 0;
            nMBState            = 0;
            nScrDirection       = 0;
            pDataSink           = NULL;
            vMenu[0]            = NULL;
            vMenu[1]            = NULL;
            vMenu[2]            = NULL;
            vMenu[3]            = NULL;

            sTextArea.nLeft     = -1;
            sTextArea.nTop      = -1;
            sTextArea.nWidth    = 0;
            sTextArea.nHeight   = 0;

            pClass          = &metadata;
        }

        Edit::~Edit()
        {
            nFlags     |= FINALIZED;
            do_destroy();
        }

        void Edit::destroy()
        {
            nFlags     |= FINALIZED;
            do_destroy();

            if (pDataSink != NULL)
            {
                pDataSink->unbind();
                pDataSink   = NULL;
            }

            Widget::destroy();
        }

        void Edit::do_destroy()
        {
            for (size_t i=0; i<4; ++i)
                if (vMenu[i] != NULL)
                {
                    vMenu[i]->destroy();
                    delete vMenu[i];
                    vMenu[i] = NULL;
                }
        }

        status_t Edit::init()
        {
            handler_id_t id;
            status_t result = Widget::init();
            if (result != STATUS_OK)
                return result;

            result = create_default_menu();
            if (result != STATUS_OK)
                return result;

            // Initialize keyboard and scroll timer
            if (pDisplay != NULL)
            {
                sInput.init(pDisplay);
                sScroll.bind(pDisplay);
                sScroll.set_handler(timer_handler, self());
            }

            sText.bind(&sStyle, pDisplay->dictionary());
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
            sPopup.bind(widget_ptrcast<Menu>(vMenu[0]));

            // Bind slots
            id = sSlots.add(SLOT_CHANGE, slot_on_change, self());
            if (id < 0)
                return -id;

            return STATUS_OK;
        }

        status_t Edit::create_default_menu()
        {
            Menu *menu      = new Menu(pDisplay);
            if (menu == NULL)
                return STATUS_NO_MEM;
            vMenu[0]        = menu;
            LSP_STATUS_ASSERT(menu->init());

            handler_id_t id = 0;
            MenuItem *mi    = new MenuItem(pDisplay);
            if (mi == NULL)
                return STATUS_NO_MEM;
            vMenu[1]        = mi;
            LSP_STATUS_ASSERT(mi->init());
            LSP_STATUS_ASSERT(menu->add(mi));
            LSP_STATUS_ASSERT(mi->text()->set("actions.edit.cut"));
            id = mi->slots()->bind(SLOT_SUBMIT, slot_popup_cut_action, self());
            if (id < 0)
                return -id;

            mi              = new MenuItem(pDisplay);
            if (mi == NULL)
                return STATUS_NO_MEM;
            vMenu[2]        = mi;
            LSP_STATUS_ASSERT(mi->init());
            LSP_STATUS_ASSERT(menu->add(mi));
            LSP_STATUS_ASSERT(mi->text()->set("actions.edit.copy"));
            id = mi->slots()->bind(SLOT_SUBMIT, slot_popup_copy_action, self());
            if (id < 0)
                return -id;

            mi              = new MenuItem(pDisplay);
            if (mi == NULL)
                return STATUS_NO_MEM;
            vMenu[3]        = mi;
            LSP_STATUS_ASSERT(mi->init());
            LSP_STATUS_ASSERT(menu->add(mi));
            LSP_STATUS_ASSERT(mi->text()->set("actions.edit.paste"));
            id = mi->slots()->bind(SLOT_SUBMIT, slot_popup_paste_action, self());
            if (id < 0)
                return -id;

            return STATUS_OK;
        }

        void Edit::property_changed(Property *prop)
        {
            Widget::property_changed(prop);
            if (sVisibility.is(prop))
                sCursor.set_visibility(sVisibility.get() && has_focus());

            // Self properties
            if (sSelection.is(prop))
                query_draw();

            if (sText.is(prop))
            {
                LSPString *text = sText.formatted();
                sSelection.set_limit(text->length());
                sCursor.move(0);
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
            ssize_t radius  = (sBorderRadius.get() > 0) ? lsp_max(1.0f, sBorderRadius.get() * scaling) : 0;
            ssize_t border  = (sBorderSize.get() > 0) ? lsp_max(1.0f, sBorderSize.get() * scaling) : 0;
            if (border > 0)
                border         += (sBorderGapSize.get() > 0) ? lsp_max(1.0f, sBorderGapSize.get()) : 0;

            size_t extra    = lsp_max(radius, border);

            r->nMinWidth    = extra*2;
            r->nMinHeight   = extra*2;

            // Compute text parameters
            ssize_t rgap    = radius - lsp_max(0.0f, truncf(M_SQRT1_2 * (radius - border)));
            rgap            = lsp_max(rgap, border);

            // Estimate sizes
            ws::font_parameters_t fp;
            sFont.get_parameters(pDisplay, scaling, &fp);
            r->nMinHeight   = lsp_max(r->nMinHeight, rgap*2 + fp.Height);
            r->nMinWidth   += lsp_max(1.0f, scaling); // Additional place for cursor

            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;
            r->nPreWidth    = -1;
            r->nPreHeight   = -1;

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
                border             += (sBorderGapSize.get() > 0) ? lsp_max(1.0f, sBorderGapSize.get() * scaling) : 0;

            border              = lsp_max(border, radius - truncf(M_SQRT1_2 * (radius - border)));

            sTextArea.nLeft     = r->nLeft  + border;
            sTextArea.nTop      = r->nTop   + border;
            sTextArea.nWidth    = r->nWidth - border*2;
            sTextArea.nHeight   = r->nHeight- border*2;
        }

        status_t Edit::timer_handler(ws::timestamp_t sched, ws::timestamp_t time, void *arg)
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

            LSPString *text = sText.formatted();
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
                LSPString *text = sText.formatted();
                if (text != NULL)
                {
                    ssize_t first = sSelection.starting(), last = sSelection.ending();
                    status_t result = src->set_text(text, first, last);
                    if (result == STATUS_OK)
                        pDisplay->set_clipboard(bufid, src);
                }

                src->release();
            }
        }

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
            size_t cursize  = lsp_max(1.0f, scaling);

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

                ssize_t gap     = (sBorderGapSize.get() > 0) ? lsp_max(1.0f, sBorderGapSize.get() * scaling) : 0;
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

            // Draw main background
            color.copy(sColor);
            color.scale_lightness(lightness);
            s->fill_round_rect(color, SURFMASK_ALL_CORNER, radius, &xr);

            // Draw text
            xr.nLeft    = sTextArea.nLeft  - sSize.nLeft;
            xr.nTop     = sTextArea.nTop   - sSize.nTop;
            xr.nWidth   = sTextArea.nWidth;
            xr.nHeight  = sTextArea.nHeight;

            s->clip_begin(&xr);
            xr.nWidth      -= cursize; // leave some place for cursor

            // Obtain text parameters
            LSPString *text = sText.formatted();
            size_t cpos     = lsp_limit(sCursor.location(), 0, ssize_t(text->length()));

            sFont.get_parameters(s, scaling, &fp);
            sFont.get_text_parameters(s, &tp, scaling, text, 0, cpos);

            ssize_t textw   = tp.XAdvance;
            if (sCursor.visible() && sCursor.replacing() && (cpos >= text->length()))
            {
                sFont.get_text_parameters(s, &tp, scaling, "_");
                xr.nWidth  -= tp.Width;
            }

            // Adjust the proper text position
            ssize_t cleft   = sTextPos + textw;
            if (sCursor.visible())
            {
                // Make cursor visible within the range of edit
                if (cleft < 0)
                {
                    sTextPos    = -textw;
                    cleft       = 0;
                }
                else if (cleft >= ssize_t(xr.nWidth))
                {
                    sTextPos    = xr.nWidth - textw;
                    cleft       = xr.nWidth;
                }
                else if ((sTextPos < 0) && (cpos >= text->length()))
                {
                    sTextPos   += xr.nWidth - cleft;
                    cleft       = xr.nWidth;
                }
            }

            if (sTextPos > 0)
            {
                cleft          -= sTextPos;
                sTextPos        = 0;
            }

            xr.nTop         = xr.nTop + (xr.nHeight - fp.Height)*0.5f;
            xr.nHeight      = fp.Height;

            // Draw the text
            if ((sSelection.valid()) && (!sSelection.is_empty()))
            {
                ssize_t first   = sSelection.starting();
                ssize_t last    = sSelection.ending();
                ssize_t xpos    = xr.nLeft + sTextPos;

                lsp::Color scolor(sSelectionColor);
                lsp::Color stcolor(sTextSelectedColor);
                color.copy(sTextColor);
                color.scale_lightness(lightness);
                scolor.scale_lightness(lightness);
                stcolor.scale_lightness(lightness);

                ssize_t xshift  = (sSelection.reverted() && sCursor.inserting()) ? cursize : 0;

                if (first > 0)
                {
                    sFont.get_text_parameters(s, &tp, scaling, text, 0, first);
                    sFont.draw(s, color, xpos, xr.nTop + fp.Ascent, scaling, text, 0, first);
                    xpos           += tp.XAdvance;
                }

                sFont.get_text_parameters(s, &tp, scaling, text, first, last);
                s->fill_rect(scolor, xpos + xshift, xr.nTop, tp.XAdvance, xr.nHeight);
                sFont.draw(s, stcolor, xpos, xr.nTop + fp.Ascent, scaling, text, first, last);
                xpos           += /*tp.XBearing + */ tp.XAdvance;

                if (last < ssize_t(text->length()))
                {
                    sFont.get_text_parameters(s, &tp, scaling, text, last);
                    sFont.draw(s, color, xpos, xr.nTop + fp.Ascent, scaling, text, last);
                    xpos           += /*tp.XBearing + */ tp.XAdvance;
                }
            }
            else
            {
                color.copy(sTextColor);
                color.scale_lightness(lightness);

                sFont.draw(s, color, xr.nLeft + sTextPos, xr.nTop + fp.Ascent, scaling, text);
            }

            xr.nLeft       += cleft;

            // Draw cursor if required
            if (sCursor.visible() && sCursor.shining())
            {
                color.copy(sCursorColor);
                color.scale_lightness(lightness);

                if (sCursor.inserting())
                    s->fill_rect(color, xr.nLeft, xr.nTop, cursize, xr.nHeight);
                else // replacing
                {
                    if (cpos >= text->length())
                    {
                        sFont.get_text_parameters(s, &tp, scaling, "_");
                        s->fill_rect(color, xr.nLeft, xr.nTop, tp.Width, xr.nHeight);
                    }
                    else
                    {
                        // Draw background
                        lsp::Color bcolor(sColor);
                        bcolor.scale_lightness(lightness);

                        sFont.get_text_parameters(s, &tp, scaling, text, sCursor.position(), sCursor.position() + 1);
                        ssize_t xw = (tp.XAdvance > tp.Width) ? tp.XAdvance : tp.Width + 1;
                        s->fill_rect(color, xr.nLeft + tp.XBearing - 1, xr.nTop, xw, xr.nHeight);

                        // Draw letter
                        sFont.draw(s, bcolor, xr.nLeft, xr.nTop + fp.Ascent, scaling, text, sCursor.position(), sCursor.position() + 1);
                    }
                }
            }

            s->clip_end();
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
                ssize_t first = mouse_to_cursor_pos(e->nLeft, e->nTop, false);
                if (first >= 0)
                {
                    sSelection.set(first);
                    sCursor.set_position(first);
                }
            }

            return STATUS_OK;
        }

        ssize_t Edit::mouse_to_cursor_pos(ssize_t x, ssize_t y, bool range)
        {
            x                  -= sTextArea.nLeft;
            if ((range) && ((x < 0) || (x >= sTextArea.nWidth)))
                return -1;

            LSPString *text     = sText.formatted();
            if (text == NULL)
                return -1;

            ssize_t tpos        = sTextPos;
            float scaling       = lsp_max(0.0f, sScaling.get());

//            lsp_trace("x=%d", int(x));

            ws::text_parameters_t tp;
            if (sFont.get_text_parameters(pDisplay, &tp, scaling, text))
            {
                if (x > (tpos + tp.XAdvance))
                    return text->length();
            }

            ssize_t left = 0, right = text->length();
            while ((right - left) > 1)
            {
                ssize_t middle = (left + right) >> 1;
                if (!sFont.get_text_parameters(pDisplay, &tp, scaling, text, left, middle))
                    return -1;

                ssize_t tx      = tpos + tp.XAdvance;
//                lsp_trace("x=%d, tpos=%d, xadvance=%d, tx=%d, left=%d, right=%d, middle=%d",
//                        int(x), int(tpos), int(tp.XAdvance), int(tx),
//                        int(left), int(right), int(middle)
//                    );

                if (tx > x)
                    right       = middle;
                else if (tx < x)
                {
                    left        = middle;
                    tpos       += tp.XAdvance;
                }
                else // tx == x
                    return middle;
            }

            // Position may be somewhere in the middle of character, determine the actual position
            if (sFont.get_text_parameters(pDisplay, &tp, scaling, text, left, right))
            {
                float tx        = tpos + tp.XAdvance * 0.75f;
//                lsp_trace("x=%d, tpos=%d, xadvance=%d, tx=%d, left=%d, right=%d",
//                        int(x), int(tpos), int(tp.XAdvance), int(tx),
//                        int(left), int(right)
//                    );

                return (tx < x) ? right : left;
            }

            return left;
        }

        status_t Edit::on_mouse_dbl_click(const ws::event_t *e)
        {
            if (e->nCode == ws::MCB_LEFT)
            {
                const LSPString *text = sText.formatted();
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
                sCursor.set_position(sSelection.ending());
                update_clipboard(ws::CBUF_PRIMARY);
            }
            return STATUS_OK;
        }

        status_t Edit::on_mouse_up(const ws::event_t *e)
        {
            lsp_trace("mouse up");
            if ((nMBState == (1 << ws::MCB_RIGHT)) && (e->nCode == ws::MCB_RIGHT))
            {
                Menu *popup = sPopup.get();
                if (popup != NULL)
                {
                    sSlots.execute(SLOT_BEFORE_POPUP, popup, self());
                    popup->show();
                    sSlots.execute(SLOT_POPUP, popup, self());
                }
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
            LSPString *text = sText.formatted();
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
                    sSelection.set_limit(text->length());
                    sSelection.set(pos);
                    ++changes;
                }
            }

            // Invalidate text after changes
            if (changes > 0)
            {
                sSelection.set_limit(text->length());
                sText.invalidate();
                sSlots.execute(SLOT_CHANGE, this);
            }
        }

        status_t Edit::on_key_down(const ws::event_t *e)
        {
            LSPString s;
            s.set(lsp_wchar_t(e->nCode));
            lsp_trace("Key code pressed=%x, symbol received=%s, modifiers=%x", int(e->nCode), s.get_native(), int(e->nState));

            ws::code_t key = KeyboardHandler::translate_keypad(e->nCode);
            LSPString *text = sText.formatted();

            if (ws::is_character_key(key))
            {
                if (!(e->nState & (ws::MCF_CONTROL | ws::MCF_ALT)))
                {
                    if (sSelection.valid() && sSelection.length() > 0)
                    {
                        text->remove(sSelection.starting(), sSelection.ending());
                        sCursor.set_location(sSelection.starting());
                        sSelection.clear();
                        update_clipboard(ws::CBUF_PRIMARY);
                    }

                    ssize_t loc = lsp_limit(sCursor.location(), 0, ssize_t(text->length()));
                    if (sCursor.replacing())
                    {
                        if (loc < ssize_t(text->length()))
                            text->set(loc, lsp_wchar_t(key));
                        else
                            text->append(lsp_wchar_t(key));
                    }
                    else
                        text->insert(sCursor.location(), lsp_wchar_t(key));

                    sCursor.move(1);
                    sText.invalidate();
                    sSlots.execute(SLOT_CHANGE, this);

                    return STATUS_OK;
                }
                else if (e->nState & ws::MCF_CONTROL)
                {
                    switch (e->nCode)
                    {
                        case 'C': case 'c':
                            if (sSelection.valid() && sSelection.non_empty())
                                update_clipboard(ws::CBUF_CLIPBOARD);
                            break;
                        case 'V': case 'v':
                            request_clipboard(ws::CBUF_CLIPBOARD);
                            break;
                        case 'A': case 'a':
                            sSelection.set(0, text->length());
                            update_clipboard(ws::CBUF_PRIMARY);
                            break;
                        case 'X': case 'x':
                            cut_data(ws::CBUF_CLIPBOARD);
                            break;
                    }
                    return STATUS_OK;
                }
            }

            if (e->nState & ws::MCF_SHIFT)
            {
                if (sSelection.first() < 0)
                    sSelection.set_first(sCursor.location());
            }

            switch (key)
            {
                case ws::WSK_HOME:
                    if (e->nState & ws::MCF_SHIFT)
                        sSelection.set_last(0);
                    else
                        sSelection.clear();
                    sCursor.set_location(0);
                    break;
                case ws::WSK_END:
                    if (e->nState & ws::MCF_SHIFT)
                        sSelection.set_last(text->length());
                    else
                        sSelection.clear();
                    sCursor.set_location(text->length());
                    break;
                case ws::WSK_LEFT:
                    sCursor.move(-1);
                    if (e->nState & ws::MCF_SHIFT)
                        sSelection.set_last(sCursor.location());
                    else
                        sSelection.clear();
                    break;
                case ws::WSK_RIGHT:
                    sCursor.move(1);
                    if (e->nState & ws::MCF_SHIFT)
                        sSelection.set_last(sCursor.location());
                    else
                        sSelection.clear();
                    break;
                case ws::WSK_BACKSPACE:
                {
                    if (sSelection.valid() && sSelection.length() > 0)
                    {
                        text->remove(sSelection.starting(), sSelection.ending());
                        sCursor.set_location(sSelection.starting());
                        sSelection.clear();
                    }
                    else
                    {
                        ssize_t pos = lsp_limit(sCursor.location(), 0, ssize_t(text->length()));
                        if (pos <= 0)
                            break;
                        text->remove(pos-1, pos);
                        sCursor.set_location(pos-1);
                    }

                    sText.invalidate();
                    sSlots.execute(SLOT_CHANGE, this);
                    break;
                }
                case ws::WSK_DELETE:
                {
                    if (sSelection.valid() && sSelection.length() > 0)
                    {
                        text->remove(sSelection.starting(), sSelection.ending());
                        sCursor.set_location(sSelection.starting());
                        sSelection.clear();
                    }
                    else
                    {
                        ssize_t pos = lsp_limit(sCursor.location(), 0, ssize_t(text->length()));
                        if (pos >= ssize_t(text->length()))
                            break;
                        text->remove(pos, pos + 1);
                        sCursor.set_location(pos);
                    }

                    sText.invalidate();
                    sSlots.execute(SLOT_CHANGE, this);
                    break;
                }
                case ws::WSK_INSERT:
                {
                    size_t flags = (e->nState & (ws::MCF_CONTROL | ws::MCF_SHIFT | ws::MCF_ALT));
                    if (flags == ws::MCF_SHIFT)
                        request_clipboard(ws::CBUF_CLIPBOARD);
                    else if (flags == ws::MCF_CONTROL)
                    {
                        if (sSelection.valid() && sSelection.non_empty())
                            update_clipboard(ws::CBUF_CLIPBOARD);
                    }
                    else
                        sCursor.toggle_mode();
                    break;
                }
            }

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
            if (sSelection.valid() && sSelection.non_empty())
            {
                update_clipboard(bufid);
                LSPString *text = sText.formatted();
                text->remove(sSelection.starting(), sSelection.ending());
                sCursor.set_location(sSelection.starting());
                sSelection.clear();
                sText.invalidate(); // Will update limit of selection
                sSlots.execute(SLOT_CHANGE, this);
            }
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

        status_t Edit::on_before_popup(Menu *menu)
        {
            return STATUS_OK;
        }

        status_t Edit::on_popup(Menu *menu)
        {
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

        status_t Edit::slot_on_before_popup(Widget *sender, void *ptr, void *data)
        {
            Edit *_this = widget_ptrcast<Edit>(ptr);
            Menu *_menu = widget_ptrcast<Menu>(sender);
            return (_this != NULL) ? _this->on_before_popup(_menu) : STATUS_BAD_ARGUMENTS;
        }

        status_t Edit::slot_on_popup(Widget *sender, void *ptr, void *data)
        {
            Edit *_this = widget_ptrcast<Edit>(ptr);
            Menu *_menu = widget_ptrcast<Menu>(sender);
            return (_this != NULL) ? _this->on_popup(_menu) : STATUS_BAD_ARGUMENTS;
        }

    } /* namespace tk */
} /* namespace lsp */
