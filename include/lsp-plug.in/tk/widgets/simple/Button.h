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
                prop::Boolean           sTextClip;
                prop::Padding           sTextPadding;

            protected:
                void                update_mode(button_mode_t mode);

                static status_t     slot_on_change(Widget *sender, void *ptr, void *data);
                static status_t     slot_on_submit(Widget *sender, void *ptr, void *data);
                static ws::IGradient   *create_gradient(ws::ISurface *s, ws::rectangle_t &r, size_t pressed, float r1, float r2);

            protected:
                virtual void        size_request(ws::size_limit_t *r);
                virtual void        realize(const ws::rectangle_t *r);
                virtual void        property_changed(Property *prop);

            public:
                explicit Button(Display *dpy);
                virtual ~Button();

                virtual status_t                init();

            public:
                LSP_TK_PROPERTY(Color,              color,              &sColor)
                LSP_TK_PROPERTY(Color,              led_color,          &sLightColor)
                LSP_TK_PROPERTY(Color,              text_color,         &sTextColor)
                LSP_TK_PROPERTY(Color,              led_text_color,     &sLTextColor)
                LSP_TK_PROPERTY(Color,              hole_color,         &sHoleColor)
                LSP_TK_PROPERTY(Font,               font,               &sFont)
                LSP_TK_PROPERTY(String,             text,               &sText)
                LSP_TK_PROPERTY(SizeConstraints,    constraints,        &sConstraints)
                LSP_TK_PROPERTY(ButtonMode,         mode,               &sMode)
                LSP_TK_PROPERTY(Boolean,            down,               &sDown)
                LSP_TK_PROPERTY(Integer,            led,                &sLed)
                LSP_TK_PROPERTY(Boolean,            editable,           &sEditable)
                LSP_TK_PROPERTY(Boolean,            hole,               &sHole)
                LSP_TK_PROPERTY(Boolean,            flat,               &sFlat)
                LSP_TK_PROPERTY(TextLayout,         text_layouts,       &sTextLayout)
                LSP_TK_PROPERTY(Boolean,            text_clip,          &sTextClip)
                LSP_TK_PROPERTY(Padding,            text_padding,       &sTextPadding)

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
