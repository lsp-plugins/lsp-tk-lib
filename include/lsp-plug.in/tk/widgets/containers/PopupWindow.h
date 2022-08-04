/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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
            private:
                PopupWindow & operator = (const PopupWindow &);
                PopupWindow(const PopupWindow &);

            public:
                static const w_class_t          metadata;

            protected:
                prop::Rectangle                 sTrgArea;           // The restricted area where window should never appear
                prop::WidgetPtr<Widget>         sTrgWidget;         // The widget triggered the show
                prop::Integer                   sTrgScreen;         // The target screen to appear
                prop::Boolean                   sAutoClose;         // Automatically close when the pointer delivers event ouside the window

//                lltl::darray<arrangement_t>     vArrangements;      // Arrangements
                lltl::darray<tether_t>          vTether;            // List of tether rules
                bool                            bInitialized;       // Initalization flag

            protected:
                virtual void                    hide_widget();
                virtual void                    show_widget();
                virtual status_t                post_init();
                virtual status_t                sync_size();
                virtual void                    size_request(ws::size_limit_t *r);
                bool                            init_window();
                bool                            arrange_preferred(ws::rectangle_t *dst, const ws::rectangle_t *trg, const arrangement_t *ar, bool force, bool truncate);
                bool                            arrange_window(ws::rectangle_t *dst, const ws::rectangle_t *trg, const arrangement_t *ar, bool force);
                void                            forced_arrange(ws::rectangle_t *dst, const ws::rectangle_t *trg, const arrangement_t *ar);
                void                            arrange_window_geometry();

            public:
                explicit PopupWindow(Display *dpy);
                virtual ~PopupWindow();

                virtual status_t                init();

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
                virtual status_t                handle_event(const ws::event_t *e);
        };
    }
}



#endif /* LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_POPUPWINDOW_H_ */
