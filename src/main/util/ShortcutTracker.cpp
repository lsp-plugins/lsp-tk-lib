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

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        ShortcutTracker::ShortcutTracker(Shortcuts *shortcuts)
        {
            pShortcuts      = shortcuts;
            nEnabled        = 0;
        }

        ShortcutTracker::~ShortcutTracker()
        {
        }

        bool ShortcutTracker::set_modifier(ws::code_t code, bool set)
        {
            size_t modifier = 0;
            switch (code)
            {
                case ws::WSK_CONTROL_L: modifier = KM_LCTRL; break;
                case ws::WSK_CONTROL_R: modifier = KM_RCTRL; break;
                case ws::WSK_ALT_L: modifier = KM_LALT; break;
                case ws::WSK_ALT_R: modifier = KM_RALT; break;
                case ws::WSK_SHIFT_L: modifier = KM_LSHIFT; break;
                case ws::WSK_SHIFT_R: modifier = KM_RSHIFT; break;
                case ws::WSK_META_L: modifier = KM_LMETA; break;
                case ws::WSK_META_R: modifier = KM_RMETA; break;
                case ws::WSK_SUPER_L: modifier = KM_LSUPER; break;
                case ws::WSK_SUPER_R: modifier = KM_RSUPER; break;
                case ws::WSK_HYPER_L: modifier = KM_LHYPER; break;
                case ws::WSK_HYPER_R: modifier = KM_RHYPER; break;
                default: break;
            }

            if (modifier == 0)
                return false;

            if (set)
                sState.add_modifiers(code);
            else
                sState.remove_modifiers(code);

            return true;
        }

        bool ShortcutTracker::push_key(ws::code_t code)
        {
            bool fired = vFired.is_empty();
            if (!vFired.add(&code))
                return false;

            sState.set_key((fired) ? code : ws::WSK_UNKNOWN);

            return fired;
        }

        bool ShortcutTracker::pop_key(ws::code_t code)
        {
            sState.set_key(ws::WSK_UNKNOWN);

            for (size_t i=0, n=vFired.size(); i < n; ++i)
            {
                ws::code_t *key = vFired.uget(i);
                if (*key == code)
                    return vFired.remove(i);
            }

            return false;
        }

        status_t ShortcutTracker::handle(Widget *widget, const ws::event_t *ev)
        {
            if (!enabled())
                return STATUS_OK;

            status_t res        = STATUS_OK;

            switch (ev->nType)
            {
                case ws::UIE_KEY_DOWN:
                {
                    if (set_modifier(ev->nCode, true))
                        return STATUS_OK;
                    if (!push_key(ev->nCode))
                        return STATUS_OK;

                    // Key combination has fired
                    if (pShortcuts != NULL)
                    {
                        for (size_t i=0, n = pShortcuts->size(); i<n; )
                        {
                            // Get matching shortcut and update index
                            ssize_t idx = pShortcuts->index_match(&sState, i);
                            if (idx < 0)
                                break;
                            i           = idx + 1;

                            // Get and execute shortcut link
                            Shortcut *scut = pShortcuts->get(idx);
                            if (scut == NULL)
                                continue;

                            ws::event_t xev;
                            if (ev != NULL)
                                xev         = *ev;
                            res    = scut->slot()->execute(widget, (ev != NULL) ? &xev : NULL);
                            if (res != STATUS_OK)
                                break;
                        }
                    }

                    break;
                }
                case ws::UIE_KEY_UP:
                {
                    if (set_modifier(ev->nCode, false))
                        return STATUS_OK;
                    pop_key(ev->nCode);
                    break;
                }
                default:
                    break;
            }

            return res;
        }

        bool ShortcutTracker::enabled() const
        {
            return nEnabled >= 0;
        }

        bool ShortcutTracker::set_enabled(bool enabled)
        {
            return (enabled) ? enable() : disable();
        }

        bool ShortcutTracker::enable()
        {
            const bool state = enabled();
            ++nEnabled;
            return state;
        }

        bool ShortcutTracker::disable()
        {
            const bool state = enabled();
            --nEnabled;
            return state;
        }

    } /* namespace tk */
} /* namespace lsp */



