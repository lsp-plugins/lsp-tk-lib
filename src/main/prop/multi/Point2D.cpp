/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 17 июн. 2023 г.
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
#include <lsp-plug.in/stdlib/locale.h>
#include <lsp-plug.in/stdlib/math.h>

namespace lsp
{
    namespace tk
    {
        const prop::desc_t Point2D::DESC[] =
        {
            { "",               PT_STRING   },
            { ".x",             PT_FLOAT    },
            { ".y",             PT_FLOAT    },
            { NULL,             PT_UNKNOWN  }
        };

        Point2D::Point2D(prop::Listener *listener):
            MultiProperty(vAtoms, P_COUNT, listener)
        {
            fX              = 0.0f;
            fY              = 0.0f;
        }

        Point2D::~Point2D()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void Point2D::commit(atom_t property)
        {
            LSPString s;
            float v;
            if ((property == vAtoms[P_X]) && (pStyle->get_float(vAtoms[P_X], &v) == STATUS_OK))
                fX              = v;
            if ((property == vAtoms[P_Y]) && (pStyle->get_float(vAtoms[P_Y], &v) == STATUS_OK))
                fY              = v;

            // Compound property
            if ((property == vAtoms[P_VALUE]) && (pStyle->get_string(vAtoms[P_VALUE], &s) == STATUS_OK))
                parse(&fX, &fY, &s);
        }

        void Point2D::push()
        {
            LSPString s;

            // Simple components
            if (vAtoms[P_X] >= 0)
                pStyle->set_float(vAtoms[P_X], fX);
            if (vAtoms[P_Y] >= 0)
                pStyle->set_float(vAtoms[P_Y], fY);

            // Compound properties
            SET_LOCALE_SCOPED(LC_NUMERIC, "C");
            s.fmt_ascii("{%.10f, %.10f}", fX, fY);
            if (vAtoms[P_VALUE] >= 0)
                pStyle->set_string(vAtoms[P_VALUE], &s);
        }

        bool Point2D::parse(float *x, float *y, const LSPString *s)
        {
            io::InStringSequence ins(s);
            expr::Tokenizer tok(&ins);

            int brace   = -1;
            float v[2];

            // Get first token
            expr::token_t t = tok.get_token(expr::TF_GET);
            if (t == expr::TT_LCBRACE) // Decart form
                brace = expr::TT_RCBRACE;
            else if (t == expr::TT_LBRACE) // Decart form, second option
                brace = expr::TT_RBRACE;
            else if ((t != expr::TT_IVALUE) && (t != expr::TT_FVALUE)) // Number, consider to be decart form
                return false;

            // Get next token if required
            if (brace >= 0)
                t = tok.get_token(expr::TF_GET);

            // Parse first value
            if (t == expr::TT_IVALUE)
                v[0]    = tok.int_value();
            else if (t == expr::TT_FVALUE)
                v[0]    = tok.float_value();
            else
                return false;

            // Get next token
            t = tok.get_token(expr::TF_GET);

            // Semicolon/comma required?
            if (brace >= 0)
            {
                if ((t != expr::TT_COMMA) && (t != expr::TT_SEMICOLON))
                    return false;

                t = tok.get_token(expr::TF_GET);
            }

            // Parse second value
            if (t == expr::TT_IVALUE)
                v[1]    = tok.int_value();
            else if (t == expr::TT_FVALUE)
                v[1]    = tok.float_value();
            else
                return false;

            // Get next token
            t = tok.get_token(expr::TF_GET);
            if (brace >= 0)
            {
                if (t != brace)
                    return false;

                t = tok.get_token(expr::TF_GET);
            }

            // EOF now should occur
            if (t != expr::TT_EOF)
                return false;

            *x  = v[0];
            *y  = v[1];

            return true;
        }

        float Point2D::set_x(float v)
        {
            float old = fX;
            if (old == v)
                return old;

            fX      = v;
            sync();

            return old;
        }

        float Point2D::set_y(float v)
        {
            float old = fY;
            if (old == v)
                return old;

            fY      = v;
            sync();

            return old;
        }

        void Point2D::set(float x, float y)
        {
            if ((x == fX) && (y == fY))
                return;

            fX      = x;
            fY      = y;
            sync();
        }

    } /* namespace tk */
} /* namespace lsp */



