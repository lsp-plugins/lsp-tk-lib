/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef LSP_PLUG_IN_TK_PROP_MULTI_POINT2D_H_
#define LSP_PLUG_IN_TK_PROP_MULTI_POINT2D_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/stdlib/math.h>

namespace lsp
{
    namespace tk
    {
        /**
         * Two-dimensional point property interface
         */
        class Point2D: public MultiProperty
        {
            protected:
                enum property_t
                {
                    P_VALUE,
                    P_X,
                    P_Y,

                    P_COUNT
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                float               fX;                 // X position
                float               fY;                 // Y position

            protected:
                virtual void        push() override;
                virtual void        commit(atom_t property) override;

                static bool         parse(float *x, float *y, const LSPString *s);

            protected:
                explicit Point2D(prop::Listener *listener = NULL);
                Point2D(const Point2D &) = delete;
                Point2D(Point2D &&) = delete;
                virtual ~Point2D() override;

                Point2D & operator = (const Point2D &) = delete;
                Point2D & operator = (Point2D &&) = delete;

            public:
                inline float        x() const                   { return fX;                    }
                inline float        y() const                   { return fY;                    }
                inline float        ax() const                  { return fabs(fX);              }
                inline float        ay() const                  { return fabs(fY);              }

                float               set_x(float v);
                float               set_y(float v);
                void                set(float x, float y);
        };

        namespace prop
        {
            /**
             * Vector2D property implementation
             */
            class Point2D: public tk::Point2D
            {
                public:
                    explicit Point2D(prop::Listener *listener = NULL): tk::Point2D(listener) {};
                    Point2D(const Point2D &) = delete;
                    Point2D(Point2D &&) = delete;

                    Point2D & operator = (const Point2D &) = delete;
                    Point2D & operator = (Point2D &&) = delete;

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::Point2D::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::Point2D::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::Point2D::bind(property, style, vAtoms, DESC, &sListener); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::Point2D::unbind(vAtoms, DESC, &sListener); };
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */


#endif /* LSP_PLUG_IN_TK_PROP_MULTI_POINT2D_H_ */
