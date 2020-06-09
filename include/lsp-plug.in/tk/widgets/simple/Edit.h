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
                class TextCursor: public LSPTextCursor
                {
                    protected:
                        Edit     *pEdit;

                    protected:
                        virtual ssize_t limit(ssize_t value);
                        virtual void on_change();
                        virtual void on_blink();

                    public:
                        explicit TextCursor(Edit *widget);
                        virtual ~TextCursor();
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

                class DataSink: public ws::IDataSink
                {
                    protected:
                        Edit               *pEdit;
                        io::OutMemoryStream sOS;
                        char               *pMime;

                    public:
                        explicit DataSink(Edit *widget);
                        virtual ~DataSink();

                    public:
                        void unbind();

                    public:
                        virtual ssize_t     open(const char * const *mime_types);
                        virtual status_t    write(const void *buf, size_t count);
                        virtual status_t    close(status_t code);
                };

            protected:
                LSPString               sText;          // Cached text
                size_t                  nMBState;
                ssize_t                 sTextPos;
                ssize_t                 nScrDirection;  // Scroll direction

                DataSink               *pDataSink;      // Data sink
                KeyboardInput           sInput;         // Keyboard input handler
                TextCursor              sCursor;
                Timer                   sScroll;

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
                prop::Integer           sBorderGap;
                prop::Integer           sBorderRadius;
                prop::SizeConstraints   sConstraints;

                // TODO
//                Menu                    sStdPopup;
//                MenuItem               *vStdItems[3];
//                Menu                   *pPopup;

            protected:
                static status_t             timer_handler(ws::timestamp_t time, void *arg);
                static status_t             slot_on_change(Widget *sender, void *ptr, void *data);
                static status_t             clipboard_handler(void *arg, status_t s, io::IInStream *is);

// TODO
//                static status_t             slot_popup_cut_action(Widget *sender, void *ptr, void *data);
//                static status_t             slot_popup_copy_action(Widget *sender, void *ptr, void *data);
//                static status_t             slot_popup_paste_action(Widget *sender, void *ptr, void *data);

            protected:
                ssize_t                     mouse_to_cursor_pos(ssize_t x, ssize_t y);
                void                        run_scroll(ssize_t dir);
                void                        update_scroll();
                void                        update_clipboard(size_t bufid);
                void                        request_clipboard(size_t bufid);
                status_t                    paste_data(io::IInStream *is);
                status_t                    cut_data(size_t bufid);
                status_t                    copy_data(size_t bufid);
                status_t                    paste_data(size_t bufid);
                void                        paste_clipboard(const LSPString *data);

            public:
                explicit Edit(Display *dpy);
                virtual ~Edit();

                virtual status_t init();
                virtual void destroy();

            public:
                inline LSPTextSelection   *selection()         { return &sSelection;   }
                inline LSPTextCursor      *cursor()            { return &sCursor;      }
                inline LSPFont            *font()              { return &sFont;        }
                inline const char         *text() const        { return sText.get_native(); }
                inline status_t            get_text(LSPString *dst) const { return dst->set(&sText) ? STATUS_OK : STATUS_NO_MEM; };
                inline const ssize_t       min_width() const   { return nMinWidth;     }
                inline LSPColor           *sel_color()         { return &sSelColor;    }
                inline LSPColor           *color()             { return &sColor;       }
                inline LSPMenu            *get_popup()         { return pPopup;        }

            public:
//                status_t                    set_text(const char *text);
//                status_t                    set_text(const LSPString *text);
//                inline void         set_popup(LSPMenu *popup)   { pPopup = popup; }

            public:
                virtual void                    size_request(ws::size_limit_t *r);

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
