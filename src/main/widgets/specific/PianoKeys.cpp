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

                pc = &vColors[style::PIANO_INACTIVE];
                pc->sBorderColor.bind("inactive.border.color", this);
                pc->sSplitColor.bind("inactive.split.color", this);

                // Bind other properties
                sBorderSize.bind("border.size", this);
                sSplitSize.bind("split.size", this);
                sMinNote.bind("note.min", this);
                sMaxNote.bind("note.max", this);
                sAngle.bind("angle", this);

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

                pc = &vColors[style::PIANO_INACTIVE];
                pc->sBorderColor.set_rgb24(0x444444);
                pc->sSplitColor.set_rgb24(0x888888);

                sBorderSize.set(1);
                sSplitSize.set(1);
                sMinNote.set(36);               // C2
                sMaxNote.set(36 + 4*12 - 1);    // H5
                sAngle.set(0);

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
            }

            bool PianoColors::property_changed(Property *prop)
            {
                return prop->one_of(sBorderColor, sSplitColor);
            }
        }

        //-----------------------------------------------------------------------------
        // PianoKeys implementation
        const w_class_t PianoKeys::metadata             = { "PianoKeys", &Widget::metadata };

        PianoKeys::PianoKeys(Display *dpy):
            tk::Widget(dpy),
            sBorderSize(&sProperties),
            sSplitSize(&sProperties),
            sMinNote(&sProperties),
            sMaxNote(&sProperties),
            sAngle(&sProperties)
        {
            pClass          = &metadata;

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

            pc = &vColors[style::PIANO_INACTIVE];
            pc->sBorderColor.bind("inactive.border.color", &sStyle);
            pc->sSplitColor.bind("inactive.split.color", &sStyle);

            // Bind other properties
            sBorderSize.bind("border.size", &sStyle);
            sSplitSize.bind("split.size", &sStyle);
            sMinNote.bind("note.min", &sStyle);
            sMaxNote.bind("note.max", &sStyle);
            sAngle.bind("angle", &sStyle);

            // Bind slots
            handler_id_t id;
            id = sSlots.add(SLOT_SUBMIT, slot_on_submit, self());
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
        }

        void PianoKeys::size_request(ws::size_limit_t *r)
        {
        }

        void PianoKeys::realize(const ws::rectangle_t *r)
        {
        }

        void PianoKeys::draw(ws::ISurface *s, bool force)
        {
        }

        status_t PianoKeys::on_mouse_down(const ws::event_t *e)
        {
            // TODO
            return STATUS_OK;
        }

        status_t PianoKeys::on_mouse_up(const ws::event_t *e)
        {
            // TODO
            return STATUS_OK;
        }

        status_t PianoKeys::on_submit()
        {
            return STATUS_OK;
        }

        status_t PianoKeys::slot_on_submit(Widget *sender, void *ptr, void *data)
        {
            PianoKeys *self = widget_ptrcast<PianoKeys>(ptr);
            return (self != NULL) ? self->on_submit() : STATUS_BAD_ARGUMENTS;
        }

    } /* namespace tk */
} /* namespace lsp */



