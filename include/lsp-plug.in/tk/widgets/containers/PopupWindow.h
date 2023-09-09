/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 16 июн. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_POPUPWINDOW_H_
#define LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_POPUPWINDOW_H_


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
            LSP_TK_STYLE_DEF_BEGIN(PopupWindow, Window)
                prop::Rectangle                 sTrgArea;           // The restricted area where window should never appear
                prop::Integer                   sTrgScreen;         // The target screen to appear
                prop::Boolean                   sAutoClose;         // Automatically close when the pointer delivers event ouside the window
            LSP_TK_STYLE_DEF_END
        }

        /**
         * Popup window used for different pop-up elements like combo boxes, menus, etc
         */
        class PopupWindow: public Window
        {
            public:
                static const w_class_t          metadata;

            protected:
                prop::Rectangle                 sTrgArea;           // The restricted area where window should never appear
                prop::WidgetPtr<Widget>         sTrgWidget;         // The widget triggered the show
                prop::Integer                   sTrgScreen;         // The target screen to appear
                prop::Boolean                   sAutoClose;         // Automatically close when the pointer delivers event ouside the window

                lltl::darray<tether_t>          vTether;            // List of tether rules
                bool                            bInitialized;       // Initalization flag

            protected:
                virtual void                    hide_widget() override;
                virtual void                    show_widget() override;
                virtual status_t                sync_size() override;
                virtual void                    size_request(ws::size_limit_t *r) override;

                virtual status_t                post_init();

                bool                            init_window();
                void                            arrange_window_geometry();

            public:
                explicit PopupWindow(Display *dpy);
                PopupWindow(const PopupWindow &) = delete;
                PopupWindow(PopupWindow &&) = delete;
                virtual ~PopupWindow() override;

                PopupWindow & operator = (const PopupWindow &) = delete;
                PopupWindow & operator = (PopupWindow &&) = delete;

                virtual status_t                init() override;

            public:
                LSP_TK_PROPERTY(Rectangle,          trigger_area,               &sTrgArea)
                LSP_TK_PROPERTY(Integer,            trigger_screen,             &sTrgScreen)
                LSP_TK_PROPERTY(WidgetPtr<Widget>,  trigger_widget,             &sTrgWidget)
                LSP_TK_PROPERTY(Boolean,            auto_close,                 &sAutoClose)

            public:
                bool                            set_tether(const lltl::darray<tether_t> *list);
                bool                            set_tether(const tether_t *list, size_t count);
                bool                            add_tether(const tether_t *item);
                bool                            add_tether(size_t flags, float halign=1.0f, float valign=1.0f);

            public:
                virtual status_t                handle_event(const ws::event_t *e) override;
        };

    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_POPUPWINDOW_H_ */
