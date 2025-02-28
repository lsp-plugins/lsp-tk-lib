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

#ifndef LSP_PLUG_IN_TK_PROP_ENUM_POINTER_H_
#define LSP_PLUG_IN_TK_PROP_ENUM_POINTER_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Mouse pointer enumeration
         */
        class Pointer: public Enum
        {
            protected:
                static const prop::enum_t ENUM[];

            protected:
                explicit Pointer(prop::Listener *listener = NULL): Enum(ENUM, ws::MP_DEFAULT, listener) {};
                Pointer(const Pointer &) = delete;
                Pointer(Pointer &&) = delete;
                Pointer & operator = (const Pointer &) = delete;
                Pointer & operator = (Pointer &&) = delete;

            public:
                inline ws::mouse_pointer_t  get() const                         { return ws::mouse_pointer_t(nValue);           }
                inline ws::mouse_pointer_t  get(ws::mouse_pointer_t dfl) const  { return (nValue == ws::MP_DEFAULT) ? dfl : ws::mouse_pointer_t(nValue);    }
                inline ws::mouse_pointer_t  set(ws::mouse_pointer_t v)          { return ws::mouse_pointer_t(Enum::set(v));     }
        };
    
        namespace prop
        {
            class Pointer: public tk::Pointer
            {
                public:
                    explicit Pointer(prop::Listener *listener = NULL): tk::Pointer(listener) {};
                    Pointer(const Pointer &) = delete;
                    Pointer(Pointer &&) = delete;
                    Pointer & operator = (const Pointer &) = delete;
                    Pointer & operator = (Pointer &&) = delete;

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::Pointer::bind(property, style); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::Pointer::bind(property, style); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::Pointer::bind(property, style); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::Pointer::unbind(); }

                    inline void         listener(prop::Listener *listener)              { pListener = listener;                     }
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_ENUM_POINTER_H_ */
