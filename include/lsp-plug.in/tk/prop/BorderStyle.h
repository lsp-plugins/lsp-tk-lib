/*
 * BorderStyle.h
 *
 *  Created on: 8 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_BORDERSTYLE_H_
#define LSP_PLUG_IN_TK_PROP_BORDERSTYLE_H_

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
                    BorderStyle & operator = (const BorderStyle *);

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
            };
        }
    }
}



#endif /* LSP_PLUG_IN_TK_PROP_BORDERSTYLE_H_ */
