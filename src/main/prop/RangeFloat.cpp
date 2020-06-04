/*
 * RangeFloat.cpp
 *
 *  Created on: 23 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const prop::desc_t RangeFloat::DESC[] =
        {
            { "",               PT_FLOAT    },
            { ".min",           PT_FLOAT    },
            { ".max",           PT_FLOAT    },
            { ".step",          PT_FLOAT    },
            { ".quick",         PT_FLOAT    },
            { ".slow",          PT_FLOAT    },
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
            fStep       = 0.1f;
            fQuick      = 0.2f;
            fSlow       = 0.05f;
        }

        RangeFloat::~RangeFloat()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void RangeFloat::commit(atom_t property)
        {
            if ((pStyle == NULL) || (property < 0))
                return;

            float v;
            if ((property == vAtoms[P_VALUE]) && (pStyle->get_float(vAtoms[P_VALUE], &v) == STATUS_OK))
                fValue          = v;
            if ((property == vAtoms[P_MIN]) && (pStyle->get_float(vAtoms[P_MIN], &v) == STATUS_OK))
                fMin            = v;
            if ((property == vAtoms[P_MAX]) && (pStyle->get_float(vAtoms[P_MAX], &v) == STATUS_OK))
                fMax            = v;
            if ((property == vAtoms[P_STEP]) && (pStyle->get_float(vAtoms[P_STEP], &v) == STATUS_OK))
                fStep           = v;
            if ((property == vAtoms[P_QUICK]) && (pStyle->get_float(vAtoms[P_QUICK], &v) == STATUS_OK))
                fQuick          = v;
            if ((property == vAtoms[P_SLOW]) && (pStyle->get_float(vAtoms[P_SLOW], &v) == STATUS_OK))
                fSlow           = v;

            if (pListener != NULL)
                pListener->notify(this);
        }

        void RangeFloat::sync()
        {
            if (pStyle != NULL)
            {
                pStyle->begin(&sListener);
                {
                    // Simple components
                    if (vAtoms[P_VALUE] >= 0)
                        pStyle->set_float(vAtoms[P_VALUE], fValue);
                    if (vAtoms[P_MIN] >= 0)
                        pStyle->set_float(vAtoms[P_MIN], fMin);
                    if (vAtoms[P_MAX] >= 0)
                        pStyle->set_float(vAtoms[P_MAX], fMax);
                    if (vAtoms[P_STEP] >= 0)
                        pStyle->set_float(vAtoms[P_STEP], fStep);
                    if (vAtoms[P_QUICK] >= 0)
                        pStyle->set_float(vAtoms[P_QUICK], fQuick);
                    if (vAtoms[P_SLOW] >= 0)
                        pStyle->set_float(vAtoms[P_SLOW], fSlow);
                }
                pStyle->end();
            }
            if (pListener != NULL)
                pListener->notify(this);
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

        float RangeFloat::set(float value)
        {
            float old   = fValue;
            value       = limited(value);
            if (value == old)
                return old;

            fValue              = value;
            sync();
            return old;
        }

        float RangeFloat::set_step(float value)
        {
            float old = fStep;
            if (value == old)
                return old;

            fStep               = value;
            sync();
            return old;
        }

        float RangeFloat::set_quick(float value)
        {
            float old = fQuick;
            if (value == old)
                return old;

            fQuick              = value;
            sync();
            return old;
        }

        float RangeFloat::set_slow(float value)
        {
            float old = fSlow;
            if (value == old)
                return old;

            fSlow               = value;
            sync();
            return old;
        }

        float RangeFloat::limited(float value) const
        {
            if (fMin > fMax)
                return lsp_limit(value, fMax, fMin);
            return lsp_limit(value, fMin, fMax);
        }

        float RangeFloat::change(float k, float step)
        {
            float old   = fValue;
            float v     = limited(old + k*step);
            if (old == v)
                return old;

            fValue      = v;
            sync();
            return old;
        }

        float RangeFloat::add(float v)
        {
            float old   = fValue;
            v           = limited(old + v);
            if (old == v)
                return old;

            fValue      = v;
            sync();
            return old;
        }

        float RangeFloat::sub(float v)
        {
            float old   = fValue;
            v           = limited(old - v);
            if (old == v)
                return old;

            fValue      = v;
            sync();
            return old;
        }

        namespace prop
        {
            status_t RangeFloat::init(Style *style, float value, float min, float max, float step, float quick, float slow)
            {
                if (pStyle == NULL)
                    return STATUS_BAD_STATE;

                style->begin();
                {
                    style->create_int(vAtoms[P_VALUE], value);
                    style->create_int(vAtoms[P_MIN], min);
                    style->create_int(vAtoms[P_MAX], max);
                    style->create_int(vAtoms[P_STEP], step);
                    style->create_int(vAtoms[P_QUICK], quick);
                    style->create_int(vAtoms[P_SLOW], slow);
                }
                style->end();
                return STATUS_OK;
            }
        }
    } /* namespace tk */
} /* namespace lsp */


