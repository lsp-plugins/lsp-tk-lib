/*
 * Led.h
 *
 *  Created on: 10 июл. 2017 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LED_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LED_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Led: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                prop::Color             sColor;
                prop::Color             sHoleColor;
                prop::Color             sLightColor;
                prop::SizeRange         sSizeRange;
                prop::Boolean           sOn;
                prop::Boolean           sHole;
                prop::Integer           sLed;

            protected:
                virtual void                    size_request(ws::size_limit_t *r);
                virtual void                    property_changed(Property *prop);

            public:
                explicit Led(Display *dpy);
                virtual ~Led();

                virtual status_t                init();

            public:
                inline Color                   *color()             { return &sColor;       }
                inline const Color             *color() const       { return &sColor;       }

                inline Color                   *hole_color()        { return &sHoleColor;   }
                inline const Color             *hole_color() const  { return &sHoleColor;   }

                inline Color                   *led_color()         { return &sLightColor;  }
                inline const Color             *led_color() const   { return &sLightColor;  }

                inline SizeRange               *size()              { return &sSizeRange;   }
                inline const SizeRange         *size() const        { return &sSizeRange;   }

                inline Boolean                 *on()                { return &sOn;          }
                inline const Boolean           *on() const          { return &sOn;          }

                inline Boolean                 *hole()              { return &sHole;        }
                inline const Boolean           *hole() const        { return &sHole;        }

                inline Integer                 *led()               { return &sLed;         }
                inline const Integer           *led() const         { return &sLed;         }

            public:
                virtual void                    draw(ws::ISurface *s);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LED_H_ */
