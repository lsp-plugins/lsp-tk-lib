/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 17 мая 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_ENUM_ORIENTATION_H_
#define LSP_PLUG_IN_TK_PROP_ENUM_ORIENTATION_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Orientation: public Enum
        {
            protected:
                static const prop::enum_t ENUM[];

            protected:
                explicit Orientation(prop::Listener *listener = NULL): Enum(ENUM, O_HORIZONTAL, listener) {};
                Orientation(const Orientation &) = delete;
                Orientation(Orientation &&) = delete;
                Orientation & operator = (const Orientation &) = delete;
                Orientation & operator = (Orientation &&) = delete;

            public:
                inline orientation_t        get() const                 { return orientation_t(nValue);         }
                inline orientation_t        set(orientation_t v)        { return orientation_t(Enum::set(v));   }

                inline bool                 horizontal() const          { return nValue == O_HORIZONTAL;        }
                inline bool                 vertical() const            { return nValue == O_VERTICAL;          }
                inline orientation_t        set_horizontal()            { return set(O_HORIZONTAL);             }
                inline orientation_t        set_vertical()              { return set(O_VERTICAL);               }
        };
    
        namespace prop
        {
            class Orientation: public tk::Orientation
            {
                public:
                    explicit Orientation(prop::Listener *listener = NULL): tk::Orientation(listener) {};
                    Orientation(const Orientation &) = delete;
                    Orientation(Orientation &&) = delete;
                    Orientation & operator = (const Orientation &) = delete;
                    Orientation & operator = (Orientation &&) = delete;

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::Orientation::bind(property, style); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::Orientation::bind(property, style); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::Orientation::bind(property, style); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::Orientation::unbind(); };

                    inline void         listener(prop::Listener *listener)              { pListener = listener;                     }
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_ENUM_ORIENTATION_H_ */
