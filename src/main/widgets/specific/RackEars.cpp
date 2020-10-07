/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 7 окт. 2020 г.
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

namespace lsp
{
    namespace tk
    {
        const w_class_t RackEars::metadata              = { "RackEars", &Widget::metadata };

        RackEars::RackEars(Display *dpy):
            Widget(dpy),
            sFont(&sProperties),
            sText(&sProperties),
            sColor(&sProperties),
            sTextColor(&sProperties),
            sHoleColor(&sProperties),
            sAngle(&sProperties),
            sButtonPadding(&sProperties),
            sScrewPadding(&sProperties),
            sScrewSize(&sProperties)
        {
            nBMask              = 0;
            nXFlags             = 0;

            sButton.nLeft       = 0;
            sButton.nTop        = 0;
            sButton.nWidth      = 0;
            sButton.nHeight     = 0;

            for (size_t i=0; i<2; ++i)
            {
                sScrew[i].nLeft     = 0;
                sScrew[i].nTop      = 0;
                sScrew[i].nWidth    = 0;
                sScrew[i].nHeight   = 0;
            }

            pClass              = &metadata;
        }

        RackEars::~RackEars()
        {
        }

        status_t RackEars::init()
        {
            status_t res = Widget::init();
            if (res != STATUS_OK)
                return res;

            sFont.bind("font", &sStyle);
            sText.bind(&sStyle, pDisplay->dictionary());
            sColor.bind("color", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sHoleColor.bind("hole.color", &sStyle);
            sScrewColor.bind("screw.color", &sStyle);
            sAngle.bind("angle", &sStyle);
            sButtonPadding.bind("button.padding", &sStyle);
            sScrewPadding.bind("screw.padding", &sStyle);
            sScrewSize.bind("screw.size", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sFont.init(sclass, 16, ws::FF_BOLD);
                sColor.init(sclass, "#00ccff");
                sScrewColor.init(sclass, "#cccccc");
                sTextColor.init(sclass, "#ffffff");
                sHoleColor.init(sclass, "#000000");
                sAngle.init(sclass, 0);
                sButtonPadding.init(sclass, 4);
                sScrewPadding.init(sclass, 2);
                sScrewSize.init(sclass, 40);
            }

            handler_id_t id = sSlots.add(SLOT_SUBMIT, slot_on_submit, self());

            return (id >= 0) ? STATUS_OK : -id;
        }

        void RackEars::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (sFont.is(prop))
                query_resize();
            if (sText.is(prop))
                query_resize();
            if (sColor.is(prop))
                query_draw();
            if (sTextColor.is(prop))
                query_draw();
            if (sHoleColor.is(prop))
                query_draw();
            if (sAngle.is(prop))
                query_resize();
        }

        void RackEars::estimate_sizes(ws::rectangle_t *screw, ws::rectangle_t *btn)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());

            // Screw parameters
            screw->nLeft        = 0;
            screw->nTop         = 0;
            screw->nHeight      = ceilf(sScrewSize.get() * scaling);
            screw->nWidth       = ceilf(screw->nHeight * 1.5f);

            sScrewPadding.add(screw, scaling);

            // Button parameters
            ws::font_parameters_t fp;
            ws::text_parameters_t tp1;
            LSPString text;

            sText.format(&text);
            sFont.get_parameters(pDisplay, scaling, &fp);
            sFont.get_text_parameters(pDisplay, &tp1, scaling, &text);

            btn->nLeft          = 0;
            btn->nTop           = 0;
            btn->nWidth         = tp1.Width;
            btn->nHeight        = tp1.Height;

            if (!(sAngle.get() & 0x2))
            {
                btn->nHeight        = lsp_max(btn->nHeight, 2 * screw->nHeight);
                btn->nWidth         = lsp_max(btn->nWidth, btn->nHeight * M_GOLD_RATIO);
            }

