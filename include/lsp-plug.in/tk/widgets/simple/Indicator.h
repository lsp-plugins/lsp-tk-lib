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
                prop::Integer       sTextGap;       // Text gap for loop
                prop::Boolean       sLoop;          // Loop flag
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
                inline Color                   *color()             { return &sColor;       }
                inline const Color             *color() const       { return &sColor;       }

                inline Color                   *text_color()        { return &sTextColor;   }
                inline const Color             *text_color() const  { return &sTextColor;   }

                inline Integer                 *rows()              { return &sRows;        }
                inline const Integer           *rows() const        { return &sRows;        }

                inline Integer                 *columns()           { return &sColumns;     }
                inline const Integer           *columns() const     { return &sColumns;     }

                inline Integer                 *text_shift()        { return &sShift;       }
                inline const Integer           *text_shift() const  { return &sShift;       }

                inline Integer                 *text_gap()          { return &sTextGap;     }
                inline const Integer           *text_gap() const    { return &sTextGap;     }

                inline Boolean                 *text_loop()         { return &sLoop;        }
                inline const Boolean           *text_loop() const   { return &sLoop;        }

                inline String                  *text()              { return &sText;        }
                inline const String            *text() const        { return &sText;        }

            public:
                virtual void        draw(ws::ISurface *s);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPINDICATOR_H_ */
