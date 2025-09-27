/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 27 сент. 2025 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_SPECIFIC_NOTESTATE_H_
#define LSP_PLUG_IN_TK_PROP_SPECIFIC_NOTESTATE_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class NoteState: public SimpleProperty
        {
            protected:
                static constexpr size_t NOTES_TOTAL     = 128;
                static constexpr size_t NUM_BINS        = NOTES_TOTAL / sizeof(umword_t);
                static constexpr size_t NUM_CHARS       = NOTES_TOTAL / 4; // 4 bits per character
                static constexpr size_t NUM_BIN_CHARS   = NUM_CHARS / NUM_BINS;
                static constexpr size_t BITS_PER_BIN    = sizeof(umword_t) * 8;

                static constexpr umword_t ALL_ZERO      = umword_t(0);
                static constexpr umword_t ALL_ONES      = ~umword_t(0);
                static constexpr umword_t ONE           = umword_t(1);

            protected:
                umword_t        nState[NUM_BINS];   // State of all possible MIDI notes

            protected:
                virtual void        commit(atom_t property) override;
                virtual void        push() override;

            protected:
                static void         deserialize(umword_t *dst, const char *src);
                static void         deserialize(umword_t *dst, const lsp_wchar_t *src, size_t count);
                static void         serialize(char *dst, const umword_t *src);

            public:
                explicit NoteState(prop::Listener *listener);
                NoteState(const NoteState &) = delete;
                NoteState(NoteState &&) = delete;
                virtual ~NoteState() override;

                NoteState & operator = (const NoteState &) = delete;
                NoteState & operator = (NoteState &&) = delete;

            public:
                bool            clear(size_t index);
                void            clear_range(size_t first, size_t last);
                void            clear_count(size_t first, size_t last);
                void            clear_all();
                bool            reset(size_t index);

                bool            set(size_t index);
                bool            set(size_t index, bool on);
                void            set_range(size_t first, size_t last);
                void            set_count(size_t first, size_t last);
                void            set_all();

                bool            toggle(size_t index);
                void            toggle_range(size_t first, size_t last);
                void            toggle_count(size_t first, size_t last);
                void            toggle_all();

                void            set_state(const char *state);
                void            set_state(const LSPString *state);
                bool            get(size_t index) const;

                bool            all_set() const;
                bool            all_unset() const;

                void            swap(NoteState *dst);
                void            swap(NoteState &dst);
        };

        namespace prop
        {
            class NoteState: public tk::NoteState
            {
                public:
                    explicit inline NoteState(prop::Listener *listener = NULL): tk::NoteState(listener) {}
                    NoteState(const NoteState &) = delete;
                    NoteState(NoteState &&) = delete;

                    NoteState & operator = (const NoteState &) = delete;
                    NoteState & operator = (NoteState &&) = delete;

                public:
                    inline status_t     bind(atom_t property, Style *style)             { return SimpleProperty::bind(property, style, PT_STRING, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return SimpleProperty::bind(property, style, PT_STRING, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return SimpleProperty::bind(property, style, PT_STRING, &sListener); }

            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */




#endif /* LSP_PLUG_IN_TK_PROP_SPECIFIC_NOTESTATE_H_ */
