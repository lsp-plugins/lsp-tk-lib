/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 13 июн. 2021 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_MULTI_ARRANGEMENT_H_
#define LSP_PLUG_IN_TK_PROP_MULTI_ARRANGEMENT_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Arrangement property.
         * Allows to align widget within parent widget
         * from the most left/top (-1) position to the
         * most right/bottom (1) position.
         *
         */
        class Arrangement: public MultiProperty
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
                explicit Arrangement(prop::Listener *listener = NULL);
                Arrangement(const Arrangement &) = delete;
                Arrangement(Arrangement &&) = delete;
                virtual ~Arrangement() override;

                Arrangement & operator = (const Arrangement &) = delete;
                Arrangement & operator = (Arrangement &&) = delete;

            public:
                inline float        halign() const              { return hAlign;    }
                inline float        valign() const              { return vAlign;    }

                float               set_halign(float v);
                float               set_valign(float v);

                void                set(float h, float v);

                inline void         set(float value)            { set(value, value);  }
                inline void         set_default()               { MultiProperty::set_default(vAtoms, DESC); }

            public:
                inline void         apply(ws::rectangle_t *dst, const ws::rectangle_t *area) { return apply(dst, dst, area); }
                void                apply(ws::rectangle_t *dst, const ws::rectangle_t *src, const ws::rectangle_t *area);

        };

        namespace prop
        {
            /**
             * Arrangement property implementation
             */
            class Arrangement: public tk::Arrangement
            {
                public:
                    explicit Arrangement(prop::Listener *listener = NULL): tk::Arrangement(listener) {};
                    Arrangement(const Arrangement &) = delete;
                    Arrangement(Arrangement &&) = delete;

                    Arrangement & operator = (const Arrangement &) = delete;
                    Arrangement & operator = (Arrangement &&) = delete;

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::Arrangement::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::Arrangement::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::Arrangement::bind(property, style, vAtoms, DESC, &sListener); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::Arrangement::unbind(vAtoms, DESC, &sListener); };

                    inline void         listener(prop::Listener *listener)              { pListener = listener;                     }
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */




#endif /* LSP_PLUG_IN_TK_PROP_MULTI_ARRANGEMENT_H_ */
