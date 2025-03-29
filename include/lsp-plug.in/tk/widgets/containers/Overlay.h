/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 28 мар. 2025 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_OVERLAY_H_
#define LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_OVERLAY_H_

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
            LSP_TK_STYLE_DEF_BEGIN(Overlay, WidgetContainer)
                prop::Float             sTransparency;      // The transparency of the overlay widget
                prop::Integer           sPriority;          // The draw priority of the overlay widget
                prop::Layout            sLayout;            // The layout of the overlay widget
                prop::Position          sPosition;          // The position of the overlay widget
                prop::SizeConstraints   sConstraints;       // Size constraints
                prop::Boolean           sAutoClose;         // Automatically close when the pointer delivers event ouside of it's area
            LSP_TK_STYLE_DEF_END
        }

        /**
         * Overlayment, implements a single widget container that Overlays the child widget
         * according to the layout settings. The container ignores allocation() property
         * of the child widget.
         */
        class Overlay: public WidgetContainer
        {
            public:
                static const w_class_t    metadata;

            protected:
                Widget                 *wWidget;

                prop::Float             sTransparency;      // The transparency of the overlay widget
                prop::Integer           sPriority;          // The draw priority of the overlay widget
                prop::Layout            sLayout;            // The layout of the overlay widget
                prop::Position          sPosition;          // The position of the overlay widget
                prop::SizeConstraints   sConstraints;       // Size constraints
                prop::Boolean           sAutoClose;         // Automatically close when the pointer delivers event ouside of it's area

            protected:
                void                    do_destroy();

            protected:
                virtual Widget         *find_widget(ssize_t x, ssize_t y) override;
                virtual void            property_changed(Property *prop) override;
                virtual void            size_request(ws::size_limit_t *r) override;
                virtual void            realize(const ws::rectangle_t *r) override;

            public:
                explicit Overlay(Display *dpy);
                Overlay(const Overlay &) = delete;
                Overlay(Overlay &&) = delete;
                virtual ~Overlay() override;

                Overlay & operator = (const Overlay &) = delete;
                Overlay & operator = (Overlay &&) = delete;

                virtual status_t        init() override;
                virtual void            destroy() override;

            public:
                LSP_TK_PROPERTY(Float,              transparency,       &sTransparency)
                LSP_TK_PROPERTY(Integer,            priority,           &sPriority)
                LSP_TK_PROPERTY(Layout,             layout,             &sLayout)
                LSP_TK_PROPERTY(Position,           position,           &sPosition)
                LSP_TK_PROPERTY(SizeConstraints,    constraints,        &sConstraints)
                LSP_TK_PROPERTY(Boolean,            auto_close,         &sAutoClose)

            public:
                virtual void            draw(ws::ISurface *s, bool force) override;
                virtual status_t        add(Widget *widget) override;
                virtual status_t        remove(Widget *widget) override;
        };

    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_OVERLAY_H_ */
