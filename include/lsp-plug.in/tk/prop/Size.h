/*
 * Size.h
 *
 *  Created on: 10 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_SIZE_H_
#define LSP_PLUG_IN_TK_PROP_SIZE_H_

#include <lsp-plug.in/tk/version.h>
#include <lsp-plug.in/tk/types.h>

#include <lsp-plug.in/tk/style.h>
#include <lsp-plug.in/tk/prop.h>
#include <lsp-plug.in/runtime/Color.h>

namespace lsp
{
    namespace tk
    {
        class Display;
        class Widget;
        class Style;

        class Size: public MultiProperty
        {
            protected:
                Size & operator = (const Size &);

            protected:
                enum property_t
                {
                    P_VALUE,
                    P_WIDTH,
                    P_HEIGHT,

                    P_COUNT
                };

                class Listener: public IStyleListener
                {
                    private:
                        Size        *pValue;

                    public:
                        inline Listener(Size *color)   { pValue = color; };

                    public:
                        virtual void    notify(atom_t property);
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                size_t              nWidth;             // Width
                size_t              nHeight;            // Height
                Listener            sListener;          // Listener

            protected:
                void                sync();
                void                commit(atom_t property);
                void                parse(const LSPString *s);

            protected:
                explicit Size(prop::Listener *listener = NULL);
                ~Size();

            public:
                inline void         set_default()       { MultiProperty::set_default(vAtoms, DESC); };

            public:
                inline size_t width() const             { return nWidth;                        }
                inline size_t height() const            { return nHeight;                       }

                inline void get(size_t *width, size_t *height) const        { *width = nWidth; *height = nHeight;           }

                size_t  set_width(size_t value);
                size_t  set_height(size_t value);
                void    set(size_t width, size_t height);
                void    set(const Size *p);
        };

        namespace prop
        {
            /**
             * Size property implementation
             */
            class Size: public tk::Size
            {
                private:
                    Size & operator = (const Size *);

                public:
                    explicit Size(prop::Listener *listener = NULL): tk::Size(listener) {};

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    status_t            bind(const char *property, Widget *widget);
                    status_t            bind(atom_t property, Widget *widget);
                    status_t            bind(const char *property, Style *style, Display *dpy);
                    status_t            bind(atom_t property, Style *style, Display *dpy);

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()            { return MultiProperty::unbind(vAtoms, DESC, &sListener); };
            };
        }
    }
}



#endif /* LSP_PLUG_IN_TK_PROP_SIZE_H_ */
