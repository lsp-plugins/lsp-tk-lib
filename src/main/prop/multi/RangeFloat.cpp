/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 23 мая 2023 г.
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
#include <lsp-plug.in/stdlib/math.h>

namespace lsp
{
    namespace tk
    {
        const prop::desc_t RangeFloat::DESC[] =
        {
            { "",               PT_STRING   },
            { ".value",         PT_FLOAT    },
            { ".min",           PT_FLOAT    },
            { ".max",           PT_FLOAT    },
            { NULL,             PT_UNKNOWN  }
        };

        RangeFloat::RangeFloat(prop::Listener *listener):
            MultiProperty(vAtoms, P_COUNT, listener)
        {
            fMin            = 0.0f;
            fMax            = 1.0f;
            fValue          = 0.0f;
            nFlags          = F_AUTO_LIMIT;
            pTransform      = NULL;
            pTransformArg   = NULL;
        }

        RangeFloat::~RangeFloat()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void RangeFloat::commit(atom_t property)
        {
            LSPString s;
            float v;
            if ((property == vAtoms[P_RVALUE]) && (pStyle->get_float(vAtoms[P_RVALUE], &v) == STATUS_OK))
                fValue          = v;

            if (!(nFlags & F_RANGE_LOCK))
            {
                if ((property == vAtoms[P_MIN]) && (pStyle->get_float(vAtoms[P_MIN], &v) == STATUS_OK))
                    fMin            = v;
                if ((property == vAtoms[P_MAX]) && (pStyle->get_float(vAtoms[P_MAX], &v) == STATUS_OK))
                    fMax            = v;
            }

            // Compound property
            if ((property == vAtoms[P_VALUE]) && (pStyle->get_string(vAtoms[P_VALUE], &s) == STATUS_OK))
            {
                float v[3];
                size_t n    = Property::parse_floats(v, 3, &s);
                switch (n)
                {
                    case 1:
                        fValue      = v[0];
                        fMin        = fValue;
                        fMax        = fValue;
                        break;
                    case 2:
                        fValue      = v[0];
                        fMin        = v[1];
                        fMax        = fValue*2.0f - fMin;
                        break;
                    case 3:
                        fValue      = v[0];
                        fMin        = v[1];
                        fMax        = v[2];
                        break;
                    default:
                        break;
                }
            }
        }

        void RangeFloat::push()
        {
            LSPString s;

            // Simple components
            if (vAtoms[P_RVALUE] >= 0)
                pStyle->set_float(vAtoms[P_RVALUE], fValue);
            if (vAtoms[P_MIN] >= 0)
                pStyle->set_float(vAtoms[P_MIN], fMin);
            if (vAtoms[P_MAX] >= 0)
                pStyle->set_float(vAtoms[P_MAX], fMax);

            // Compound properties
            SET_LOCALE_SCOPED(LC_NUMERIC, "C");
            s.fmt_ascii("%.10f %.10f %.10f", fValue, fMin, fMax);
            if (vAtoms[P_VALUE] >= 0)
                pStyle->set_string(vAtoms[P_VALUE], &s);
        }

        float RangeFloat::set_min(float value)
        {
            float old = fMin;
            if ((nFlags & F_RANGE_LOCK) || (value == old))
                return old;

            fMin                = value;
            sync();
            return old;
        }

        float RangeFloat::set_max(float value)
        {
            float old = fMax;
            if ((nFlags & F_RANGE_LOCK) || (value == old))
                return old;

            fMax                = value;
            sync();
            return old;
        }

        void RangeFloat::set_range(float min, float max)
        {
            if (nFlags & F_RANGE_LOCK)
                return;
            if ((min == fMin) &&
                (max == fMax))
                return;

            fMin                = min;
            fMax                = max;
            sync();
        }

        float RangeFloat::set_all(float value, float min, float max)
        {
            bool need_sync = false;

            if (!(nFlags & F_RANGE_LOCK))
            {
                if ((min != fMin) ||
                    (max != fMax))
                {
                    fMin                = min;
                    fMax                = max;
                    need_sync           = true;
                }
            }

            float old   = fValue;
            value       = do_limit(value);
            if (value != old)
            {
                fValue              = value;
                need_sync           = true;
            }

            if (need_sync)
                sync();
            return old;
        }

