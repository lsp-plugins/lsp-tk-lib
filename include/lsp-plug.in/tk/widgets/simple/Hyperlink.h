/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 23 окт. 2017 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_HYPERLINK_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_HYPERLINK_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Menu;

        // Style definition
        namespace style
        {
            LSP_TK_STYLE_DEF_BEGIN(Hyperlink, Widget)
                prop::TextLayout            sTextLayout;    // Text layout
                prop::TextAdjust            sTextAdjust;    // Text adjustment
                prop::Font                  sFont;          // Font parameters
                prop::Color                 sColor;         // Font color
                prop::Color                 sHoverColor;    // Hover font color
                prop::String                sText;          // Text to display
                prop::SizeConstraints       sConstraints;   // Size constraints
                prop::Boolean               sFollow;        // Follow hyperlink
                prop::String                sUrl;           // URL
            LSP_TK_STYLE_DEF_END
        }

        class Hyperlink: public Widget
        {
            public:
                static const w_class_t    metadata;

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
                Widget                     *vMenus[3];

                prop::TextLayout            sTextLayout;    // Text layout
                prop::TextAdjust            sTextAdjust;    // Text adjustment
                prop::Font                  sFont;          // Font parameters
                prop::Color                 sColor;         // Font color
                prop::Color                 sHoverColor;    // Hover font color
                prop::String                sText;          // Text to display
                prop::SizeConstraints       sConstraints;   // Size constraints
                prop::Boolean               sFollow;        // Follow hyperlink
                prop::String                sUrl;           // URL
                prop::WidgetPtr<Menu>       sPopup;         // Popup menu

            protected:
                static status_t                 slot_on_submit(Widget *sender, void *ptr, void *data);
                static status_t                 slot_on_before_popup(Widget *sender, void *ptr, void *data);
                static status_t                 slot_on_popup(Widget *sender, void *ptr, void *data);
                static status_t                 slot_copy_link_action(Widget *sender, void *ptr, void *data);

                status_t                        create_default_menu();
                void                            do_destroy();

            protected:
                virtual void                    size_request(ws::size_limit_t *r) override;
                virtual void                    property_changed(Property *prop) override;

            public:
                explicit Hyperlink(Display *dpy);
                Hyperlink(const Hyperlink &) = delete;
                Hyperlink(Hyperlink &&) = delete;
                virtual ~Hyperlink() override;
                Hyperlink & operator = (const Hyperlink &) = delete;
                Hyperlink & operator = (Hyperlink &&) = delete;

                virtual status_t                init() override;
                virtual void                    destroy() override;

            public:
                LSP_TK_PROPERTY(TextLayout,         text_layout,        &sTextLayout)
                LSP_TK_PROPERTY(TextAdjust,         text_adjust,        &sTextAdjust)
                LSP_TK_PROPERTY(Font,               font,               &sFont)
                LSP_TK_PROPERTY(Color,              color,              &sColor)
                LSP_TK_PROPERTY(Color,              hover_color,        &sHoverColor)
                LSP_TK_PROPERTY(String,             text,               &sText)
                LSP_TK_PROPERTY(SizeConstraints,    constraints,        &sConstraints)
                LSP_TK_PROPERTY(Boolean,            follow,             &sFollow)
                LSP_TK_PROPERTY(String,             url,                &sUrl)
                LSP_TK_PROPERTY(WidgetPtr<Menu>,    popup,              &sPopup)

            public:
                status_t                        copy_url(ws::clipboard_id_t cb);
                status_t                        follow_url() const;

            public:
                virtual void                    draw(ws::ISurface *s) override;
                virtual status_t                on_mouse_in(const ws::event_t *e) override;
                virtual status_t                on_mouse_out(const ws::event_t *e) override;
                virtual status_t                on_mouse_move(const ws::event_t *e) override;
                virtual status_t                on_mouse_down(const ws::event_t *e) override;
                virtual status_t                on_mouse_up(const ws::event_t *e) override;

            public:
                virtual status_t                on_before_popup(Menu *menu);
                virtual status_t                on_popup(Menu *menu);
                virtual status_t                on_submit();
        };

    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_HYPERLINK_H_ */
