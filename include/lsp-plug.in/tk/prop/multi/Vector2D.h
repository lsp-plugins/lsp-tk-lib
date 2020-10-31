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

#ifndef LSP_PLUG_IN_TK_PROP_MULTI_VECTOR2D_H_
#define LSP_PLUG_IN_TK_PROP_MULTI_VECTOR2D_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/stdlib/math.h>

namespace lsp
{
    namespace tk
    {
        /**
         * Floating-point property interface
         */
        class Vector2D: public MultiProperty
        {
            private:
                Vector2D & operator = (const Vector2D &);

            protected:
                enum property_t
                {
                    P_VALUE,
                    P_DX,
                    P_DY,
                    P_RHO,
                    P_PHI,
                    P_RPHI,
                    P_DPHI,

                    P_COUNT
                };

                class Listener: public IStyleListener
                {
                    protected:
                        Vector2D      *pValue;

                    public:
                        inline Listener(Vector2D *ptr)             { pValue = ptr;     }

                    public:
                        virtual void notify(atom_t property);
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                float               fDX;                // X axis projection
                float               fDY;                // Y axis projection
                float               fRho;               // Length
                float               fPhi;               // Angle
                Listener            sListener;

            protected:
                void                sync();
                void                commit(atom_t property);
                status_t            init();
                status_t            override();

                static void         calc_cart  (float *dx, float *dy, float rho, float phi);
                static void         calc_polar (float *rho, float *phi, float dx, float dy);
                static bool         parse(float *dx, float *dy, float *rho, float *phi, const LSPString *s);

            protected:
                explicit Vector2D(prop::Listener *listener = NULL);
                ~Vector2D();

            public:
                inline float        dx() const                  { return fDX;                   }
                inline float        dy() const                  { return fDY;                   }
                inline float        rho() const                 { return fRho;                  }
                inline float        phi() const                 { return fPhi;                  }
                inline float        rphi() const                { return fPhi;                  }
                inline float        dphi() const                { return fPhi * (180.0f / M_PI);}
                inline float        length() const              { return rho();                 }
                inline float        rangle() const              { return rphi();                }
                inline float        dangle() const              { return dphi();                }

                float               set_dx(float v);
                float               set_dy(float v);
                float               set_rho(float v);
                float               set_rphi(float v);
                float               set_dphi(float v);
                inline float        set_phi(float v)            { return set_rphi(v);           }
                inline float        set_length(float v)         { return set_rho(v);            }
                inline float        set_angle(float v)          { return set_rphi(v);           }
                inline float        set_rangle(float v)         { return set_rphi(v);           }
                inline float        set_dangle(float v)         { return set_dphi(v);           }

                void                set_cart(float dx, float dy);
                void                set_rpolar(float rho, float phi);
                inline void         set_dpolar(float rho, float phi){ set_rpolar(rho, phi * (M_PI / 180.0f));   }
                inline void         set_polar(float rho, float phi) { set_rpolar(rho, phi);                     }

                void                normalize();
        };

        namespace prop
        {
            /**
             * Vector2D property implementation
             */
            class Vector2D: public tk::Vector2D
            {
                private:
                    Vector2D & operator = (const Vector2D &);

                public:
                    explicit Vector2D(prop::Listener *listener = NULL): tk::Vector2D(listener) {};

                protected:
                    status_t            init(Style *style, float dx, float dy, float rho, float phi);

                protected:
                    using               tk::Vector2D::init;
                    using               tk::Vector2D::override;

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::Vector2D::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::Vector2D::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::Vector2D::bind(property, style, vAtoms, DESC, &sListener); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::Vector2D::unbind(vAtoms, DESC, &sListener); };

                    /**
                     * Initialize
                     * @param style style
                     * @return status of operation
                     */
                    status_t            init_cart(Style *style, float dx, float dy);
                    status_t            init_rpolar(Style *style, float rho, float phi);
                    inline status_t     init_dpolar(Style *style, float rho, float phi)  { return init_rpolar(style, rho, phi * (M_PI / 180.0f));   }
                    inline status_t     init_polar(Style *style, float rho, float phi)   { return init_rpolar(style, rho, phi);                     }

                    static status_t     init_cart(const char *name, Style *style, float dx, float dy);
                    static status_t     init_rpolar(const char *name, Style *style, float rho, float phi);
                    static status_t     init_dpolar(const char *name, Style *style, float rho, float phi);

                    static status_t     override_cart(const char *name, Style *style, float dx, float dy);
                    static status_t     override_rpolar(const char *name, Style *style, float rho, float phi);
                    static status_t     override_dpolar(const char *name, Style *style, float rho, float phi);
            };
        }

    } /* namespace tk */
} /* namespace lsp */


#endif /* LSP_PLUG_IN_TK_PROP_MULTI_VECTOR2D_H_ */
