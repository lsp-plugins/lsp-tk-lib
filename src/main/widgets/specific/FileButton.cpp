/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 7 сент. 2020 г.
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
        FileButton::FileButton(Display *dpy):
            Widget(dpy),
            sValue(&sProperties),
            sText(&sProperties),
            sTextList(&sProperties),
            sFont(&sProperties),
            sTextLayout(&sProperties),
            sTextPadding(&sProperties),
            sConstraints(&sProperties),
            sColor(&sProperties),
            sInvColor(&sProperties),
            sTextColor(&sProperties),
            sInvTextColor(&sProperties)
        {
            nBMask              = 0;

            sButton.nLeft       = 0;
            sButton.nTop        = 0;
            sButton.nWidth      = 0;
            sButton.nHeight     = 0;

            sLabel.nLeft        = 0;
            sLabel.nTop         = 0;
            sLabel.nWidth       = 0;
            sLabel.nHeight      = 0;
        }

        FileButton::~FileButton()
        {
        }

        void FileButton::destroy()
        {
        }

        status_t FileButton::init()
        {
            status_t result = Widget::init();
            if (result != STATUS_OK)
                return result;

            sValue.bind("value", &sStyle);
            sText.bind(&sStyle, pDisplay->dictionary());
            sTextList.bind(&sStyle, pDisplay->dictionary());
            sFont.bind("font", &sStyle);
            sTextLayout.bind("text.layout", &sStyle);
            sTextPadding.bind("text.padding", &sStyle);
            sConstraints.bind("size.constraints", &sStyle);
            sColor.bind("color", &sStyle);
            sInvColor.bind("inv.color", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sInvTextColor.bind("text.inv.color", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sValue.init(sclass, 0.0f, 0.0f, 1.0f);
                sFont.init(sclass, 10.0f);
                sTextLayout.init(sclass, 0.0f, 0.0f);
                sTextPadding.init(sclass, 2, 2, 2, 2);
                sConstraints.init(sclass);
                sColor.init(sclass, "#cccccc");
                sInvColor.init(sclass, "#00cc00");
                sTextColor.init(sclass, "#cccccc");
                sInvTextColor.init(sclass, "#00cc00");
            }

            // Additional slots
            handler_id_t id = 0;
            id = sSlots.add(SLOT_CHANGE, slot_on_change, self());
            if (id >= 0) id = sSlots.add(SLOT_SUBMIT, slot_on_submit, self());

            return (id >= 0) ? STATUS_OK : -id;
        }

        void FileButton::property_changed(Property *prop)
        {
            // TODO
        }

        void FileButton::size_request(ws::size_limit_t *r)
        {
            // TODO
        }

        void FileButton::realize(const ws::rectangle_t *r)
        {
            // TODO
        }

        void FileButton::draw(ws::ISurface *s)
        {
            // TODO
        }

        status_t FileButton::on_mouse_down(const ws::event_t *e)
        {
            // TODO
            return STATUS_OK;
        }

        status_t FileButton::on_mouse_up(const ws::event_t *e)
        {
            // TODO
            return STATUS_OK;
        }

        status_t FileButton::on_mouse_move(const ws::event_t *e)
        {
            // TODO
            return STATUS_OK;
        }

        status_t FileButton::slot_on_change(Widget *sender, void *ptr, void *data)
        {
            FileButton *_this = widget_ptrcast<FileButton>(ptr);
            return (_this != NULL) ? _this->on_change() : STATUS_BAD_ARGUMENTS;
        }

        status_t FileButton::slot_on_submit(Widget *sender, void *ptr, void *data)
        {
            FileButton *_this = widget_ptrcast<FileButton>(ptr);
            return (_this != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }

        status_t FileButton::on_change()
        {
            // TODO
            return STATUS_OK;
        }

        status_t FileButton::on_submit()
        {
            // TODO
            return STATUS_OK;
        }
    }
}


