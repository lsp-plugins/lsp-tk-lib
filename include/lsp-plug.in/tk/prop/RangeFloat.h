/*
 * RangeFloat.h
 *
 *  Created on: 4 июн. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_RANGEFLOAT_H_
#define LSP_PLUG_IN_TK_PROP_RANGEFLOAT_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Floating-point property interface
         */
        class RangeFloat: public MultiProperty
        {
            private:
                RangeFloat & operator = (const RangeFloat &);

            protected:
                enum property_t
                {
                    P_VALUE,
                    P_RVALUE,
                    P_MIN,
                    P_MAX,

                    P_COUNT
                };


                class Listener: public IStyleListener
                {
                    protected:
                        RangeFloat      *pValue;

                    public:
                        inline Listener(RangeFloat *ptr)             { pValue = ptr;     }

                    public:
                        virtual void notify(atom_t property);
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                float               fValue;
                float               fMin;
                float               fMax;
                Listener            sListener;

            protected:
                void                sync();
                void                commit(atom_t property);
                float               climited(float v) const;
                float               change(float k, float step);

            protected:
                explicit RangeFloat(prop::Listener *listener = NULL);
                ~RangeFloat();

            public:
                inline float        get() const             { return limit(fValue);         }
                inline float        min() const             { return fMin;                  }
                inline float        max() const             { return fMax;                  }
                inline float        range() const           { return fMax - fMin;           }
                inline float        abs_range() const       { return (fMax > fMin) ? fMax - fMin : fMin - fMax; }

                float               set(float v);
                float               set_min(float v);
                float               set_max(float v);
                void                set_range(float min, float max);

                float               add(float value, bool cyclic = false);
                float               sub(float value, bool cyclic = false);

                float               get_normalized(float value) const;
                inline float        get_normalized() const  { return get_normalized(fValue); }
                float               set_normalized(float value, bool cyclic = false);
                float               limit(float v) const;
        };

        namespace prop
        {
            /**
             * RangeFloat property implementation
             */
            class RangeFloat: public tk::RangeFloat
            {
                private:
                    RangeFloat & operator = (const RangeFloat &);

                public:
                    explicit RangeFloat(prop::Listener *listener = NULL): tk::RangeFloat(listener) {};

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::RangeFloat::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::RangeFloat::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::RangeFloat::bind(property, style, vAtoms, DESC, &sListener); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::RangeFloat::unbind(vAtoms, DESC, &sListener); };

                    /**
                     * Initialize
                     * @param style style
                     * @return status of operation
                     */
                    status_t            init(Style *style, float value, float min, float max);
                    inline status_t     init(Style *style, float value)                 { return init(style, value, 0.0f, 1.0f); }
            };
        }

    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_PROP_RANGEFLOAT_H_ */
