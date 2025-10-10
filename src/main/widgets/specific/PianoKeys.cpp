/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 25 сент. 2025 г.
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
#include <lsp-plug.in/tk/tk.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        // Style definition
        namespace style
        {
            // Fraction style
            LSP_TK_STYLE_IMPL_BEGIN(PianoKeys, Widget)
                // Bind key colors
                style::PianoKeyColors *c = &vKeyColors[style::PIANOKEY_NORMAL];
                c->sWhiteColor.bind("white.color", this);
                c->sBlackColor.bind("black.color", this);

                c = &vKeyColors[style::PIANOKEY_DOWN];
                c->sWhiteColor.bind("white.down.color", this);
                c->sBlackColor.bind("black.down.color", this);

                c = &vKeyColors[style::PIANOKEY_SELECTED];
                c->sWhiteColor.bind("white.selected.color", this);
                c->sBlackColor.bind("black.selected.color", this);

                c = &vKeyColors[style::PIANOKEY_SELECTED | style::PIANOKEY_DOWN];
                c->sWhiteColor.bind("white.selected.down.color", this);
                c->sBlackColor.bind("black.selected.down.color", this);

                c = &vKeyColors[style::PIANOKEY_INACTIVE];
                c->sWhiteColor.bind("white.inactive.color", this);
                c->sBlackColor.bind("black.inactive.color", this);

                c = &vKeyColors[style::PIANOKEY_INACTIVE | style::PIANOKEY_DOWN];
                c->sWhiteColor.bind("white.inactive.down.color", this);
                c->sBlackColor.bind("black.inactive.down.color", this);

                c = &vKeyColors[style::PIANOKEY_INACTIVE | style::PIANOKEY_SELECTED];
                c->sWhiteColor.bind("white.inactive.selected.color", this);
                c->sBlackColor.bind("black.inactive.selected.color", this);

                c = &vKeyColors[style::PIANOKEY_INACTIVE | style::PIANOKEY_SELECTED | style::PIANOKEY_DOWN];
                c->sWhiteColor.bind("white.inactive.selected.down.color", this);
                c->sBlackColor.bind("black.inactive.selected.down.color", this);

                c = &vKeyColors[style::PIANOKEY_HOVER];
                c->sWhiteColor.bind("white.hover.color", this);
                c->sBlackColor.bind("black.hover.color", this);

                c = &vKeyColors[style::PIANOKEY_HOVER | style::PIANOKEY_DOWN];
                c->sWhiteColor.bind("white.down.hover.color", this);
                c->sBlackColor.bind("black.down.hover.color", this);

                c = &vKeyColors[style::PIANOKEY_HOVER | style::PIANOKEY_SELECTED];
                c->sWhiteColor.bind("white.selected.hover.color", this);
                c->sBlackColor.bind("black.selected.hover.color", this);

                c = &vKeyColors[style::PIANOKEY_HOVER | style::PIANOKEY_SELECTED | style::PIANOKEY_DOWN];
                c->sWhiteColor.bind("white.selected.down.hover.color", this);
                c->sBlackColor.bind("black.selected.down.hover.color", this);

                c = &vKeyColors[style::PIANOKEY_HOVER | style::PIANOKEY_INACTIVE];
                c->sWhiteColor.bind("white.inactive.hover.color", this);
                c->sBlackColor.bind("black.inactive.hover.color", this);

                c = &vKeyColors[style::PIANOKEY_HOVER | style::PIANOKEY_INACTIVE | style::PIANOKEY_DOWN];
                c->sWhiteColor.bind("white.inactive.down.hover.color", this);
                c->sBlackColor.bind("black.inactive.down.hover.color", this);

                c = &vKeyColors[style::PIANOKEY_HOVER | style::PIANOKEY_INACTIVE | style::PIANOKEY_SELECTED];
                c->sWhiteColor.bind("white.inactive.selected.hover.color", this);
                c->sBlackColor.bind("black.inactive.selected.hover.color", this);

                c = &vKeyColors[style::PIANOKEY_HOVER | style::PIANOKEY_INACTIVE | style::PIANOKEY_SELECTED | style::PIANOKEY_DOWN];
                c->sWhiteColor.bind("white.inactive.selected.down.hover.color", this);
                c->sBlackColor.bind("black.inactive.selected.down.hover.color", this);

                // Bind state colors
                style::PianoColors *pc = &vColors[style::PIANO_NORMAL];
                pc->sBorderColor.bind("border.color", this);
                pc->sSplitColor.bind("split.color", this);
                pc->sOctaveSplitColor.bind("octave.split.color", this);

                pc = &vColors[style::PIANO_INACTIVE];
                pc->sBorderColor.bind("inactive.border.color", this);
                pc->sSplitColor.bind("inactive.split.color", this);
                pc->sOctaveSplitColor.bind("inactive.octave.split.color", this);

                // Bind other properties
                sBorder.bind("border", this);
                sSplitSize.bind("split.size", this);
                sMinNote.bind("note.min", this);
                sMaxNote.bind("note.max", this);
                sAngle.bind("angle", this);
                sConstraints.bind("size.contstraints", this);
                sKeyAspect.bind("key.aspect", this);
                sNatural.bind("natural", this);
                sPressed.bind("note.pressed", this);
                sMousePressed.bind("note.mouse.pressed", this);
                sSelectionStart.bind("note.selection.start", this);
                sSelectionEnd.bind("note.selection.end", this);
                sEditable.bind("editable", this);
                sSelectable.bind("selectable", this);
                sClearSelection.bind("note.selection.clear", this);

                // Init values
                for (size_t i=0; i<style::PIANOKEY_TOTAL; ++i)
                {
                    c = &vKeyColors[i];
                    if (i & style::PIANOKEY_INACTIVE)
                    {
                        c->sWhiteColor.set_rgb24(0x444444);
                        c->sBlackColor.set_rgb24(0xcccccc);
                    }
                    else
                    {
                        c->sWhiteColor.set_rgb24(0xffffff);
                        c->sBlackColor.set_rgb24(0x000000);
                    }
                }

                pc = &vColors[style::PIANO_NORMAL];
                pc->sBorderColor.set_rgb24(0x000000);
                pc->sSplitColor.set_rgb24(0xcccccc);
                pc->sOctaveSplitColor.set_rgb24(0x888888);

                pc = &vColors[style::PIANO_INACTIVE];
                pc->sBorderColor.set_rgb24(0x444444);
                pc->sSplitColor.set_rgb24(0x888888);
                pc->sOctaveSplitColor.set_rgb24(0x444444);

                sBorder.set_all(1);
                sSplitSize.set(1);
                sMinNote.set(36);               // C2
                sMaxNote.set(36 + 4*12 - 1);    // H5
                sAngle.set(0);
                sConstraints.set_min_height(32);
                sKeyAspect.set(0.66f);
                sMousePressed.set(-1);
                sSelectionStart.set(-1);
                sSelectionEnd.set(-1);
                sEditable.set(false);
                sSelectable.set(false);
                sClearSelection.set(true);

            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(PianoKeys, "PianoKeys", "root");

            void PianoKeyColors::listener(tk::prop::Listener *listener)
            {
                sWhiteColor.listener(listener);
                sBlackColor.listener(listener);
            }

            bool PianoKeyColors::property_changed(Property *prop)
            {
                return prop->one_of(sWhiteColor, sBlackColor);
            }

            void PianoColors::listener(tk::prop::Listener *listener)
            {
                sBorderColor.listener(listener);
                sSplitColor.listener(listener);
                sOctaveSplitColor.listener(listener);
            }

            bool PianoColors::property_changed(Property *prop)
            {
                return prop->one_of(sBorderColor, sSplitColor, sOctaveSplitColor);
            }
        }

        //-----------------------------------------------------------------------------
        // PianoKeys implementation
        enum key_type_t
        {
            KT_BLACK = 0,
            KT_LEFT = 1,
            KT_RIGHT = 2,
            KT_MIDDLE = 3
        };

        // Types of keys on a keypad within an octave
        static constexpr uint32_t piano_key_types =
            (KT_LEFT << 0) |            // C
            (KT_BLACK << 2) |           // C#
            (KT_MIDDLE << 4) |          // D
            (KT_BLACK << 6) |           // D#
            (KT_RIGHT << 8) |           // E
            (KT_LEFT << 10) |           // F
            (KT_BLACK << 12) |          // F#
            (KT_MIDDLE << 14) |         // G
            (KT_BLACK << 16) |          // G#
            (KT_MIDDLE << 18) |         // A
            (KT_BLACK << 20) |          // A#
            (KT_RIGHT << 22);           // B

        inline size_t key_type(size_t note)
        {
            const size_t shift = (note % 12) << 1;
            return (piano_key_types >> shift) & 0x3;
        }

        inline bool is_black_key(size_t note)
        {
            return key_type(note) == KT_BLACK;
        }

        inline size_t key_width(size_t note, bool natural)
        {
            switch (key_type(note))
            {
                case KT_LEFT:
                    return 3;
                case KT_BLACK:
                    return 2;
                case KT_RIGHT:
                    return 3;
                default:
                case KT_MIDDLE:
                    break;
            }
            return (natural) ? 3 : 4;
        }

        inline size_t key_advance(size_t note, bool natural)
        {
            switch (key_type(note))
            {
                case KT_LEFT:
                    return 2;
                case KT_BLACK:
                    return 1;
                case KT_RIGHT:
                    return 3;
                default:
                case KT_MIDDLE:
                    break;
            }
            return (natural) ? 2 : 3;
        }


        const w_class_t PianoKeys::metadata             = { "PianoKeys", &Widget::metadata };

        PianoKeys::PianoKeys(Display *dpy):
            tk::Widget(dpy),
            sBorder(&sProperties),
            sSplitSize(&sProperties),
            sMinNote(&sProperties),
            sMaxNote(&sProperties),
            sAngle(&sProperties),
            sConstraints(&sProperties),
            sKeyAspect(&sProperties),
            sNatural(&sProperties),
            sPressed(&sProperties),
            sMousePressed(&sProperties),
            sSelectionStart(&sProperties),
            sSelectionEnd(&sProperties),
            sEditable(&sProperties),
            sSelectable(&sProperties),
            sClearSelection(&sProperties)
        {
            pClass          = &metadata;

            nHoverNote      = -1;
            nMBState        = 0;
            enWorkMode      = WMODE_OFF;

            for (size_t i=0; i<2; ++i)
            {
                key_t *sp       = &vSplit[i];

                sp->fLeft       = 0.0f;
                sp->fTop        = 0.0f;
                sp->fWidth      = 0.0f;
                sp->fHeight     = 0.0f;
                sp->nKey        = 0;
            }

            for (size_t i=0; i<style::PIANOKEY_TOTAL; ++i)
                vKeyColors[i].listener(&sProperties);
            for (size_t i=0; i<style::PIANO_TOTAL; ++i)
                vColors[i].listener(&sProperties);
        }

        PianoKeys::~PianoKeys()
        {
            nFlags     |= FINALIZED;
            do_destroy();
        }

        void PianoKeys::do_destroy()
        {
        }

        void PianoKeys::destroy()
        {
            do_destroy();
            Widget::destroy();
        }

        status_t PianoKeys::init()
        {
            status_t res = Widget::init();
            if (res != STATUS_OK)
                return res;

            // Bind key colors
            style::PianoKeyColors *c = &vKeyColors[style::PIANOKEY_NORMAL];
            c->sWhiteColor.bind("white.color", &sStyle);
            c->sBlackColor.bind("black.color", &sStyle);

            c = &vKeyColors[style::PIANOKEY_DOWN];
            c->sWhiteColor.bind("white.down.color", &sStyle);
            c->sBlackColor.bind("black.down.color", &sStyle);

            c = &vKeyColors[style::PIANOKEY_SELECTED];
            c->sWhiteColor.bind("white.selected.color", &sStyle);
            c->sBlackColor.bind("black.selected.color", &sStyle);

            c = &vKeyColors[style::PIANOKEY_SELECTED | style::PIANOKEY_DOWN];
            c->sWhiteColor.bind("white.selected.down.color", &sStyle);
            c->sBlackColor.bind("black.selected.down.color", &sStyle);

            c = &vKeyColors[style::PIANOKEY_INACTIVE];
            c->sWhiteColor.bind("white.inactive.color", &sStyle);
            c->sBlackColor.bind("black.inactive.color", &sStyle);

            c = &vKeyColors[style::PIANOKEY_INACTIVE | style::PIANOKEY_DOWN];
            c->sWhiteColor.bind("white.inactive.down.color", &sStyle);
            c->sBlackColor.bind("black.inactive.down.color", &sStyle);

            c = &vKeyColors[style::PIANOKEY_INACTIVE | style::PIANOKEY_SELECTED];
            c->sWhiteColor.bind("white.inactive.selected.color", &sStyle);
            c->sBlackColor.bind("black.inactive.selected.color", &sStyle);

            c = &vKeyColors[style::PIANOKEY_INACTIVE | style::PIANOKEY_SELECTED | style::PIANOKEY_DOWN];
            c->sWhiteColor.bind("white.inactive.selected.down.color", &sStyle);
            c->sBlackColor.bind("black.inactive.selected.down.color", &sStyle);

            c = &vKeyColors[style::PIANOKEY_HOVER];
            c->sWhiteColor.bind("white.hover.color", &sStyle);
            c->sBlackColor.bind("black.hover.color", &sStyle);

            c = &vKeyColors[style::PIANOKEY_HOVER | style::PIANOKEY_DOWN];
            c->sWhiteColor.bind("white.down.hover.color", &sStyle);
            c->sBlackColor.bind("black.down.hover.color", &sStyle);

            c = &vKeyColors[style::PIANOKEY_HOVER | style::PIANOKEY_SELECTED];
            c->sWhiteColor.bind("white.selected.hover.color", &sStyle);
            c->sBlackColor.bind("black.selected.hover.color", &sStyle);

            c = &vKeyColors[style::PIANOKEY_HOVER | style::PIANOKEY_SELECTED | style::PIANOKEY_DOWN];
            c->sWhiteColor.bind("white.selected.down.hover.color", &sStyle);
            c->sBlackColor.bind("black.selected.down.hover.color", &sStyle);

            c = &vKeyColors[style::PIANOKEY_HOVER | style::PIANOKEY_INACTIVE];
            c->sWhiteColor.bind("white.inactive.hover.color", &sStyle);
            c->sBlackColor.bind("black.inactive.hover.color", &sStyle);

            c = &vKeyColors[style::PIANOKEY_HOVER | style::PIANOKEY_INACTIVE | style::PIANOKEY_DOWN];
            c->sWhiteColor.bind("white.inactive.down.hover.color", &sStyle);
            c->sBlackColor.bind("black.inactive.down.hover.color", &sStyle);

            c = &vKeyColors[style::PIANOKEY_HOVER | style::PIANOKEY_INACTIVE | style::PIANOKEY_SELECTED];
            c->sWhiteColor.bind("white.inactive.selected.hover.color", &sStyle);
            c->sBlackColor.bind("black.inactive.selected.hover.color", &sStyle);

            c = &vKeyColors[style::PIANOKEY_HOVER | style::PIANOKEY_INACTIVE | style::PIANOKEY_SELECTED | style::PIANOKEY_DOWN];
            c->sWhiteColor.bind("white.inactive.selected.down.hover.color", &sStyle);
            c->sBlackColor.bind("black.inactive.selected.down.hover.color", &sStyle);

            // Bind state colors
            style::PianoColors *pc = &vColors[style::PIANO_NORMAL];
            pc->sBorderColor.bind("border.color", &sStyle);
            pc->sSplitColor.bind("split.color", &sStyle);
            pc->sOctaveSplitColor.bind("octave.split.color", &sStyle);

            pc = &vColors[style::PIANO_INACTIVE];
            pc->sBorderColor.bind("inactive.border.color", &sStyle);
            pc->sSplitColor.bind("inactive.split.color", &sStyle);
            pc->sOctaveSplitColor.bind("inactive.octave.split.color", &sStyle);

            // Bind other properties
            sBorder.bind("border", &sStyle);
            sSplitSize.bind("split.size", &sStyle);
            sMinNote.bind("note.min", &sStyle);
            sMaxNote.bind("note.max", &sStyle);
            sAngle.bind("angle", &sStyle);
            sConstraints.bind("size.contstraints", &sStyle);
            sKeyAspect.bind("key.aspect", &sStyle);
            sNatural.bind("natural", &sStyle);
            sPressed.bind("note.pressed", &sStyle);
            sMousePressed.bind("note.mouse.pressed", &sStyle);
            sSelectionStart.bind("note.selection.start", &sStyle);
            sSelectionEnd.bind("note.selection.end", &sStyle);
            sEditable.bind("editable", &sStyle);
            sSelectable.bind("selectable", &sStyle);
            sClearSelection.bind("note.selection.clear", &sStyle);

            // Bind slots
            handler_id_t id;
            id = sSlots.add(SLOT_SUBMIT, slot_on_submit, self());
            if (id >= 0) id = sSlots.add(SLOT_CHANGE, slot_on_change, self());
            if (id < 0)
                return -id;

            return STATUS_OK;
        }

        void PianoKeys::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            // Check that key color has changed
            const size_t pk_mask = (sActive.get()) ? 0 : style::PIANOKEY_INACTIVE;
            for (size_t i=0; i<PK_TOTAL; ++i)
            {
                if ((i & style::PIANOKEY_INACTIVE) != pk_mask)
                    continue;

                if (vKeyColors[i].property_changed(prop))
                {
                    query_draw();
                    break;
                }
            }

            // Check that keyboard color has changed
            style::PianoColors *c = get_piano_colors();
            if (c->property_changed(prop))
                query_draw();

            if (prop->one_of(sBorder, sSplitSize, sMinNote, sMaxNote, sAngle, sConstraints))
                query_resize();
            if (prop->one_of(sKeyAspect, sNatural))
                query_resize();
            if (prop->one_of(sSelectionStart, sSelectionEnd, sMousePressed, sPressed))
                query_draw();
        }

        void PianoKeys::size_request(ws::size_limit_t *r)
        {
            const float scaling     = lsp_max(0.0f, sScaling.get());
            const ssize_t angle     = sAngle.get() & 0x3;
            const float split       = scaling * lsp_max(0.0f, sSplitSize.get());
            const float key_unit    = lsp_max(split * 0.5f, 1.0f);
            const bool natural      = sNatural.get();

            // Compute the keyboard layout
            layout_t layout;
            compute_layout(&layout, natural);

            // Now compute the size
            size_t height           = (layout.nWhite > 0) ? 4 : (layout.nBlack > 0) ? 2 : 0;
            const size_t min_width  = floorf(layout.nLength * key_unit);
            const size_t min_height = floorf(height * key_unit);

            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;
            r->nPreWidth    = -1;
            r->nPreHeight   = -1;
            r->nMinWidth    = min_width;
            r->nMinHeight   = min_height;
            if (angle & 1)
            {
                SizeConstraints::transpose(r);
                sBorder.add(r, scaling);
                SizeConstraints::transpose(r);
            }
            else
                sBorder.add(r, scaling);

            sConstraints.apply(r, scaling);

            // Swap role of width and height for vertical orientation
            if (angle & 1)
                SizeConstraints::transpose(r);
        }

        void PianoKeys::realize(const ws::rectangle_t *r)
        {
            Widget::realize(r);

            const float scaling     = lsp_max(0.0f, sScaling.get());
            const ssize_t angle     = sAngle.get() & 0x3;
            const float aspect      = lsp_limit(sKeyAspect.get(), 0.0f, 1.0f);
            const bool natural      = sNatural.get();

            ws::rectangle_t kr;
            sBorder.enter(&kr, r, scaling);

            // Compute the keyboard layout
            layout_t layout;
            compute_layout(&layout, natural);

            const float step = float((angle & 1) ? kr.nHeight : kr.nWidth) / float(layout.nLength);

            // Reset state of splits
            for (size_t i=0; i<2; ++i)
            {
                key_t *sp = &vSplit[i];
                sp->fLeft       = 0.0f;
                sp->fTop        = 0.0f;
                sp->fWidth      = 0.0f;
                sp->fHeight     = 0.0f;
            }

            // Allocate array of keys
            vKeys.clear();
            key_t *k = vKeys.append_n(layout.nLast - layout.nFirst + 1);
            if (k == NULL)
                return;

            // Fill array of keys
            switch (angle)
            {
                case 0:
                default:
                {
                    size_t x                = 0;
                    const float left        = kr.nLeft;
                    const float top         = kr.nTop;
                    const float w_length    = kr.nHeight;
                    const float b_length    = w_length * aspect;

                    for (ssize_t key = layout.nFirst; key <= layout.nLast; ++key, ++k)
                    {
                        const float k_width     = key_width(key, natural) * step;
                        const bool is_black     = is_black_key(key);

                        k->fWidth               = k_width;
                        k->fHeight              = (is_black) ? b_length : w_length;
                        k->fLeft                = left + x * step;
                        k->fTop                 = top;
                        k->nKey                 = key;

                        x                      += key_advance(key, natural);
                    }

                    // Analyze first key
                    k = vKeys.first();
                    if (is_black_key(k->nKey)) // Current key is black?
                    {
                        vSplit[0].fWidth    = step;
                        vSplit[0].fHeight   = w_length;
                        vSplit[0].fLeft     = left;
                        vSplit[0].fTop      = top;
                    }
                    else if (is_black_key(k->nKey + 11)) // Previous key is black?
                    {
                        vSplit[0].fWidth    = step;
                        vSplit[0].fHeight   = b_length;
                        vSplit[0].fLeft     = left;
                        vSplit[0].fTop      = top;
                    }

                    // Analyze last key
                    k = vKeys.last();
                    if (is_black_key(k->nKey)) // Current key is black?
                    {
                        vSplit[1].fWidth    = step;
                        vSplit[1].fHeight   = w_length;
                        vSplit[1].fLeft     = k->fLeft + k->fWidth - step;
                        vSplit[1].fTop      = top;
                    }
                    else if (is_black_key(k->nKey + 1)) // Next key is black?
                    {
                        vSplit[1].fWidth    = step;
                        vSplit[1].fHeight   = b_length;
                        vSplit[1].fLeft     = k->fLeft + k->fWidth - step;
                        vSplit[1].fTop      = top;
                    }

                    break;
                }

                case 1:
                {
                    size_t y                = 0;
                    const float left        = kr.nLeft;
                    const float bottom      = kr.nTop + kr.nHeight;
                    const float w_length    = kr.nWidth;
                    const float b_length    = w_length * aspect;

                    for (ssize_t key = layout.nFirst; key <= layout.nLast; ++key, ++k)
                    {
                        const float k_width     = key_width(key, natural) * step;
                        const bool is_black     = is_black_key(key);

                        k->fWidth               = (is_black) ? b_length : w_length;
                        k->fHeight              = k_width;
                        k->fLeft                = left;
                        k->fTop                 = bottom - y * step - k->fHeight;
                        k->nKey                 = key;

                        y                      += key_advance(key, natural);
                    }

                    // Analyze first key
                    k = vKeys.first();
                    if (is_black_key(k->nKey)) // Current key is black?
                    {
                        vSplit[0].fWidth    = w_length;
                        vSplit[0].fHeight   = step;
                        vSplit[0].fLeft     = left;
                        vSplit[0].fTop      = k->fTop + k->fHeight - step;
                    }
                    else if (is_black_key(k->nKey + 11)) // Previous key is black?
                    {
                        vSplit[0].fWidth    = b_length;
                        vSplit[0].fHeight   = step;
                        vSplit[0].fLeft     = left;
                        vSplit[0].fTop      = k->fTop + k->fHeight - step;
                    }

                    // Analyze last key
                    k = vKeys.last();
                    if (is_black_key(k->nKey)) // Current key is black?
                    {
                        vSplit[1].fWidth    = w_length;
                        vSplit[1].fHeight   = step;
                        vSplit[1].fLeft     = left;
                        vSplit[1].fTop      = k->fTop;
                    }
                    else if (is_black_key(k->nKey + 1)) // Next key is black?
                    {
                        vSplit[1].fWidth    = b_length;
                        vSplit[1].fHeight   = step;
                        vSplit[1].fLeft     = left;
                        vSplit[1].fTop      = k->fTop;
                    }

                    break;
                }

                case 2:
                {
                    size_t x                = 0;
                    const float right       = kr.nLeft + kr.nWidth;
                    const float bottom      = kr.nTop + kr.nHeight;
                    const float w_length    = kr.nHeight;
                    const float b_length    = w_length * aspect;

                    for (ssize_t key = layout.nFirst; key <= layout.nLast; ++key, ++k)
                    {
                        const float k_width     = key_width(key, natural) * step;
                        const bool is_black     = is_black_key(key);

                        k->fWidth               = k_width;
                        k->fHeight              = (is_black) ? b_length : w_length;
                        k->fLeft                = right - x * step - k_width;
                        k->fTop                 = bottom - k->fHeight;
                        k->nKey                 = key;

                        x                      += key_advance(key, natural);
                    }

                    // Analyze first key
                    k = vKeys.first();
                    if (is_black_key(k->nKey)) // Current key is black?
                    {
                        vSplit[0].fWidth    = step;
                        vSplit[0].fHeight   = w_length;
                        vSplit[0].fLeft     = k->fLeft + k->fWidth - step;
                        vSplit[0].fTop      = bottom - w_length;
                    }
                    else if (is_black_key(k->nKey + 11)) // Previous key is black?
                    {
                        vSplit[0].fWidth    = step;
                        vSplit[0].fHeight   = b_length;
                        vSplit[0].fLeft     = k->fLeft + k->fWidth - step;
                        vSplit[0].fTop      = bottom - b_length;
                    }

                    // Analyze last key
                    k = vKeys.last();
                    if (is_black_key(k->nKey)) // Current key is black?
                    {
                        vSplit[1].fWidth    = step;
                        vSplit[1].fHeight   = w_length;
                        vSplit[1].fLeft     = k->fLeft;
                        vSplit[1].fTop      = bottom - w_length;
                    }
                    else if (is_black_key(k->nKey + 1)) // Next key is black?
                    {
                        vSplit[1].fWidth    = step;
                        vSplit[1].fHeight   = b_length;
                        vSplit[1].fLeft     = k->fLeft;
                        vSplit[1].fTop      = bottom - b_length;
                    }

                    break;
                }

                case 3:
                {
                    size_t y                = 0;
                    const float right       = kr.nLeft + kr.nWidth;
                    const float top         = kr.nTop;
                    const float w_length    = kr.nWidth;
                    const float b_length    = w_length * aspect;

                    for (ssize_t key = layout.nFirst; key <= layout.nLast; ++key, ++k)
                    {
                        const float k_width     = key_width(key, natural) * step;
                        const bool is_black     = is_black_key(key);

                        k->fWidth               = (is_black) ? b_length : w_length;
                        k->fHeight              = k_width;
                        k->fLeft                = right - k->fWidth;
                        k->fTop                 = top + y * step;
                        k->nKey                 = key;

                        y                      += key_advance(key, natural);
                    }

                    // Analyze first key
                    k = vKeys.first();
                    if (is_black_key(k->nKey)) // Current key is black?
                    {
                        vSplit[0].fWidth    = w_length;
                        vSplit[0].fHeight   = step;
                        vSplit[0].fLeft     = right - w_length;
                        vSplit[0].fTop      = k->fTop;
                    }
                    else if (is_black_key(k->nKey + 11)) // Previous key is black?
                    {
                        vSplit[0].fWidth    = b_length;
                        vSplit[0].fHeight   = step;
                        vSplit[0].fLeft     = right - b_length;
                        vSplit[0].fTop      = k->fTop;
                    }

                    // Analyze last key
                    k = vKeys.last();
                    if (is_black_key(k->nKey)) // Current key is black?
                    {
                        vSplit[1].fWidth    = w_length;
                        vSplit[1].fHeight   = step;
                        vSplit[1].fLeft     = right - w_length;
                        vSplit[1].fTop      = k->fTop + k->fHeight - step;
                    }
                    else if (is_black_key(k->nKey + 1)) // Next key is black?
                    {
                        vSplit[1].fWidth    = b_length;
                        vSplit[1].fHeight   = step;
                        vSplit[1].fLeft     = right - b_length;
                        vSplit[1].fTop      = k->fTop + k->fHeight - step;
                    }

                    break;
                }
            }
        }

        void PianoKeys::draw_key(ws::ISurface *s, const key_t * key, bool black, ssize_t sel_first, ssize_t sel_last)
        {
            const bool is_black = is_black_key(key->nKey);
            if (is_black != black)
                return;

            const ssize_t code = key->nKey;
            const bool selected = (code >= sel_first) && (code <= sel_last);
            const bool pressed = (sMousePressed.get() == code) || (sPressed.get(code));
            const bool hover = (code == nHoverNote);

            lsp::Color col;
            const style::PianoKeyColors *c = get_key_colors(pressed, selected, hover);

            col.copy((is_black) ? c->sBlackColor : c->sWhiteColor);
            s->fill_rect(col, SURFMASK_NO_CORNER, 0.0f,
                key->fLeft - sSize.nLeft,
                key->fTop - sSize.nTop,
                key->fWidth,
                key->fHeight);
        }

        void PianoKeys::draw_split(ws::ISurface *s, const key_t * key, const lsp::Color & c, const lsp::Color & oc, size_t angle, float split)
        {
            if (is_black_key(key->nKey))
                return;

            const lsp::Color *sc = ((key->nKey % 12) == 11) ? &oc : &c;

            switch (angle)
            {
                case 0:
                default:
                    s->fill_rect(
                        *sc, SURFMASK_NO_CORNER, 0.0f,
                        key->fLeft - sSize.nLeft + key->fWidth - split,
                        key->fTop - sSize.nTop,
                        split,
                        key->fHeight);
                    break;
                case 1:
                    s->fill_rect(
                        *sc, SURFMASK_NO_CORNER, 0.0f,
                        key->fLeft - sSize.nLeft,
                        key->fTop - sSize.nTop,
                        key->fWidth,
                        split);
                    break;
                case 2:
                    s->fill_rect(
                        *sc, SURFMASK_NO_CORNER, 0.0f,
                        key->fLeft - sSize.nLeft,
                        key->fTop - sSize.nTop,
                        split,
                        key->fHeight);
                    break;
                case 3:
                    s->fill_rect(
                        *sc, SURFMASK_NO_CORNER, 0.0f,
                        key->fLeft - sSize.nLeft,
                        key->fTop  - sSize.nTop + key->fHeight - split,
                        key->fWidth,
                        split);
                    break;
            }
        }

        void PianoKeys::draw(ws::ISurface *s, bool force)
        {
            const float scaling     = lsp_max(0.0f, sScaling.get());
            const ssize_t angle     = sAngle.get() & 0x3;
            const style::PianoColors *c = get_piano_colors();
            const float split       = scaling * lsp_max(0.0f, sSplitSize.get());
            const bool selectable   = sSelectable.get();

            ws::rectangle_t kr;
            kr.nLeft    = 0;
            kr.nTop     = 0;
            kr.nWidth   = sSize.nWidth;
            kr.nHeight  = sSize.nHeight;

            // Draw the border
            lsp::Color col, ocol;
            col.copy(c->sBorderColor);
            s->fill_rect(col, SURFMASK_NO_CORNER, 0.0f, &kr);

            // Draw the keys
            ssize_t first = lsp_max(sMinNote.get(), 0);
            ssize_t last  = lsp_max(sMaxNote.get(), 0);
            if (first > last)
                lsp::swap(first, last);

            ssize_t sel_first   = (selectable) ? sSelectionStart.get() : -1;
            ssize_t sel_last    = (selectable) ? sSelectionEnd.get() : -1;
            if ((sel_first < 0) || (sel_last < 0))
            {
                sel_first   = -1;
                sel_last    = -1;
            }
            else if (sel_first > sel_last)
                lsp::swap(sel_first, sel_last);

            // Draw white keys first
            for (lltl::iterator<key_t> it=vKeys.values(); it; ++it)
                draw_key(s, *it, false, sel_first, sel_last);

            // Draw key splits
            col.copy(c->sSplitColor);
            ocol.copy(c->sOctaveSplitColor);
            for (lltl::iterator<key_t> it=vKeys.values(); it; ++it)
                draw_split(s, *it, col, ocol, angle, split);
            for (size_t i=0; i<2; ++i)
            {
                const key_t *sp = &vSplit[i];
                if ((sp->fWidth > 0.0f) && (sp->fHeight > 0.0f))
                {
                    s->fill_rect(col, SURFMASK_NO_CORNER, 0.0f,
                        sp->fLeft - sSize.nLeft,
                        sp->fTop - sSize.nTop,
                        sp->fWidth,
                        sp->fHeight);
                }
            }

            // Then black keys
            for (lltl::iterator<key_t> it=vKeys.values(); it; ++it)
                draw_key(s, *it, true, sel_first, sel_last);
        }

        void PianoKeys::compute_layout(layout_t * layout, bool natural)
        {
            ssize_t first = lsp_max(sMinNote.get(), 0);
            ssize_t last  = lsp_max(sMaxNote.get(), 0);
            if (first > last)
                lsp::swap(first, last);

            layout->nFirst  = first;
            layout->nLast   = last;
            layout->nLength = 0;
            layout->nBlack  = 0;
            layout->nWhite  = 0;

            // Compute the width of the keyboard
            for (; first < last; ++first)
            {
                layout->nLength    += key_advance(first, natural);
                if (is_black_key(first))
                    ++layout->nBlack;
                else
                    ++layout->nWhite;
            }

            layout->nLength    += key_width(first, natural);
            if (is_black_key(first))
                ++layout->nBlack;
            else
                ++layout->nWhite;
        }

        style::PianoColors *PianoKeys::get_piano_colors()
        {
            const size_t index = (sActive.get()) ? style::PIANO_NORMAL : style::PIANO_INACTIVE;
            return &vColors[index];
        }

        style::PianoKeyColors *PianoKeys::get_key_colors(bool down, bool selected, bool hover)
        {
            size_t index = (down) ? style::PIANOKEY_DOWN : style::PIANOKEY_NORMAL;
            index = lsp_setflag(index, style::PIANOKEY_SELECTED, selected);
            index = lsp_setflag(index, style::PIANOKEY_INACTIVE, !sActive.get());
            index = lsp_setflag(index, style::PIANOKEY_HOVER, hover);

            return &vKeyColors[index];
        }

        PianoKeys::key_t *PianoKeys::find_key(ssize_t x, ssize_t y)
        {
            const float fx = x, fy = y;

            // First search among black keys
            for (lltl::iterator<key_t> it=vKeys.values(); it; ++it)
            {
                key_t *key = it.get();
                if (!is_black_key(key->nKey))
                    continue;

                if ((fx >= key->fLeft) && (fy >= key->fTop) &&
                    (fx < (key->fLeft + key->fWidth)) && (fy < (key->fTop + key->fHeight)))
                    return key;
            }

            // Now search among splits
            for (size_t i=0; i<2; ++i)
            {
                key_t *key = &vSplit[i];
                if ((fx >= key->fLeft) && (fy >= key->fTop) &&
                    (fx < (key->fLeft + key->fWidth)) && (fy < (key->fTop + key->fHeight)))
                    return NULL;
            }

            // Now search among white keys
            for (lltl::iterator<key_t> it=vKeys.values(); it; ++it)
            {
                key_t *key = it.get();
                if (is_black_key(key->nKey))
                    continue;

                if ((fx >= key->fLeft) && (fy >= key->fTop) &&
                    (fx < (key->fLeft + key->fWidth)) && (fy < (key->fTop + key->fHeight)))
                    return key;
            }

            return NULL;
        }

        void PianoKeys::handle_note_press(ssize_t note)
        {
            ssize_t old = sMousePressed.get();
            if (old == note)
                return;

            // Clear selection if automatic clear is enabled
            if (sClearSelection.get())
            {
                bool notify = false;
                if (sSelectionStart.get() >= 0)
                {
                    sSelectionStart.set(-1);
                    notify = true;
                }
                if (sSelectionEnd.get() >= 0)
                {
                    sSelectionEnd.set(-1);
                    notify = true;
                }
                if (notify)
                    sSlots.execute(SLOT_CHANGE, this);
            }

            // Notify that previous note has been un-pressed
            if (old >= 0)
            {
                sMousePressed.set(-1);
                sSlots.execute(SLOT_SUBMIT, this);
            }

            // Notify that new note has been pressed
            if (note >= 0)
            {
                sMousePressed.set(note);
                sSlots.execute(SLOT_SUBMIT, this);
            }
        }

        void PianoKeys::set_hover_note(ssize_t note)
        {
            if (((sEditable.get()) || (sSelectable.get())) && (note != nHoverNote))
            {
                nHoverNote      = note;
                query_draw();
            }
            else if ((note < 0) && (note != nHoverNote))
            {
                nHoverNote      = note;
                query_draw();
            }
        }

        status_t PianoKeys::on_mouse_down(const ws::event_t *e)
        {
            const size_t mb_state = nMBState;
            nMBState |= 1 << e->nCode;
            if (nMBState != 0)
                set_hover_note(-1);

            key_t *key          = find_key(e->nLeft, e->nTop);
            const ssize_t note  = (key != NULL) ? key->nKey : -1;

            lsp_trace("note = %d", int(note));

            if (mb_state == 0)
            {
                if (e->nCode != ws::MCB_LEFT)
                {
                    enWorkMode = WMODE_IGNORE;
                    return STATUS_OK;
                }

                if ((sEditable.get()) && ((e->nState & ws::MCF_SHIFT) == 0))
                {
                    enWorkMode      = WMODE_PRESS;
                    handle_note_press(note);
                }
                else if (sSelectable.get())
                {
                    if (note >= 0)
                    {
                        enWorkMode      = WMODE_SELECT;

                        bool notify = false;
                        if (sSelectionStart.get() != note)
                        {
                            sSelectionStart.set(note);
                            notify = true;
                        }
                        if (sSelectionEnd.get() != note)
                        {
                            sSelectionEnd.set(note);
                            notify = true;
                        }

                        lsp_trace("selection = [%d, %d]", int(sSelectionStart.get()), int(sSelectionEnd.get()));
                        if (notify)
                            sSlots.execute(SLOT_CHANGE, this);
                    }
                    else
                        enWorkMode      = WMODE_IGNORE;
                }
                else
                    enWorkMode      = WMODE_IGNORE;

                return STATUS_OK;
            }

            // Proceed note press
            if (enWorkMode == WMODE_PRESS)
            {
                if (mb_state != ws::MCF_LEFT)
                    handle_note_press(-1);
                else
                    handle_note_press(note);
            }

            return STATUS_OK;
        }

        status_t PianoKeys::on_mouse_move(const ws::event_t *e)
        {
            key_t *key          = find_key(e->nLeft, e->nTop);
            ssize_t note        = (key != NULL) ? key->nKey : -1;

            lsp_trace("note = %d", int(note));

            set_hover_note((enWorkMode == WMODE_OFF) ? note : -1);

            if (nMBState != ws::MCF_LEFT)
                return STATUS_OK;

            if (enWorkMode == WMODE_PRESS)
            {
                handle_note_press(note);
            }
            else if (enWorkMode == WMODE_SELECT)
            {
                if (note < 0)
                    return STATUS_OK;

                bool notify = false;
                if ((sSelectionStart.get() < 0) && (sSelectionStart.get() != note))
                {
                    sSelectionStart.set(note);
                    notify = true;
                }
                if (sSelectionEnd.get() != note)
                {
                    sSelectionEnd.set(note);
                    notify = true;
                }

                lsp_trace("selection = [%d, %d]", int(sSelectionStart.get()), int(sSelectionEnd.get()));

                if (notify)
                    sSlots.execute(SLOT_CHANGE, this);
            }

            return STATUS_OK;
        }

        status_t PianoKeys::on_mouse_up(const ws::event_t *e)
        {
            nMBState &= ~(1 << e->nCode);

            if (nMBState == 0)
            {
                enWorkMode          = WMODE_OFF;
                handle_note_press(-1);

                // Set hover note
                key_t *key          = find_key(e->nLeft, e->nTop);
                set_hover_note((key != NULL) ? key->nKey : -1);
            }

            return STATUS_OK;
        }

        status_t PianoKeys::on_submit()
        {
            return STATUS_OK;
        }

        status_t PianoKeys::on_change()
        {
            return STATUS_OK;
        }

        status_t PianoKeys::slot_on_submit(Widget *sender, void *ptr, void *data)
        {
            PianoKeys *self = widget_ptrcast<PianoKeys>(ptr);
            return (self != NULL) ? self->on_submit() : STATUS_BAD_ARGUMENTS;
        }

        status_t PianoKeys::slot_on_change(Widget *sender, void *ptr, void *data)
        {
            PianoKeys *self = widget_ptrcast<PianoKeys>(ptr);
            return (self != NULL) ? self->on_change() : STATUS_BAD_ARGUMENTS;
        }

    } /* namespace tk */
} /* namespace lsp */



