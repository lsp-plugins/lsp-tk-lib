/*
 * Scrolling.h
 *
 *  Created on: 17 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_ENUM_SCROLLING_H_
#define LSP_PLUG_IN_TK_PROP_ENUM_SCROLLING_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Scrolling: public Enum
        {
            private:
                Scrolling & operator = (const Scrolling &);

            protected:
                static const prop::enum_t ENUM[];

            protected:
                explicit Scrolling(prop::Listener *listener = NULL): Enum(ENUM, listener) {};

            public:
                inline scrolling_t          get() const                 { return scrolling_t(nValue);           }
                inline scrolling_t          set(scrolling_t v)          { return scrolling_t(Enum::set(v));     }

                inline bool                 none() const                { return nValue == SCROLL_NONE;         }
                inline bool                 clip() const                { return nValue == SCROLL_CLIP;         }
                inline bool                 optional() const            { return nValue == SCROLL_OPTIONAL;     }
                inline bool                 always() const              { return nValue == SCROLL_ALWAYS;       }


                inline scrolling_t          set_none()                  { return set(SCROLL_NONE);              }
                inline scrolling_t          set_clip()                  { return set(SCROLL_CLIP);              }
                inline scrolling_t          set_optional()              { return set(SCROLL_OPTIONAL);          }
                inline scrolling_t          set_always()                { return set(SCROLL_ALWAYS);            }
        };
    
        namespace prop
        {
            class Scrolling: public tk::Scrolling
            {
                private:
                    Scrolling & operator = (const Scrolling &);

                public:
                    explicit Scrolling(prop::Listener *listener = NULL): tk::Scrolling(listener) {};

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::Scrolling::bind(property, style); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::Scrolling::bind(property, style); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::Scrolling::bind(property, style); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::Scrolling::unbind(); };

                    /**
                     * Initialize default value
                     * @param style style
                     * @param o default orientation
                     * @return status of operation
                     */
                    inline status_t     init(Style *style, scrolling_t o)               { return tk::Scrolling::init(style, o);      };
            };
        }
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_ENUM_SCROLLING_H_ */
