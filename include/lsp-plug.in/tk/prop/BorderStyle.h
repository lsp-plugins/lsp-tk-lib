/*
 * BorderStyle.h
 *
 *  Created on: 8 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_BORDERSTYLE_H_
#define LSP_PLUG_IN_TK_PROP_BORDERSTYLE_H_

#include <lsp-plug.in/tk/version.h>
#include <lsp-plug.in/tk/types.h>

#include <lsp-plug.in/tk/style.h>
#include <lsp-plug.in/tk/prop.h>
#include <lsp-plug.in/ws/types.h>

namespace lsp
{
    namespace tk
    {
        class Display;
        class Widget;

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
                    status_t            bind(const char *property, Widget *widget);
                    status_t            bind(atom_t property, Widget *widget);
                    status_t            bind(const char *property, Display *dpy, Style *style);
                    status_t            bind(atom_t property, Style *style);

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                    { return tk::BorderStyle::unbind(); };
            };
        }
    }
}



#endif /* LSP_PLUG_IN_TK_PROP_BORDERSTYLE_H_ */
