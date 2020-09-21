/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 21 сент. 2020 г.
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
#include <lsp-plug.in/io/InStringSequence.h>
#include <lsp-plug.in/expr/Tokenizer.h>

namespace lsp
{
    namespace tk
    {
        const prop::desc_t ColorRange::DESC[] =
        {
            { "",               PT_STRING   },
            { ".min",           PT_FLOAT    },
            { ".max",           PT_FLOAT    },
            { NULL,             PT_UNKNOWN  }
        };

        void ColorRange::ColorListener::notify(Property *prop)
        {
            if (pValue->sColor.is(prop))
                pValue->color_changed();
        }

        void ColorRange::Listener::notify(atom_t prop)
        {
            pValue->commit(prop);
        }

        ColorRange::ColorRange(prop::Listener *listener):
            MultiProperty(vAtoms, P_COUNT, listener),
            sColor(&sColorListener),
            sColorListener(this),
            sListener(this)
        {
            fMin        = 0.0f;
            fMax        = 1.0f;
            nFlags      = 0;
        }

        ColorRange::~ColorRange()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void ColorRange::sync()
        {
            // TODO
        }

        void ColorRange::commit(atom_t property)
        {
        }

        void ColorRange::color_changed()
        {
        }

        float ColorRange::set_min(float v)
        {
            float old = fMin;
            if (v == old)
                return old;

            fMin                = v;
            sync();
            return old;
        }

        float ColorRange::set_max(float v)
        {
            float old = fMin;
            if (v == old)
                return old;

            fMin                = v;
            sync();
            return old;
        }

        void ColorRange::set_range(float min, float max)
        {
            if ((fMin == min) && (fMax == max))
                return;

            fMin        = min;
            fMax        = max;
            sync();
        }

        status_t ColorRange::set(const char *s)
        {
            status_t res, res2;
            io::InStringSequence is;
            if ((res = is.wrap(s)) != STATUS_OK)
                return res;

            expr::Tokenizer t(&is);
            res             = do_parse(&t);
            res2            = is.close();
            return (res != STATUS_OK) ? res : res2;
        }

        status_t ColorRange::set(const LSPString *s)
        {
            status_t res, res2;
            io::InStringSequence is;
            if ((res = is.wrap(s)) != STATUS_OK)
                return res;

            expr::Tokenizer t(&is);
            res             = do_parse(&t);
            res2            = is.close();
            return (res != STATUS_OK) ? res : res2;
        }

        status_t ColorRange::do_parse(expr::Tokenizer *t)
        {
            // TODO
            return STATUS_OK;
        }

    }
}

