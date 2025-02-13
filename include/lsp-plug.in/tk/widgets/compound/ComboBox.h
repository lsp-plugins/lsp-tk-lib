/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 8 авг. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_COMPOUND_COMBOBOX_H_
#define LSP_PLUG_IN_TK_WIDGETS_COMPOUND_COMBOBOX_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class ListBoxItem;

        // Style definition
        namespace style
        {
            typedef struct ComboBoxColors
            {
                prop::Color                 sColor;
                prop::Color                 sSpinColor;
                prop::Color                 sTextColor;
                prop::Color                 sSpinTextColor;
                prop::Color                 sBorderColor;
                prop::Color                 sBorderGapColor;

                void listener(tk::prop::Listener *listener);
                bool property_changed(Property *prop);
            } ComboBoxColors;

            enum ComboBoxColorState
            {
                COMBOBOX_NORMAL         = 0,
                COMBOBOX_INACTIVE       = 1 << 0,

                COMBOBOX_TOTAL          = 1 << 1
            };

            LSP_TK_STYLE_DEF_BEGIN(ComboBox, WidgetContainer)
                ComboBoxColors              vColors[COMBOBOX_TOTAL];

                prop::Integer               sBorderSize;
                prop::Integer               sBorderGap;
                prop::Integer               sBorderRadius;
                prop::Integer               sSpinSize;
                prop::Integer               sSpinSeparator;
                prop::Boolean               sOpened;
                prop::Boolean               sActive;
                prop::TextFitness           sTextFit;
                prop::Font                  sFont;
                prop::TextAdjust            sTextAdjust;
                prop::SizeConstraints       sConstraints;
                prop::TextLayout            sTextLayout;
                prop::String                sEmptyText;
                prop::Boolean               sInvertMouseVScroll;
            LSP_TK_STYLE_DEF_END
        }

        class ComboBox: public WidgetContainer
        {
            public:
                static const w_class_t      metadata;

            private:
                static const tether_t       tether_list[];

            protected:
                typedef struct alloc_t
                {
                    ssize_t             radius;     // The rounding radius
                    ssize_t             border;     // The size of border
                    ssize_t             bgap;       // Border gap
                    ssize_t             bsize;      // Overall border size
                    ssize_t             swidth;     // Width of the spin
                    ssize_t             sborder;    // Spin border siz
                    ssize_t             sgap;       // Spin gap
                    ssize_t             spad;       // Padding to not to cross round corner
                } alloc_t;

                class Window: public PopupWindow
                {
                    public:
                        static const w_class_t    metadata;

                    private:
                        ComboBox   *pCBox;

                    public:
                        explicit Window(Display *dpy, ComboBox *cbox);

                        virtual status_t        on_hide() override;
                        virtual status_t        on_show() override;
                };

                class List: public ListBox
                {
                    public:
                        static const w_class_t    metadata;

                    private:
                        ComboBox   *pCBox;

                    public:
                        explicit List(Display *dpy, ComboBox *cbox);

                    protected:
                        virtual void        property_changed(Property *prop) override;

                    public:
                        virtual status_t    on_submit() override;
                        virtual status_t    on_change() override;
                };

                enum cbox_flags_t
                {
                    CBOX_0              = style::COMBOBOX_NORMAL,
                    CBOX_1              = style::COMBOBOX_INACTIVE,
                    CBOX_TOTAL          = style::COMBOBOX_TOTAL
                };

            protected:
                List                        sLBox;          // List box
                Window                      sWindow;        // Popup window

                style::ComboBoxColors       vColors[style::COMBOBOX_TOTAL];
                prop::Integer               sBorderSize;
                prop::Integer               sBorderGap;
                prop::Integer               sBorderRadius;
                prop::Integer               sSpinSize;
                prop::Integer               sSpinSeparator;
                prop::Boolean               sOpened;
                prop::Boolean               sActive;
                prop::TextFitness           sTextFit;
                prop::Font                  sFont;
                prop::TextAdjust            sTextAdjust;
                prop::SizeConstraints       sConstraints;
                prop::TextLayout            sTextLayout;
                prop::String                sEmptyText;
                prop::Boolean               sInvertMouseVScroll;
                prop::WidgetPtr<ListBoxItem> sSelected;

                ws::rectangle_t             sTArea;         // Text area
                ws::rectangle_t             sSArea;         // Spin area
                ws::rectangle_t             sVArea;         // Splitter area
                size_t                      nMBState;       // Mouse button state

            protected:
                void                    do_destroy();
                void                    estimate_parameters(alloc_t *alloc, float scaling);
                bool                    scroll_item(ssize_t direction, size_t count);
                style::ComboBoxColors  *select_colors();

            protected:
                static status_t         slot_on_change(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_submit(Widget *sender, void *ptr, void *data);

            protected:
                virtual void            property_changed(Property *prop) override;
                virtual void            size_request(ws::size_limit_t *r) override;
                virtual void            realize(const ws::rectangle_t *r) override;

            public:
                explicit ComboBox(Display *dpy);
                ComboBox(const ComboBox &) = delete;
                ComboBox(ComboBox &&) = delete;
                virtual ~ComboBox() override;

                ComboBox & operator = (const ComboBox &) = delete;
                ComboBox & operator = (ComboBox &&) = delete;

                virtual status_t            init() override;
                virtual void                destroy() override;

            public:
                LSP_TK_PROPERTY(Color,                      color,                      &vColors[CBOX_0].sColor)
                LSP_TK_PROPERTY(Color,                      spin_color,                 &vColors[CBOX_0].sSpinColor)
                LSP_TK_PROPERTY(Color,                      text_color,                 &vColors[CBOX_0].sTextColor)
                LSP_TK_PROPERTY(Color,                      spin_text_color,            &vColors[CBOX_0].sSpinTextColor)
                LSP_TK_PROPERTY(Color,                      border_color,               &vColors[CBOX_0].sBorderColor)
                LSP_TK_PROPERTY(Color,                      border_gap_color,           &vColors[CBOX_0].sBorderGapColor)
                LSP_TK_PROPERTY(Color,                      inactive_color,             &vColors[CBOX_1].sColor)
                LSP_TK_PROPERTY(Color,                      inactive_spin_color,        &vColors[CBOX_1].sSpinColor)
                LSP_TK_PROPERTY(Color,                      inactive_text_color,        &vColors[CBOX_1].sTextColor)
                LSP_TK_PROPERTY(Color,                      inactive_spin_text_color,   &vColors[CBOX_1].sSpinTextColor)
                LSP_TK_PROPERTY(Color,                      inactive_border_color,      &vColors[CBOX_1].sBorderColor)
                LSP_TK_PROPERTY(Color,                      inactive_border_gap_color,  &vColors[CBOX_1].sBorderGapColor)

                LSP_TK_PROPERTY(Integer,                    border_size,                &sBorderSize)
                LSP_TK_PROPERTY(Integer,                    border_gap,                 &sBorderGap)
                LSP_TK_PROPERTY(Integer,                    border_radius,              &sBorderRadius)
                LSP_TK_PROPERTY(Integer,                    spin_size,                  &sSpinSize)
                LSP_TK_PROPERTY(Integer,                    spin_separator,             &sSpinSeparator)
                LSP_TK_PROPERTY(Boolean,                    opened,                     &sOpened)
                LSP_TK_PROPERTY(Boolean,                    active,                     &sActive)
                LSP_TK_PROPERTY(TextFitness,                text_fit,                   &sTextFit)
                LSP_TK_PROPERTY(SizeConstraints,            constraints,                &sConstraints)
                LSP_TK_PROPERTY(Font,                       font,                       &sFont)
                LSP_TK_PROPERTY(TextAdjust,                 text_adjust,                &sTextAdjust)
                LSP_TK_PROPERTY(TextLayout,                 text_layout,                &sTextLayout)
                LSP_TK_PROPERTY(String,                     empty_text,                 &sEmptyText)
                LSP_TK_PROPERTY(Boolean,                    invert_mouse_vscroll,       &sInvertMouseVScroll);
                LSP_TK_PROPERTY(WidgetPtr<ListBoxItem>,     selected,                   &sSelected)

                LSP_TK_PROPERTY(Font,                       list_font,                  sLBox.font())
                LSP_TK_PROPERTY(WidgetList<ListBoxItem>,    items,                      sLBox.items())

            public:
                virtual status_t            add(Widget *child) override;
                virtual status_t            remove(Widget *child) override;
                virtual status_t            remove_all() override;
                virtual void                draw(ws::ISurface *s) override;

                virtual status_t            on_mouse_down(const ws::event_t *e) override;
                virtual status_t            on_mouse_up(const ws::event_t *e) override;
                virtual status_t            on_mouse_move(const ws::event_t *e) override;
                virtual status_t            on_mouse_scroll(const ws::event_t *e) override;
                virtual status_t            on_key_down(const ws::event_t *e) override;

            public:
                virtual status_t            on_change();
                virtual status_t            on_submit();
        };
    } /* namespace tk */
} /* namespace lsp */


#endif /* LSP_PLUG_IN_TK_WIDGETS_COMPOUND_COMBOBOX_H_ */
