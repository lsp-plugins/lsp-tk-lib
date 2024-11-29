/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins-referencer
 * Created on: 29 нояб. 2024 г.
 *
 * lsp-plugins-referencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins-referencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins-referencer. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef LSP_PLUG_IN_TK_UTIL_SHORCUTTRACKER_H_
#define LSP_PLUG_IN_TK_UTIL_SHORCUTTRACKER_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/tk/prop/multi/Shortcut.h>
#include <lsp-plug.in/tk/sys/Shortcuts.h>

namespace lsp
{
    namespace tk
    {
        class ShortcutTracker
        {
            private:
                prop::Shortcut              sState;
                Shortcuts                  *pShortcuts;
                ssize_t                     nEnabled;
                lltl::darray<ws::code_t>    vFired;

            public:
                explicit ShortcutTracker(Shortcuts *shortcuts);
                ShortcutTracker(const ShortcutTracker &) = delete;
                ShortcutTracker(ShortcutTracker &&) = delete;
                virtual ~ShortcutTracker();

                ShortcutTracker & operator = (const ShortcutTracker &) = delete;
                ShortcutTracker & operator = (ShortcutTracker &&) = delete;

            private:
                bool                set_modifier(ws::code_t code, bool set);
                bool                push_key(ws::code_t code);
                bool                pop_key(ws::code_t code);

            public:
                /**
                 * Handle event
                 * @param widget originator of the event
                 * @param ev event
                 * @return status of operation
                 */
                virtual status_t    handle(Widget *widget, const ws::event_t *ev);

                /**
                 * Check if tracker is enabled
                 * @return true if tracker is enabled
                 */
                virtual bool        enabled() const;

                /**
                 * Set enabled status of the tracker
                 * @param enabled enabled status of the tracker
                 * @return the previous enabled status
                 */
                virtual bool        set_enabled(bool enabled);

                /**
                 * Enable tracker
                 * @return the previous enabled status
                 */
                virtual bool        enable();

                /**
                 * Disable tracker
                 * @return the prevous enabled status
                 */
                virtual bool        disable();
        };

    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_UTIL_SHORCUTTRACKER_H_ */
