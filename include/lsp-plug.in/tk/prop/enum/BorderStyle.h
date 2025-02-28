/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 8 мая 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_ENUM_BORDERSTYLE_H_
#define LSP_PLUG_IN_TK_PROP_ENUM_BORDERSTYLE_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Border style class of window
         */
        class BorderStyle: public Enum
        {
            protected:
                static const prop::enum_t ENUM[];

            protected:
                explicit BorderStyle(prop::Listener *listener = NULL): Enum(ENUM, ws::BS_SINGLE, listener) {};
                BorderStyle(const BorderStyle &) = delete;
                BorderStyle(BorderStyle &&) = delete;
                BorderStyle & operator = (const BorderStyle &) = delete;
                BorderStyle & operator = (BorderStyle &&) = delete;

            public:
                inline ws::border_style_t   get() const     { return ws::border_style_t(nValue); }

                inline ws::border_style_t   set(ws::border_style_t v)
                    { return ws::border_style_t(Enum::set(v)); };
        };

        namespace prop
        {
            class BorderStyle: public tk::BorderStyle
            {
                public:
                    explicit BorderStyle(prop::Listener *listener = NULL): tk::BorderStyle(listener) {};
                    BorderStyle(const BorderStyle &) = delete;
                    BorderStyle(BorderStyle &&) = delete;
                    BorderStyle & operator = (const BorderStyle &) = delete;
                    BorderStyle & operator = (BorderStyle &&) = delete;

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::BorderStyle::bind(property, style); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::BorderStyle::bind(property, style); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::BorderStyle::bind(property, style); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::BorderStyle::unbind(); };

                    inline void         listener(prop::Listener *listener)              { pListener = listener;                     }
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */


#endif /* LSP_PLUG_IN_TK_PROP_ENUM_BORDERSTYLE_H_ */
