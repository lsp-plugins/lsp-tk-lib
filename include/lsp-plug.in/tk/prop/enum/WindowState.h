/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 15 мая 2025 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_ENUM_WINDOWSTATE_H_
#define LSP_PLUG_IN_TK_PROP_ENUM_WINDOWSTATE_H_

namespace lsp
{
    namespace tk
    {
        /**
         * Border style class of window
         */
        class WindowState: public Enum
        {
            protected:
                static const prop::enum_t ENUM[];

            protected:
                explicit WindowState(prop::Listener *listener = NULL): Enum(ENUM, ws::BS_SINGLE, listener) {};
                WindowState(const WindowState &) = delete;
                WindowState(WindowState &&) = delete;
                WindowState & operator = (const WindowState &) = delete;
                WindowState & operator = (WindowState &&) = delete;

            public:
                inline ws::window_state_t   get() const     { return ws::window_state_t(nValue); }
                inline ws::window_state_t   set(ws::window_state_t v)
                    { return ws::window_state_t(Enum::set(v)); };

                inline bool                 minimized() const   { return get() == ws::WS_MINIMIZED; }
                inline bool                 maximized() const   { return get() == ws::WS_MAXIMIZED; }
                inline bool                 normal() const      { return get() == ws::WS_NORMAL;    }

                inline ws::window_state_t   set_minimized()     { return set(ws::WS_MINIMIZED);     }
                inline ws::window_state_t   set_maximized()     { return set(ws::WS_MAXIMIZED);     }
                inline ws::window_state_t   set_normal()        { return set(ws::WS_NORMAL);        }
        };

        namespace prop
        {
            class WindowState: public tk::WindowState
            {
                public:
                    explicit WindowState(prop::Listener *listener = NULL): tk::WindowState(listener) {};
                    WindowState(const WindowState &) = delete;
                    WindowState(WindowState &&) = delete;
                    WindowState & operator = (const WindowState &) = delete;
                    WindowState & operator = (WindowState &&) = delete;

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::WindowState::bind(property, style); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::WindowState::bind(property, style); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::WindowState::bind(property, style); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::WindowState::unbind(); };

                    /**
                     * Change value without notification of any listener
                     * @param value value to set
                     */
                    inline void         commit_value(ws::window_state_t state)          { nValue    = state;                        }

                    inline void         listener(prop::Listener *listener)              { pListener = listener;                     }
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_PROP_ENUM_WINDOWSTATE_H_ */
