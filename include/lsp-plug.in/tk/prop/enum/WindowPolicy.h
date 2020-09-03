/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 16 мая 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_ENUM_WINDOWPOLICY_H_
#define LSP_PLUG_IN_TK_PROP_ENUM_WINDOWPOLICY_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class WindowPolicy: public Enum
        {
            private:
                WindowPolicy & operator = (const WindowPolicy &);

            protected:
                static const prop::enum_t ENUM[];

            protected:
                explicit WindowPolicy(prop::Listener *listener = NULL): Enum(ENUM, listener) {};

            public:
                inline window_policy_t      get() const             { return window_policy_t(nValue); }
                inline window_policy_t      set(window_policy_t v)  { return window_policy_t(Enum::set(v)); };
        };
    
        namespace prop
        {
            class WindowPolicy: public tk::WindowPolicy
            {
                private:
                    WindowPolicy & operator = (const WindowPolicy &);

                public:
                    explicit WindowPolicy(prop::Listener *listener = NULL): tk::WindowPolicy(listener) {};

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::WindowPolicy::bind(property, style); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::WindowPolicy::bind(property, style); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::WindowPolicy::bind(property, style); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::WindowPolicy::unbind(); };

                    /**
                     * Initialize default value
                     * @param style style
                     * @param wp default window policy
                     * @return status of operation
                     */
                    inline status_t     init(Style *style, window_policy_t wp)          { return tk::WindowPolicy::init(style, wp);      };
            };
        }
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_ENUM_WINDOWPOLICY_H_ */
