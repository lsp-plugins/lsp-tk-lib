/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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
            LSP_TK_STYLE_DEF_BEGIN(ComboBox, WidgetContainer)
                prop::Integer               sBorderSize;
                prop::Integer               sBorderGap;
                prop::Integer               sBorderRadius;
                prop::Integer               sSpinSize;
                prop::Integer               sSpinSeparator;
                prop::Color                 sColor;
                prop::Color                 sSpinColor;
                prop::Color                 sTextColor;
                prop::Color                 sSpinTextColor;
                prop::Color                 sBorderColor;
                prop::Color                 sBorderGapColor;
                prop::Boolean               sOpened;
                prop::TextFitness           sTextFit;
                prop::Font                  sFont;
                prop::TextAdjust            sTextAdjust;
                prop::SizeConstraints       sConstraints;
                prop::TextLayout            sTextLayout;
                prop::String                sEmptyText;
            LSP_TK_STYLE_DEF_END
        }

        class ComboBox: public WidgetContainer
        {
            private:
                ComboBox & operator = (const ComboBox &);
                ComboBox(const ComboBox &);

            public:
                static const w_class_t      metadata;

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

                        virtual status_t        on_hide();

                        virtual status_t        on_show();
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
                        virtual void        property_changed(Property *prop);

                    public:
                        virtual status_t    on_submit();

                        virtual status_t    on_change();
                };

            protected:
                List                        sLBox;          // List box
                Window                      sWindow;        // Popup window

                prop::Integer               sBorderSize;
                prop::Integer               sBorderGap;
                prop::Integer               sBorderRadius;
                prop::Integer               sSpinSize;
                prop::Integer               sSpinSeparator;
                prop::Color                 sColor;
                prop::Color                 sSpinColor;
                prop::Color                 sTextColor;
                prop::Color                 sSpinTextColor;
                prop::Color                 sBorderColor;
                prop::Color                 sBorderGapColor;
                prop::Boolean               sOpened;
                prop::TextFitness           sTextFit;
                prop::Font                  sFont;
                prop::TextAdjust            sTextAdjust;
                prop::SizeConstraints       sConstraints;
                prop::TextLayout            sTextLayout;
                prop::String                sEmptyText;
                prop::WidgetPtr<ListBoxItem> sSelected;

                ws::rectangle_t             sTArea;         // Text area
                ws::rectangle_t             sSArea;         // Spin area
                ws::rectangle_t             sVArea;         // Splitter area
                size_t                      nMBState;       // Mouse button state

            protected:
                void                    do_destroy();
                void                    estimate_parameters(alloc_t *alloc, float scaling);
                bool                    scroll_item(ssize_t direction, size_t count);

                static status_t         slot_on_change(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_submit(Widget *sender, void *ptr, void *data);

            protected:
                virtual void            property_changed(Property *prop);
                virtual void            size_request(ws::size_limit_t *r);
                virtual void            realize(const ws::rectangle_t *r);

            public:
                explicit ComboBox(Display *dpy);
                virtual ~ComboBox();

                virtual status_t            init();
                virtual void                destroy();

            public:
                LSP_TK_PROPERTY(Integer,                    border_size,            &sBorderSize)
                LSP_TK_PROPERTY(Integer,                    border_gap,             &sBorderGap)
                LSP_TK_PROPERTY(Integer,                    border_radius,          &sBorderRadius)
                LSP_TK_PROPERTY(Integer,                    spin_size,              &sSpinSize)
                LSP_TK_PROPERTY(Integer,                    spin_separator,         &sSpinSeparator)
                LSP_TK_PROPERTY(Color,                      color,                  &sColor)
                LSP_TK_PROPERTY(Color,                      spin_color,             &sSpinColor)
                LSP_TK_PROPERTY(Color,                      text_color,             &sTextColor)
                LSP_TK_PROPERTY(Color,                      spin_text_color,        &sSpinTextColor)
                LSP_TK_PROPERTY(Color,                      border_color,           &sBorderColor)
                LSP_TK_PROPERTY(Color,                      border_gap_color,       &sBorderGapColor)
                LSP_TK_PROPERTY(Boolean,                    opened,                 &sOpened)
                LSP_TK_PROPERTY(TextFitness,                text_fit,               &sTextFit)
                LSP_TK_PROPERTY(SizeConstraints,            constraints,            &sConstraints)
                LSP_TK_PROPERTY(Font,                       font,                   &sFont)
                LSP_TK_PROPERTY(TextAdjust,                 text_adjust,            &sTextAdjust)
                LSP_TK_PROPERTY(TextLayout,                 text_layout,            &sTextLayout)
                LSP_TK_PROPERTY(String,                     empty_text,             &sEmptyText)
                LSP_TK_PROPERTY(WidgetPtr<ListBoxItem>,     selected,               &sSelected)

                LSP_TK_PROPERTY(Font,                       list_font,              sLBox.font())
                LSP_TK_PROPERTY(WidgetList<ListBoxItem>,    items,                  sLBox.items())

            public:
                virtual status_t            add(Widget *child);

                virtual status_t            remove(Widget *child);

                virtual status_t            remove_all();

                virtual void                draw(ws::ISurface *s);

                virtual status_t            on_mouse_down(const ws::event_t *e);

                virtual status_t            on_mouse_up(const ws::event_t *e);

                virtual status_t            on_mouse_move(const ws::event_t *e);

                virtual status_t            on_mouse_scroll(const ws::event_t *e);

                virtual status_t            on_key_down(const ws::event_t *e);

                virtual status_t            on_change();

                virtual status_t            on_submit();
        };
    }
}


#endif /* LSP_PLUG_IN_TK_WIDGETS_COMPOUND_COMBOBOX_H_ */
