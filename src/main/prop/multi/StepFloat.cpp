/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/stdlib/locale.h>

namespace lsp
{
    namespace tk
    {
        const prop::desc_t StepFloat::DESC[] =
        {
            { "",               PT_FLOAT    },
            { ".step",          PT_FLOAT    },
            { ".accel",         PT_FLOAT    },
            { ".decel",         PT_FLOAT    },
            { NULL,             PT_UNKNOWN  }
        };

        StepFloat::StepFloat(prop::Listener *listener):
            MultiProperty(vAtoms, P_COUNT, listener)
        {
            fStep       = 0.01f;
            fAccel      = 10.0f;
            fDecel      = 0.1f;
        }

        StepFloat::~StepFloat()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void StepFloat::commit(atom_t property)
        {
            float v;
            LSPString s;

            if ((property == vAtoms[P_STEP]) && (pStyle->get_float(vAtoms[P_STEP], &v) == STATUS_OK))
                fStep           = v;
            if ((property == vAtoms[P_ACCEL]) && (pStyle->get_float(vAtoms[P_ACCEL], &v) == STATUS_OK))
                fAccel          = v;
            if ((property == vAtoms[P_DECEL]) && (pStyle->get_float(vAtoms[P_DECEL], &v) == STATUS_OK))
                fDecel          = v;

            // Compound property
            if ((property == vAtoms[P_VALUE]) && (pStyle->get_string(vAtoms[P_VALUE], &s) == STATUS_OK))
            {
                float v[3];
                size_t n    = Property::parse_floats(v, 3, &s);
                switch (n)
                {
                    case 1:
                        fStep       = v[0];
                        fAccel      = 10.0f;
                        fDecel      = 0.1f;
                        break;
                    case 2:
                        fStep       = v[0];
                        fAccel      = v[1];
                        fDecel      = 1.0f / v[1];
                        break;
                    case 3:
                        fStep       = v[0];
                        fAccel      = v[1];
                        fDecel      = v[2];
                        break;
                    default:
                        break;
                }
            }
        }

        void StepFloat::push()
        {
            LSPString s;

            // Simple components
            if (vAtoms[P_STEP] >= 0)
                pStyle->set_float(vAtoms[P_STEP], fStep);
            if (vAtoms[P_ACCEL] >= 0)
                pStyle->set_float(vAtoms[P_ACCEL], fAccel);
            if (vAtoms[P_DECEL] >= 0)
                pStyle->set_float(vAtoms[P_DECEL], fDecel);

            // Compound properties
            SET_LOCALE_SCOPED(LC_NUMERIC, "C");
            s.fmt_ascii("%.10f %.10f %.10f", fStep, fAccel, fDecel);
            if (vAtoms[P_VALUE] >= 0)
                pStyle->set_string(vAtoms[P_VALUE], &s);
        }

        float StepFloat::set(float v)
        {
            float old = fStep;
            if (old == v)
                return old;

            fStep       = v;
            sync();
            return old;
        }

        void StepFloat::set(float v, float accel, float decel)
        {
            if ((fStep == v) &&
                (fAccel == accel) &&
                (fDecel == decel))
                return;

            fStep       = v;
            fAccel      = accel;
            fDecel      = decel;
            sync();
        }

        float StepFloat::set_accel(float v)
        {
            float old = fAccel;
            if (old == v)
                return old;

            fAccel      = v;
            sync();
            return old;
        }

        float StepFloat::set_decel(float v)
        {
            float old = fDecel;
            if (old == v)
                return old;

            fDecel      = v;
            sync();
            return old;
        }

        float StepFloat::get(bool accel, bool decel) const
        {
            if (accel != decel)
                return (accel) ? fStep * fAccel : fStep * fDecel;

            return fStep;
        }

    } /* namespace tk */
} /* namespace lsp */


