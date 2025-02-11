/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 30 июл. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LISTBOXITEM_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LISTBOXITEM_H_

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
            typedef struct ListBoxItemColors
            {
                prop::Color                 sBgColor;
                prop::Color                 sTextColor;

                void listener(tk::prop::Listener *listener);
                bool property_changed(Property *prop);
            } ListBoxItemColors;

            enum ListBoxItemColorState
            {
                LISTBOXITEM_NORMAL          = 0,
                LISTBOXITEM_SELECTED        = 1 << 0,
                LISTBOXITEM_HOVER           = 1 << 1,
                LISTBOXITEM_INACTIVE        = 1 << 2,

                LISTBOXITEM_TOTAL           = 1 << 3
            };

            LSP_TK_STYLE_DEF_BEGIN(ListBoxItem, Widget)
                ListBoxItemColors           vColors[LISTBOXITEM_TOTAL];

                prop::String                sText;
                prop::TextAdjust            sTextAdjust;
                prop::Boolean               sActive;
            LSP_TK_STYLE_DEF_END
        }
        
        class ListBoxItem: public Widget
        {
            public:
                static const w_class_t    metadata;

            private:
                friend class ListBox;

            protected:
                enum litm_flags_t
                {
                    LITM_0          = style::LISTBOXITEM_NORMAL,
                    LITM_1          = style::LISTBOXITEM_SELECTED,
                    LITM_2          = style::LISTBOXITEM_HOVER,
                    LITM_3          = style::LISTBOXITEM_SELECTED | style::LISTBOXITEM_HOVER,
                    LITM_4          = style::LISTBOXITEM_INACTIVE,
                    LITM_5          = style::LISTBOXITEM_SELECTED | style::LISTBOXITEM_INACTIVE,
                    LITM_6          = style::LISTBOXITEM_HOVER | style::LISTBOXITEM_INACTIVE,
                    LITM_7          = style::LISTBOXITEM_SELECTED | style::LISTBOXITEM_HOVER | style::LISTBOXITEM_INACTIVE,

                    LITM_TOTAL      = style::LISTBOXITEM_TOTAL
                };

            protected:
                style::ListBoxItemColors    vColors[style::LISTBOXITEM_TOTAL];
                prop::String                sText;
                prop::TextAdjust            sTextAdjust;
                prop::Boolean               sActive;

            protected:
                style::ListBoxItemColors   *select_colors(bool selected, bool hover);

            protected:
                virtual void                property_changed(Property *prop) override;

            public:
                explicit ListBoxItem(Display *dpy);
                ListBoxItem(const ListBoxItem &) = delete;
                ListBoxItem(ListBoxItem &&) = delete;
                virtual ~ListBoxItem() override;

                ListBoxItem & operator = (const ListBoxItem &) = delete;
                ListBoxItem & operator = (ListBoxItem &&) = delete;

                virtual status_t            init() override;

            public:
                LSP_TK_PROPERTY(Color,      text_color,                         &vColors[LITM_0].sTextColor)
                LSP_TK_PROPERTY(Color,      bg_selected_color,                  &vColors[LITM_1].sBgColor)
                LSP_TK_PROPERTY(Color,      text_selected_color,                &vColors[LITM_1].sTextColor)
                LSP_TK_PROPERTY(Color,      bg_hover_color,                     &vColors[LITM_2].sBgColor)
                LSP_TK_PROPERTY(Color,      text_hover_color,                   &vColors[LITM_2].sTextColor)
                LSP_TK_PROPERTY(Color,      bg_selected_hover_color,            &vColors[LITM_3].sBgColor)
                LSP_TK_PROPERTY(Color,      text_selected_hover_color,          &vColors[LITM_3].sTextColor)
                LSP_TK_PROPERTY(Color,      inactive_bg_color,                  &vColors[LITM_4].sBgColor)
                LSP_TK_PROPERTY(Color,      inactive_text_color,                &vColors[LITM_4].sTextColor)
                LSP_TK_PROPERTY(Color,      inactive_bg_selected_color,         &vColors[LITM_5].sBgColor)
                LSP_TK_PROPERTY(Color,      inactive_text_selected_color,       &vColors[LITM_5].sTextColor)
                LSP_TK_PROPERTY(Color,      inactive_bg_hover_color,            &vColors[LITM_6].sBgColor)
                LSP_TK_PROPERTY(Color,      inactive_text_hover_color,          &vColors[LITM_6].sTextColor)
                LSP_TK_PROPERTY(Color,      inactive_bg_selected_hover_color,   &vColors[LITM_7].sBgColor)
                LSP_TK_PROPERTY(Color,      inactive_text_selected_hover_color, &vColors[LITM_7].sTextColor)

                LSP_TK_PROPERTY(String,     text,                               &sText)
                LSP_TK_PROPERTY(TextAdjust, text_adjust,                        &sTextAdjust)
                LSP_TK_PROPERTY(Boolean,    active,                             &sActive)

        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LISTBOXITEM_H_ */
