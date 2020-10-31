/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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
                TextFitness & operator = (const TextFitness &);

            protected:
                enum property_t
                {
                    P_VALUE,
                    P_HFIT,
                    P_VFIT,

                    P_COUNT
                };

                class Listener: public IStyleListener
                {
                    private:
                        TextFitness        *pValue;

                    public:
                        inline Listener(TextFitness *color)   { pValue = color; };

                    public:
                        virtual void    notify(atom_t property);
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                float               hFit;               // Horizontal fitness
                float               vFit;               // Vertical fitness
                Listener            sListener;          // Listener

            protected:
                void                sync();
                void                commit(atom_t property);
                void                parse(const LSPString *s);
                status_t            init();
                status_t            override();

            protected:
                explicit TextFitness(prop::Listener *listener = NULL);
                ~TextFitness();

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
                private:
                    TextFitness & operator = (const TextFitness &);

                public:
                    explicit TextFitness(prop::Listener *listener = NULL): tk::TextFitness(listener) {};

                protected:
                    using               tk::TextFitness::init;
                    using               tk::TextFitness::override;

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

                    /**
                     * Initialize default values
                     * @return status of operation
                     */
                    status_t            init(Style *style, float hfit, float vfit);
                    inline status_t     init(Style *style, float fit)                   { return init(style, fit, fit);         }
                    inline status_t     init(Style *style)                              { return init(style, 1.0f, 1.0f);       }

                    static status_t            init(const char *name, Style *style, float hfit, float vfit);
                    static inline status_t     init(const char *name, Style *style, float fit)  { return init(name, style, fit, fit);         }
                    static inline status_t     init(const char *name, Style *style)             { return init(name, style, 1.0f, 1.0f);       }

                    static status_t            override(const char *name, Style *style, float hfit, float vfit);
                    static inline status_t     override(const char *name, Style *style, float fit)  { return override(name, style, fit, fit);         }
                    static inline status_t     override(const char *name, Style *style)             { return override(name, style, 1.0f, 1.0f);       }
            };
        }
    }
}

#endif /* LSP_PLUG_IN_TK_PROP_MULTI_TEXTFITNESS_H_ */
