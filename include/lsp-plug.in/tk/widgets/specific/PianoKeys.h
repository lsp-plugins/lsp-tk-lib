/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 25 сент. 2025 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_PIANOKEYS_H_
#define LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_PIANOKEYS_H_

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
            typedef struct PianoKeyColors
            {
                prop::Color                 sWhiteColor;    // White key color
                prop::Color                 sBlackColor;    // Black key color

                void listener(tk::prop::Listener *listener);
                bool property_changed(Property *prop);
            } PianoKeyColors;

            enum PianoKeyColorState
            {
                PIANOKEY_NORMAL         = 0,
                PIANOKEY_DOWN           = 1 << 0,
                PIANOKEY_SELECTED       = 1 << 1,
                PIANOKEY_INACTIVE       = 1 << 2,
                PIANOKEY_HOVER          = 1 << 3,

                PIANOKEY_TOTAL          = 1 << 4
            };

            typedef struct PianoColors
            {
                prop::Color                 sBorderColor;   // Border color
                prop::Color                 sSplitColor;    // Split color

                void listener(tk::prop::Listener *listener);
                bool property_changed(Property *prop);
            } PianoColors;

            enum PianoColorState
            {
                PIANO_NORMAL            = 0,
                PIANO_INACTIVE          = 1 << 0,

                PIANO_TOTAL             = 1 << 1
            };


            LSP_TK_STYLE_DEF_BEGIN(PianoKeys, Widget)
                PianoKeyColors              vKeyColors[PIANOKEY_TOTAL];
                PianoColors                 vColors[PIANO_TOTAL];

                prop::Integer               sBorderSize;    // Border size
                prop::Integer               sSplitSize;     // Split size
                prop::Integer               sMinNote;       // Minimal note code
                prop::Integer               sMaxNote;       // Maximum note code
                prop::Integer               sAngle;         // Rotation angle

                // TODO: add note selection bitmask
            LSP_TK_STYLE_DEF_END
        }

        /**
         * Fraction widgets, implements A / B fraction drawn in the UI where A and B provides
         * dropdown lists of any value.
         */
        class PianoKeys: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum pk_flags_t
                {
                    PK_0            = style::PIANOKEY_NORMAL,
                    PK_1            = style::PIANOKEY_DOWN,
                    PK_2            = style::PIANOKEY_SELECTED,
                    PK_3            = style::PIANOKEY_DOWN | style::PIANOKEY_SELECTED,
                    PK_4            = style::PIANOKEY_NORMAL | style::PIANOKEY_INACTIVE,
                    PK_5            = style::PIANOKEY_DOWN | style::PIANOKEY_INACTIVE,
                    PK_6            = style::PIANOKEY_SELECTED | style::PIANOKEY_INACTIVE,
                    PK_7            = style::PIANOKEY_DOWN | style::PIANOKEY_SELECTED | style::PIANOKEY_INACTIVE,

                    PK_8            = style::PIANOKEY_NORMAL | style::PIANOKEY_HOVER,
                    PK_9            = style::PIANOKEY_DOWN | style::PIANOKEY_HOVER,
                    PK_10           = style::PIANOKEY_SELECTED | style::PIANOKEY_HOVER,
                    PK_11           = style::PIANOKEY_DOWN | style::PIANOKEY_SELECTED | style::PIANOKEY_HOVER,
                    PK_12           = style::PIANOKEY_NORMAL | style::PIANOKEY_INACTIVE | style::PIANOKEY_HOVER,
                    PK_13           = style::PIANOKEY_DOWN | style::PIANOKEY_INACTIVE | style::PIANOKEY_HOVER,
                    PK_14           = style::PIANOKEY_SELECTED | style::PIANOKEY_INACTIVE | style::PIANOKEY_HOVER,
                    PK_15           = style::PIANOKEY_DOWN | style::PIANOKEY_SELECTED | style::PIANOKEY_INACTIVE | style::PIANOKEY_HOVER,

                    PK_TOTAL        = style::PIANOKEY_TOTAL
                };

                enum pc_flags_t
                {
                    PC_0            = style::PIANO_NORMAL,
                    PC_1            = style::PIANO_INACTIVE,
                };

            protected:
                style::PianoKeyColors       vKeyColors[style::PIANOKEY_TOTAL];
                style::PianoColors          vColors[style::PIANO_TOTAL];

                prop::Integer               sBorderSize;    // Border size
                prop::Integer               sSplitSize;     // Split size
                prop::Integer               sMinNote;       // Minimal note code
                prop::Integer               sMaxNote;       // Maximum note code
                prop::Integer               sAngle;         // Rotation angle

            protected:
                static status_t             slot_on_submit(Widget *sender, void *ptr, void *data);

            protected:
                void                        do_destroy();

            protected:
                virtual void                property_changed(Property *prop) override;
                virtual void                size_request(ws::size_limit_t *r) override;
                virtual void                realize(const ws::rectangle_t *r) override;

            public:
                explicit PianoKeys(Display *dpy);
                PianoKeys(const PianoKeys &) = delete;
                PianoKeys(PianoKeys &&) = delete;
                virtual ~PianoKeys() override;
                PianoKeys & operator = (const PianoKeys &) = delete;
                PianoKeys & operator = (PianoKeys &&) = delete;

                virtual status_t            init() override;
                virtual void                destroy() override;

            public:
                LSP_TK_PROPERTY(Color,              white_color,                                &vKeyColors[PK_0].sWhiteColor)
                LSP_TK_PROPERTY(Color,              black_color,                                &vKeyColors[PK_0].sBlackColor)
                LSP_TK_PROPERTY(Color,              white_down_color,                           &vKeyColors[PK_1].sWhiteColor)
                LSP_TK_PROPERTY(Color,              black_down_color,                           &vKeyColors[PK_1].sBlackColor)
                LSP_TK_PROPERTY(Color,              white_selected_color,                       &vKeyColors[PK_2].sWhiteColor)
                LSP_TK_PROPERTY(Color,              black_selected_color,                       &vKeyColors[PK_2].sBlackColor)
                LSP_TK_PROPERTY(Color,              white_down_selected_color,                  &vKeyColors[PK_3].sWhiteColor)
                LSP_TK_PROPERTY(Color,              black_down_selected_color,                  &vKeyColors[PK_3].sBlackColor)

                LSP_TK_PROPERTY(Color,              white_inactive_color,                       &vKeyColors[PK_4].sWhiteColor)
                LSP_TK_PROPERTY(Color,              black_inactive_color,                       &vKeyColors[PK_4].sBlackColor)
                LSP_TK_PROPERTY(Color,              white_inactive_down_color,                  &vKeyColors[PK_5].sWhiteColor)
                LSP_TK_PROPERTY(Color,              black_inactive_down_color,                  &vKeyColors[PK_5].sBlackColor)
                LSP_TK_PROPERTY(Color,              white_inactive_selected_color,              &vKeyColors[PK_6].sWhiteColor)
                LSP_TK_PROPERTY(Color,              black_inactive_selected_color,              &vKeyColors[PK_6].sBlackColor)
                LSP_TK_PROPERTY(Color,              white_inactive_down_selected_color,         &vKeyColors[PK_7].sWhiteColor)
                LSP_TK_PROPERTY(Color,              black_inactive_down_selected_color,         &vKeyColors[PK_7].sBlackColor)

                LSP_TK_PROPERTY(Color,              white_hover_color,                          &vKeyColors[PK_8].sWhiteColor)
                LSP_TK_PROPERTY(Color,              black_hover_color,                          &vKeyColors[PK_8].sBlackColor)
                LSP_TK_PROPERTY(Color,              white_hover_down_color,                     &vKeyColors[PK_9].sWhiteColor)
                LSP_TK_PROPERTY(Color,              black_hover_down_color,                     &vKeyColors[PK_9].sBlackColor)
                LSP_TK_PROPERTY(Color,              white_hover_selected_color,                 &vKeyColors[PK_10].sWhiteColor)
                LSP_TK_PROPERTY(Color,              black_hover_selected_color,                 &vKeyColors[PK_10].sBlackColor)
                LSP_TK_PROPERTY(Color,              white_hover_down_selected_color,            &vKeyColors[PK_11].sWhiteColor)
                LSP_TK_PROPERTY(Color,              black_hover_down_selected_color,            &vKeyColors[PK_11].sBlackColor)

                LSP_TK_PROPERTY(Color,              white_hover_inactive_color,                 &vKeyColors[PK_12].sWhiteColor)
                LSP_TK_PROPERTY(Color,              black_hover_inactive_color,                 &vKeyColors[PK_12].sBlackColor)
                LSP_TK_PROPERTY(Color,              white_hover_inactive_down_color,            &vKeyColors[PK_13].sWhiteColor)
                LSP_TK_PROPERTY(Color,              black_hover_inactive_down_color,            &vKeyColors[PK_13].sBlackColor)
                LSP_TK_PROPERTY(Color,              white_hover_inactive_selected_color,        &vKeyColors[PK_14].sWhiteColor)
                LSP_TK_PROPERTY(Color,              black_hover_inactive_selected_color,        &vKeyColors[PK_14].sBlackColor)
                LSP_TK_PROPERTY(Color,              white_hover_inactive_down_selected_color,   &vKeyColors[PK_15].sWhiteColor)
                LSP_TK_PROPERTY(Color,              black_hover_inactive_down_selected_color,   &vKeyColors[PK_15].sBlackColor)

                LSP_TK_PROPERTY(Color,              border_color,                               &vColors[PC_0].sBorderColor)
                LSP_TK_PROPERTY(Color,              split_color,                                &vColors[PC_0].sSplitColor)
                LSP_TK_PROPERTY(Color,              inactive_border_color,                      &vColors[PC_1].sBorderColor)
                LSP_TK_PROPERTY(Color,              inactive_split_color,                       &vColors[PC_1].sSplitColor)

                LSP_TK_PROPERTY(Integer,            border_size,                                &sBorderSize)
                LSP_TK_PROPERTY(Integer,            split_size,                                 &sSplitSize)
                LSP_TK_PROPERTY(Integer,            min_note,                                   &sMinNote)
                LSP_TK_PROPERTY(Integer,            max_note,                                   &sMaxNote)
                LSP_TK_PROPERTY(Integer,            angle,                                      &sAngle)

            public:
                virtual status_t            on_mouse_down(const ws::event_t *e) override;
                virtual status_t            on_mouse_up(const ws::event_t *e) override;
                virtual void                draw(ws::ISurface *s, bool force) override;

            public:
                virtual status_t            on_submit();
        };

    } /* namespace tk */
} /* namespace lsp */




#endif /* LSP_PLUG_IN_TK_WIDGETS_SPECIFIC_PIANOKEYS_H_ */
