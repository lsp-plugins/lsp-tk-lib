/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 2 июл. 2019 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_PROGRESSBAR_H_
#define LSP_PLUG_IN_TK_WIDGETS_PROGRESSBAR_H_

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
            typedef struct ProgressBarColors
            {
                prop::Color                     sColor;             // Color of the progress slider
                prop::Color                     sTextColor;         // Text color over the progress slider
                prop::Color                     sInvColor;          // Inverse color of the progress slider
                prop::Color                     sInvTextColor;      // Inverse text color over the progress slider
                prop::Color                     sBorderColor;       // Color of the border
                prop::Color                     sBorderGapColor;    // Color of the border gap

                void listener(tk::prop::Listener *listener);
                bool property_changed(Property *prop);
            } ProgressBarColors;

            enum ProgressBarColorState
            {
                PROGRESSBAR_NORMAL          = 0,
                PROGRESSBAR_INACTIVE        = 1 << 0,

                PROGRESSBAR_TOTAL           = 1 << 1
            };

            LSP_TK_STYLE_DEF_BEGIN(ProgressBar, Widget)
                ProgressBarColors               vColors[PROGRESSBAR_TOTAL];
                prop::RangeFloat                sValue;             // The actual progress value
                prop::SizeConstraints           sConstraints;       // Size constraings
                prop::String                    sText;              // Text to display
                prop::TextLayout                sTextLayout;        // Text layout
                prop::Boolean                   sShowText;          // Show text
                prop::Boolean                   sActive;            // Activity flag
                prop::Font                      sFont;              // Font of the text
                prop::Integer                   sBorderSize;        // Size of the border
                prop::Integer                   sBorderGapSize;     // Size of the border gap
                prop::Integer                   sBorderRadius;      // Radius of the border
            LSP_TK_STYLE_DEF_END
        }
        
        class ProgressBar: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum pbar_flags_t
                {
                    PBAR_0          = style::PROGRESSBAR_NORMAL,
                    PBAR_1          = style::PROGRESSBAR_INACTIVE,
                    PBAR_TOTAL      = style::PROGRESSBAR_TOTAL
                };

            protected:
                ws::rectangle_t                 sTextArea;          // Actual text area used

                style::ProgressBarColors        vColors[PBAR_TOTAL];
                prop::RangeFloat                sValue;             // The actual progress value
                prop::SizeConstraints           sConstraints;       // Size constraings
                prop::String                    sText;              // Text to display
                prop::TextLayout                sTextLayout;        // Text layout
                prop::Boolean                   sShowText;          // Show text
                prop::Boolean                   sActive;            // Activity flag
                prop::Font                      sFont;              // Font of the text
                prop::Integer                   sBorderSize;        // Size of the border
                prop::Integer                   sBorderGapSize;     // Size of the border gap
                prop::Integer                   sBorderRadius;      // Radius of the border

            protected:
                virtual void                    size_request(ws::size_limit_t *r) override;
                virtual void                    property_changed(Property *prop) override;
                virtual void                    realize(const ws::rectangle_t *r) override;

            protected:
                void                            out_text(ws::ISurface *s, const LSPString *text, lsp::Color &color);
                style::ProgressBarColors       *select_colors();

            public:
                explicit ProgressBar(Display *dpy);
                ProgressBar(const ProgressBar &) = delete;
                ProgressBar(ProgressBar &&) = delete;
                virtual ~ProgressBar() override;
                ProgressBar & operator = (const ProgressBar &) = delete;
                ProgressBar & operator = (ProgressBar &&) = delete;

                virtual status_t                init() override;

            public:
                LSP_TK_PROPERTY(Color,              color,                      &vColors[PBAR_0].sColor)
                LSP_TK_PROPERTY(Color,              text_color,                 &vColors[PBAR_0].sTextColor)
                LSP_TK_PROPERTY(Color,              inv_color,                  &vColors[PBAR_0].sInvColor)
                LSP_TK_PROPERTY(Color,              inv_text_color,             &vColors[PBAR_0].sInvTextColor)
                LSP_TK_PROPERTY(Color,              border_color,               &vColors[PBAR_0].sBorderColor)
                LSP_TK_PROPERTY(Color,              border_gap_color,           &vColors[PBAR_0].sBorderGapColor)
                LSP_TK_PROPERTY(Color,              inactive_color,             &vColors[PBAR_1].sColor)
                LSP_TK_PROPERTY(Color,              inactive_text_color,        &vColors[PBAR_1].sTextColor)
                LSP_TK_PROPERTY(Color,              inactive_inv_color,         &vColors[PBAR_1].sInvColor)
                LSP_TK_PROPERTY(Color,              inactive_inv_text_color,    &vColors[PBAR_1].sInvTextColor)
                LSP_TK_PROPERTY(Color,              inactive_border_color,      &vColors[PBAR_1].sBorderColor)
                LSP_TK_PROPERTY(Color,              inactive_border_gap_color,  &vColors[PBAR_1].sBorderGapColor)

                LSP_TK_PROPERTY(RangeFloat,         value,                      &sValue)
                LSP_TK_PROPERTY(SizeConstraints,    constraints,                &sConstraints)
                LSP_TK_PROPERTY(String,             text,                       &sText)
                LSP_TK_PROPERTY(TextLayout,         text_layout,                &sTextLayout)
                LSP_TK_PROPERTY(Boolean,            show_text,                  &sShowText)
                LSP_TK_PROPERTY(Boolean,            active,                     &sActive)
                LSP_TK_PROPERTY(Font,               font,                       &sFont)
                LSP_TK_PROPERTY(Integer,            border_size,                &sBorderSize)
                LSP_TK_PROPERTY(Integer,            border_gap_size,            &sBorderGapSize)
                LSP_TK_PROPERTY(Integer,            border_radius,              &sBorderRadius)

            public:
                virtual void                    draw(ws::ISurface *s, bool force) override;
        };

    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_LSPPROGRESSBAR_H_ */
