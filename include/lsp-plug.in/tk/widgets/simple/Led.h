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
                LSP_TK_PROPERTY(Color,              color,              &sColor)
                LSP_TK_PROPERTY(Color,              hole_color,         &sHoleColor)
                LSP_TK_PROPERTY(Color,              led_color,          &sLightColor)
                LSP_TK_PROPERTY(SizeRange,          size,               &sSizeRange)
                LSP_TK_PROPERTY(Boolean,            on,                 &sOn)
                LSP_TK_PROPERTY(Boolean,            hole,               &sHole)
                LSP_TK_PROPERTY(Integer,            led,                &sLed)

            public:
                virtual void                    draw(ws::ISurface *s);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_LED_H_ */
