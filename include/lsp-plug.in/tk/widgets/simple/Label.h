/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 6 июл. 2017 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LABEL_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LABEL_H_

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
            typedef struct LabelColors
            {
                prop::Color             sColor;

                void listener(tk::prop::Listener *listener);
                bool property_changed(Property *prop) const;
            } LabelColors;

            enum LabelColorState
            {
                LABEL_NORMAL        = 0,
                LABEL_HOVER         = 1 << 0,
                LABEL_INACTIVE      = 1 << 1,

                LABEL_TOTAL         = 1 << 2
            };

            LSP_TK_STYLE_DEF_BEGIN(Label, Widget)
                LabelColors                 vColors[LABEL_TOTAL];

                prop::TextLayout            sTextLayout;    // Text layout
                prop::TextLayout            sClipTextLayout;// Text layout when clipping
                prop::TextAdjust            sTextAdjust;    // Text adjustment
                prop::Font                  sFont;          // Font parameters
                prop::Boolean               sHover;         // Hover enable
                prop::String                sText;          // Text to display
                prop::Boolean               sTextClip;      // Clip text
                prop::SizeConstraints       sConstraints;   // Size constraints
                prop::Padding               sIPadding;      // Internal padding
            LSP_TK_STYLE_DEF_END
        }

        class Label: public Widget
        {
            public:
                static const w_class_t    metadata;

            private:
                friend class MultiLabel;

            protected:
                enum state_t
                {
                    F_MOUSE_IN      = 1 << 0,
                    F_MOUSE_DOWN    = 1 << 1,
                    F_MOUSE_IGN     = 1 << 2,
                };

                enum lbl_flags_t
                {
                    LBL_0       = style::LABEL_NORMAL,
                    LBL_1       = style::LABEL_HOVER,
                    LBL_2       = LBL_0 | style::LABEL_INACTIVE,
                    LBL_3       = LBL_1 | style::LABEL_INACTIVE,

                    LBL_TOTAL   = style::HYPERLINK_TOTAL
                };

                typedef struct estimation_t
                {
                    float scaling;
                    float fscaling;
                    ws::size_limit_t *r;
                    ws::font_parameters_t fp;
                    ws::text_parameters_t tp;
                } estimation_t;

            protected:
                size_t                      nMFlags;
                size_t                      nState;

                style::LabelColors          vColors[style::LABEL_TOTAL];
                prop::TextLayout            sTextLayout;    // Text layout
                prop::TextLayout            sClipTextLayout;// Text layout when clipping
                prop::TextAdjust            sTextAdjust;    // Text adjustment
                prop::Font                  sFont;          // Font parameters
                prop::Boolean               sHover;         // Hover enable
                prop::String                sText;          // Text to display
                prop::Boolean               sTextClip;      // Clip text
                prop::SizeConstraints       sConstraints;   // Size constraints
                prop::Padding               sIPadding;      // Internal padding
                prop::WidgetPtr<Menu>       sPopup;         // Popup menu
                lltl::parray<prop::String>  vEstimations;   // Estimation string

            protected:
                static status_t                 slot_on_submit(Widget *sender, void *ptr, void *data);
                static status_t                 slot_on_before_popup(Widget *sender, void *ptr, void *data);
                static status_t                 slot_on_popup(Widget *sender, void *ptr, void *data);

            protected:
                size_t                          destroy_text_estimations();
                void                            estimate_string_size(estimation_t *e, tk::String *s);
                void                            estimate_string_size(estimation_t *e, const LSPString *s);
                const style::LabelColors       *select_colors() const;
                const style::LabelColors       *select_colors(bool active, bool hover) const;

            protected:
                static bool                     contains_digit(const LSPString *s);
                static void                     set_all_digits(LSPString *s, lsp_wchar_t new_ch);

            protected:
                virtual void                    size_request(ws::size_limit_t *r) override;
                virtual void                    property_changed(Property *prop) override;

            public:
                explicit Label(Display *dpy);
                Label(const Label &) = delete;
                Label(Label &&) = delete;
                virtual ~Label() override;
                Label & operator = (const Label &) = delete;
                Label & operator = (Label &&) = delete;

                virtual status_t                init() override;

            public:
                LSP_TK_PROPERTY(TextLayout,         text_layout,            &sTextLayout)
                LSP_TK_PROPERTY(TextLayout,         clip_text_layout,       &sClipTextLayout)
                LSP_TK_PROPERTY(TextAdjust,         text_adjust,            &sTextAdjust)
                LSP_TK_PROPERTY(Font,               font,                   &sFont)
                LSP_TK_PROPERTY(Color,              color,                  &vColors[LBL_0].sColor)
                LSP_TK_PROPERTY(Color,              hover_color,            &vColors[LBL_1].sColor)
                LSP_TK_PROPERTY(Color,              inactive_color,         &vColors[LBL_2].sColor)
                LSP_TK_PROPERTY(Color,              inactive_hover_color,   &vColors[LBL_3].sColor)
                LSP_TK_PROPERTY(Boolean,            hover,                  &sHover)
                LSP_TK_PROPERTY(String,             text,                   &sText)
                LSP_TK_PROPERTY(Boolean,            text_clip,              &sTextClip)
                LSP_TK_PROPERTY(SizeConstraints,    constraints,            &sConstraints)
                LSP_TK_PROPERTY(Padding,            ipadding,               &sIPadding)
                LSP_TK_PROPERTY(WidgetPtr<Menu>,    popup,                  &sPopup)

            public:
                void                            clear_text_estimations();
                tk::String                     *add_text_estimation();

            public:
                virtual void                    draw(ws::ISurface *s, bool force) override;
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

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LABEL_H_ */
