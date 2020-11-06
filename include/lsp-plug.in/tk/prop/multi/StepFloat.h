/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 6 июн. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_MULTI_STEPFLOAT_H_
#define LSP_PLUG_IN_TK_PROP_MULTI_STEPFLOAT_H_

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

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                float               fStep;
                float               fAccel;
                float               fDecel;

            protected:
                virtual void        push();
                virtual void        commit(atom_t property);

                float               climited(float v) const;
                float               change(float k, float step);

            protected:
                explicit StepFloat(prop::Listener *listener = NULL);
                virtual ~StepFloat();

            public:
                inline float        get() const             { return fStep;                             }
                inline float        get_abs() const         { return (fStep >= 0.0f) ? fStep : -fStep;  }
                inline float        accel() const           { return fAccel;                            }
                inline float        decel() const           { return fDecel;                            }
                inline float        step() const            { return fStep;                             }
                inline float        step_accel() const      { return fStep * fAccel;                    }
                inline float        step_decel() const      { return fStep * fDecel;                    }
                float               get(bool accel, bool decel = false) const;

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

            };
        }

    } /* namespace tk */
} /* namespace lsp */


#endif /* LSP_PLUG_IN_TK_PROP_MULTI_STEPFLOAT_H_ */
