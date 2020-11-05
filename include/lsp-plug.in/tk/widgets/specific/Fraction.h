/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 17 авг. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_FRACTION_H_
#define LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_FRACTION_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        // Style definition
        namespace style
        {
            LSP_TK_STYLE_DEF_BEGIN(Fraction, Widget)
                prop::Color                 sColor;         // Fraction color
                prop::Font                  sFont;          // Font
                prop::Float                 sAngle;         // Fraction angle
                prop::Integer               sTextPad;       // Text padding
                prop::Integer               sThick;         // Thickness

                prop::Color                 sItemColor[2];
                prop::String                sItemText[2];
                prop::Boolean               sItemOpened[2];
            LSP_TK_STYLE_DEF_END
        }

        /**
         * Fraction widgets, implements A / B fraction drawn in the UI where A and B provides
         * dropdown lists of any value.
         */
        class Fraction: public Widget
        {
            public:
                static const w_class_t    metadata;

            private:
                Fraction & operator = (const Fraction &);

            protected:
                enum mstate_t
                {
                    NUM_CLICK       = 1 << 0,
                    DENOM_CLICK     = 1 << 1,
                    NONE_CLICK      = 1 << 2
                };

                typedef struct alloc_t
                {
                    ws::rectangle_t sNum;
                    ws::rectangle_t sDen;
                    ws::rectangle_t sSize;
                    float           dx;
                    float           dy;
                } alloc_t;

                class Combo;

                class Window: public PopupWindow
                {
                    public:
                        static const w_class_t    metadata;

                    private:
                        Fraction   *pFrac;
                        Combo      *pCombo;

                    public:
                        explicit Window(Display *dpy, Fraction *frac, Combo *combo);

                        virtual status_t        on_hide();

                        virtual status_t        on_show();
                };

                class List: public ListBox
                {
                    public:
                        static const w_class_t    metadata;

                    private:
                        Fraction   *pFrac;
                        Combo      *pCombo;

                    public:
                        explicit List(Display *dpy, Fraction *cbox, Combo *combo);

                    protected:
                        virtual void        property_changed(Property *prop);

                    public:
                        virtual status_t    on_submit();

                        virtual status_t    on_change();
                };

                class Combo
                {
                    public:
                        Fraction                       *pFrac;
                        List                            sList;
                        Window                          sWindow;
                        prop::Color                     sColor;
                        prop::String                    sText;
                        prop::WidgetPtr<ListBoxItem>    sSelected;
                        prop::Boolean                   sOpened;
                        ws::rectangle_t                 sArea;

                    public:
                        explicit Combo(Display *dpy, Fraction *frac);

                        status_t            init(const char *prefix);
                        void                destroy();

                    public:
                        void                property_changed(Property *prop);
                        bool                scroll_item(ssize_t direction, size_t count);
                };

            protected:
                Combo                       sNum;           // Numerator combo
                Combo                       sDen;           // Denominator combo

                prop::Color                 sColor;         // Fraction color
                prop::Font                  sFont;          // Font
                prop::Float                 sAngle;         // Fraction angle
                prop::Integer               sTextPad;       // Text padding
                prop::Integer               sThick;         // Thickness

                size_t                      nMBState;       // Mouse button state
                mstate_t                    enTrgState;     // Trigger state

            protected:
                void                        do_destroy();
                void                        set_visibility(Combo *cb, bool visible);
                ssize_t                     estimate_size(Combo *cb, ws::rectangle_t *r);
                void                        allocate(alloc_t *a);

                static status_t             slot_on_change(Widget *sender, void *ptr, void *data);
                static status_t             slot_on_submit(Widget *sender, void *ptr, void *data);
                bool                        check_mouse_over(const ws::rectangle_t *area, const ws::event_t *ev);

            protected:
                virtual void                property_changed(Property *prop);
                virtual void                size_request(ws::size_limit_t *r);
                virtual void                realize(const ws::rectangle_t *r);

            public:
                explicit Fraction(Display *dpy);
                virtual ~Fraction();

                virtual status_t            init();
                virtual void                destroy();

            public:
                LSP_TK_PROPERTY(Color,                      color,                  &sColor)
                LSP_TK_PROPERTY(Font,                       font,                   &sFont)
                LSP_TK_PROPERTY(Float,                      angle,                  &sAngle)
                LSP_TK_PROPERTY(Integer,                    text_pad,               &sTextPad)
                LSP_TK_PROPERTY(Integer,                    thickness,              &sThick)

                LSP_TK_PROPERTY(Color,                      num_color,              &sNum.sColor)
                LSP_TK_PROPERTY(String,                     num_empty_text,         &sNum.sText)
                LSP_TK_PROPERTY(WidgetPtr<ListBoxItem>,     num_selected,           &sNum.sSelected)
                LSP_TK_PROPERTY(WidgetList<ListBoxItem>,    num_items,               sNum.sList.items())
                LSP_TK_PROPERTY(Boolean,                    num_opened,             &sNum.sOpened)

                LSP_TK_PROPERTY(Color,                      den_color,              &sDen.sColor)
                LSP_TK_PROPERTY(String,                     den_empty_text,         &sDen.sText)
                LSP_TK_PROPERTY(WidgetPtr<ListBoxItem>,     den_selected,           &sDen.sSelected)
                LSP_TK_PROPERTY(WidgetList<ListBoxItem>,    den_items,               sDen.sList.items())
                LSP_TK_PROPERTY(Boolean,                    den_opened,             &sDen.sOpened)

            public:
                virtual status_t            on_mouse_down(const ws::event_t *e);

                virtual status_t            on_mouse_up(const ws::event_t *e);

                virtual status_t            on_mouse_scroll(const ws::event_t *e);

                virtual status_t            on_change();

                virtual status_t            on_submit();

                virtual void                draw(ws::ISurface *s);
        };
    }
}

#endif /* LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_FRACTION_H_ */
