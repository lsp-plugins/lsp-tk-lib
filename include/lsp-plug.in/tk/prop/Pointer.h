/*
 * Pointer.h
 *
 *  Created on: 17 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_POINTER_H_
#define LSP_PLUG_IN_TK_PROP_POINTER_H_

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
            private:
                Pointer & operator = (const Pointer &);

            protected:
                static const prop::enum_t ENUM[];

            protected:
                explicit Pointer(prop::Listener *listener = NULL): Enum(ENUM, listener) {};

            public:
                inline ws::mouse_pointer_t  get() const                         { return ws::mouse_pointer_t(nValue);           }
                inline ws::mouse_pointer_t  get(ws::mouse_pointer_t dfl) const  { return (nValue == ws::MP_DEFAULT) ? dfl : ws::mouse_pointer_t(nValue);    }
                inline ws::mouse_pointer_t  set(ws::mouse_pointer_t v)          { return ws::mouse_pointer_t(Enum::set(v));     }
        };
    
        namespace prop
        {
            class Pointer: public tk::Pointer
            {
                private:
                    Pointer & operator = (const Pointer &);

                public:
                    explicit Pointer(prop::Listener *listener = NULL): tk::Pointer(listener) {};

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

                    /**
                     * Initialize default value
                     * @param style style
                     * @param mp default mouse pointer
                     * @return status of operation
                     */
                    inline status_t     init(Style *style, ws::mouse_pointer_t mp)      { return tk::Pointer::init(style, mp);      }
                    inline status_t     override(Style *style, ws::mouse_pointer_t mp)  { return tk::Pointer::override(style, mp);  }
            };
        }
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_POINTER_H_ */
