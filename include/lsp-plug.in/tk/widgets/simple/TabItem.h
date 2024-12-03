/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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
            LSP_TK_STYLE_DEF_BEGIN(TabItem, Widget)
                prop::String                sText;                  // Heading tab text
                prop::TextAdjust            sTextAdjust;            // Heading tab text adjustment
                prop::TextLayout            sTextLayout;            // Heading tab text layout
                prop::Padding               sTextPadding;           // Heading tab text padding
                prop::Font                  sFont;                  // Heading tab font
                prop::Color                 sColor;                 // Heading tab color
                prop::Color                 sSelectedColor;         // Heading tab selected color
                prop::Color                 sHoverColor;            // Heading tab hover color
                prop::Color                 sBorderColor;           // Heading tab border color
                prop::Color                 sBorderSelectedColor;   // Heading tab selected border color
                prop::Color                 sBorderHoverColor;      // Heading tab hover border color
                prop::Color                 sTextColor;             // Heading tab text color
                prop::Color                 sTextSelectedColor;     // Heading tab text selected color
                prop::Color                 sTextHoverColor;        // Heading tab text hover color
                prop::Integer               sBorderSize;            // Border size of the heading tab
                prop::Integer               sBorderRadius;          // Border radius of the heading tab
            LSP_TK_STYLE_DEF_END
        } /* namespace style */

        class TabItem: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                Widget                     *pWidget;

                prop::String                sText;                  // Heading tab text
                prop::TextAdjust            sTextAdjust;            // Heading tab text adjustment
                prop::TextLayout            sTextLayout;            // Heading tab text layout
                prop::Padding               sTextPadding;           // Heading tab text padding
                prop::Font                  sFont;                  // Heading tab font
                prop::Color                 sColor;                 // Heading tab color
                prop::Color                 sSelectedColor;         // Heading tab selected color
                prop::Color                 sHoverColor;            // Heading tab hover color
                prop::Color                 sBorderColor;           // Heading tab border color
                prop::Color                 sBorderSelectedColor;   // Heading tab selected border color
                prop::Color                 sBorderHoverColor;      // Heading tab hover border color
                prop::Color                 sTextColor;             // Heading tab text color
                prop::Color                 sTextSelectedColor;     // Heading tab text selected color
                prop::Color                 sTextHoverColor;        // Heading tab text hover color
                prop::Integer               sBorderSize;            // Border size of the heading tab
                prop::Integer               sBorderRadius;          // Border radius of the heading tab

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
                LSP_TK_PROPERTY(String,             text,                   &sText);
                LSP_TK_PROPERTY(TextAdjust,         text_adjust,            &sTextAdjust);
                LSP_TK_PROPERTY(TextLayout,         text_layout,            &sTextLayout);
                LSP_TK_PROPERTY(Padding,            text_padding,           &sTextPadding);
                LSP_TK_PROPERTY(Font,               font,                   &sFont);
                LSP_TK_PROPERTY(Color,              color,                  &sColor);
                LSP_TK_PROPERTY(Color,              selected_color,         &sSelectedColor);
                LSP_TK_PROPERTY(Color,              hover_color,            &sHoverColor);
                LSP_TK_PROPERTY(Color,              border_color,           &sBorderColor);
                LSP_TK_PROPERTY(Color,              border_selected_color,  &sBorderSelectedColor);
                LSP_TK_PROPERTY(Color,              border_hover_color,     &sBorderHoverColor);
                LSP_TK_PROPERTY(Color,              text_color,             &sTextColor);
                LSP_TK_PROPERTY(Color,              text_selected_color,    &sTextSelectedColor);
                LSP_TK_PROPERTY(Color,              text_hover_color,       &sTextHoverColor);
                LSP_TK_PROPERTY(Integer,            border_size,            &sBorderSize);
                LSP_TK_PROPERTY(Integer,            border_radius,          &sBorderRadius);
        };

    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_TABITEM_H_ */
