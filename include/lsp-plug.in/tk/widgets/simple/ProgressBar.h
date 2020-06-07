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
                inline RangeFloat              *value()                 { return &sValue;           }
                inline const RangeFloat        *value() const           { return &sValue;           }

                inline SizeConstraints         *constraints()           { return &sConstraints;     }
                inline const SizeConstraints   *constraints() const     { return &sConstraints;     }

                inline String                  *text()                  { return &sText;            }
                inline const String            *text() const            { return &sText;            }

                inline TextLayout              *text_layout()           { return &sTextLayout;      }
                inline const TextLayout        *text_layout() const     { return &sTextLayout;      }

                inline Boolean                 *show_text()             { return &sShowText;        }
                inline const Boolean           *show_text() const       { return &sShowText;        }

                inline Font                    *font()                  { return &sFont;            }
                inline const Font              *font() const            { return &sFont;            }

                inline Color                   *border_color()          { return &sBorderColor;     }
                inline const Color             *border_color() const    { return &sBorderColor;     }

                inline Color                   *border_gap_color()      { return &sBorderGapColor;  }
                inline const Color             *border_gap_color() const{ return &sBorderGapColor;  }

                inline Integer                 *border_size()           { return &sBorderSize;      }
                inline const Integer           *border_size() const     { return &sBorderSize;      }

                inline Integer                 *border_gap_size()       { return &sBorderGapSize;   }
                inline const Integer           *border_gap_size() const { return &sBorderGapSize;   }

                inline Integer                 *border_radius()         { return &sBorderRadius;    }
                inline const Integer           *border_radius() const   { return &sBorderRadius;    }

                inline Color                   *color()                 { return &sColor;           }
                inline const Color             *color() const           { return &sColor;           }

                inline Color                   *text_color()            { return &sTextColor;       }
                inline const Color             *text_color() const      { return &sTextColor;       }

                inline Color                   *inv_color()             { return &sInvColor;        }
                inline const Color             *inv_color() const       { return &sInvColor;        }

                inline Color                   *inv_text_color()        { return &sInvTextColor;    }
                inline const Color             *inv_text_color() const  { return &sInvTextColor;    }

            public:
                virtual void                    draw(ws::ISurface *s);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_LSPPROGRESSBAR_H_ */
