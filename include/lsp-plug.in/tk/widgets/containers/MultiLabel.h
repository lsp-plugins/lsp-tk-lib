/*
 * Copyright (C) 2021 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2021 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 16 июн. 2021 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_MULTILABEL_H_
#define LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_MULTILABEL_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        // Style definition
        namespace style
        {
            LSP_TK_STYLE_DEF_BEGIN(MultiLabel, WidgetContainer)
                prop::SizeConstraints   sConstraints;
                prop::Boolean           sBearing;
                prop::Boolean           sHover;
            LSP_TK_STYLE_DEF_END
        }

        /**
         * MultiLabelment, implements a single widget container that aligns the child widget
         * according to the layout settings. The container ignores allocation() property
         * of the child widget.
         */
        class MultiLabel: public WidgetContainer
        {
            private:
                MultiLabel & operator = (const MultiLabel &);
                MultiLabel(const MultiLabel &);

            public:
                static const w_class_t      metadata;


            protected:
                enum state_t
                {
                    F_MOUSE_IN      = 1 << 0,
                    F_MOUSE_DOWN    = 1 << 1,
                    F_MOUSE_IGN     = 1 << 2,
                };

            protected:
                size_t                      nMFlags;
                size_t                      nState;

                prop::SizeConstraints       sConstraints;
                prop::Boolean               sBearing;
                prop::Boolean               sHover;         // Hover enable
                prop::WidgetList<Label>     vItems;
                prop::CollectionListener    sIListener;     // Listener to trigger vItems content change
                prop::WidgetPtr<Menu>       sPopup;         // Popup menu

            protected:
                void                    do_destroy();

            protected:
                virtual void            property_changed(Property *prop);
                virtual void            size_request(ws::size_limit_t *r);
                virtual void            realize(const ws::rectangle_t *r);

            protected:
                static void             on_add_item(void *obj, Property *prop, void *w);
                static void             on_remove_item(void *obj, Property *prop, void *w);

            public:
                explicit MultiLabel(Display *dpy);
                virtual ~MultiLabel();

                virtual status_t        init();
                virtual void            destroy();

            public:
                LSP_TK_PROPERTY(SizeConstraints,    constraints,        &sConstraints)
                LSP_TK_PROPERTY(Boolean,            bearing,            &sBearing)
                LSP_TK_PROPERTY(Boolean,            hover,              &sHover)
                LSP_TK_PROPERTY(WidgetList<Label>,  items,              &vItems)
                LSP_TK_PROPERTY(WidgetPtr<Menu>,    popup,              &sPopup)

            public:
                virtual void            render(ws::ISurface *s, const ws::rectangle_t *area, bool force);

                virtual status_t        add(Widget *widget);

                virtual status_t        remove(Widget *widget);

                virtual status_t        on_mouse_in(const ws::event_t *e);

                virtual status_t        on_mouse_out(const ws::event_t *e);

                virtual status_t        on_mouse_move(const ws::event_t *e);

                virtual status_t        on_mouse_down(const ws::event_t *e);

                virtual status_t        on_mouse_up(const ws::event_t *e);
        };

    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_MULTILABEL_H_ */
