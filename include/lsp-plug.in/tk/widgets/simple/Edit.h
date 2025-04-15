/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_EDIT_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_EDIT_H_

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
            typedef struct EditColors
            {
                prop::Color             sColor;
                prop::Color             sBorderColor;
                prop::Color             sBorderGapColor;
                prop::Color             sCursorColor;
                prop::Color             sTextColor;
                prop::Color             sTextSelectedColor;
                prop::Color             sEmptyTextColor;
                prop::Color             sSelectionColor;

                void listener(tk::prop::Listener *listener);
                bool property_changed(Property *prop);
            } EditColors;

            enum EditColorState
            {
                EDIT_NORMAL         = 0,
                EDIT_INACTIVE       = 1 << 0,

                EDIT_TOTAL          = 1 << 1
            };

            LSP_TK_STYLE_DEF_BEGIN(Edit, Widget)
                EditColors              vColors[EDIT_TOTAL];

                prop::String            sText;
                prop::TextSelection     sSelection;
                prop::Font              sFont;
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

                enum chk_flags_t
                {
                    EDIT_0      = style::EDIT_NORMAL,
                    EDIT_1      = style::EDIT_INACTIVE,
                    EDIT_TOTAL  = style::EDIT_TOTAL
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

                style::EditColors       vColors[EDIT_TOTAL];
                prop::String            sText;
                prop::String            sEmptyText;
                prop::TextSelection     sSelection;
                prop::Font              sFont;
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
                style::EditColors                  *select_colors();

            protected:
                virtual void                        size_request(ws::size_limit_t *r) override;
                virtual void                        property_changed(Property *prop) override;
                virtual void                        realize(const ws::rectangle_t *r) override;

            public:
                explicit Edit(Display *dpy);
                Edit(const Edit &) = delete;
                Edit(Edit &&) = delete;
                virtual ~Edit() override;
                Edit & operator = (const Edit &) = delete;
                Edit & operator = (Edit &&) = delete;

                virtual status_t                    init() override;
                virtual void                        destroy() override;

            public:
                LSP_TK_PROPERTY(Color,              color,                              &vColors[EDIT_0].sColor)
                LSP_TK_PROPERTY(Color,              border_color,                       &vColors[EDIT_0].sBorderColor)
                LSP_TK_PROPERTY(Color,              border_gap_color,                   &vColors[EDIT_0].sBorderGapColor)
                LSP_TK_PROPERTY(Color,              cursor_color,                       &vColors[EDIT_0].sCursorColor)
                LSP_TK_PROPERTY(Color,              text_color,                         &vColors[EDIT_0].sTextColor)
                LSP_TK_PROPERTY(Color,              text_selected_color,                &vColors[EDIT_0].sTextSelectedColor)
                LSP_TK_PROPERTY(Color,              placeholder_text_color,             &vColors[EDIT_0].sEmptyTextColor)
                LSP_TK_PROPERTY(Color,              selection_color,                    &vColors[EDIT_0].sSelectionColor)

                LSP_TK_PROPERTY(Color,              inactive_color,                     &vColors[EDIT_1].sColor)
                LSP_TK_PROPERTY(Color,              inactive_border_color,              &vColors[EDIT_1].sBorderColor)
                LSP_TK_PROPERTY(Color,              inactive_border_gap_color,          &vColors[EDIT_1].sBorderGapColor)
                LSP_TK_PROPERTY(Color,              inactive_cursor_color,              &vColors[EDIT_1].sCursorColor)
                LSP_TK_PROPERTY(Color,              inactive_text_color,                &vColors[EDIT_1].sTextColor)
                LSP_TK_PROPERTY(Color,              inactive_text_selected_color,       &vColors[EDIT_1].sTextSelectedColor)
                LSP_TK_PROPERTY(Color,              inactive_placeholder_text_color,    &vColors[EDIT_1].sEmptyTextColor)
                LSP_TK_PROPERTY(Color,              inactive_selection_color,           &vColors[EDIT_1].sSelectionColor)

                LSP_TK_PROPERTY(String,             text,                               &sText)
                LSP_TK_PROPERTY(String,             empty_text,                         &sEmptyText)
                LSP_TK_PROPERTY(TextSelection,      selection,                          &sSelection)
                LSP_TK_PROPERTY(Font,               font,                               &sFont)
                LSP_TK_PROPERTY(Integer,            border_size,                        &sBorderSize)
                LSP_TK_PROPERTY(Integer,            border_gap_size,                    &sBorderGapSize)
                LSP_TK_PROPERTY(Integer,            border_radius,                      &sBorderRadius)
                LSP_TK_PROPERTY(SizeConstraints,    constraints,                        &sConstraints)
                LSP_TK_PROPERTY(WidgetPtr<Menu>,    popup,                              &sPopup)

            public:
                virtual void                    draw(ws::ISurface *s, bool force) override;
                virtual status_t                on_mouse_down(const ws::event_t *e) override;
                virtual status_t                on_mouse_up(const ws::event_t *e) override;
                virtual status_t                on_mouse_move(const ws::event_t *e) override;
                virtual status_t                on_mouse_dbl_click(const ws::event_t *e) override;
                virtual status_t                on_mouse_tri_click(const ws::event_t *e) override;
                virtual status_t                on_focus_in(const ws::event_t *e) override;
                virtual status_t                on_focus_out(const ws::event_t *e) override;
                virtual status_t                on_key_down(const ws::event_t *e) override;
                virtual status_t                on_key_up(const ws::event_t *e) override;

            public:
                virtual status_t                on_change();
                virtual status_t                on_before_popup(Menu *menu);
                virtual status_t                on_popup(Menu *menu);
        };

    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_EDIT_H_ */
