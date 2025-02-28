/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 9 нояб. 2022 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_TAB_H_
#define LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_TAB_H_

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
            typedef struct TabColors
            {
                prop::Color                 sColor;                 // Heading tab color
                prop::Color                 sBorderColor;           // Heading tab border color
                prop::Color                 sTextColor;             // Heading tab text color

                void listener(tk::prop::Listener *listener);
                bool property_changed(Property *prop);
            } TabColors;

            enum TabColorState
            {
                TAB_NORMAL              = 0,
                TAB_SELECTED            = 1 << 0,
                TAB_HOVER               = 1 << 1,
                TAB_INACTIVE            = 1 << 2,

                TAB_TOTAL               = 1 << 3
            };

            LSP_TK_STYLE_DEF_BEGIN(Tab, WidgetContainer)
                TabColors                   vColors[TAB_TOTAL];

                prop::Layout                sLayout;                // Widget layout inside of the tab
                prop::String                sText;                  // Heading tab text
                prop::TextAdjust            sTextAdjust;            // Heading tab text adjustment
                prop::TextLayout            sTextLayout;            // Heading tab text layout
                prop::Padding               sTextPadding;           // Heading tab text padding
                prop::Font                  sFont;                  // Heading tab font
                prop::Integer               sBorderSize;            // Border size of the heading tab
                prop::Integer               sBorderRadius;          // Border radius of the heading tab
                prop::Boolean               sActive;                // Activity flag
            LSP_TK_STYLE_DEF_END
        } /* namespace style */

        class Tab: public WidgetContainer
        {
            public:
                static const w_class_t      metadata;

            private:
                friend class TabControl;

            protected:
                enum titm_flags_t
                {
                    TAB_0           = style::TAB_NORMAL,
                    TAB_1           = style::TAB_SELECTED,
                    TAB_2           = style::TAB_HOVER,
                    TAB_3           = style::TAB_SELECTED | style::TAB_HOVER,
                    TAB_4           = style::TAB_INACTIVE,
                    TAB_5           = style::TAB_SELECTED | style::TAB_INACTIVE,
                    TAB_6           = style::TAB_HOVER | style::TAB_INACTIVE,
                    TAB_7           = style::TAB_SELECTED | style::TAB_HOVER | style::TAB_INACTIVE,

                    TAB_TOTAL       = style::TAB_TOTAL
                };

            protected:
                Widget                     *pWidget;

                style::TabColors            vColors[style::TAB_TOTAL];
                prop::Layout                sLayout;                // Widget layout inside of the tab
                prop::String                sText;                  // Heading tab text
                prop::TextAdjust            sTextAdjust;            // Heading tab text adjustment
                prop::TextLayout            sTextLayout;            // Heading tab text layout
                prop::Padding               sTextPadding;           // Heading tab text padding
                prop::Font                  sFont;                  // Heading tab font
                prop::Integer               sBorderSize;            // Border size of the heading tab
                prop::Integer               sBorderRadius;          // Border radius of the heading tab
                prop::Boolean               sActive;                // Activity flag

            protected:
                void                    do_destroy();
                style::TabColors       *select_colors(bool selected, bool hover);

            protected:
                virtual void            property_changed(Property *prop) override;
                virtual void            size_request(ws::size_limit_t *r) override;
                virtual void            realize(const ws::rectangle_t *r) override;

            public:
                explicit Tab(Display *dpy);
                Tab(const Tab &) = delete;
                Tab(Tab &&) = delete;
                virtual ~Tab() override;

                Tab & operator = (const Tab &) = delete;
                Tab & operator = (Tab &&) = delete;

                virtual status_t            init() override;
                virtual void                destroy() override;

            public:
                LSP_TK_PROPERTY(Color,              color,                                  &vColors[TAB_0].sColor);
                LSP_TK_PROPERTY(Color,              border_color,                           &vColors[TAB_0].sBorderColor);
                LSP_TK_PROPERTY(Color,              text_color,                             &vColors[TAB_0].sTextColor);
                LSP_TK_PROPERTY(Color,              selected_color,                         &vColors[TAB_1].sColor);
                LSP_TK_PROPERTY(Color,              border_selected_color,                  &vColors[TAB_1].sBorderColor);
                LSP_TK_PROPERTY(Color,              text_selected_color,                    &vColors[TAB_1].sTextColor);
                LSP_TK_PROPERTY(Color,              hover_color,                            &vColors[TAB_2].sColor);
                LSP_TK_PROPERTY(Color,              border_hover_color,                     &vColors[TAB_2].sBorderColor);
                LSP_TK_PROPERTY(Color,              text_hover_color,                       &vColors[TAB_2].sTextColor);
                LSP_TK_PROPERTY(Color,              hover_selected_color,                   &vColors[TAB_3].sColor);
                LSP_TK_PROPERTY(Color,              border_hover_selected_color,            &vColors[TAB_3].sBorderColor);
                LSP_TK_PROPERTY(Color,              text_hover_selected_color,              &vColors[TAB_3].sTextColor);
                LSP_TK_PROPERTY(Color,              inactive_color,                         &vColors[TAB_4].sColor);
                LSP_TK_PROPERTY(Color,              inactive_border_color,                  &vColors[TAB_4].sBorderColor);
                LSP_TK_PROPERTY(Color,              inactive_text_color,                    &vColors[TAB_4].sTextColor);
                LSP_TK_PROPERTY(Color,              inactive_selected_color,                &vColors[TAB_5].sColor);
                LSP_TK_PROPERTY(Color,              inactive_border_selected_color,         &vColors[TAB_5].sBorderColor);
                LSP_TK_PROPERTY(Color,              inactive_text_selected_color,           &vColors[TAB_5].sTextColor);
                LSP_TK_PROPERTY(Color,              inactive_hover_color,                   &vColors[TAB_6].sColor);
                LSP_TK_PROPERTY(Color,              inactive_border_hover_color,            &vColors[TAB_6].sBorderColor);
                LSP_TK_PROPERTY(Color,              inactive_text_hover_color,              &vColors[TAB_6].sTextColor);
                LSP_TK_PROPERTY(Color,              inactive_hover_selected_color,          &vColors[TAB_7].sColor);
                LSP_TK_PROPERTY(Color,              inactive_border_hover_selected_color,   &vColors[TAB_7].sBorderColor);
                LSP_TK_PROPERTY(Color,              inactive_text_hover_selected_color,     &vColors[TAB_7].sTextColor);

                LSP_TK_PROPERTY(Layout,             layout,                                 &sLayout);
                LSP_TK_PROPERTY(String,             text,                                   &sText);
                LSP_TK_PROPERTY(TextAdjust,         text_adjust,                            &sTextAdjust);
                LSP_TK_PROPERTY(TextLayout,         text_layout,                            &sTextLayout);
                LSP_TK_PROPERTY(Padding,            text_padding,                           &sTextPadding);
                LSP_TK_PROPERTY(Font,               font,                                   &sFont);
                LSP_TK_PROPERTY(Integer,            border_size,                            &sBorderSize);
                LSP_TK_PROPERTY(Integer,            border_radius,                          &sBorderRadius);
                LSP_TK_PROPERTY(Boolean,            active,                                 &sActive);

            public:
                virtual Widget             *find_widget(ssize_t x, ssize_t y) override;
                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force) override;
                virtual status_t            add(Widget *widget) override;
                virtual status_t            remove(Widget *widget) override;
        };

    } /* namespace tk */
} /* namespace lsp */


#endif /* LSP_PLUG_IN_TK_WIDGETS_CONTAINERS_TAB_H_ */
