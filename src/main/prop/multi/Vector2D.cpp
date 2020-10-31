/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 25 авг. 2020 г.
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
#include <lsp-plug.in/stdlib/math.h>

namespace lsp
{
    namespace tk
    {
        const prop::desc_t Vector2D::DESC[] =
        {
            { "",               PT_STRING   },
            { ".dx",            PT_FLOAT    },
            { ".dy",            PT_FLOAT    },
            { ".rho",           PT_FLOAT    },
            { ".phi",           PT_FLOAT    },
            { ".rphi",          PT_FLOAT    },
            { ".dphi",          PT_FLOAT    },
            { NULL,             PT_UNKNOWN  }
        };

        void Vector2D::Listener::notify(atom_t property)
        {
            pValue->commit(property);
        }

        Vector2D::Vector2D(prop::Listener *listener):
            MultiProperty(vAtoms, P_COUNT, listener),
            sListener(this)
        {
            fDX             = 1.0f;
            fDY             = 0.0f;
            fRho            = 1.0f;
            fPhi            = 0.0f;
        }

        Vector2D::~Vector2D()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void Vector2D::commit(atom_t property)
        {
            if ((pStyle == NULL) || (property < 0))
                return;

            LSPString s;
            float v;
            if ((property == vAtoms[P_DX]) && (pStyle->get_float(vAtoms[P_DX], &v) == STATUS_OK))
            {
                fDX             = v;
                calc_polar(&fRho, &fPhi, fDX, fDY);
            }
            if ((property == vAtoms[P_DY]) && (pStyle->get_float(vAtoms[P_DY], &v) == STATUS_OK))
            {
                fDY             = v;
                calc_polar(&fRho, &fPhi, fDX, fDY);
            }
            if ((property == vAtoms[P_RHO]) && (pStyle->get_float(vAtoms[P_RHO], &v) == STATUS_OK))
            {
                fRho             = v;
                calc_cart(&fDX, &fDY, fRho, fPhi);
            }
            if ((property == vAtoms[P_PHI]) && (pStyle->get_float(vAtoms[P_PHI], &v) == STATUS_OK))
            {
                fPhi             = v;
                calc_cart(&fDX, &fDY, fRho, fPhi);
            }
            if ((property == vAtoms[P_RPHI]) && (pStyle->get_float(vAtoms[P_RPHI], &v) == STATUS_OK))
            {
                fPhi             = v;
                calc_cart(&fDX, &fDY, fRho, fPhi);
            }
            if ((property == vAtoms[P_DPHI]) && (pStyle->get_float(vAtoms[P_DPHI], &v) == STATUS_OK))
            {
                fPhi             = v * (M_PI / 180.0f);
                calc_cart(&fDX, &fDY, fRho, fPhi);
            }

            // Compound property
            if ((property == vAtoms[P_VALUE]) && (pStyle->get_string(vAtoms[P_VALUE], &s) == STATUS_OK))
                parse(&fDX, &fDY, &fRho, &fPhi, &s);

            if (pListener != NULL)
                pListener->notify(this);
        }

        void Vector2D::sync()
        {
            if (pStyle != NULL)
            {
                pStyle->begin(&sListener);
                {
                    LSPString s;

                    // Simple components
                    if (vAtoms[P_DX] >= 0)
                        pStyle->set_float(vAtoms[P_DX], fDX);
                    if (vAtoms[P_DY] >= 0)
                        pStyle->set_float(vAtoms[P_DY], fDY);
                    if (vAtoms[P_RHO] >= 0)
                        pStyle->set_float(vAtoms[P_RHO], fRho);
                    if (vAtoms[P_PHI] >= 0)
                        pStyle->set_float(vAtoms[P_PHI], fPhi);
                    if (vAtoms[P_RPHI] >= 0)
                        pStyle->set_float(vAtoms[P_RPHI], fPhi);
                    if (vAtoms[P_DPHI] >= 0)
                        pStyle->set_float(vAtoms[P_DPHI], fPhi * (180.0f / M_PI));

                    // Compound properties
                    s.fmt_ascii("{%.10f, %.10f}", fDX, fDY);
                    if (vAtoms[P_VALUE] >= 0)
                        pStyle->set_string(vAtoms[P_VALUE], &s);
                }
                pStyle->end();
            }
            if (pListener != NULL)
                pListener->notify(this);
        }