            sButtonPadding.add(btn, scaling);
        }

        void RackEars::size_request(ws::size_limit_t *r)
        {
            ws::rectangle_t screw, btn;
            estimate_sizes(&screw, &btn);

            if (sAngle.get() & 1)
            {
                // Horizontal orientation
                r->nMinWidth    = screw.nWidth * 2  + btn.nWidth;
                r->nMinHeight   = lsp_max(screw.nHeight, btn.nHeight);

                r->nMaxWidth    = -1;
                r->nMaxHeight   = r->nMinHeight;
            }
            else
            {
                // Vertical orientation
                r->nMinWidth    = lsp_max(screw.nWidth, btn.nWidth);
                r->nMinHeight   = screw.nHeight * 2 + btn.nHeight;

                r->nMaxWidth    = r->nMinWidth;
                r->nMaxHeight   = -1;
            }

            r->nPreWidth    = -1;
            r->nPreHeight   = -1;
        }

        void RackEars::realize(const ws::rectangle_t *r)
        {
            Widget::realize(r);

            float scaling       = lsp_max(0.0f, sScaling.get());
            int angle           = (sAngle.get() & 0x03);

            ws::rectangle_t screw[2], btn;
            estimate_sizes(&screw[0], &btn);
            screw[1] = screw[0];

            // Position of screws
            if (angle & 1)
            {
                // Horizontal orientation
                screw[0].nLeft  = (angle & 0x02) ? sSize.nLeft + sSize.nWidth - screw[0].nWidth : sSize.nLeft;
                screw[1].nLeft  = screw[0].nLeft;
                screw[0].nTop   = sSize.nTop;
                screw[1].nTop   = sSize.nTop + sSize.nHeight - screw[1].nHeight;
            }
            else
            {
                screw[0].nLeft  = sSize.nLeft;
                screw[1].nLeft  = sSize.nLeft + sSize.nWidth - screw[1].nWidth;
                screw[0].nTop   = sSize.nTop  + ((sSize.nHeight - screw[0].nHeight) >> 1);
                screw[1].nTop   = screw[0].nTop;
            }


            // Position of button
            btn.nLeft       = sSize.nLeft + ((sSize.nWidth  - btn.nWidth ) >> 1);
            btn.nTop        = sSize.nTop  + ((sSize.nHeight - btn.nHeight) >> 1);

            // Realize screws and button
            sScrewPadding.enter(&sScrew[0], &screw[0], scaling);
            sScrewPadding.enter(&sScrew[1], &screw[1], scaling);
            sButtonPadding.enter(&sButton, &btn, scaling);
        }

        void RackEars::draw(ws::ISurface *s)
        {
        }

        status_t RackEars::on_mouse_down(const ws::event_t *e)
        {
            // Handle mouse event
            if (nBMask == 0)
            {
                if (Position::inside(&sButton, e->nLeft, e->nTop))
                {
                    if (e->nCode == ws::MCB_LEFT)
                        nXFlags    |= XF_LBUTTON;
                    else if (e->nCode == ws::MCB_RIGHT)
                        nXFlags    |= XF_RBUTTON;
                }
            }

            nBMask |= 1 << e->nCode;

            return handle_mouse_move(e);
        }

        status_t RackEars::on_mouse_up(const ws::event_t *e)
        {
            size_t mask = nBMask;
            nBMask &= ~(1 << e->nCode);

            if (mask != (1U << e->nCode))
                return handle_mouse_move(e);

            // Last button released, process the vent
            size_t flags    = nXFlags;
            nXFlags         = 0;

            if (Position::inside(&sButton, e->nLeft, e->nTop))
            {
                if ((e->nCode == ws::MCB_LEFT) && (flags & XF_LBUTTON))
                    sSlots.execute(SLOT_SUBMIT, this, NULL);
            }

            if (flags != nXFlags)
                query_draw();

            return STATUS_OK;
        }

        status_t RackEars::on_mouse_move(const ws::event_t *e)
        {
            if (nXFlags == 0)
                return STATUS_OK;

            return handle_mouse_move(e);
        }

        status_t RackEars::handle_mouse_move(const ws::event_t *e)
        {
            if (nXFlags & XF_LBUTTON)
            {
                bool pressed= (nBMask == ws::MCF_LEFT) && (Position::inside(&sButton, e->nLeft, e->nTop));

                size_t old  = nXFlags;
                nXFlags     = lsp_setflag(nXFlags, XF_DOWN, pressed);

                if (old != nXFlags)
                    query_draw();
            }

            return STATUS_OK;
        }

        status_t RackEars::on_submit()
        {
            return STATUS_OK;
        }

        status_t RackEars::slot_on_submit(Widget *sender, void *ptr, void *data)
        {
            RackEars *_this = widget_ptrcast<RackEars>(ptr);
            return (ptr != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }
    }
}
