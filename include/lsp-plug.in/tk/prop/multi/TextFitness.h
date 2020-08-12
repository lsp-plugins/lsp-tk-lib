/*
 * TextFitness.h
 *
 *  Created on: 12 авг. 2020 г.
 *      Author: sadko
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
            };
        }
    }
}

#endif /* LSP_PLUG_IN_TK_PROP_MULTI_TEXTFITNESS_H_ */