        void Vector2D::calc_cart(float *dx, float *dy, float rho, float phi)
        {
            *dx     = cosf(phi) * rho;
            *dy     = sinf(phi) * rho;
        }

        void Vector2D::calc_polar (float *rho, float *phi, float dx, float dy)
        {
            float l = sqrtf(dx*dx + dy*dy);
            if (l == 0.0f)
            {
                *rho    = 0.0f;
                *phi    = 0.0f;
                return;
            }

            float a;
            if (dx < dy) // calc with acos
            {
                a = acos(dx / l); // 0..PI
                a = (dy >= 0.0) ? a : M_PI*2 - a;
            }
            else // calc with asin
            {
                a = asin(dy / l); // -PI/2 .. PI/2
                a = (dx >= 0.0) ? a : M_PI - a;
            }

            // Return values
            *rho = l;
            *phi = (a < 0.0) ? M_PI*2 + a : a;
        }

        bool Vector2D::parse(float *dx, float *dy, float *rho, float *phi, const LSPString *s)
        {
            io::InStringSequence ins(s);
            expr::Tokenizer tok(&ins);

            int brace   = -1;
            float v[2];

            // Get first token
            expr::token_t t = tok.get_token(expr::TF_GET);
            if (t == expr::TT_LBRACE) // Polar form, radians
                brace = expr::TT_RBRACE;
            else if (t == expr::TT_LCBRACE) // Decart form
                brace = expr::TT_RCBRACE;
            else if (t == expr::TT_LQBRACE) // Polar form, degrees
                brace = expr::TT_RQBRACE;
            else if ((t == expr::TT_IVALUE) || (t == expr::TT_FVALUE)) // Number, consider to be decart form
                brace = -1;
            else
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
            if (brace)
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

            // Commit parsed changes
            if (brace == expr::TT_RBRACE) // Polar (radians)
            {
                *rho    = v[0];
                *phi    = v[1];
                calc_cart(dx, dy, *rho, *phi);
            }
            else if (brace == expr::TT_RQBRACE) // Polar (degrees)
            {
                *rho    = v[0];
                *phi    = v[1] * (M_PI / 180.0f);
                calc_cart(dx, dy, *rho, *phi);
            }
            else // Decart
            {
                *dx     = v[0];
                *dy     = v[1];
                calc_polar(rho, phi, *dx, *dy);
            }

            return true;
        }

        float Vector2D::set_dx(float v)
        {
            float old = fDX;
            if (old == v)
                return old;

            fDX     = v;
            calc_polar(&fRho, &fPhi, fDX, fDY);
            sync();

            return old;
        }

        float Vector2D::set_dy(float v)
        {
            float old = fDY;
            if (old == v)
                return old;

            fDY     = v;
            calc_polar(&fRho, &fPhi, fDX, fDY);
            sync();

            return old;
        }

        float Vector2D::set_rho(float v)
        {
            float old = fRho;
            if (old == v)
                return old;

            fRho     = v;
            calc_cart(&fDX, &fDY, fRho, fPhi);
            sync();

            return old;
        }

        float Vector2D::set_rphi(float v)
        {
            float old = fPhi;
            if (old == v)
                return old;

            fPhi     = v;
            calc_cart(&fDX, &fDY, fRho, fPhi);
            sync();

            return old;
        }

        float Vector2D::set_dphi(float v)
        {
            float old = fPhi * (180.0f / M_PI);
            if (old == v)
                return old;

            fPhi     = v * (M_PI / 180.0f);
            calc_cart(&fDX, &fDY, fRho, fPhi);
            sync();

            return old;
        }

        void Vector2D::set_cart(float dx, float dy)
        {
            if ((fDX == dx) && (fDY == dy))
                return;

            fDX     = dx;
            fDY     = dy;
            calc_polar(&fRho, &fPhi, fDX, fDY);
            sync();
        }

        void Vector2D::set_rpolar(float rho, float phi)
        {
            if ((fRho == rho) && (fPhi == phi))
                return;

            fRho    = rho;
            fPhi    = phi;
            calc_cart(&fDX, &fDY, fRho, fPhi);
            sync();
        }

        void Vector2D::normalize()
        {
            float d = sqrtf(fDX*fDX + fDY*fDY);
            if ((d == 0.0f) || (d == 1.0f))
                return;

            d       = 1.0f / d;
            fDX    *= d;
            fDY    *= d;
            calc_polar(&fRho, &fPhi, fDX, fDY);
            sync();
        }

