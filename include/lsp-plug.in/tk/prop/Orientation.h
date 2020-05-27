/*
 * Orientation.h
 *
 *  Created on: 17 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_ORIENTATION_H_
#define LSP_PLUG_IN_TK_PROP_ORIENTATION_H_

namespace lsp
{
    namespace tk
    {
        class Orientation: public Enum
        {
            private:
                Orientation & operator = (const Orientation &);

            protected:
                static const prop::enum_t ENUM[];

            protected:
                explicit Orientation(prop::Listener *listener = NULL): Enum(ENUM, listener) {};

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
                private:
                    Orientation & operator = (const Orientation &);

                public:
                    explicit Orientation(prop::Listener *listener = NULL): tk::Orientation(listener) {};

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

                    /**
                     * Initialize default value
                     * @param style style
                     * @param o default orientation
                     * @return status of operation
                     */
                    inline status_t     init(Style *style, orientation_t o)             { return tk::Orientation::init(style, o);      };
            };
        }
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_ORIENTATION_H_ */
