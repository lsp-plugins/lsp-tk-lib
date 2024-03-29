/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 11 сент. 2017 г.
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
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace tk
    {
        KeyboardHandler::KeyboardHandler()
        {
            nPause      = 1000;
            nRepeat     = 250;
            nRepeatSize = 0;

            ws::init_event(&sLast);
            for (size_t i=0; i<RPT_BUF_SIZE; ++i)
                vRepeat[i]      = 0;
        }

        KeyboardHandler::~KeyboardHandler()
        {
        }

        status_t KeyboardHandler::init(Display *dpy)
        {
            sTimer.bind(dpy);
            return STATUS_OK;
        }

        void KeyboardHandler::set_pause_time(size_t value)
        {
            nPause      = value;
        }

        void KeyboardHandler::set_repeat_time(size_t value)
        {
            nRepeat     = value;
        }

        // Event handling callbacks
        status_t KeyboardHandler::handle_event(const ws::event_t *e)
        {
            switch (e->nType)
            {
                case ws::UIE_KEY_DOWN:
                    return process_key_down(e);

                case ws::UIE_KEY_UP:
                    return process_key_up(e);

                default:
                    return STATUS_OK;
            }
        }

        ws::code_t KeyboardHandler::translate_keypad(ws::code_t code)
        {
            using namespace ws;

            switch (code)
            {
                // Keypad characters
                case WSK_KEYPAD_SPACE:      code = ' '; break;
                case WSK_KEYPAD_TAB:        code = WSK_TAB; break;
                case WSK_KEYPAD_ENTER:      code = WSK_RETURN; break;
                case WSK_KEYPAD_F1:         code = WSK_F1; break;
                case WSK_KEYPAD_F2:         code = WSK_F2; break;
                case WSK_KEYPAD_F3:         code = WSK_F3; break;
                case WSK_KEYPAD_F4:         code = WSK_F4; break;
                case WSK_KEYPAD_HOME:       code = WSK_HOME; break;
                case WSK_KEYPAD_LEFT:       code = WSK_LEFT; break;
                case WSK_KEYPAD_UP:         code = WSK_UP; break;
                case WSK_KEYPAD_RIGHT:      code = WSK_RIGHT; break;
                case WSK_KEYPAD_DOWN:       code = WSK_DOWN; break;
                case WSK_KEYPAD_PAGE_UP:    code = WSK_PAGE_UP; break;
                case WSK_KEYPAD_PAGE_DOWN:  code = WSK_PAGE_DOWN; break;
                case WSK_KEYPAD_END:        code = WSK_END; break;
                case WSK_KEYPAD_BEGIN:      code = WSK_BEGIN; break;
                case WSK_KEYPAD_INSERT:     code = WSK_INSERT; break;
                case WSK_KEYPAD_DELETE:     code = WSK_DELETE; break;
                case WSK_KEYPAD_EQUAL:      code = '='; break;
                case WSK_KEYPAD_MULTIPLY:   code = '*'; break;
                case WSK_KEYPAD_ADD:        code = '+'; break;
                case WSK_KEYPAD_SEPARATOR:  code = ','; break;
                case WSK_KEYPAD_SUBTRACT:   code = '-'; break;
                case WSK_KEYPAD_DECIMAL:    code = '.'; break;
                case WSK_KEYPAD_DIVIDE:     code = '/'; break;

                case WSK_KEYPAD_0:          code = '0'; break;
                case WSK_KEYPAD_1:          code = '1'; break;
                case WSK_KEYPAD_2:          code = '2'; break;
                case WSK_KEYPAD_3:          code = '3'; break;
                case WSK_KEYPAD_4:          code = '4'; break;
                case WSK_KEYPAD_5:          code = '5'; break;
                case WSK_KEYPAD_6:          code = '6'; break;
                case WSK_KEYPAD_7:          code = '7'; break;
                case WSK_KEYPAD_8:          code = '8'; break;
                case WSK_KEYPAD_9:          code = '9'; break;
            }

            return code;
        }

        status_t KeyboardHandler::process_key_down(const ws::event_t *e)
        {
            ws::event_t ev  = *e;
            ev.nCode        = translate_keypad(ev.nCode);
            sLast           = ev;

            if (ws::is_modifier_key(ev.nCode))
                return on_key_down(e);

            if (nRepeatSize >= RPT_BUF_SIZE)
                return STATUS_NO_MEM;

            // Pass the key to the callback handler
            status_t result = on_key_down(&ev);
            if (result != STATUS_OK)
                return result;
            result          = on_key_press(&ev);
            if (result != STATUS_OK)
                return result;

            vRepeat[nRepeatSize++] = ev.nCode;
            sTimer.launch(1, nPause);
            return STATUS_OK;
        }

        status_t KeyboardHandler::process_key_up(const ws::event_t *e)
        {
            ws::event_t ev  = *e;
            ev.nCode        = translate_keypad(ev.nCode);
            sLast           = ev;

            if (ws::is_modifier_key(ev.nCode))
                return on_key_down(e);

            // Remove key from list and decrement the size of buffer
            ws::code_t *c   = vRepeat, *last = &vRepeat[nRepeatSize];
            while (c < last)
                if (*(c++) == ev.nCode)
                    break;
            if (c != last)
                memmove(&c[-1], c, (last - c)*sizeof(ws::code_t));

            if (nRepeatSize > 0)
                nRepeatSize--;
            if (nRepeatSize == 1)
                lsp_trace("debug");

            if (nRepeatSize <= 0)
                sTimer.cancel();

            // Pass the key to the callback handler
            return on_key_up(&ev);
        }

        status_t KeyboardHandler::simulate_repeat(ws::timestamp_t ts, void *arg)
        {
            if (arg == NULL)
                return STATUS_BAD_ARGUMENTS;

            KeyboardHandler *c = static_cast<KeyboardHandler *>(arg);
            c->simulate_repeat(ts);

            return STATUS_OK;
        }

        void KeyboardHandler::simulate_repeat(ws::timestamp_t ts)
        {
            if (nRepeatSize <= 0)
            {
                sTimer.cancel();
                return;
            }

            ws::event_t ev  = sLast;
            ev.nCode        = vRepeat[nRepeatSize-1];
            ev.nTime        = ts;

            // Simulate Key up event
            ev.nType        = ws::UIE_KEY_UP;
            status_t ret    = on_key_up(&ev);
            if (ret != STATUS_OK)
                return;

            // Simulate Key down event
            ev.nType        = ws::UIE_KEY_DOWN;
            ret             = on_key_down(&ev);
            if (ret != STATUS_OK)
                return;

            // Simulate Key press event
            ret             = on_key_press(&ev);
            if (ret != STATUS_OK)
                return;

            // Relaunch timer if needed
            if (sTimer.is_finite())
                sTimer.launch(0, nRepeat);
        }

        status_t KeyboardHandler::on_key_press(const ws::event_t *e)
        {
            return STATUS_OK;
        }

        status_t KeyboardHandler::on_key_down(const ws::event_t *e)
        {
            return STATUS_OK;
        }

        status_t KeyboardHandler::on_key_up(const ws::event_t *e)
        {
            return STATUS_OK;
        }

    } /* namespace tk */
} /* namespace lsp */
