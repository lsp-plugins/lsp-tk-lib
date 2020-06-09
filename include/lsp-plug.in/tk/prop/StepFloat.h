/*
 * StepFloat.h
 *
 *  Created on: 6 июн. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_STEPFLOAT_H_
#define LSP_PLUG_IN_TK_PROP_STEPFLOAT_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Floating-point value change step with accelerators:
         *   step  - the change step for regular change
         *   accel - acceleracion multiplier to the step
         *   decel - deceleration multiplier to the step
         */
        class StepFloat: public MultiProperty
        {
            private:
                StepFloat & operator = (const StepFloat &);

            protected:
                enum property_t
                {
                    P_VALUE,
                    P_STEP,
                    P_ACCEL,
                    P_DECEL,

                    P_COUNT
                };

                class Listener: public IStyleListener
                {
                    protected:
                        StepFloat      *pValue;

                    public:
                        inline Listener(StepFloat *ptr)             { pValue = ptr;     }

                    public:
                        virtual void notify(atom_t property);
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                float               fStep;
                float               fAccel;
                float               fDecel;
                Listener            sListener;

            protected:
                void                sync();
                void                commit(atom_t property);
                float               climited(float v) const;
                float               change(float k, float step);

            protected:
                explicit StepFloat(prop::Listener *listener = NULL);
                ~StepFloat();

            public:
                inline float        get() const             { return fStep;                             }
                inline float        get_abs() const         { return (fStep >= 0.0f) ? fStep : -fStep;  }
                inline float        accel() const           { return fAccel;                            }
                inline float        decel() const           { return fDecel;                            }
                inline float        step() const            { return fStep;                             }
                inline float        step_accel() const      { return fStep * fAccel;                    }
                inline float        step_decel() const      { return fStep * fDecel;                    }

                float               set(float v);
                void                set(float v, float accel, float decel);
                float               set_accel(float v);
                float               set_decel(float v);

                inline void         set_step(float v, float accel, float decel)     { set(v, accel / v, decel / v);     }
                float               set_step_accel(float v) { return set_accel(v / fStep);  }
                float               set_step_decel(float v) { return set_decel(v / fStep);  }
                inline float        set_step(float v)       { return set(v);                }
        };

        namespace prop
        {
            /**
             * StepFloat property implementation
             */
            class StepFloat: public tk::StepFloat
            {
                private:
                    StepFloat & operator = (const StepFloat &);

                public:
                    explicit StepFloat(prop::Listener *listener = NULL): tk::StepFloat(listener) {};

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::StepFloat::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::StepFloat::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::StepFloat::bind(property, style, vAtoms, DESC, &sListener); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::StepFloat::unbind(vAtoms, DESC, &sListener); };

                    /**
                     * Initialize
                     * @param style style
                     * @return status of operation
                     */
                    status_t            init(Style *style, float step, float accel = 10.0f, float decel = 0.1f);
            };
        }

    } /* namespace tk */
} /* namespace lsp */


#endif /* LSP_PLUG_IN_TK_PROP_STEPFLOAT_H_ */
