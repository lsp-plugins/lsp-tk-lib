/*
 * Hyperlink.h
 *
 *  Created on: 23 окт. 2017 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_HYPERLINK_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_HYPERLINK_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Menu;

        class Hyperlink: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum state_t
                {
                    F_MOUSE_IN      = 1 << 0,
                    F_MOUSE_DOWN    = 1 << 1,
                    F_MOUSE_IGN     = 1 << 2,
                };

            protected:
                size_t                      nMFlags;
                size_t                      nState;
//                Menu                       *pPopup;         // TODO

                prop::TextLayout            sTextLayout;    // Text layout
                prop::Font                  sFont;          // Font parameters
                prop::Color                 sColor;         // Font color
                prop::Color                 sHoverColor;    // Hover font color
                prop::String                sText;          // Text to display
                prop::SizeConstraints       sConstraints;   // Size constraints
                prop::Boolean               sFollow;        // Follow hyperlink
                prop::String                sUrl;           // URL

            protected:
                static status_t                 slot_on_submit(Widget *sender, void *ptr, void *data);
                static status_t                 slot_on_before_popup(Widget *sender, void *ptr, void *data);
                static status_t                 slot_on_popup(Widget *sender, void *ptr, void *data);
                static status_t                 slot_copy_link_action(Widget *sender, void *ptr, void *data);

            protected:
                virtual void                    size_request(ws::size_limit_t *r);
                virtual void                    property_changed(Property *prop);

            public:
                explicit Hyperlink(Display *dpy);
                virtual ~Hyperlink();

                virtual status_t                init();

            public:
                inline TextLayout              *text_layout()       { return &sTextLayout;  }
                inline const TextLayout        *text_layout() const { return &sTextLayout;  }

                inline Font                    *font()              { return &sFont;        }
                inline const Font              *font() const        { return &sFont;        }

                inline Color                   *color()             { return &sColor;       }
                inline const Color             *color() const       { return &sColor;       }

                inline Color                   *hover_color()       { return &sHoverColor;  }
                inline const Color             *hover_color() const { return &sHoverColor;  }

                inline String                  *text()              { return &sText;        }
                inline const String            *text() const        { return &sText;        }

                inline SizeConstraints         *constraints()       { return &sConstraints; }
                inline const SizeConstraints   *constraints() const { return &sConstraints; }

                inline Boolean                 *follow()            { return &sFollow;      }
                inline const Boolean           *follow() const      { return &sFollow;      }

                inline String                  *url()               { return &sUrl;         }
                inline const String            *url() const         { return &sUrl;         }

            public:
                status_t                        copy_url(ws::clipboard_id_t cb);
                status_t                        follow_url() const;

            public:
                virtual void                    draw(ws::ISurface *s);

                virtual status_t                on_mouse_in(const ws::event_t *e);

                virtual status_t                on_mouse_out(const ws::event_t *e);

                virtual status_t                on_mouse_move(const ws::event_t *e);

                virtual status_t                on_mouse_down(const ws::event_t *e);

                virtual status_t                on_mouse_up(const ws::event_t *e);

                virtual status_t                on_before_popup(/* Menu *menu */);

                virtual status_t                on_popup(/* Menu *menu */);

                virtual status_t                on_submit();

        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_HYPERLINK_H_ */
