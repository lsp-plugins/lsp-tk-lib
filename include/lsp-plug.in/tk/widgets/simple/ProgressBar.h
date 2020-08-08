/*
 * ProgressBar.h
 *
 *  Created on: 2 июл. 2019 г.
 *      Author: sadko
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
        
        class ProgressBar: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                ws::rectangle_t                 sTextArea;          // Actual text area used

                prop::RangeFloat                sValue;             // The actual progress value
                prop::SizeConstraints           sConstraints;       // Size constraings
                prop::String                    sText;              // Text to display
                prop::TextLayout                sTextLayout;        // Text layout
                prop::Boolean                   sShowText;          // Show text
                prop::Font                      sFont;              // Font of the text
                prop::Color                     sBorderColor;       // Color of the border
                prop::Color                     sBorderGapColor;    // Color of the border gap
                prop::Integer                   sBorderSize;        // Size of the border
                prop::Integer                   sBorderGapSize;     // Size of the border gap
                prop::Integer                   sBorderRadius;      // Radius of the border
                prop::Color                     sColor;             // Color of the progress slider
                prop::Color                     sTextColor;         // Text color over the progress slider
                prop::Color                     sInvColor;          // Inverse color of the progress slider
                prop::Color                     sInvTextColor;      // Inverse text color over the progress slider

            protected:
                virtual void                    size_request(ws::size_limit_t *r);
                virtual void                    property_changed(Property *prop);
                virtual void                    realize(const ws::rectangle_t *r);

            protected:
                void                            out_text(ws::ISurface *s, const LSPString *text, lsp::Color &color);

            public:
                explicit ProgressBar(Display *dpy);
                virtual ~ProgressBar();

                virtual status_t                init();

            public:
                LSP_TK_PROPERTY(RangeFloat,         value,                      &sValue)
                LSP_TK_PROPERTY(SizeConstraints,    constraints,                &sConstraints)
                LSP_TK_PROPERTY(String,             text,                       &sText)
                LSP_TK_PROPERTY(TextLayout,         text_layout,                &sTextLayout)
                LSP_TK_PROPERTY(Boolean,            show_text,                  &sShowText)
                LSP_TK_PROPERTY(Font,               font,                       &sFont)
                LSP_TK_PROPERTY(Color,              border_color,               &sBorderColor)
                LSP_TK_PROPERTY(Color,              border_gap_color,           &sBorderGapColor)
                LSP_TK_PROPERTY(Integer,            border_size,                &sBorderSize)
                LSP_TK_PROPERTY(Integer,            border_gap_size,            &sBorderGapSize)
                LSP_TK_PROPERTY(Integer,            border_radius,              &sBorderRadius)
                LSP_TK_PROPERTY(Color,              color,                      &sColor)
                LSP_TK_PROPERTY(Color,              text_color,                 &sTextColor)
                LSP_TK_PROPERTY(Color,              inv_color,                  &sInvColor)
                LSP_TK_PROPERTY(Color,              inv_text_color,             &sInvTextColor)

            public:
                virtual void                    draw(ws::ISurface *s);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_LSPPROGRESSBAR_H_ */
