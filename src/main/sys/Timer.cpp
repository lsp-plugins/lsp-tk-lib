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

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/runtime/system.h>

namespace lsp
{
    namespace tk
    {
        Timer::Timer()
        {
            pDisplay        = NULL;
            pHandler        = NULL;
            pArguments      = NULL;
            nRepeatInterval = 1000;
            nRepeatCount    = 0;
            nFlags          = TF_DEFAULT;
            nErrorCode      = STATUS_OK;
            nTaskID         = -1;
        }

        Timer::~Timer()
        {
            cancel();
        }

        status_t Timer::submit_task(ws::timestamp_t sched, ws::timestamp_t ctime)
        {
            // Check error status
            if ((nFlags & (TF_ERROR | TF_STOP_ON_ERR)) == (TF_ERROR | TF_STOP_ON_ERR))
                return STATUS_OK;
            else if (!(nFlags & TF_LAUNCHED))
                return STATUS_OK;

            // Now check that timer is finite
            if ((!(nFlags & TF_INFINITE)) && (nRepeatCount <= 0))
            {
                nFlags      |= TF_COMPLETED;
                return STATUS_OK;
            }

            // Submit task to display's queue
            ws::timestamp_t time    = lsp_max(ctime, sched + nRepeatInterval);
            nTaskID     = pDisplay->submit_task(time, execute, this);
            if (nTaskID < 0)
                return -nTaskID;
            return STATUS_OK;
        }

        status_t Timer::execute(ws::timestamp_t sched, ws::timestamp_t time, void *arg)
        {
            Timer *_this      = static_cast<Timer *>(arg);
            if (_this == NULL)
                return STATUS_BAD_ARGUMENTS;

            return _this->execute_task(sched, time, arg);
        }

        status_t Timer::execute_task(ws::timestamp_t sched, ws::timestamp_t time, void *arg)
        {
            // Decrement number of repeats
            nTaskID             = -1;
            nRepeatCount        --;

            // First execute run() method
            status_t code       = run(time, pArguments);

            // Analyze status of execution
            if ((nFlags & TF_STOP_ON_ERR) && (code != STATUS_OK))
            {
                nErrorCode          = code;
                nFlags             |= TF_ERROR;
            }
            else if (pHandler != NULL)
            {
                // How run handler if present
                code       = pHandler(sched, time, pArguments);

                // Analyze status of execution
                if ((nFlags & TF_STOP_ON_ERR) && (code != STATUS_OK))
                {
                    nErrorCode          = code;
                    nFlags             |= TF_ERROR;
                }
            }

            return submit_task(sched, time);
        }

        void Timer::bind(ws::IDisplay *dpy)
        {
            // Cancel previous execution
            cancel();

            // Store new display pointer
            pDisplay        = dpy;
        }

        void Timer::bind(Display *dpy)
        {
            // Cancel previous execution
            cancel();

            // Store new display pointer
            pDisplay        = dpy->display();
        }

        status_t Timer::launch(ssize_t count, size_t interval, ws::timestamp_t delay)
        {
            // Cancel previous execution
            status_t result = cancel();
            if (result != STATUS_OK)
                return result;

            // Update settings
            nFlags          = TF_DEFAULT;
            nErrorCode      = STATUS_OK;
            if (count <= 0)
                nFlags          |= TF_INFINITE;
            nRepeatInterval = interval;

            // Submit first task: compute the acutal time when the event should trigger
            if (delay > 0)
                delay      += system::get_time_millis();

            nTaskID     = pDisplay->submit_task(delay, execute, this);
            if (nTaskID < 0)
                return -nTaskID;

            nFlags         |= TF_LAUNCHED;
            return result;
        }

        void Timer::set_handler(ws::task_handler_t handler)
        {
            pHandler        = handler;
        }

        void Timer::set_handler(ws::task_handler_t handler, void *args)
        {
            pHandler        = handler;
            pArguments      = args;
        }

        void Timer::set_argument(void *args)
        {
            pArguments      = args;
        }

        status_t Timer::cancel()
        {
            if (pDisplay == NULL)
                return STATUS_NOT_BOUND;

            // Cancel task if present
            if (nTaskID >= 0)
            {
                pDisplay->cancel_task(nTaskID);
                nTaskID = -1;
            }

            nFlags         &= ~TF_LAUNCHED;

            return STATUS_OK;
        }

        void Timer::set_stop_on_error(bool stop)
        {
            if (stop)
                nFlags         |= TF_STOP_ON_ERR;
            else
                nFlags         &= ~TF_STOP_ON_ERR;
        }

        status_t Timer::resume()
        {
            // Check that timer is bound
            if (pDisplay == NULL)
                return STATUS_NOT_BOUND;

            // Check execution status
            if ((nFlags & (TF_LAUNCHED | TF_ERROR)) != (TF_LAUNCHED | TF_ERROR))
                return STATUS_BAD_STATE;

            nFlags         &= ~TF_ERROR;
            return submit_task(0, 0);
        }

        status_t Timer::run(ws::timestamp_t time, void *args)
        {
            // Just stub, can be overridden
            return STATUS_OK;
        }

    }
} /* namespace lsp */
