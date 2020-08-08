/*
 * ComboBox.h
 *
 *  Created on: 8 авг. 2020 г.
 *      Author: sadko
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

        class ComboBox: public WidgetContainer
        {
            private:
                ComboBox & operator = (const ComboBox &);

            public:
                static const w_class_t      metadata;

            protected:
                ListBox                     sLBox;

                prop::Integer               sBorderSize;
                prop::Integer               sBorderGap;
                prop::Integer               sBorderRadius;
                prop::Integer               sSpinSize;
                prop::Color                 sColor;
                prop::Color                 sTextColor;
                prop::Color                 sBorderColor;
                prop::Color                 sBorderGapColor;
                prop::Color                 sSpinColor;
                prop::Boolean               sOpened;
                prop::Boolean               sTextFit;

            protected:
                void                    do_destroy();

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
                LSP_TK_PROPERTY(Color,                      color,                  &sColor)
                LSP_TK_PROPERTY(Color,                      text_color,             &sTextColor)
                LSP_TK_PROPERTY(Color,                      border_color,           &sBorderColor)
                LSP_TK_PROPERTY(Color,                      border_gap_color,       &sBorderGapColor)
                LSP_TK_PROPERTY(Color,                      spin_color,             &sSpinColor)
                LSP_TK_PROPERTY(Boolean,                    opened,                 &sOpened)
                LSP_TK_PROPERTY(Boolean,                    text_fit,               &sTextFit)

                LSP_TK_PROPERTY(Font,                       font,                   sLBox.font())
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

                virtual status_t            on_key_up(const ws::event_t *e);
        };
    }
}


#endif /* LSP_PLUG_IN_TK_WIDGETS_COMPOUND_COMBOBOX_H_ */
