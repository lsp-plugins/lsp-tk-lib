/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 23 сент. 2025 г.
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
        const prop::desc_t Range::DESC[] =
        {
            { "",               PT_STRING   },
            { ".min",           PT_FLOAT    },
            { ".max",           PT_FLOAT    },
            { NULL,             PT_UNKNOWN  }
        };

        Range::Range(prop::Listener *listener):
            MultiProperty(vAtoms, P_COUNT, listener)
        {
            fMin            = 0.0f;
            fMax            = 1.0f;
            nFlags          = 0;
            pTransform      = NULL;
            pTransformArg   = NULL;
        }

        Range::~Range()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void Range::commit(atom_t property)
        {
            LSPString s;
            float v;
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
                float v[2];
                size_t n    = Property::parse_floats(v, 2, &s);
                switch (n)
                {
                    case 1:
                        fMin        = v[0];
                        fMax        = v[0];
                        break;
                    case 2:
                        fMin        = v[0];
                        fMax        = v[1];
                        break;
                    default:
                        break;
                }
            }
        }

        void Range::push()
        {
            LSPString s;

            // Simple components
            if (vAtoms[P_MIN] >= 0)
                pStyle->set_float(vAtoms[P_MIN], fMin);
            if (vAtoms[P_MAX] >= 0)
                pStyle->set_float(vAtoms[P_MAX], fMax);

            // Compound properties
            SET_LOCALE_SCOPED(LC_NUMERIC, "C");
            s.fmt_ascii("%.10f %.10f", fMin, fMax);
            if (vAtoms[P_VALUE] >= 0)
                pStyle->set_string(vAtoms[P_VALUE], &s);
        }

        float Range::set_min(float value)
        {
            float old = fMin;
            value               = do_limit(value, true);
            if ((nFlags & F_RANGE_LOCK) || (value == old))
                return old;

            fMin                = value;
            sync();
            return old;
        }

        float Range::set_max(float value)
        {
            float old = fMax;
            value               = do_limit(value, true);
            if ((nFlags & F_RANGE_LOCK) || (value == old))
                return old;

            fMax                = value;
            sync();
            return old;
        }

        void Range::set(float min, float max)
        {
            if (nFlags & F_RANGE_LOCK)
                return;
            min                 = do_limit(min, true);
            max                 = do_limit(max, true);
            if ((min == fMin) &&
                (max == fMax))
                return;

            fMin                = min;
            fMax                = max;
            sync();
        }

        float Range::transform(float v, bool write) const
        {
            return (pTransform != NULL) ? pTransform(v, write, pTransformArg) : v;
        }

        float Range::do_limit(float value, bool write) const
        {
            value = transform(value, write);
            return Property::limit(value, fMin, fMax);
        }

        namespace prop
        {
            bool Range::lock_range(bool lock)
            {
                bool prev       = nFlags & F_RANGE_LOCK;
                nFlags          = lsp_setflag(nFlags, F_RANGE_LOCK, lock);
                return prev;
            }

            float Range::commit_min(float value)
            {
                float old = fMin;
                if (value == old)
                    return old;

                fMin                = value;
                sync();
                return old;
            }

            float Range::commit_max(float value)
            {
                float old = fMax;
                if (value == old)
                    return old;

                fMax                = value;
                sync();
                return old;
            }

            void Range::commit_range(float min, float max)
            {
                if ((min == fMin) &&
                    (max == fMax))
                    return;

                fMin                = min;
                fMax                = max;
                sync();
            }
        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */





