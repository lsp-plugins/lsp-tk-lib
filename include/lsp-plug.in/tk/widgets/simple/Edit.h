/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 29 авг. 2017 г.
 *
 * lsp-tk-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-tk-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-tk-lib. If not, see <https://www.gnu.org/licenses/>.
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
        class Menu;

        // Style definition
        namespace style
        {
            LSP_TK_STYLE_DEF_BEGIN(Edit, Widget)
                prop::String            sText;
                prop::TextSelection     sSelection;
                prop::Font              sFont;
                prop::Color             sColor;
                prop::Color             sBorderColor;
                prop::Color             sBorderGapColor;
                prop::Color             sCursorColor;
                prop::Color             sTextColor;
                prop::Color             sTextSelectedColor;
                prop::Color             sEmptyTextColor;
                prop::Color             sSelectionColor;
                prop::Integer           sBorderSize;
                prop::Integer           sBorderGapSize;
                prop::Integer           sBorderRadius;
                prop::SizeConstraints   sConstraints;
            LSP_TK_STYLE_DEF_END
        }

        class Edit: public Widget
        {
            public:
                static const w_class_t    metadata;

            private:
                Edit & operator = (const Edit &);
                Edit(const Edit &);

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

                Widget                 *vMenu[4];

                ws::rectangle_t         sTextArea;

                prop::String            sText;
                prop::String            sEmptyText;
                prop::TextSelection     sSelection;
                prop::Font              sFont;
                prop::Color             sColor;
                prop::Color             sBorderColor;
                prop::Color             sBorderGapColor;
                prop::Color             sCursorColor;
                prop::Color             sTextColor;
                prop::Color             sTextSelectedColor;
                prop::Color             sEmptyTextColor;
                prop::Color             sSelectionColor;
                prop::Integer           sBorderSize;
                prop::Integer           sBorderGapSize;
                prop::Integer           sBorderRadius;
                prop::SizeConstraints   sConstraints;
                prop::WidgetPtr<Menu>   sPopup;

            protected:
                static status_t                     timer_handler(ws::timestamp_t sched, ws::timestamp_t time, void *arg);
                static status_t                     slot_on_change(Widget *sender, void *ptr, void *data);
                static status_t                     slot_popup_cut_action(Widget *sender, void *ptr, void *data);
                static status_t                     slot_popup_copy_action(Widget *sender, void *ptr, void *data);
                static status_t                     slot_popup_paste_action(Widget *sender, void *ptr, void *data);
                static status_t                     slot_on_before_popup(Widget *sender, void *ptr, void *data);
                static status_t                     slot_on_popup(Widget *sender, void *ptr, void *data);

            protected:
                ssize_t                             mouse_to_cursor_pos(ssize_t x, ssize_t y, bool range = true);
                void                                run_scroll(ssize_t dir);
                void                                update_scroll();
                void                                update_clipboard(size_t bufid);
                void                                request_clipboard(size_t bufid);
                status_t                            cut_data(size_t bufid);
                status_t                            copy_data(size_t bufid);
                status_t                            paste_data(size_t bufid);
                void                                paste_clipboard(const LSPString *data);
                void                                do_destroy();
                status_t                            create_default_menu();

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
                LSP_TK_PROPERTY(String,             text,                           &sText)
                LSP_TK_PROPERTY(String,             empty_text,                     &sEmptyText)
                LSP_TK_PROPERTY(TextSelection,      selection,                      &sSelection)
                LSP_TK_PROPERTY(Font,               font,                           &sFont)
                LSP_TK_PROPERTY(Color,              color,                          &sColor)
                LSP_TK_PROPERTY(Color,              border_color,                   &sBorderColor)
                LSP_TK_PROPERTY(Color,              border_gap_color,               &sBorderGapColor)
                LSP_TK_PROPERTY(Color,              cursor_color,                   &sCursorColor)
                LSP_TK_PROPERTY(Color,              text_color,                     &sTextColor)
                LSP_TK_PROPERTY(Color,              text_selected_color,            &sTextSelectedColor)
                LSP_TK_PROPERTY(Color,              placeholder_text_color,         &sEmptyTextColor)
                LSP_TK_PROPERTY(Color,              selection_color,                &sSelectionColor)
                LSP_TK_PROPERTY(Integer,            border_size,                    &sBorderSize)
                LSP_TK_PROPERTY(Integer,            border_gap_size,                &sBorderGapSize)
                LSP_TK_PROPERTY(Integer,            border_radius,                  &sBorderRadius)
                LSP_TK_PROPERTY(SizeConstraints,    constraints,                    &sConstraints)
                LSP_TK_PROPERTY(WidgetPtr<Menu>,    popup,                          &sPopup)

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

                virtual status_t                on_before_popup(Menu *menu);

                virtual status_t                on_popup(Menu *menu);
        };

    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_EDIT_H_ */
