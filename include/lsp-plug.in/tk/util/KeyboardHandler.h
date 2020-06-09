/*
 * KeyboardHandler.h
 *
 *  Created on: 11 сент. 2017 г.
 *      Author: sadko
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
            private:
                KeyboardHandler & operator = (const KeyboardHandler &);

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
                virtual ~KeyboardHandler();

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
                virtual status_t            handle_event(const ws::event_t *e);

                // Event handling callbacks
                virtual status_t            on_key_down(const ws::event_t *e);

                virtual status_t            on_key_press(const ws::event_t *e);

                virtual status_t            on_key_up(const ws::event_t *e);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_UTIL_KEYBOARDHANDLER_H_ */
