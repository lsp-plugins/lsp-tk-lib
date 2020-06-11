/*
 * Edit.h
 *
 *  Created on: 29 авг. 2017 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_EDIT_H_
#define LSP_PLUG_IN_TK_WIDGETS_EDIT_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Edit: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                class EditCursor: public TextCursor
                {
                    protected:
                        Edit                   *pEdit;

                    protected:
                        virtual ssize_t         limit(ssize_t value);
                        virtual void            on_change();
                        virtual void            on_blink();

                    public:
                        explicit                EditCursor(Edit *widget);
                        virtual                ~EditCursor();
                };

                class KeyboardInput: public KeyboardHandler
                {
                    protected:
                        Edit     *pEdit;

                    public:
                        explicit KeyboardInput(Edit *widget);
                        virtual ~KeyboardInput();

                    public:
                        virtual status_t on_key_press(const ws::event_t *e);
                };

                class DataSink: public TextDataSink
                {
                    protected:
                        Edit                   *pEdit;

                    protected:
                        virtual status_t        receive(const LSPString *text, const char *mime);

                    public:
                        explicit DataSink(Edit *widget);
                        virtual ~DataSink();

                    public:
                        virtual status_t        close(status_t code);

                    public:
                        void                    unbind();
                };

            protected:
                size_t                  nMBState;
                ssize_t                 sTextPos;
                ssize_t                 nScrDirection;  // Scroll direction

                DataSink               *pDataSink;      // Data sink
                KeyboardInput           sInput;         // Keyboard input handler
                EditCursor              sCursor;
                Timer                   sScroll;

                ws::rectangle_t         sTextArea;

                prop::String            sText;
                prop::TextSelection     sSelection;
                prop::Font              sFont;
                prop::Color             sColor;
                prop::Color             sBorderColor;
                prop::Color             sBorderGapColor;
                prop::Color             sCursorColor;
                prop::Color             sTextColor;
                prop::Color             sTextSelectedColor;
                prop::Color             sSelectionColor;
                prop::Integer           sBorderSize;
                prop::Integer           sBorderGapSize;
                prop::Integer           sBorderRadius;
                prop::SizeConstraints   sConstraints;

                // TODO
//                Menu                    sStdPopup;
//                MenuItem               *vStdItems[3];
//                Menu                   *pPopup;

            protected:
                static status_t                     timer_handler(ws::timestamp_t time, void *arg);
                static status_t                     slot_on_change(Widget *sender, void *ptr, void *data);
                static status_t                     slot_popup_cut_action(Widget *sender, void *ptr, void *data);
                static status_t                     slot_popup_copy_action(Widget *sender, void *ptr, void *data);
                static status_t                     slot_popup_paste_action(Widget *sender, void *ptr, void *data);

            protected:
                ssize_t                             mouse_to_cursor_pos(ssize_t x, ssize_t y);
                void                                run_scroll(ssize_t dir);
                void                                update_scroll();
                void                                update_clipboard(size_t bufid);
                void                                request_clipboard(size_t bufid);
                status_t                            cut_data(size_t bufid);
                status_t                            copy_data(size_t bufid);
                status_t                            paste_data(size_t bufid);
                void                                paste_clipboard(const LSPString *data);

            protected:
                virtual void                        size_request(ws::size_limit_t *r);
                virtual void                        property_changed(Property *prop);
                virtual void                        realize(const ws::rectangle_t *r);

            public:
                explicit                            Edit(Display *dpy);
                virtual                            ~Edit();

                virtual status_t                    init();
                virtual void                        destroy();

            public:
                inline String                      *text()                      { return &sText;                }

                inline TextSelection               *selection()                 { return &sSelection;           }
                inline const TextSelection         *selection() const           { return &sSelection;           }

                inline Font                        *font()                      { return &sFont;                }
                inline const Font                  *font() const                { return &sFont;                }

                inline Color                       *color()                     { return &sColor;               }
                inline const Color                 *color() const               { return &sColor;               }

                inline Color                       *border_color()              { return &sBorderColor;         }
                inline const Color                 *border_color() const        { return &sBorderColor;         }

                inline Color                       *border_gap_color()          { return &sBorderGapColor;      }
                inline const Color                 *border_gap_color() const    { return &sBorderGapColor;      }

                inline Color                       *cursor_color()              { return &sCursorColor;         }
                inline const Color                 *cursor_color() const        { return &sCursorColor;         }

                inline Color                       *text_color()                { return &sTextColor;           }
                inline const Color                 *text_color() const          { return &sTextColor;           }

                inline Color                       *text_selected_color()       { return &sTextSelectedColor;   }
                inline const Color                 *text_selected_color() const { return &sTextSelectedColor;   }

                inline Color                       *selection_color()           { return &sSelectionColor;      }
                inline const Color                 *selection_color() const     { return &sSelectionColor;      }

                inline Integer                     *border_size()               { return &sBorderSize;          }
                inline const Integer               *border_size() const         { return &sBorderSize;          }

                inline Integer                     *border_gap_size()           { return &sBorderGapSize;       }
                inline const Integer               *border_gap_size() const     { return &sBorderGapSize;       }

                inline Integer                     *border_radius()             { return &sBorderRadius;        }
                inline const Integer               *border_radius() const       { return &sBorderRadius;        }

                inline SizeConstraints             *constraints()               { return &sConstraints;         }
                inline const SizeConstraints       *constraints() const         { return &sConstraints;         }

//                inline Menu                        *get_popup()             { return pPopup;            }

            public:
//                inline void         set_popup(LSPMenu *popup)   { pPopup = popup; }

            public:
                virtual void                    draw(ws::ISurface *s);

                virtual status_t                on_change();

                virtual status_t                on_mouse_down(const ws::event_t *e);

                virtual status_t                on_mouse_up(const ws::event_t *e);

                virtual status_t                on_mouse_move(const ws::event_t *e);

                virtual status_t                on_mouse_dbl_click(const ws::event_t *e);

                virtual status_t                on_mouse_tri_click(const ws::event_t *e);

                virtual status_t                on_focus_in(const ws::event_t *e);

                virtual status_t                on_focus_out(const ws::event_t *e);

                virtual status_t                on_key_down(const ws::event_t *e);

                virtual status_t                on_key_up(const ws::event_t *e);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_EDIT_H_ */
