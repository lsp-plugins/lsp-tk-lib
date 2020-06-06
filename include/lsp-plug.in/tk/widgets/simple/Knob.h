/*
 * Knob.h
 *
 *  Created on: 10 июл. 2017 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_KNOB_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_KNOB_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Knob: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum state_t
                {
                    S_NONE,
                    S_MOVING,
                    S_CLICK
                };

            protected:
                ssize_t             nLastY;
                size_t              nState;
                size_t              nButtons;

                prop::Color         sColor;
                prop::Color         sScaleColor;
                prop::Color         sHoleColor;
                prop::Color         sTipColor;
                prop::SizeRange     sSizeRange;
                prop::Float         sScale;

                prop::RangeFloat    sValue;
                prop::StepFloat     sStep;
                prop::Float         sBalance;
                prop::Boolean       sCycling;

            protected:
                size_t                          check_mouse_over(ssize_t x, ssize_t y);
                void                            update_value(float delta);
                void                            on_click(ssize_t x, ssize_t y);

            protected:
                static status_t                 slot_on_change(Widget *sender, void *ptr, void *data);

            protected:
                virtual void                    size_request(ws::size_limit_t *r);
                virtual void                    property_changed(Property *prop);

            public:
                explicit Knob(Display *dpy);
                virtual ~Knob();

                virtual status_t                init();

            public:
                inline Color                   *color()                 { return &sColor;       }
                inline const Color             *color() const           { return &sColor;       }

                inline Color                   *scale_color()           { return &sScaleColor;  }
                inline const Color             *scale_color() const     { return &sScaleColor;  }

                inline Color                   *hole_color()            { return &sHoleColor;   }
                inline const Color             *hole_color() const      { return &sHoleColor;   }

                inline Color                   *tip_color()             { return &sTipColor;    }
                inline const Color             *tip_color() const       { return &sTipColor;    }

                inline SizeRange               *size()                  { return &sSizeRange;   }
                inline const SizeRange         *size() const            { return &sSizeRange;   }

                inline Float                   *scale()                 { return &sScale;       }
                inline const Float             *scale() const           { return &sScale;       }

                inline RangeFloat              *value()                 { return &sValue;       }
                inline const RangeFloat        *value() const           { return &sValue;       }

                inline StepFloat               *step()                  { return &sStep;        }
                inline const StepFloat         *step() const            { return &sStep;        }

                inline Float                   *balance()               { return &sBalance;     }
                inline const Float             *balance() const         { return &sBalance;     }

                inline Boolean                 *cycling()               { return &sCycling;     }
                inline const Boolean           *cycling() const         { return &sCycling;     }

            public:
                virtual status_t                on_mouse_down(const ws::event_t *e);

                virtual status_t                on_mouse_up(const ws::event_t *e);

                virtual status_t                on_mouse_move(const ws::event_t *e);

                virtual status_t                on_mouse_scroll(const ws::event_t *e);

                virtual status_t                on_change();

                virtual void                    draw(ws::ISurface *s);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_KNOB_H_ */
