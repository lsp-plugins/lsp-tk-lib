/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 16 июн. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_MULTI_RECTANGLE_H_
#define LSP_PLUG_IN_TK_PROP_MULTI_RECTANGLE_H_

namespace lsp
{
    namespace tk
    {
        /**
         * Floating-point property interface
         */
        class Rectangle: public MultiProperty
        {
            protected:
                enum property_t
                {
                    P_VALUE,
                    P_LEFT,
                    P_TOP,
                    P_WIDTH,
                    P_HEIGHT,

                    P_COUNT
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                ws::rectangle_t     sRect;

            protected:
                virtual void        push() override;
                virtual void        commit(atom_t property) override;

                float               climited(float v) const;
                float               change(float k, float step);

            protected:
                explicit Rectangle(prop::Listener *listener = NULL);
                Rectangle(const Rectangle &) = delete;
                Rectangle(Rectangle &&) = delete;
                virtual ~Rectangle() override;

                Rectangle & operator = (const Rectangle &) = delete;
                Rectangle & operator = (Rectangle &&) = delete;

            public:
                inline ssize_t      left() const                    { return sRect.nLeft;           }
                inline ssize_t      top() const                     { return sRect.nTop;            }
                inline ssize_t      width() const                   { return sRect.nWidth;          }
                inline ssize_t      height() const                  { return sRect.nHeight;         }
                void                get(ws::rectangle_t *r) const   { *r = sRect;                   }

                ssize_t             set_left(ssize_t v);
                ssize_t             set_top(ssize_t v);
                ssize_t             set_width(ssize_t v);
                ssize_t             set_height(ssize_t v);
                void                set_position(ssize_t left, ssize_t top);
                void                set_size(ssize_t width, ssize_t height);
                void                set(ssize_t left, ssize_t top, ssize_t width, ssize_t height);
                void                set(const ws::rectangle_t *r);
                inline void         clear()                         { set(0, 0, 0, 0);              }

            public:
                static void         enter_border(ws::rectangle_t *r, const ws::rectangle_t *sr, ssize_t border);
                static void         enter_border(ws::rectangle_t *r, ssize_t border);
        };

        namespace prop
        {
            /**
             * Rectangle property implementation
             */
            class Rectangle: public tk::Rectangle
            {
                public:
                    explicit Rectangle(prop::Listener *listener = NULL): tk::Rectangle(listener) {};
                    Rectangle(const Rectangle &) = delete;
                    Rectangle(Rectangle &&) = delete;

                    Rectangle & operator = (const Rectangle &) = delete;
                    Rectangle & operator = (Rectangle &&) = delete;

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::Rectangle::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::Rectangle::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::Rectangle::bind(property, style, vAtoms, DESC, &sListener); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::Rectangle::unbind(vAtoms, DESC, &sListener); };
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */




#endif /* LSP_PLUG_IN_TK_PROP_MULTI_RECTANGLE_H_ */
