/*
 * RangeFloat.h
 *
 *  Created on: 4 июн. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_RANGEFLOAT_H_
#define LSP_PLUG_IN_TK_PROP_RANGEFLOAT_H_

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
                    P_MIN,
                    P_MAX,
                    P_STEP,
                    P_QUICK,
                    P_SLOW,

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
                float               fStep;
                float               fQuick;
                float               fSlow;
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
                inline float        step() const            { return fStep;                 }
                inline float        quick() const           { return fQuick;                }
                inline float        slow() const            { return fSlow;                 }

                float               set(float v);
                float               set_min(float v);
                float               set_max(float v);
                void                set_range(float min, float max);
                void                set_step(float step, float quick, float slow);
                float               set_step(float v);
                float               set_quick(float v);
                float               set_slow(float v);

                float               add(float value, bool cyclic = false);
                float               sub(float value, bool cyclic = false);
                inline float        change_step(float k)    { return change(k, fStep);      }
                inline float        change_quick(float k)   { return change(k, fQuick);     }
                inline float        change_slow(float k)    { return change(k, fSlow);      }

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
                    status_t            init(Style *style, float value, float min, float max, float step, float quick, float slow);
                    inline status_t     init(Style *style, float value, float min = 0.0f, float max = 1.0f, float step = 0.1f) { return init(style, value, min, max, step, step * 10.0f, step * 0.1f); }
            };
        }

    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_PROP_RANGEFLOAT_H_ */
