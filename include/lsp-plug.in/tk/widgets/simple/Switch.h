/*
 * Switch.h
 *
 *  Created on: 1 июл. 2017 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_SWITCH_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_SWITCH_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Switch: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum state_t
                {
                    S_PRESSED   = (1 << 0),
                    S_TOGGLED   = (1 << 1)
                };

            protected:
                size_t                      nState;
                size_t                      nBMask;
                ws::rectangle_t             sButton;
                ws::mouse_pointer_t         enPointer;

                prop::Color                 sColor;
                prop::Color                 sTextColor;
                prop::Color                 sBorderColor;
                prop::Color                 sHoleColor;
                prop::Integer               sBorder;
                prop::SizeRange             sSizeRange;
                prop::Float                 sAspect;
                prop::Integer               sAngle;
                prop::Boolean               sDown;
                prop::Pointer               sButtonPointer;

            protected:
                bool                        check_mouse_over(ssize_t x, ssize_t y);

                void                        sync_state(bool down);

            protected:
                static status_t             slot_on_change(Widget *sender, void *ptr, void *data);

            protected:
                virtual void                size_request(ws::size_limit_t *r);
                virtual void                property_changed(Property *prop);
                virtual void                realize(const ws::rectangle_t *r);

            public:
                explicit Switch(Display *dpy);
                virtual ~Switch();

                virtual status_t            init();

            public:
                LSP_TK_PROPERTY(Color,              color,                      &sColor)
                LSP_TK_PROPERTY(Color,              text_color,                 &sTextColor)
                LSP_TK_PROPERTY(Color,              border_color,               &sBorderColor)
                LSP_TK_PROPERTY(Color,              hole_color,                 &sHoleColor)
                LSP_TK_PROPERTY(SizeRange,          size,                       &sSizeRange)
                LSP_TK_PROPERTY(Integer,            border,                     &sBorder)
                LSP_TK_PROPERTY(Float,              aspect,                     &sAspect)
                LSP_TK_PROPERTY(Integer,            angle,                      &sAngle)
                LSP_TK_PROPERTY(Boolean,            down,                       &sDown)
                LSP_TK_PROPERTY(Pointer,            button_pointer,             &sButtonPointer)

            public:
                virtual ws::mouse_pointer_t     current_pointer();

                virtual void                draw(ws::ISurface *s);

                virtual status_t            on_mouse_down(const ws::event_t *e);

                virtual status_t            on_mouse_up(const ws::event_t *e);

                virtual status_t            on_mouse_move(const ws::event_t *e);

                virtual status_t            on_change(bool set);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_SWITCH_H_ */
