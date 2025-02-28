/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 10 мая 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_MULTI_SIZE_H_
#define LSP_PLUG_IN_TK_PROP_MULTI_SIZE_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Size: public MultiProperty
        {
            protected:
                enum property_t
                {
                    P_VALUE,
                    P_WIDTH,
                    P_HEIGHT,

                    P_COUNT
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                size_t              nWidth;             // Width
                size_t              nHeight;            // Height

            protected:
                virtual void        push() override;
                virtual void        commit(atom_t property) override;

            protected:
                explicit Size(prop::Listener *listener = NULL);
                Size(const Size &) = delete;
                Size(Size &&) = delete;
                virtual ~Size() override;

                Size & operator = (const Size &) = delete;
                Size & operator = (Size &&) = delete;

            public:
                inline void         set_default()       { MultiProperty::set_default(vAtoms, DESC); };

            public:
                inline size_t width() const             { return nWidth;                        }
                inline size_t height() const            { return nHeight;                       }

                inline void get(size_t *width, size_t *height) const        { *width = nWidth; *height = nHeight;           }
                inline void get(size_t &width, size_t &height) const        { width = nWidth; height = nHeight;             }

                size_t  set_width(size_t value);
                size_t  set_height(size_t value);
                void    set(size_t width, size_t height);
                void    set(size_t width, size_t height, float scale);
                void    set(const Size *p);

                void    compute(ws::rectangle_t *dst, float scale);

                static bool         intersection(ws::rectangle_t *dst, const ws::rectangle_t *a, const ws::rectangle_t *b);
                static inline bool  intersection(ws::rectangle_t *dst, const ws::rectangle_t *src) { return intersection(dst, dst, src); }
                static bool         inside(const ws::rectangle_t *outer, const ws::rectangle_t *inner);

                static bool         overlap(const ws::rectangle_t *a, const ws::rectangle_t *b);
                static bool         is_empty(const ws::rectangle_t *r);

                static void         init(ws::rectangle_t *dst, ssize_t left, ssize_t top, ssize_t width, ssize_t height);
        };

        namespace prop
        {
            /**
             * Size property implementation
             */
            class Size: public tk::Size
            {
                public:
                    explicit Size(prop::Listener *listener = NULL): tk::Size(listener) {};
                    Size(const Size &) = delete;
                    Size(Size &&) = delete;

                    Size & operator = (const Size &) = delete;
                    Size & operator = (Size &&) = delete;

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::Size::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::Size::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::Size::bind(property, style, vAtoms, DESC, &sListener); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::Size::unbind(vAtoms, DESC, &sListener); };

                    /**
                     * Commit new size
                     * @param width new width
                     * @param height new height
                     * @param scale scaling factor
                     */
                    void                commit_value(size_t width, size_t height, float scale);
                    void                commit_value(const ws::rectangle_t *rect, float scale);
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_PROP_MULTI_SIZE_H_ */
