/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 12 авг. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_MULTI_TEXTFITNESS_H_
#define LSP_PLUG_IN_TK_PROP_MULTI_TEXTFITNESS_H_

namespace lsp
{
    namespace tk
    {
        /**
         * Text fitness: 0 means no text fitness, 1.0 means full text fitness
         */
        class TextFitness: public MultiProperty
        {
            protected:
                enum property_t
                {
                    P_VALUE,
                    P_HFIT,
                    P_VFIT,

                    P_COUNT
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                float               hFit;               // Horizontal fitness
                float               vFit;               // Vertical fitness

            protected:
                virtual void        push() override;
                virtual void        commit(atom_t property) override;

                void                parse(const LSPString *s);

            protected:
                explicit TextFitness(prop::Listener *listener = NULL);
                TextFitness(const TextFitness &) = delete;
                TextFitness(TextFitness &&) = delete;
                virtual ~TextFitness() override;

                TextFitness & operator = (const TextFitness &) = delete;
                TextFitness & operator = (TextFitness &&) = delete;

            public:
                inline float        hfit() const                { return hFit;      }
                inline float        vfit() const                { return vFit;      }

                float               set_hfit(float v);
                float               set_vfit(float v);

                void                set(float hfit, float vfit);
                inline void         set(float value)            { set(value, value);  }

                void                apply(ws::rectangle_t *r);
        };

        namespace prop
        {
            /**
             * TextFitness property implementation
             */
            class TextFitness: public tk::TextFitness
            {
                public:
                    explicit TextFitness(prop::Listener *listener = NULL): tk::TextFitness(listener) {};
                    TextFitness(const TextFitness &) = delete;
                    TextFitness(TextFitness &&) = delete;

                    TextFitness & operator = (const TextFitness &) = delete;
                    TextFitness & operator = (TextFitness &&) = delete;

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::TextFitness::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::TextFitness::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::TextFitness::bind(property, style, vAtoms, DESC, &sListener); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::TextFitness::unbind(vAtoms, DESC, &sListener); };
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */


#endif /* LSP_PLUG_IN_TK_PROP_MULTI_TEXTFITNESS_H_ */
