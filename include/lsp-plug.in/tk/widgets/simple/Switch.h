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
                size_t                  nState;
                size_t                  nBMask;

                prop::Color             sColor;
                prop::Color             sTextColor;
                prop::Color             sBorderColor;
                prop::Color             sHoleColor;
                prop::Integer           sBorder;
                prop::Integer           sSize;
                prop::Float             fAspect;
                prop::Boolean           sDown;

            protected:
                bool                    check_mouse_over(ssize_t x, ssize_t y);
                void                    dimensions(ssize_t &w, ssize_t &h);

                void                    on_click(bool down);

            public:
                explicit Switch(Display *dpy);
                virtual ~Switch();

                virtual status_t    init();

            public:
                inline bool is_down() const     { return nState & S_TOGGLED; }

                inline bool is_up() const       { return !(nState & S_TOGGLED); }

                inline LSPColor *color()        { return &sColor; }

                inline LSPColor *text_color()   { return &sTextColor; }

                inline LSPColor *border_color() { return &sBorderColor; }

                inline LSPColor *hole_color()   { return &sHoleColor; }

                inline ssize_t size() const     { return nSize; }

                inline size_t border() const    { return nBorder; }

                inline float aspect() const     { return nAspect; }

                inline size_t angle() const     { return nAngle; }

            public:
                void set_down(bool down = true);

                void set_up(bool up = true);

                void set_size(ssize_t size);

                void set_border(size_t border);

                void set_aspect(float aspect);

                void set_angle(size_t angle);

            public:
                virtual void draw(ISurface *s);

                virtual void size_request(size_request_t *r);

                virtual status_t on_mouse_down(const ws_event_t *e);

                virtual status_t on_mouse_up(const ws_event_t *e);

                virtual status_t on_mouse_move(const ws_event_t *e);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_SWITCH_H_ */
