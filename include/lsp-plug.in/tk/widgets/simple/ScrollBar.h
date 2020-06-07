/*
 * ScrollBar.h
 *
 *  Created on: 3 авг. 2017 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_SCROLLBAR_H_
#define LSP_PLUG_IN_TK_WIDGETS_SCROLLBAR_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Scroll bar implementation
         */
        class ScrollBar: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum flags_t
                {
                    F_ACTIVITY_BITS         = 5,

                    F_BTN_UP_ACTIVE         = 1 << 0,
                    F_BTN_DOWN_ACTIVE       = 1 << 1,
                    F_SLIDER_ACTIVE         = 1 << 2,
                    F_SPARE_UP_ACTIVE       = 1 << 3,
                    F_SPARE_DOWN_ACTIVE     = 1 << 4,

                    F_TRG_BTN_UP_ACTIVE     = F_BTN_UP_ACTIVE << F_ACTIVITY_BITS,
                    F_TRG_BTN_DOWN_ACTIVE   = F_BTN_DOWN_ACTIVE << F_ACTIVITY_BITS,
                    F_TRG_SLIDER_ACTIVE     = F_SLIDER_ACTIVE << F_ACTIVITY_BITS,
                    F_TRG_SPARE_UP_ACTIVE   = F_SPARE_UP_ACTIVE << F_ACTIVITY_BITS,
                    F_TRG_SPARE_DOWN_ACTIVE = F_SPARE_DOWN_ACTIVE << F_ACTIVITY_BITS,

                    F_FILL                  = 1 << (F_ACTIVITY_BITS << 1),
                    F_OUTSIDE               = 1 << ((F_ACTIVITY_BITS << 1) + 1),
                    F_PRECISION             = 1 << ((F_ACTIVITY_BITS << 1) + 2),

                    F_ACTIVITY_MASK         = F_BTN_UP_ACTIVE | F_BTN_DOWN_ACTIVE | F_SLIDER_ACTIVE | F_SPARE_UP_ACTIVE | F_SPARE_DOWN_ACTIVE,
                    F_TRG_ACTIVITY_MASK     = F_ACTIVITY_MASK << F_ACTIVITY_BITS,
                    F_ALL_ACTIVITY_MASK     = F_ACTIVITY_MASK | F_TRG_ACTIVITY_MASK
                };

            protected:
                size_t                  nFlags;
                size_t                  nButtons;
                size_t                  nKeys;
                ssize_t                 nLastV;
                float                   fLastValue;
                float                   fCurrValue;
                ws::mouse_pointer_t     enMousePointer;
                ws::rectangle_t         sIncButton;
                ws::rectangle_t         sDecButton;
                ws::rectangle_t         sIncSpace;
                ws::rectangle_t         sDecSpace;
                ws::rectangle_t         sSlider;

                prop::RangeFloat        sValue;
                prop::StepFloat         sStep;
                prop::StepFloat         sAccelStep;

                prop::SizeConstraints   sConstraints;
                prop::Orientation       sOrientation;
                prop::Pointer           sSliderPointer;
                prop::Pointer           sIncPointer;
                prop::Pointer           sDecPointer;
                prop::Integer           sBorderRadius;
                prop::Integer           sBorderSize;
                prop::Integer           sBorderGap;
                prop::Integer           sSliderBorderSize;

                prop::Color             sButtonColor;
                prop::Color             sButtonActiveColor;
                prop::Color             sIncColor;
                prop::Color             sIncActiveColor;
                prop::Color             sDecColor;
                prop::Color             sDecActiveColor;
                prop::Color             sBorderColor;
                prop::Color             sBorderGapColor;
                prop::Color             sSliderColor;
                prop::Color             sSliderBorderColor;
                prop::Color             sSliderActiveColor;

                Timer                   sTimer;

            protected:
                size_t                          check_mouse_over(ssize_t x, ssize_t y);
                void                            do_destroy();
                void                            update_by_timer();
                void                            update_cursor_state(ssize_t x, ssize_t y, bool set);

            protected:
                static status_t                 slot_on_change(Widget *sender, void *ptr, void *data);
                static status_t                 timer_handler(ws::timestamp_t time, void *arg);

            protected:
                virtual void                    size_request(ws::size_limit_t *r);
                virtual void                    property_changed(Property *prop);
                virtual void                    realize(const ws::rectangle_t *r);

            public:
                explicit ScrollBar(Display *dpy);
                virtual ~ScrollBar();

                virtual status_t                init();
                virtual void                    destroy();

            public:
                inline RangeFloat              *value()                     { return &sValue;               }
                inline const RangeFloat        *value() const               { return &sValue;               }

                inline StepFloat               *step()                      { return &sStep;                }
                inline const StepFloat         *step() const                { return &sStep;                }

                inline StepFloat               *accel_step()                { return &sAccelStep;           }
                inline const StepFloat         *accel_step() const          { return &sAccelStep;           }

                inline SizeConstraints         *constraints()               { return &sConstraints;         }
                inline const SizeConstraints   *constraints() const         { return &sConstraints;         }

                inline Orientation             *orientation()               { return &sOrientation;         }
                inline const Orientation       *orientation() const         { return &sOrientation;         }

                inline Pointer                 *slider_pointer()            { return &sSliderPointer;       }
                inline const Pointer           *slider_pointer() const      { return &sSliderPointer;       }

                inline Pointer                 *inc_pointer()               { return &sIncPointer;          }
                inline const Pointer           *inc_pointer() const         { return &sIncPointer;          }

                inline Pointer                 *dec_pointer()               { return &sDecPointer;          }
                inline const Pointer           *dec_pointer() const         { return &sDecPointer;          }

                inline Integer                 *border_radius()             { return &sBorderRadius;        }
                inline const Integer           *border_radius() const       { return &sBorderRadius;        }

                inline Integer                 *border_size()               { return &sBorderSize;          }
                inline const Integer           *border_size() const         { return &sBorderSize;          }

                inline Integer                 *border_gap()                { return &sBorderGap;           }
                inline const Integer           *border_gap() const          { return &sBorderGap;           }

                inline Integer                 *slider_border_size()        { return &sSliderBorderSize;    }
                inline const Integer           *slider_border_size() const  { return &sSliderBorderSize;    }

                inline Color                   *button_color()              { return &sButtonColor;         }
                inline const Color             *button_color() const        { return &sButtonColor;         }

                inline Color                   *button_active_color()       { return &sButtonActiveColor;   }
                inline const Color             *button_active_color() const { return &sButtonActiveColor;   }

                inline Color                   *inc_color()                 { return &sIncColor;            }
                inline const Color             *inc_color() const           { return &sIncColor;            }

                inline Color                   *inc_active_color()          { return &sIncActiveColor;      }
                inline const Color             *inc_active_color() const    { return &sIncActiveColor;      }

                inline Color                   *dec_color()                 { return &sDecColor;            }
                inline const Color             *dec_color() const           { return &sDecColor;            }

                inline Color                   *dec_active_color()          { return &sDecActiveColor;      }
                inline const Color             *dec_active_color() const    { return &sDecActiveColor;      }

                inline Color                   *slider_color()              { return &sSliderColor;         }
                inline const Color             *slider_color() const        { return &sSliderColor;         }

                inline Color                   *slider_border_color()       { return &sSliderBorderColor;   }
                inline const Color             *slider_border_color() const { return &sSliderBorderColor;   }

                inline Color                   *slider_active_color()       { return &sSliderActiveColor;   }
                inline const Color             *slider_active_color() const { return &sSliderActiveColor;   }

                inline Color                   *border_color()              { return &sBorderColor;         }
                inline const Color             *border_color() const        { return &sBorderColor;         }

                inline Color                   *border_gap_color()          { return &sBorderGapColor;      }
                inline const Color             *border_gap_color() const    { return &sBorderGapColor;      }

            public:
                virtual ws::mouse_pointer_t     current_pointer();

                virtual status_t                on_change();

                virtual status_t                on_mouse_down(const ws::event_t *e);

                virtual status_t                on_mouse_up(const ws::event_t *e);

                virtual status_t                on_mouse_move(const ws::event_t *e);

                virtual status_t                on_mouse_scroll(const ws::event_t *e);

                virtual void                    draw(ws::ISurface *s);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SCROLLBAR_H_ */
