/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 25 сент. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_LEDMETER_H_
#define LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_LEDMETER_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class LedMeterChannel;

        class LedMeter: public Widget
        {
            public:
                static const w_class_t    metadata;

            private:
                LedMeter & operator = (const LedMeter &);

            protected:
                lltl::parray<LedMeterChannel>       vVisible;
                prop::WidgetList<LedMeterChannel>   vItems;
                prop::CollectionListener            sIListener;     // Listener to trigger vItems content change

                prop::SizeConstraints               sConstraints;
                prop::Font                          sFont;
                prop::Integer                       sBorder;
                prop::Integer                       sAngle;
                prop::String                        sEstText;
                prop::Boolean                       sSGroups;
                prop::Boolean                       sTextVisible;
                prop::Color                         sColor;

                ws::rectangle_t                     sAAll;

            protected:
                void                        do_destroy();

            public:
                explicit LedMeter(Display *dpy);
                virtual ~LedMeter();

                virtual status_t            init();
                virtual void                destroy();

            protected:
                virtual void                property_changed(Property *prop);
                virtual void                size_request(ws::size_limit_t *r);
                virtual void                realize(const ws::rectangle_t *r);
                virtual Widget             *find_widget(ssize_t x, ssize_t y);

                void                        get_visible_items(lltl::parray<LedMeterChannel> *dst);

                static void                 on_add_item(void *obj, Property *prop, Widget *w);
                static void                 on_remove_item(void *obj, Property *prop, Widget *w);

            public:
                virtual void                query_draw(size_t flags = REDRAW_SURFACE);

            public:
                LSP_TK_PROPERTY(WidgetList<LedMeterChannel>,    items,              &vItems)
                LSP_TK_PROPERTY(SizeConstraints,                constraints,        &sConstraints)
                LSP_TK_PROPERTY(Font,                           font,               &sFont)
                LSP_TK_PROPERTY(Integer,                        border,             &sBorder)
                LSP_TK_PROPERTY(Integer,                        angle,              &sAngle)
                LSP_TK_PROPERTY(String,                         estimation_text,    &sEstText)
                LSP_TK_PROPERTY(Boolean,                        stereo_groups,      &sSGroups)
                LSP_TK_PROPERTY(Boolean,                        text_visible,       &sTextVisible)
                LSP_TK_PROPERTY(Color,                          color,              &sColor)

            public:
                virtual void                draw(ws::ISurface *s);

                virtual status_t            add(Widget *widget);

                virtual status_t            remove(Widget *child);

                virtual status_t            remove_all();

        };
    }
}

#endif /* LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_LEDMETER_H_ */
