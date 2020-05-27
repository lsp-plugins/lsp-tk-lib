/*
 * Label.h
 *
 *  Created on: 6 июл. 2017 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_LABEL_H_
#define LSP_PLUG_IN_TK_WIDGETS_LABEL_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Label: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                prop::Float                 sHAlign;        // Horizontal alignment of the font
                prop::Float                 sVAlign;        // Vertical alignment of the font
                prop::Font                  sFont;          // Font parameters
                prop::Color                 sColor;         // Font color
                prop::String                sText;          // Text to display
                prop::SizeConstraints       sConstraints;   // Size constraints

            protected:
                virtual void                    size_request(ws::size_limit_t *r);
                virtual void                    property_changed(Property *prop);

            public:
                explicit Label(Display *dpy);
                virtual ~Label();

                virtual status_t                init();

            public:
                inline Float                   *halign()            { return &sHAlign;      }
                inline const Float             *halign() const      { return &sHAlign;      }

                inline Float                   *valign()            { return &sVAlign;      }
                inline const Float             *valign() const      { return &sVAlign;      }

                inline Font                    *font()              { return &sFont;        }
                inline const Font              *font() const        { return &sFont;        }

                inline Color                   *color()             { return &sColor;       }
                inline const Color             *color() const       { return &sColor;       }

                inline String                  *text()              { return &sText;        }
                inline const String            *text() const        { return &sText;        }

                inline SizeConstraints         *constraints()       { return &sConstraints; }
                inline const SizeConstraints   *constraints() const { return &sConstraints; }

            public:
                virtual void                    draw(ws::ISurface *s);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_LABEL_H_ */
