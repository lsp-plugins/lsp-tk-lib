/*
 * ComboBox.cpp
 *
 *  Created on: 8 авг. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t ComboBox::metadata              = { "ComboBox", &WidgetContainer::metadata };

        ComboBox::ComboBox(Display *dpy):
            WidgetContainer(dpy),
            sLBox(dpy),
            sBorderSize(&sProperties),
            sBorderGap(&sProperties),
            sBorderRadius(&sProperties),
            sSpinSize(&sProperties),
            sColor(&sProperties),
            sTextColor(&sProperties),
            sBorderColor(&sProperties),
            sBorderGapColor(&sProperties),
            sSpinColor(&sProperties),
            sOpened(&sProperties),
            sTextFit(&sProperties)
        {
            pClass      = &metadata;
        }

        ComboBox::~ComboBox()
        {
            do_destroy();
        }

        void ComboBox::destroy()
        {
            do_destroy();
            WidgetContainer::destroy();
        }

        void ComboBox::do_destroy()
        {
        }

        status_t ComboBox::init()
        {
            // Initialize widgets
            status_t result = WidgetContainer::init();
            if (result == STATUS_OK)
                result  = sLBox.init();
            if (result != STATUS_OK)
                return result;

            // Configure ListBox
            sLBox.allocation()->set_embed(true);
            sLBox.set_parent(this);

            sBorderSize.bind("border.size", &sStyle);
            sBorderGap.bind("border.gap", &sStyle);
            sBorderRadius.bind("border.radius", &sStyle);
            sSpinSize.bind("spin.size", &sStyle);
            sColor.bind("color", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sBorderGapColor.bind("border.gap.color", &sStyle);
            sSpinColor.bind("spin.color", &sStyle);
            sOpened.bind("opened", &sStyle);
            sTextFit.bind("text.fit", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sBorderSize.init(sclass, 1);
                sBorderGap.init(sclass, 1);
                sBorderRadius.init(sclass, 2);
                sSpinSize.init(sclass, 12);
                sColor.init(sclass, "#ffffff");
                sTextColor.init(sclass, "#000000");
                sBorderColor.init(sclass, "#000000");
                sBorderGapColor.init(sclass, "#cccccc");
                sSpinColor.init(sclass, "#000000");
                sOpened.init(sclass, false);
                sTextFit.init(sclass, true);
            }

            return STATUS_OK;
        }

        void ComboBox::property_changed(Property *prop)
        {

        }

        void ComboBox::size_request(ws::size_limit_t *r)
        {
        }

        void ComboBox::realize(const ws::rectangle_t *r)
        {
        }

        void ComboBox::draw(ws::ISurface *s)
        {
            // TODO
        }

        status_t ComboBox::add(Widget *child)
        {
            return sLBox.add(child);
        }

        status_t ComboBox::remove(Widget *child)
        {
            return sLBox.remove(child);
        }

        status_t ComboBox::remove_all()
        {
            return sLBox.remove_all();
        }

        status_t ComboBox::on_mouse_down(const ws::event_t *e)
        {
            return STATUS_OK;
        }

        status_t ComboBox::on_mouse_up(const ws::event_t *e)
        {
            return STATUS_OK;
        }

        status_t ComboBox::on_mouse_move(const ws::event_t *e)
        {
            return STATUS_OK;
        }

        status_t ComboBox::on_mouse_scroll(const ws::event_t *e)
        {
            return STATUS_OK;
        }

        status_t ComboBox::on_key_down(const ws::event_t *e)
        {
            return STATUS_OK;
        }

        status_t ComboBox::on_key_up(const ws::event_t *e)
        {
            return STATUS_OK;
        }
    }
}


