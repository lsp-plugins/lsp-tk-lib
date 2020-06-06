/*
 * Indicator.h
 *
 *  Created on: 7 июл. 2017 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_INDICATOR_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_INDICATOR_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Indicator: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                prop::Color         sColor;         // Color of the indicator
                prop::Color         sTextColor;     // Color of the text
                prop::Integer       sRows;          // Number of rows
                prop::Integer       sColumns;       // Number of columns
                prop::Integer       sShift;         // Text shift
                prop::Integer       sLoop;          // Loop flag
                prop::String        sText;          // Actual text to display

            protected:
                void                draw_digit(ws::ISurface *s, float x, float y, size_t state, const lsp::Color &on, const lsp::Color &off);
                uint8_t             get_char(const LSPString *str, size_t index);

            protected:
                virtual void        size_request(ws::size_limit_t *r);
                virtual void        property_changed(Property *prop);

            public:
                explicit            Indicator(Display *dpy);
                virtual             ~Indicator();

                virtual status_t    init();

            public:
                virtual void        draw(ws::ISurface *s);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPINDICATOR_H_ */
