/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 2 июн. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_ENUM_BUTTONMODE_H_
#define LSP_PLUG_IN_TK_PROP_ENUM_BUTTONMODE_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class ButtonMode: public Enum
        {
            private:
                ButtonMode & operator = (const ButtonMode &);
                ButtonMode(const ButtonMode &);

            protected:
                static const prop::enum_t ENUM[];

            protected:
                explicit ButtonMode(prop::Listener *listener = NULL): Enum(ENUM, BM_NORMAL, listener) {};

            public:
                inline button_mode_t    get() const                 { return button_mode_t(nValue); }
                inline button_mode_t    set(button_mode_t v)        { return button_mode_t(Enum::set(v)); }

                inline bool             normal() const              { return nValue == BM_NORMAL;   }
                inline bool             toggle() const              { return nValue == BM_TOGGLE;   }
                inline bool             trigger() const             { return nValue == BM_TRIGGER;  }

                inline button_mode_t    set_normal()                { return set(BM_NORMAL);        }
                inline button_mode_t    set_toggle()                { return set(BM_TOGGLE);        }
                inline button_mode_t    set_trigger()               { return set(BM_TRIGGER);       }
        };

        namespace prop
        {
            class ButtonMode: public tk::ButtonMode
            {
                private:
                    ButtonMode & operator = (const ButtonMode &);
                    ButtonMode(const ButtonMode &);

                public:
                    explicit ButtonMode(prop::Listener *listener = NULL): tk::ButtonMode(listener) {};

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::ButtonMode::bind(property, style); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::ButtonMode::bind(property, style); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::ButtonMode::bind(property, style); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::ButtonMode::unbind(); };

                    inline void         listener(prop::Listener *listener)              { pListener = listener;                     }
            };
        }
    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_PROP_ENUM_BUTTONMODE_H_ */
