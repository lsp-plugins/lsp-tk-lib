/*
 * TextTextLayout.h
 *
 *  Created on: 2 июн. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_MULTI_TEXTLAYOUT_H_
#define LSP_PLUG_IN_TK_PROP_MULTI_TEXTLAYOUT_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Text TextLayout property.
         * Allows to align text placed on the widget
         * The value -1.0f means leftmost/topmost alignment,
         * the value 1.0f means rightmost/bottommost alignment
         */
        class TextLayout: public MultiProperty
        {
            protected:
                TextLayout & operator = (const TextLayout &);

            protected:
                enum property_t
                {
                    P_VALUE,
                    P_HALIGN,
                    P_VALIGN,

                    P_COUNT
                };

                class Listener: public IStyleListener
                {
                    private:
                        TextLayout        *pValue;

                    public:
                        inline Listener(TextLayout *color)   { pValue = color; };

                    public:
                        virtual void    notify(atom_t property);
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                float               hAlign;             // Horizontal alignment
                float               vAlign;             // Vertical alignment
                Listener            sListener;          // Listener

            protected:
                void                sync();
                void                commit(atom_t property);
                void                parse(const LSPString *s);

            protected:
                explicit TextLayout(prop::Listener *listener = NULL);
                ~TextLayout();

            public:
                inline float        halign() const              { return hAlign;    }
                inline float        valign() const              { return vAlign;    }

                float               set_halign(float v);
                float               set_valign(float v);

                void                set(float halign, float valign);

                inline void         set(float value)            { set(value, value);  }
        };

        namespace prop
        {
            /**
             * TextLayout property implementation
             */
            class TextLayout: public tk::TextLayout
            {
                private:
                    TextLayout & operator = (const TextLayout &);

                public:
                    explicit TextLayout(prop::Listener *listener = NULL): tk::TextLayout(listener) {};

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::TextLayout::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::TextLayout::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::TextLayout::bind(property, style, vAtoms, DESC, &sListener); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::TextLayout::unbind(vAtoms, DESC, &sListener); };

                    /**
                     * Initialize default values
                     * @return status of operation
                     */
                    status_t            init(Style *style, float halign, float valign);
            };
        }

    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_PROP_MULTI_TEXTLAYOUT_H_ */
