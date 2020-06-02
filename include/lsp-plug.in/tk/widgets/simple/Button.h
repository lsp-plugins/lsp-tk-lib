/*
 * Button.h
 *
 *  Created on: 21 июн. 2017 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_BUTTON_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_BUTTON_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Button control widget
         */
        class Button: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum state_t
                {
                    S_PRESSED   = (1 << 0),
                    S_TOGGLED   = (1 << 1),
                    S_OUT       = (1 << 2),
                    S_LED       = (1 << 3),
                    S_TRIGGER   = (1 << 4),
                    S_TOGGLE    = (1 << 5),
                    S_DOWN      = (1 << 6),
                    S_EDITABLE  = (1 << 7),
                    S_HOLE      = (1 << 8),
                    S_FLAT      = (1 << 9)
                };

            protected:
                size_t                  nState;
                size_t                  nBMask;
                size_t                  nChanges;
                ws::rectangle_t         sButton;

                prop::Color             sColor;
                prop::Color             sLightColor;
                prop::Color             sTextColor;
                prop::Color             sLTextColor;
                prop::Color             sHoleColor;
                prop::Font              sFont;
                prop::String            sText;
                prop::SizeConstraints   sConstraints;
                prop::TextLayout        sTextLayout;
                prop::ButtonMode        sMode;
                prop::Boolean           sDown;
                prop::Integer           sLed;
                prop::Boolean           sEditable;
                prop::Boolean           sHole;
                prop::Boolean           sFlat;

            protected:
                bool                check_mouse_over(ssize_t x, ssize_t y);
                void                update_mode(button_mode_t mode);

                static status_t     slot_on_change(Widget *sender, void *ptr, void *data);
                static status_t     slot_on_submit(Widget *sender, void *ptr, void *data);

            protected:
                virtual void        size_request(ws::size_limit_t *r);
                virtual void        realize(const ws::rectangle_t *r);
                virtual void        property_changed(Property *prop);

            public:
                explicit Button(Display *dpy);
                virtual ~Button();

                virtual status_t                init();

            public:
                inline Color                   *color()                     { return &sColor;       }
                inline const Color             *color() const               { return &sColor;       }

                inline Color                   *light_color()               { return &sLightColor;  }
                inline const Color             *light_color() const         { return &sLightColor;  }

                inline Color                   *text_color()                { return &sTextColor;   }
                inline const Color             *text_color() const          { return &sTextColor;   }

                inline Color                   *light_text_color()          { return &sLTextColor;  }
                inline const Color             *light_text_color() const    { return &sLTextColor;  }

                inline Color                   *hole_color()                { return &sHoleColor;   }
                inline const Color             *hole_color() const          { return &sHoleColor;   }

                inline Font                    *font()                      { return &sFont;        }
                inline const Font              *font() const                { return &sFont;        }

                inline String                  *text()                      { return &sText;        }
                inline const String            *text() const                { return &sText;        }

                inline SizeConstraints         *constraints()               { return &sConstraints; }
                inline const SizeConstraints   *constraints() const         { return &sConstraints; }

                inline ButtonMode              *mode()                      { return &sMode;        }
                inline const ButtonMode        *mode() const                { return &sMode;        }

                inline Boolean                 *down()                      { return &sDown;        }
                inline const Boolean           *down() const                { return &sDown;        }

                inline Integer                 *led()                       { return &sLed;         }
                inline const Integer           *led() const                 { return &sLed;         }

                inline Boolean                 *editable()                  { return &sEditable;    }
                inline const Boolean           *editable() const            { return &sEditable;    }

                inline Boolean                 *hole()                      { return &sHole;        }
                inline const Boolean           *hole() const                { return &sHole;        }

                inline Boolean                 *flat()                      { return &sFlat;        }
                inline const Boolean           *flat() const                { return &sFlat;        }

                inline TextLayout              *text_layout()               { return &sTextLayout;  }
                inline const TextLayout        *text_layout() const         { return &sTextLayout;  }

            public:
                virtual void        draw(ws::ISurface *s);

                virtual status_t    on_mouse_down(const ws::event_t *e);

                virtual status_t    on_mouse_up(const ws::event_t *e);

                virtual status_t    on_mouse_move(const ws::event_t *e);

                virtual status_t    on_change();

                virtual status_t    on_submit();
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_BUTTON_H_ */
