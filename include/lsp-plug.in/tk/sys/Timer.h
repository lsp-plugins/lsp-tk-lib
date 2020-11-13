/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 1 июн. 2017 г.
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

#ifndef LSP_PLUG_IN_TK_SYS_TIMER_H_
#define LSP_PLUG_IN_TK_SYS_TIMER_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/ws/IDisplay.h>

namespace lsp
{
    namespace tk
    {
        class Display;

        /** Simple timer interface to launch scheduled or periodic tasks
         *
         */
        class Timer
        {
            protected:
                enum flags_t
                {
                    TF_INFINITE     = 1 << 0,
                    TF_LAUNCHED     = 1 << 1,
                    TF_ERROR        = 1 << 2,
                    TF_STOP_ON_ERR  = 1 << 3,
                    TF_COMPLETED    = 1 << 4,

                    TF_DEFAULT      = 0
                };

            protected:
                ws::IDisplay       *pDisplay;
                ws::task_handler_t  pHandler;
                void               *pArguments;
                size_t              nRepeatInterval;
                ssize_t             nRepeatCount;
                size_t              nFlags;
                status_t            nErrorCode;
                ws::taskid_t        nTaskID;

            protected:
                static  status_t    execute(ws::timestamp_t sched, ws::timestamp_t time, void *arg);
                status_t            execute_task(ws::timestamp_t sched, ws::timestamp_t time, void *arg);

                status_t            submit_task(ws::timestamp_t sched, ws::timestamp_t at);

            public:
                /** Constructor
                 *
                 */
                explicit Timer();

                /** Destructor
                 *
                 */
                virtual ~Timer();

            public:
                /** Bind timer to the display
                 *
                 * @param dpy native display
                 */
                void    bind(ws::IDisplay *dpy);

                /** Bind timer to the display
                 *
                 * @param dpy LSP display
                 */
                void    bind(Display *dpy);

                /** Cancel the previous execution and start the timer
                 *
                 * @param count number of repeats, negative or zero value for infinite
                 * @param interval the interval in milliseconds between repeats
                 * @param delay the relative to the current time delay to trigger first tick, 0 if immediate
                 * @return status of operation
                 */
                status_t    launch(ssize_t count = 1, size_t interval = 1000, ws::timestamp_t delay = 0);

                /** Set handler, cancels previously used handler,
                 * does not drop argument
                 *
                 * @param handler timer handler
                 */
                void    set_handler(ws::task_handler_t handler);

                /** Set handler, cancels previously used handler,
                 * overrides arguments
                 *
                 * @param handler timer handler
                 * @param args argument passed to the handler
                 */
                void    set_handler(ws::task_handler_t handler, void *args);

                /** Set argument to pass to the handler, do not cancel previously used handler
                 *
                 * @param args argument to pass to the handler
                 */
                void    set_argument(void *args);

                /** Cancel timer
                 *
                 */
                status_t    cancel();

                /** Stop timer on error
                 *
                 */
                void set_stop_on_error(bool stop = true);

                /** Get timer error handling status
                 *
                 * @return true if timer will be stopped on error
                 */
                inline bool get_stop_on_error() const { return nFlags & TF_STOP_ON_ERR; }

                /** Check if there is pending error
                 *
                 * @return true if there is pending error
                 */
                inline bool has_error() const { return nFlags & TF_ERROR; }

                /** Check if timer has finished and will never execute more
                 *
                 * @return true if timer has finished
                 */
                inline bool has_finished() const { return nFlags & TF_COMPLETED; }

                /** Clear error state and resume the timer
                 *
                 */
                status_t resume();

                /** Get last execution error code
                 *
                 * @return last execution error code
                 */
                inline status_t get_last_error() const { return nErrorCode; };

                /** Check if timer runs in infinite repeat mode
                 *
                 * @return true if timer runs in infinite repeat mode
                 */
                inline bool is_infinite() const { return nFlags & TF_INFINITE; };

                /** Check if timer runs in finite repeat mode
                 *
                 * @return true if timer runs in finite repeat mode
                 */
                inline bool is_finite() const { return !(nFlags & TF_INFINITE); };

                /** Check whether the timer is in launched state
                 *
                 * @return true if timer is in launched state
                 */
                inline bool is_launched() const { return nFlags & TF_LAUNCHED; }

                /** Get number of repeats left before timer enters finished state
                 *
                 * @return number of repeats left, valid only for finite timers
                 */
                inline ssize_t repeats_left() const { return nRepeatCount; }

            public:
                /** This method can be overridden to handle timer events,
                 * this method will be executed even if set_handler() method was issued
                 *
                 * @param time time at whic the timer was executed
                 * @param args argument passed to the handler
                 */
                virtual status_t    run(ws::timestamp_t time, void *args);
        };
    }
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_SYS_TIMER_H_ */
