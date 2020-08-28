/*
 * RangeFloat.cpp
 *
 *  Created on: 23 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
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

        void RangeFloat::Listener::notify(atom_t property)
        {
            pValue->commit(property);
        }

        RangeFloat::RangeFloat(prop::Listener *listener):
            MultiProperty(listener),
            sListener(this)
        {
            fMin        = 0.0f;
            fMax        = 1.0f;
            fValue      = 0.0f;
            bRangeLocked= false;
        }

        RangeFloat::~RangeFloat()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void RangeFloat::commit(atom_t property)
        {
            if ((pStyle == NULL) || (property < 0))
                return;

            LSPString s;
            float v;
            if ((property == vAtoms[P_RVALUE]) && (pStyle->get_float(vAtoms[P_RVALUE], &v) == STATUS_OK))
                fValue          = v;

            if (!bRangeLocked)
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
            if (pListener != NULL)
                pListener->notify(this);
        }

        void RangeFloat::sync()
        {
            if (pStyle != NULL)
            {
                pStyle->begin(&sListener);
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
                    s.fmt_ascii("%.10f %.10f %.10f", fValue, fMin, fMax);
                    if (vAtoms[P_VALUE] >= 0)
                        pStyle->set_string(vAtoms[P_VALUE], &s);
                }
                pStyle->end();
            }
            if (pListener != NULL)
                pListener->notify(this);
        }

        float RangeFloat::set_min(float value)
        {
            float old = fMin;
            if ((bRangeLocked) || (value == old))
                return old;

            fMin                = value;
            sync();
            return old;
        }

        float RangeFloat::set_max(float value)
        {
            float old = fMax;
            if ((bRangeLocked) || (value == old))
                return old;

            fMax                = value;
            sync();
            return old;
        }

        void RangeFloat::set_range(float min, float max)
        {
            if (bRangeLocked)
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

            if (!bRangeLocked)
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

        float RangeFloat::set(float value)
        {
            float old   = fValue;
            value       = limit(value);
            if (value == old)
                return old;

            fValue              = value;
            sync();
            return old;
        }

        float RangeFloat::get_normalized(float value) const
        {
            if (fMin == fMax)
                return 0.0f;

            float v         = limit(value);
            return (v - fMin) / (fMax - fMin);
        }

        float RangeFloat::set_normalized(float value, bool cyclic)
        {
            float old   = fValue;
            if (cyclic)
                value       = value - truncf(value);
            value       = fMin + (fMax - fMin) * lsp_limit(value, 0.0f, 1.0f);
            if (value == old)
                return old;

            fValue              = value;
            sync();
            return old;
        }

        float RangeFloat::limit(float value) const
        {
            return (fMin > fMax) ?
                lsp_limit(value, fMax, fMin) :
                lsp_limit(value, fMin, fMax);
        }

        float RangeFloat::climited(float value) const
        {
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

            return value;
        }

        float RangeFloat::limit_value(float value, float min, float max)
        {
            return (min > max) ?
                lsp_limit(value, max, min) :
                lsp_limit(value, min, max);
        }

        float RangeFloat::change(float k, float step)
        {
            float old   = fValue;
            float v     = limit(old + k*step);
            if (old == v)
                return old;

            fValue      = v;
            sync();
            return old;
        }

        float RangeFloat::add(float v, bool cyclic)
        {
            float old   = fValue;
            v           = (cyclic) ? climited(old + v) : limit(old + v);
            if (old == v)
                return old;

            fValue      = v;
            sync();
            return old;
        }

        float RangeFloat::sub(float v, bool cyclic)
        {
            float old   = fValue;
            v           = (cyclic) ? climited(old - v) : limit(old - v);
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
                bool prev       = bRangeLocked;
                bRangeLocked    = lock;
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

            float RangeFloat::commit(float value)
            {
                float old   = fValue;
                fValue      = limit(value);
                return old;
            }

            status_t RangeFloat::init(Style *style, float value, float min, float max)
            {
                if (pStyle == NULL)
                    return STATUS_BAD_STATE;

                style->begin();
                {
                    LSPString s;

                    style->create_float(vAtoms[P_RVALUE], value);
                    style->create_float(vAtoms[P_MIN], min);
                    style->create_float(vAtoms[P_MAX], max);

                    // Compound properties
                    s.fmt_ascii("%.10f %.10f %.10f", value, min, max);
                    style->set_string(vAtoms[P_VALUE], &s);
                }
                style->end();
                return STATUS_OK;
            }
        }
    } /* namespace tk */
} /* namespace lsp */


