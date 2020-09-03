/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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
            private:
                BorderStyle & operator = (const BorderStyle &);

            protected:
                static const prop::enum_t ENUM[];

            protected:
                explicit BorderStyle(prop::Listener *listener = NULL): Enum(ENUM, listener) {};

            public:
                inline ws::border_style_t   get() const     { return ws::border_style_t(nValue); }

                inline ws::border_style_t   set(ws::border_style_t v)
                    { return ws::border_style_t(Enum::set(v)); };
        };

        namespace prop
        {
            class BorderStyle: public tk::BorderStyle
            {
                private:
                    BorderStyle & operator = (const BorderStyle &);

                public:
                    explicit BorderStyle(prop::Listener *listener = NULL): tk::BorderStyle(listener) {};

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

                    /**
                     * Initialize default value
                     * @param style style
                     * @param bs default value
                     * @return status of operation
                     */
                    inline status_t     init(Style *style, ws::border_style_t bs)       { return tk::BorderStyle::init(style, bs);      };
                    inline status_t     override(Style *style, ws::border_style_t bs)   { return tk::BorderStyle::override(style, bs);  };
            };
        }
    }
}



#endif /* LSP_PLUG_IN_TK_PROP_ENUM_BORDERSTYLE_H_ */
