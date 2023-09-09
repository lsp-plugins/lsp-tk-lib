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

#ifndef LSP_PLUG_IN_TK_UTIL_KEYBOARDHANDLER_H_
#define LSP_PLUG_IN_TK_UTIL_KEYBOARDHANDLER_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class KeyboardHandler: public ws::IEventHandler
        {
            protected:
                enum constants_t
                {
                    RPT_BUF_SIZE        = 64
                };

            protected:
                size_t                      nPause;                 // Pause before repeat
                size_t                      nRepeat;                // Repeat
                size_t                      nRepeatSize;            // Number of elements in repeat buffer
                ws::event_t                 sLast;                  // Last received event
                ws::code_t                  vRepeat[RPT_BUF_SIZE];  // Repeat buffer
                Timer                       sTimer;                 // Timer to simulate character repeat

            protected:
                status_t                    process_key_down(const ws::event_t *e);
                status_t                    process_key_up(const ws::event_t *e);
                static status_t             simulate_repeat(ws::timestamp_t ts, void *arg);
                void                        simulate_repeat(ws::timestamp_t ts);

            public:
                explicit KeyboardHandler();
                KeyboardHandler(const KeyboardHandler &) = delete;
                KeyboardHandler(KeyboardHandler &&) = delete;
                virtual ~KeyboardHandler() override;

                KeyboardHandler & operator = (KeyboardHandler &&) = delete;
                KeyboardHandler & operator = (const KeyboardHandler &) = delete;

                status_t                    init(Display *dpy);

            public:
                size_t                      pause_time() const       { return nPause;       }
                size_t                      repeat_time() const      { return nRepeat;      }

            public:
                void                        set_pause_time(size_t value);
                void                        set_repeat_time(size_t value);

            public:
                static ws::code_t           translate_keypad(ws::code_t code);

            public:
                // Event handling callbacks
                virtual status_t            handle_event(const ws::event_t *e) override;

                // Event handling callbacks
                virtual status_t            on_key_down(const ws::event_t *e);

                virtual status_t            on_key_press(const ws::event_t *e);

                virtual status_t            on_key_up(const ws::event_t *e);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_UTIL_KEYBOARDHANDLER_H_ */
