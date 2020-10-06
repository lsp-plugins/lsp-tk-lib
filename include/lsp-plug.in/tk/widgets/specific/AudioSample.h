/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 28 сент. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_AUDIOSAMPLE_H_
#define LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_AUDIOSAMPLE_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class AudioSample: public WidgetContainer
        {
            public:
                static const w_class_t    metadata;

                static const size_t LABELS      = 5;

            private:
                AudioSample & operator = (const AudioSample &);

            protected:
                enum flags_t
                {
                    XF_LBUTTON      = 1 << 0,
                    XF_RBUTTON      = 1 << 1,
                    XF_DOWN         = 1 << 2
                };

            protected:
                prop::WidgetList<AudioChannel>  vChannels;          // List of audio channels
                lltl::parray<AudioChannel>      vVisible;           // List of visible audio channels
                prop::CollectionListener        sIListener;         // Listener to trigger vItems content change

                prop::Integer           sWaveBorder;                // Wave border
                prop::Integer           sFadeInBorder;              // Fade in border
                prop::Integer           sFadeOutBorder;             // Fade out border
                prop::Integer           sLineWidth;                 // Line width
                prop::Color             sLineColor;                 // Line color
                prop::SizeConstraints   sConstraints;               // Size constraints
                prop::Boolean           sActive;                    // Active, allow button press
                prop::Boolean           sSGroups;                   // Stereo groups enable

                prop::String            sMainText;                  // Main text
                prop::TextLayout        sMainLayout;                // Layout of main text
                prop::Font              sMainFont;                  // Main font
                prop::Color             sMainColor;                 // Main font color
                prop::Boolean           sMainVisibility;            // Show main text
                prop::String            sLabel[LABELS];             // Label
                prop::Color             sLabelColor[LABELS];        // Label text color
                prop::TextLayout        sLabelLayout[LABELS];       // Layout of each label
                prop::Font              sLabelFont;                 // Font of label
                prop::Color             sLabelBgColor;              // Background color of label
                prop::Integer           sLabelRadius;               // Label radius
                prop::Boolean           sLabelVisibility[LABELS];   // Visibility of label

                prop::Integer           sBorder;                    // Border size
                prop::Integer           sBorderRadius;              // Border radius
                prop::Boolean           sGlass;                     // Draw glass
                prop::Color             sColor;                     // Graph color
                prop::Color             sBorderColor;               // Color of the border
                prop::Color             sGlassColor;                // Color of the glass
                prop::Padding           sIPadding;                  // Internal padding
                prop::WidgetPtr<Menu>   sPopup;                     // Popup Menu

                size_t                  nBMask;                     // Mouse button state
                size_t                  nXFlags;                    // Button flags
                ws::rectangle_t         sGraph;                     // Area for sample rendering
                ws::ISurface           *pGlass;                     // Surface to draw glass

            public:
                virtual void            size_request(ws::size_limit_t *r);
                virtual void            realize(const ws::rectangle_t *r);
                virtual void            property_changed(Property *prop);
                virtual void            hide_widget();

                void                    draw_channel1(const ws::rectangle_t *r, ws::ISurface *s, AudioChannel *c, size_t samples);
                void                    draw_fades1(const ws::rectangle_t *r, ws::ISurface *s, AudioChannel *c, size_t samples);

                void                    draw_channel2(const ws::rectangle_t *r, ws::ISurface *s, AudioChannel *c1, AudioChannel *c2, size_t samples);
                void                    draw_fades2(const ws::rectangle_t *r, ws::ISurface *s, AudioChannel *c1, AudioChannel *c2, size_t samples);

                static void             on_add_item(void *obj, Property *prop, Widget *w);
                static void             on_remove_item(void *obj, Property *prop, Widget *w);

                void                    do_destroy();

                void                    get_visible_items(lltl::parray<AudioChannel> *dst);
                status_t                handle_mouse_move(const ws::event_t *ev);
                void                    drop_glass();
                void                    draw_main_text(ws::ISurface *s);

            public:
                explicit AudioSample(Display *dpy);
                virtual ~AudioSample();

                virtual status_t            init();
                virtual void                destroy();

            public:
                LSP_TK_PROPERTY(WidgetList<AudioChannel>,   channels,           &vChannels)

                LSP_TK_PROPERTY(Integer,                wave_border,            &sWaveBorder)
                LSP_TK_PROPERTY(Integer,                fade_in_border,         &sFadeInBorder)
                LSP_TK_PROPERTY(Integer,                fade_out_border,        &sFadeOutBorder)
                LSP_TK_PROPERTY(Integer,                line_width,             &sLineWidth)
                LSP_TK_PROPERTY(Color,                  line_color,             &sLineColor)
                LSP_TK_PROPERTY(SizeConstraints,        constraints,            &sConstraints)
                LSP_TK_PROPERTY(Boolean,                active,                 &sActive)
                LSP_TK_PROPERTY(Boolean,                stereo_groups,          &sSGroups)

                LSP_TK_PROPERTY(String,                 main_text,              &sMainText)
                LSP_TK_PROPERTY(TextLayout,             main_layout,            &sMainLayout)
                LSP_TK_PROPERTY(Font,                   main_font,              &sMainFont)
                LSP_TK_PROPERTY(Color,                  main_color,             &sMainColor)
                LSP_TK_PROPERTY(Boolean,                main_visibility,        &sMainVisibility)
                LSP_TK_IPROPERTY(String,                label,                  &sLabel[index], LABELS)
                LSP_TK_IPROPERTY(Color,                 label_color,            &sLabelColor[index], LABELS)
                LSP_TK_IPROPERTY(TextLayout,            label_layout,           &sLabelLayout[index], LABELS)
                LSP_TK_PROPERTY(Font,                   label_font,             &sLabelFont)
                LSP_TK_PROPERTY(Color,                  label_bg_color,         &sLabelBgColor)
                LSP_TK_PROPERTY(Integer,                label_radius,           &sLabelRadius)
                LSP_TK_IPROPERTY(Boolean,               label_visibility,       &sLabelVisibility[index], LABELS)

                LSP_TK_PROPERTY(Integer,                border_size,            &sBorder);
                LSP_TK_PROPERTY(Integer,                border_radius,          &sBorderRadius);
                LSP_TK_PROPERTY(Boolean,                glass,                  &sGlass);
                LSP_TK_PROPERTY(Color,                  color,                  &sColor)
                LSP_TK_PROPERTY(Color,                  border_color,           &sBorderColor);
                LSP_TK_PROPERTY(Color,                  glass_color,            &sGlassColor);
                LSP_TK_PROPERTY(Padding,                internal_padding,       &sIPadding);

                LSP_TK_PROPERTY(WidgetPtr<Menu>,        popup,                  &sPopup);

            public:
                virtual void                draw(ws::ISurface *s);

                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force);

                virtual status_t            add(Widget *widget);

                virtual status_t            remove(Widget *child);

                virtual status_t            remove_all();

                virtual status_t            on_mouse_down(const ws::event_t *e);

                virtual status_t            on_mouse_up(const ws::event_t *e);

                virtual status_t            on_mouse_move(const ws::event_t *e);
        };
    }
}



#endif /* LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_AUDIOSAMPLE_H_ */
