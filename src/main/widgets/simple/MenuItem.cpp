/*
 * MenuItem.cpp
 *
 *  Created on: 18 сент. 2017 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t MenuItem::metadata      = { "MenuItem", &Widget::metadata };

        MenuItem::MenuItem(Display *dpy):
            Widget(dpy),
            sText(&sProperties),
            sType(&sProperties),
            sChecked(&sProperties)
        {
//            pMenu       = NULL;
            pClass      = &metadata;
        }
        
        MenuItem::~MenuItem()
        {
        }

        status_t MenuItem::slot_on_submit(Widget *sender, void *ptr, void *data)
        {
            MenuItem *_this = widget_ptrcast<MenuItem>(ptr);
            return (ptr != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }

        status_t MenuItem::init()
        {
            status_t res    = Widget::init();
            if (res != STATUS_OK)
                return res;

            sText.bind(&sStyle, pDisplay->dictionary());
            sType.bind("type", &sStyle);
            sChecked.bind("checked", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sType.init(sclass, MI_NORMAL);
                sChecked.init(sclass, false);
            }

            handler_id_t id = sSlots.add(SLOT_SUBMIT, slot_on_submit, self());

            return (id >= 0) ? STATUS_OK : -id;
        }

        void MenuItem::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (sText.is(prop))
                query_resize();
            if (sType.is(prop))
                query_resize();
            if (sChecked.is(prop))
                query_draw();
        }

        status_t MenuItem::on_submit()
        {
            return STATUS_OK;
        }
    
    } /* namespace tk */
} /* namespace lsp */
