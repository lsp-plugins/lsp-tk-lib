/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 2 июн. 2020 г.
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
                enum property_t
                {
                    P_VALUE,
                    P_HALIGN,
                    P_VALIGN,

                    P_COUNT
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                float               hAlign;             // Horizontal alignment
                float               vAlign;             // Vertical alignment

            protected:
                virtual void        push() override;
                virtual void        commit(atom_t property) override;

                void                parse(const LSPString *s);

            protected:
                explicit TextLayout(prop::Listener *listener = NULL);
                TextLayout(const TextLayout &) = delete;
                TextLayout(TextLayout &&) = delete;
                virtual ~TextLayout() override;

                TextLayout & operator = (const TextLayout &) = delete;
                TextLayout & operator = (TextLayout &&) = delete;

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
                public:
                    explicit TextLayout(prop::Listener *listener = NULL): tk::TextLayout(listener) {};
                    TextLayout(const TextLayout &) = delete;
                    TextLayout(TextLayout &&) = delete;

                    TextLayout & operator = (const TextLayout &) = delete;
                    TextLayout & operator = (TextLayout &&) = delete;

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

                    inline void         listener(prop::Listener *listener)              { pListener = listener;                     }
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */


#endif /* LSP_PLUG_IN_TK_PROP_MULTI_TEXTLAYOUT_H_ */