        status_t Vector2D::init()
        {
            pStyle->begin();
            {
                LSPString s;
                pStyle->create_float(vAtoms[P_DX], dx());
                pStyle->create_float(vAtoms[P_DY], dy());
                pStyle->create_float(vAtoms[P_RHO], rho());
                pStyle->create_float(vAtoms[P_PHI], phi());
                pStyle->create_float(vAtoms[P_RPHI], phi());
                pStyle->create_float(vAtoms[P_DPHI], phi() * (180.0 / M_PI));

                // Compound properties
                s.fmt_ascii("{%.10f %.10f}", dx(), dy());
                pStyle->create_string(vAtoms[P_VALUE], &s);
            }
            pStyle->end();
            return STATUS_OK;
        }

        status_t Vector2D::override()
        {
            pStyle->begin();
            {
                LSPString s;
                pStyle->create_float(vAtoms[P_DX], dx());
                pStyle->create_float(vAtoms[P_DY], dy());
                pStyle->create_float(vAtoms[P_RHO], rho());
                pStyle->create_float(vAtoms[P_PHI], phi());
                pStyle->create_float(vAtoms[P_RPHI], phi());
                pStyle->create_float(vAtoms[P_DPHI], phi() * (180.0 / M_PI));

                // Compound properties
                s.fmt_ascii("{%.10f %.10f}", dx(), dy());
                pStyle->set_string(vAtoms[P_VALUE], &s);
            }
            pStyle->end();
            return STATUS_OK;
        }

        namespace prop
        {
            status_t Vector2D::init(Style *style, float dx, float dy, float rho, float phi)
            {
                if (pStyle == NULL)
                    return STATUS_BAD_STATE;

                style->begin();
                {
                    LSPString s;
                    style->create_float(vAtoms[P_DX], dx);
                    style->create_float(vAtoms[P_DY], dy);
                    style->create_float(vAtoms[P_RHO], rho);
                    style->create_float(vAtoms[P_PHI], phi);
                    style->create_float(vAtoms[P_RPHI], phi);
                    style->create_float(vAtoms[P_DPHI], phi * (180 / M_PI));

                    // Compound properties
                    s.fmt_ascii("{%.10f %.10f}", dx, dy);
                    style->create_string(vAtoms[P_VALUE], &s);
                }
                style->end();
                return STATUS_OK;
            }

            status_t Vector2D::init_cart(Style *style, float dx, float dy)
            {
                float rho, phi;
                calc_polar(&rho, &phi, dx, dy);
                return init(style, dx, dy, rho, phi);
            }

            status_t Vector2D::init_rpolar(Style *style, float rho, float phi)
            {
                float dx, dy;
                calc_cart(&dx, &dy, rho, phi);
                return init(style, dx, dy, rho, phi);
            }

            status_t Vector2D::init_cart(const char *name, Style *style, float dx, float dy)
            {
                prop::Vector2D v;
                LSP_STATUS_ASSERT(v.bind(name, style));
                v.set_cart(dx, dy);
                return v.init();
            }

            status_t Vector2D::init_rpolar(const char *name, Style *style, float rho, float phi)
            {
                prop::Vector2D v;
                LSP_STATUS_ASSERT(v.bind(name, style));
                v.set_rpolar(rho, phi);
                return v.init();
            }

            status_t Vector2D::init_dpolar(const char *name, Style *style, float rho, float phi)
            {
                prop::Vector2D v;
                LSP_STATUS_ASSERT(v.bind(name, style));
                v.set_dpolar(rho, phi);
                return v.init();
            }

            status_t Vector2D::override_cart(const char *name, Style *style, float dx, float dy)
            {
                prop::Vector2D v;
                LSP_STATUS_ASSERT(v.bind(name, style));
                v.set_cart(dx, dy);
                return v.override();
            }

            status_t Vector2D::override_rpolar(const char *name, Style *style, float rho, float phi)
            {
                prop::Vector2D v;
                LSP_STATUS_ASSERT(v.bind(name, style));
                v.set_rpolar(rho, phi);
                return v.override();
            }

            status_t Vector2D::override_dpolar(const char *name, Style *style, float rho, float phi)
            {
                prop::Vector2D v;
                LSP_STATUS_ASSERT(v.bind(name, style));
                v.set_dpolar(rho, phi);
                return v.override();
            }
        }
    } /* namespace tk */
} /* namespace lsp */


