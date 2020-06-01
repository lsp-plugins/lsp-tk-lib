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

                prop::Color                 sColor;
                prop::Color                 sTextColor;
                prop::Color                 sBorderColor;
                prop::Color                 sHoleColor;
                prop::Integer               sBorder;
                prop::SizeRange             sSizeRange;
                prop::Float                 sAspect;
                prop::Integer               sAngle;
                prop::Boolean               sDown;

            protected:
                bool                        check_mouse_over(ssize_t x, ssize_t y);

                void                        on_click(bool down);
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

                virtual status_t    init();

            public:
                inline Color               *color()                 { return &sColor;       }
                inline const Color         *color() const           { return &sColor;       }

                inline Color               *text_color()            { return &sTextColor;   }
                inline const Color         *text_color() const      { return &sTextColor;   }

                inline Color               *border_color()          { return &sBorderColor; }
                inline const Color         *border_color() const    { return &sBorderColor; }

                inline Color               *hole_color()            { return &sHoleColor;   }
                inline const Color         *hole_color() const      { return &sHoleColor;   }

                inline SizeRange           *size()                  { return &sSizeRange;   }
                inline const SizeRange     *size() const            { return &sSizeRange;   }

                inline Integer             *border()                { return &sBorder;      }
                inline const Integer       *border() const          { return &sBorder;      }

                inline Float               *aspect()                { return &sAspect;      }
                inline const Float         *aspect() const          { return &sAspect;      }

                inline Integer             *angle()                 { return &sAngle;       }
                inline const Integer       *angle() const           { return &sAngle;       }

                inline Boolean             *down()                  { return &sDown;        }
                inline const Boolean       *down() const            { return &sDown;        }


            public:
                virtual void                draw(ws::ISurface *s);

                virtual status_t            on_mouse_down(const ws::event_t *e);

                virtual status_t            on_mouse_up(const ws::event_t *e);

                virtual status_t            on_mouse_move(const ws::event_t *e);

                virtual status_t            on_change(bool set);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_SWITCH_H_ */
