/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 4 дек. 2024 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_TABITEM_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_TABITEM_H_

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
            typedef struct TabItemColors
            {
                prop::Color                 sColor;                 // Heading tab color
                prop::Color                 sBorderColor;           // Heading tab border color
                prop::Color                 sTextColor;             // Heading tab text color

                void listener(tk::prop::Listener *listener);
                bool property_changed(Property *prop);
            } TabItemColors;

            enum TabItemColorState
            {
                TABITEM_NORMAL          = 0,
                TABITEM_SELECTED        = 1 << 0,
                TABITEM_HOVER           = 1 << 1,
                TABITEM_INACTIVE        = 1 << 2,

                TABITEM_TOTAL           = 1 << 3
            };

            LSP_TK_STYLE_DEF_BEGIN(TabItem, Widget)
                TabItemColors               vColors[TABITEM_TOTAL];

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

        class TabItem: public Widget
        {
            public:
                static const w_class_t    metadata;

            private:
                friend class TabGroup;

            protected:
                enum titm_flags_t
                {
                    TITM_0          = style::TABITEM_NORMAL,
                    TITM_1          = style::TABITEM_SELECTED,
                    TITM_2          = style::TABITEM_HOVER,
                    TITM_3          = style::TABITEM_SELECTED | style::TABITEM_HOVER,
                    TITM_4          = style::TABITEM_INACTIVE,
                    TITM_5          = style::TABITEM_SELECTED | style::TABITEM_INACTIVE,
                    TITM_6          = style::TABITEM_HOVER | style::TABITEM_INACTIVE,
                    TITM_7          = style::TABITEM_SELECTED | style::TABITEM_HOVER | style::TABITEM_INACTIVE,

                    TITM_TOTAL      = style::TABITEM_TOTAL
                };

            protected:
                Widget                     *pWidget;

                style::TabItemColors        vColors[style::TABITEM_TOTAL];
                prop::String                sText;                  // Heading tab text
                prop::TextAdjust            sTextAdjust;            // Heading tab text adjustment
                prop::TextLayout            sTextLayout;            // Heading tab text layout
                prop::Padding               sTextPadding;           // Heading tab text padding
                prop::Font                  sFont;                  // Heading tab font
                prop::Integer               sBorderSize;            // Border size of the heading tab
                prop::Integer               sBorderRadius;          // Border radius of the heading tab
                prop::Boolean               sActive;                // Activity flag

            protected:
                style::TabItemColors   *select_colors(bool selected, bool hover);

            protected:
                virtual void            property_changed(Property *prop) override;

            public:
                explicit TabItem(Display *dpy);
                TabItem(const TabItem &) = delete;
                TabItem(TabItem &&) = delete;
                virtual ~TabItem() override;

                TabItem & operator = (const TabItem &) = delete;
                TabItem & operator = (TabItem &&) = delete;

                virtual status_t            init() override;

            public:
                LSP_TK_PROPERTY(Color,              color,                                  &vColors[TITM_0].sColor);
                LSP_TK_PROPERTY(Color,              border_color,                           &vColors[TITM_0].sBorderColor);
                LSP_TK_PROPERTY(Color,              text_color,                             &vColors[TITM_0].sTextColor);
                LSP_TK_PROPERTY(Color,              selected_color,                         &vColors[TITM_1].sColor);
                LSP_TK_PROPERTY(Color,              border_selected_color,                  &vColors[TITM_1].sBorderColor);
                LSP_TK_PROPERTY(Color,              text_selected_color,                    &vColors[TITM_1].sTextColor);
                LSP_TK_PROPERTY(Color,              hover_color,                            &vColors[TITM_2].sColor);
                LSP_TK_PROPERTY(Color,              border_hover_color,                     &vColors[TITM_2].sBorderColor);
                LSP_TK_PROPERTY(Color,              text_hover_color,                       &vColors[TITM_2].sTextColor);
                LSP_TK_PROPERTY(Color,              hover_selected_color,                   &vColors[TITM_3].sColor);
                LSP_TK_PROPERTY(Color,              border_hover_selected_color,            &vColors[TITM_3].sBorderColor);
                LSP_TK_PROPERTY(Color,              text_hover_selected_color,              &vColors[TITM_3].sTextColor);
                LSP_TK_PROPERTY(Color,              inactive_color,                         &vColors[TITM_4].sColor);
                LSP_TK_PROPERTY(Color,              inactive_border_color,                  &vColors[TITM_4].sBorderColor);
                LSP_TK_PROPERTY(Color,              inactive_text_color,                    &vColors[TITM_4].sTextColor);
                LSP_TK_PROPERTY(Color,              inactive_selected_color,                &vColors[TITM_5].sColor);
                LSP_TK_PROPERTY(Color,              inactive_border_selected_color,         &vColors[TITM_5].sBorderColor);
                LSP_TK_PROPERTY(Color,              inactive_text_selected_color,           &vColors[TITM_5].sTextColor);
                LSP_TK_PROPERTY(Color,              inactive_hover_color,                   &vColors[TITM_6].sColor);
                LSP_TK_PROPERTY(Color,              inactive_border_hover_color,            &vColors[TITM_6].sBorderColor);
                LSP_TK_PROPERTY(Color,              inactive_text_hover_color,              &vColors[TITM_6].sTextColor);
                LSP_TK_PROPERTY(Color,              inactive_hover_selected_color,          &vColors[TITM_7].sColor);
                LSP_TK_PROPERTY(Color,              inactive_border_hover_selected_color,   &vColors[TITM_7].sBorderColor);
                LSP_TK_PROPERTY(Color,              inactive_text_hover_selected_color,     &vColors[TITM_7].sTextColor);

                LSP_TK_PROPERTY(String,             text,                                   &sText);
                LSP_TK_PROPERTY(TextAdjust,         text_adjust,                            &sTextAdjust);
                LSP_TK_PROPERTY(TextLayout,         text_layout,                            &sTextLayout);
                LSP_TK_PROPERTY(Padding,            text_padding,                           &sTextPadding);
                LSP_TK_PROPERTY(Font,               font,                                   &sFont);
                LSP_TK_PROPERTY(Integer,            border_size,                            &sBorderSize);
                LSP_TK_PROPERTY(Integer,            border_radius,                          &sBorderRadius);
                LSP_TK_PROPERTY(Boolean,            active,                                 &sActive);
        };

    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_TABITEM_H_ */
