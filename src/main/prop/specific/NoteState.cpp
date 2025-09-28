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

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        static const char *hex_table = "0123456789ABCDEF";

        static umword_t char_to_hex(lsp_wchar_t c)
        {
            if ((c >= '0') && (c <= '9'))
                return c - '0';
            if ((c >= 'a') && (c <= 'f'))
                return c - 'a' + 10;
            if ((c >= 'A') && (c <= 'F'))
                return c - 'A' + 10;

            return 0xff;
        }

        NoteState::NoteState(prop::Listener *listener):
            SimpleProperty(listener)
        {
            for (size_t i=0; i<NUM_BINS; ++i)
                nState[i] = ALL_ZERO;
        }

        NoteState::~NoteState()
        {
            SimpleProperty::unbind(&sListener);
        }

        void NoteState::deserialize(umword_t *dst, const char *src)
        {
            for (size_t i=0; i<NUM_BINS; ++i)
                dst[i]     = ALL_ZERO;

            for (size_t i=0; i<NUM_BINS; ++i)
            {
                umword_t bin = ALL_ZERO;
                for (size_t j=0; j<NUM_BIN_CHARS; ++j)
                {
                    umword_t c  = char_to_hex(*(src++));
                    if (c >= 0x10)
                    {
                        dst[i]          = bin;
                        return;
                    }
                    bin        |= c << (j << 2);
                }
                dst[i]          = bin;
            }
        }

        void NoteState::deserialize(umword_t *dst, const lsp_wchar_t *src, size_t count)
        {
            const lsp_wchar_t *end = &src[count];
            for (size_t i=0; i<NUM_BINS; ++i)
                dst[i]     = ALL_ZERO;

            for (size_t i=0; i<NUM_BINS; ++i)
            {
                umword_t bin = ALL_ZERO;
                for (size_t j=0; j<NUM_BIN_CHARS; ++j)
                {
                    umword_t c  = (src < end) ? char_to_hex(*(src++)) : 0xff;
                    if (c >= 0x10)
                    {
                        dst[i]          = bin;
                        return;
                    }
                    bin        |= c << (j << 2);
                }
                dst[i]          = bin;
            }
        }

        void NoteState::serialize(char *dst, const umword_t *src)
        {
            for (size_t i=0; i<NUM_BINS; ++i)
            {
                umword_t bin = src[i];
                for (size_t j=0; j<NUM_BIN_CHARS; ++j)
                {
                    *(dst++) = hex_table[bin & 0x0f];
                    bin    >>= 4;
                }
            }
            *dst = '\0';
        }

        void NoteState::commit(atom_t property)
        {
            // Handle change: remember new value
            const char *value = NULL;
            if (pStyle->get_string(nAtom, &value) != STATUS_OK)
                return;

            deserialize(nState, value);
        }

        void NoteState::push()
        {
            char buf[NUM_CHARS + 1];
            serialize(buf, nState);

            pStyle->set_string(nAtom, buf);
        }

        void NoteState::clear_all()
        {
            for (size_t i=0; i<NUM_BINS; ++i)
                nState[i] = ALL_ZERO;
            sync();
        }

        void NoteState::set_all()
        {
            for (size_t i=0; i<NUM_BINS; ++i)
                nState[i] = ALL_ONES;
            sync();
        }

        void NoteState::toggle_all()
        {
            for (size_t i=0; i<NUM_BINS; ++i)
                nState[i] ^= ALL_ONES;
            sync();
        }

        bool NoteState::clear(size_t index)
        {
            if (index >= NOTES_TOTAL)
                return false;

            umword_t *bin = &nState[index / BITS_PER_BIN];
            umword_t mask = ONE << (index % BITS_PER_BIN);
            const bool old  = (*bin & mask) != 0;
            *bin           &= ~mask;
            sync();

            return old;
        }

        bool NoteState::set(size_t index)
        {
            if (index >= NOTES_TOTAL)
                return false;

            umword_t *bin = &nState[index / BITS_PER_BIN];
            const umword_t mask = ONE << (index % BITS_PER_BIN);
            const bool old  = (*bin & mask) != 0;
            *bin           |= mask;
            sync();

            return old;
        }

        bool NoteState::set(size_t index, bool on)
        {
            umword_t *bin = &nState[index / BITS_PER_BIN];
            const umword_t mask = ONE << (index % BITS_PER_BIN);
            const bool old  = (*bin & mask) != 0;
            *bin            = (on) ? (*bin) | mask : (*bin) & (~mask);
            sync();

            return old;
        }

        bool NoteState::reset(size_t index)
        {
            return clear(index);
        }

        bool NoteState::toggle(size_t index)
        {
            umword_t *bin = &nState[index / BITS_PER_BIN];
            const umword_t mask = ONE << (index % BITS_PER_BIN);
            const bool old  = (*bin & mask) != 0;
            *bin           ^= mask;
            sync();

            return old;
        }

        bool NoteState::get(size_t index) const
        {
            const umword_t *bin = &nState[index / BITS_PER_BIN];
            const umword_t mask = ONE << (index % BITS_PER_BIN);
            return (*bin & mask) != 0;
        }

        bool NoteState::all_set() const
        {
            for (size_t i=0; i<NUM_BINS; ++i)
                if (nState[i] != ALL_ONES)
                    return false;
            return true;
        }

        bool NoteState::all_unset() const
        {
            for (size_t i=0; i<NUM_BINS; ++i)
                if (nState[i] != ALL_ZERO)
                    return false;
            return true;
        }

        void NoteState::clear_range(size_t first, size_t last)
        {
            last    = lsp_min(last-1, NOTES_TOTAL-1);
            if (first > last)
                return;

            size_t first_idx    = first / BITS_PER_BIN;
            size_t last_idx     = last / BITS_PER_BIN;
            size_t first_off    = first % BITS_PER_BIN;
            size_t last_off     = last % BITS_PER_BIN;
            umword_t *bin       = &nState[first_idx];

            if (first_idx == last_idx)
            {
                const size_t shift  = BITS_PER_BIN - last_off - 1;
                const umword_t mask = (ALL_ONES << (first_off + shift)) >> shift;
                *bin               &= ~mask;
                return;
            }

            while (first_idx < last_idx)
            {
                const umword_t mask = (ALL_ONES << first_off);
                *bin               &= ~mask;

                first_off           = 0;
                ++first_idx;
                ++bin;
            }

            if (last_off > 0)
            {
                const size_t shift  = BITS_PER_BIN - last_off - 1;
                const umword_t mask = (ALL_ONES << shift) >> shift;
                *bin               &= ~mask;
            }

            sync();
        }

        void NoteState::set_range(size_t first, size_t last)
        {
            last    = lsp_min(last-1, NOTES_TOTAL-1);
            if (first > last)
                return;

            size_t first_idx    = first / BITS_PER_BIN;
            size_t last_idx     = last / BITS_PER_BIN;
            size_t first_off    = first % BITS_PER_BIN;
            size_t last_off     = last % BITS_PER_BIN;
            umword_t *bin       = &nState[first_idx];

            if (first_idx == last_idx)
            {
                const size_t shift  = BITS_PER_BIN - last_off - 1;
                const umword_t mask = (ALL_ONES << (first_off + shift)) >> shift;
                *bin               |= mask;
                return;
            }

            while (first_idx < last_idx)
            {
                const umword_t mask = (ALL_ONES << first_off);
                *bin               |= mask;

                first_off           = 0;
                ++first_idx;
                ++bin;
            }

            if (last_off > 0)
            {
                const size_t shift  = BITS_PER_BIN - last_off - 1;
                const umword_t mask = (ALL_ONES << shift) >> shift;
                *bin               |= mask;
            }

            sync();
        }

        void NoteState::toggle_range(size_t first, size_t last)
        {
            last    = lsp_min(last-1, NOTES_TOTAL-1);
            if (first > last)
                return;

            size_t first_idx    = first / BITS_PER_BIN;
            size_t last_idx     = last / BITS_PER_BIN;
            size_t first_off    = first % BITS_PER_BIN;
            size_t last_off     = last % BITS_PER_BIN;
            umword_t *bin       = &nState[first_idx];

            if (first_idx == last_idx)
            {
                const size_t shift  = BITS_PER_BIN - last_off - 1;
                const umword_t mask = (ALL_ONES << (first_off + shift)) >> shift;
                *bin               ^= mask;
                return;
            }

            while (first_idx < last_idx)
            {
                const umword_t mask = (ALL_ONES << first_off);
                *bin               ^= mask;

                first_off           = 0;
                ++first_idx;
                ++bin;
            }

            if (last_off > 0)
            {
                const size_t shift  = BITS_PER_BIN - last_off - 1;
                const umword_t mask = (ALL_ONES << shift) >> shift;
                *bin               ^= mask;
            }
        }

        void NoteState::clear_count(size_t first, size_t count)
        {
            clear_range(first, first + count);
        }

        void NoteState::set_count(size_t first, size_t count)
        {
            set_range(first, first + count);
        }

        void NoteState::toggle_count(size_t first, size_t count)
        {
            toggle_range(first, first + count);
        }

        void NoteState::set_state(const char *state)
        {
            deserialize(nState, state);
            sync();
        }

        void NoteState::set_state(const LSPString *state)
        {
            deserialize(nState, state->characters(), state->length());
            sync();
        }

        void NoteState::swap(NoteState * dst)
        {
            for (size_t i=0; i<NUM_BINS; ++i)
                lsp::swap(nState[i], dst->nState[i]);

            dst->sync();
            sync();
        }

        void NoteState::swap(NoteState & dst)
        {
            swap(&dst);
        }

    } /* namespace tk */
} /* namespace lsp */


