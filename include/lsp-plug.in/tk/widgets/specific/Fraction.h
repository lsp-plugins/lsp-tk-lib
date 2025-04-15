/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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
            typedef struct FractionColors
            {
                prop::Color                 sColor;         // Fraction color
                prop::Color                 sNumColor;      // Fraction numerator color
                prop::Color                 sDenColor;      // Fraction denominator color

                void listener(tk::prop::Listener *listener);
                bool property_changed(Property *prop);
            } FractionColors;

            enum FractionColorState
            {
                FRACTION_NORMAL         = 0,
                FRACTION_INACTIVE       = 1 << 0,

                FRACTION_TOTAL          = 1 << 1
            };

            LSP_TK_STYLE_DEF_BEGIN(Fraction, Widget)
                FractionColors              vColors[FRACTION_TOTAL];

                prop::Font                  sFont;          // Font
                prop::Float                 sAngle;         // Fraction angle
                prop::Integer               sTextPad;       // Text padding
                prop::Integer               sThick;         // Thickness

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
                static const tether_t tether_list[];

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

                enum frac_flags_t
                {
                    FRAC_0          = style::FRACTION_NORMAL,
                    FRAC_1          = style::FRACTION_INACTIVE,
                    FRAC_TOTAL      = style::FRACTION_TOTAL
                };

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

                        virtual status_t        on_hide() override;
                        virtual status_t        on_show() override;
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
                        virtual void        property_changed(Property *prop) override;

                    public:
                        virtual status_t    on_submit() override;
                        virtual status_t    on_change() override;
                };

                class Combo
                {
                    public:
                        Fraction                       *pFrac;
                        List                            sList;
                        Window                          sWindow;
                        prop::String                    sText;
                        prop::WidgetPtr<ListBoxItem>    sSelected;
                        prop::Boolean                   sOpened;
                        ws::rectangle_t                 sArea;

                    public:
                        explicit Combo(Display *dpy, Fraction *frac);

                        status_t            init(size_t idx);
                        void                destroy();

                    public:
                        void                property_changed(Property *prop);
                        bool                scroll_item(ssize_t direction, size_t count);
                };

            protected:
                Combo                       sNum;           // Numerator combo
                Combo                       sDen;           // Denominator combo

                style::FractionColors       vColors[style::FRACTION_TOTAL];
                prop::Font                  sFont;          // Font
                prop::Float                 sAngle;         // Fraction angle
                prop::Integer               sTextPad;       // Text padding
                prop::Integer               sThick;         // Thickness

                size_t                      nMBState;       // Mouse button state
                mstate_t                    enTrgState;     // Trigger state

            protected:
                static status_t             slot_on_change(Widget *sender, void *ptr, void *data);
                static status_t             slot_on_submit(Widget *sender, void *ptr, void *data);

            protected:
                void                        do_destroy();
                void                        set_visibility(Combo *cb, bool visible);
                ssize_t                     estimate_size(Combo *cb, ws::rectangle_t *r);
                void                        allocate(alloc_t *a);

                bool                        check_mouse_over(const ws::rectangle_t *area, const ws::event_t *ev);
                style::FractionColors      *select_colors();

            protected:
                virtual void                property_changed(Property *prop) override;
                virtual void                size_request(ws::size_limit_t *r) override;
                virtual void                realize(const ws::rectangle_t *r) override;

            public:
                explicit Fraction(Display *dpy);
                Fraction(const Fraction &) = delete;
                Fraction(Fraction &&) = delete;
                virtual ~Fraction() override;
                Fraction & operator = (const Fraction &) = delete;
                Fraction & operator = (Fraction &&) = delete;

                virtual status_t            init() override;
                virtual void                destroy() override;

            public:
                LSP_TK_PROPERTY(Color,                      color,                  &vColors[FRAC_0].sColor)
                LSP_TK_PROPERTY(Color,                      num_color,              &vColors[FRAC_0].sNumColor)
                LSP_TK_PROPERTY(Color,                      den_color,              &vColors[FRAC_0].sDenColor)
                LSP_TK_PROPERTY(Color,                      inactive_color,         &vColors[FRAC_1].sColor)
                LSP_TK_PROPERTY(Color,                      inactive_num_color,     &vColors[FRAC_1].sNumColor)
                LSP_TK_PROPERTY(Color,                      inactive_den_color,     &vColors[FRAC_1].sDenColor)

                LSP_TK_PROPERTY(Font,                       font,                   &sFont)
                LSP_TK_PROPERTY(Float,                      angle,                  &sAngle)
                LSP_TK_PROPERTY(Integer,                    text_pad,               &sTextPad)
                LSP_TK_PROPERTY(Integer,                    thickness,              &sThick)

                LSP_TK_PROPERTY(String,                     num_empty_text,         &sNum.sText)
                LSP_TK_PROPERTY(WidgetPtr<ListBoxItem>,     num_selected,           &sNum.sSelected)
                LSP_TK_PROPERTY(WidgetList<ListBoxItem>,    num_items,               sNum.sList.items())
                LSP_TK_PROPERTY(Boolean,                    num_opened,             &sNum.sOpened)

                LSP_TK_PROPERTY(String,                     den_empty_text,         &sDen.sText)
                LSP_TK_PROPERTY(WidgetPtr<ListBoxItem>,     den_selected,           &sDen.sSelected)
                LSP_TK_PROPERTY(WidgetList<ListBoxItem>,    den_items,               sDen.sList.items())
                LSP_TK_PROPERTY(Boolean,                    den_opened,             &sDen.sOpened)

            public:
                virtual status_t            on_mouse_down(const ws::event_t *e) override;
                virtual status_t            on_mouse_up(const ws::event_t *e) override;
                virtual status_t            on_mouse_scroll(const ws::event_t *e) override;
                virtual void                draw(ws::ISurface *s, bool force) override;

            public:
                virtual status_t            on_change();
                virtual status_t            on_submit();
        };

    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_FRACTION_H_ */
