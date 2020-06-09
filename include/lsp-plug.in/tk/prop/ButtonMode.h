/*
 * ButtonMode.h
 *
 *  Created on: 2 июн. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_BUTTONMODE_H_
#define LSP_PLUG_IN_TK_PROP_BUTTONMODE_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class ButtonMode: public Enum
        {
            private:
                ButtonMode & operator = (const ButtonMode &);

            protected:
                static const prop::enum_t ENUM[];

            protected:
                explicit ButtonMode(prop::Listener *listener = NULL): Enum(ENUM, listener) {};

            public:
                inline button_mode_t    get() const                 { return button_mode_t(nValue); }
                inline button_mode_t    set(button_mode_t v)        { return button_mode_t(Enum::set(v)); }

                inline bool             normal() const              { return nValue == BM_NORMAL;   }
                inline bool             toggle() const              { return nValue == BM_TOGGLE;   }
                inline bool             trigger() const             { return nValue == BM_TRIGGER;  }

                inline button_mode_t    set_normal()                { return set(BM_NORMAL);        }
                inline button_mode_t    set_toggle()                { return set(BM_TOGGLE);        }
                inline button_mode_t    set_trigger()               { return set(BM_TRIGGER);       }
        };

        namespace prop
        {
            class ButtonMode: public tk::ButtonMode
            {
                private:
                    ButtonMode & operator = (const ButtonMode &);

                public:
                    explicit ButtonMode(prop::Listener *listener = NULL): tk::ButtonMode(listener) {};

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::ButtonMode::bind(property, style); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::ButtonMode::bind(property, style); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::ButtonMode::bind(property, style); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::ButtonMode::unbind(); };

                    /**
                     * Initialize default value
                     * @param style style
                     * @param bm default button mode
                     * @return status of operation
                     */
                    inline status_t     init(Style *style, button_mode_t bm)            { return tk::ButtonMode::init(style, bm);      };
            };
        }
    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_PROP_BUTTONMODE_H_ */