        float RangeFloat::set(float value)
        {
            float old   = fValue;
            value       = do_limit(value);
            if (value == old)
                return old;

            fValue              = value;
            sync();
            return old;
        }

        float RangeFloat::set_normalized(float value, bool cyclic)
        {
            float old   = fValue;
            if (cyclic)
                value       = value - truncf(value);
            if (nFlags & F_AUTO_LIMIT)
                value       = lsp_limit(value, 0.0f, 1.0f);
            value       = transform(fMin + (fMax - fMin) * value);
            if (value == old)
                return old;

            fValue              = value;
            sync();
            return old;
        }

        float RangeFloat::transform(float v) const
        {
            return (pTransform != NULL) ? pTransform(v, pTransformArg) : v;
        }

        float RangeFloat::do_limit(float value) const
        {
            value = transform(value);
            return (nFlags & F_AUTO_LIMIT) ?
                Property::limit(value, fMin, fMax) :
                value;
        }

        float RangeFloat::climited(float value) const
        {
            if (!(nFlags & F_AUTO_LIMIT))
                return value;

            float delta = fMax - fMin;
            if (delta > 0)
            {
                while (value > fMax)
                    value  -= delta;
                while (value < fMin)
                    value  += delta;
            }
            else
            {
                while (value > fMin)
                    value  -= delta;
                while (value < fMax)
                    value  += delta;
            }

            return do_limit(value);
        }

        float RangeFloat::change(float k, float step)
        {
            float old   = fValue;
            float v     = do_limit(old + k*step);
            if (old == v)
                return old;

            fValue      = v;
            sync();
            return old;
        }

        float RangeFloat::add(float v, bool cyclic)
        {
            float old   = fValue;
            v           = (cyclic) ? climited(old + v) : do_limit(old + v);
            if (old == v)
                return old;

            fValue      = v;
            sync();
            return old;
        }

        float RangeFloat::sub(float v, bool cyclic)
        {
            float old   = fValue;
            v           = (cyclic) ? climited(old - v) : do_limit(old - v);
            if (old == v)
                return old;

            fValue      = v;
            sync();
            return old;
        }

        namespace prop
        {
            bool RangeFloat::lock_range(bool lock)
            {
                bool prev       = nFlags & F_RANGE_LOCK;
                nFlags          = lsp_setflag(nFlags, F_AUTO_LIMIT, lock);
                return prev;
            }

            bool RangeFloat::set_auto_limit(bool enable)
            {
                bool prev       = nFlags & F_AUTO_LIMIT;
                nFlags          = lsp_setflag(nFlags, F_AUTO_LIMIT, enable);
                return prev;
            }

            float RangeFloat::set_min(float value)
            {
                float old = fMin;
                if (value == old)
                    return old;

                fMin                = value;
                sync();
                return old;
            }

            float RangeFloat::set_max(float value)
            {
                float old = fMax;
                if (value == old)
                    return old;

                fMax                = value;
                sync();
                return old;
            }

            void RangeFloat::set_range(float min, float max)
            {
                if ((min == fMin) &&
                    (max == fMax))
                    return;

                fMin                = min;
                fMax                = max;
                sync();
            }

            float RangeFloat::set_all(float value, float min, float max)
            {
                bool need_sync = false;

                if ((min != fMin) ||
                    (max != fMax))
                {
                    fMin                = min;
                    fMax                = max;
                    need_sync           = true;
                }

                float old   = fValue;
                value       = limit(value);
                if (value != old)
                {
                    fValue              = value;
                    need_sync           = true;
                }

                if (need_sync)
                    sync();

                return old;
            }

            float RangeFloat::commit_value(float value)
            {
                float old   = fValue;
                fValue      = limit(value);
                return old;
            }
        }
    } /* namespace tk */
} /* namespace lsp */


