/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 1 июн. 2021 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_MULTI_ALIGNMENT_H_
#define LSP_PLUG_IN_TK_PROP_MULTI_ALIGNMENT_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Alignment property.
         * Allows to align element within parent container
         * from the most left/top (-1) position to the
         * most right/bottom (1) position in one direction
         *
         * Additionally, it allows to control the scaling of
         * the space relative to the rest empty space if possible
         */
        class Alignment: public MultiProperty
        {
            protected:
                enum property_t
                {
                    P_VALUE,
                    P_ALIGN,
                    P_SCALE,

                    P_COUNT
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                float               nAlign;             // Alignment
                float               nScale;             // Scaling

            protected:
                virtual void        push() override;
                virtual void        commit(atom_t property) override;

                void                parse(const LSPString *s);

            protected:
                explicit Alignment(prop::Listener *listener = NULL);
                Alignment(const Alignment &) = delete;
                Alignment(Alignment &&) = delete;
                virtual ~Alignment() override;

                Alignment & operator = (const Alignment &) = delete;
                Alignment & operator = (Alignment &&) = delete;

            public:
                inline float        align() const               { return nAlign;    }
                inline float        scale() const               { return nScale;    }

                float               set_align(float v);
                float               set_scale(float v);

                void                set(float halign, float valign);

                inline void         set_default()               { MultiProperty::set_default(vAtoms, DESC); }

            public:
                void                happly(ws::rectangle_t *dst, const ws::rectangle_t *src, const ws::size_limit_t *req);
                void                happly(ws::rectangle_t *dst, const ws::rectangle_t *src, ssize_t avail);
                void                vapply(ws::rectangle_t *dst, const ws::rectangle_t *src, const ws::size_limit_t *req);
                void                vapply(ws::rectangle_t *dst, const ws::rectangle_t *src, ssize_t avail);
                void                apply(ws::rectangle_t *dst, const ws::rectangle_t *src, const ws::size_limit_t *req, orientation_t orientation);
                void                apply(ws::rectangle_t *dst, const ws::rectangle_t *src, ssize_t avail, orientation_t orientation);

                inline void         happly(ws::rectangle_t *dst, const ws::size_limit_t *req) { return happly(dst, dst, req); }
                inline void         happly(ws::rectangle_t *dst, ssize_t avail) { return happly(dst, dst, avail); }
                inline void         vapply(ws::rectangle_t *dst, const ws::size_limit_t *req) { return vapply(dst, dst, req); }
                inline void         vapply(ws::rectangle_t *dst, ssize_t avail) { return vapply(dst, dst, avail); }
                void                apply(ws::rectangle_t *dst, const ws::size_limit_t *req, orientation_t orientation);
                void                apply(ws::rectangle_t *dst, ssize_t avail, orientation_t orientation);

        };

        namespace prop
        {
            /**
             * Alignment property implementation
             */
            class Alignment: public tk::Alignment
            {
                public:
                    explicit Alignment(prop::Listener *listener = NULL): tk::Alignment(listener) {};
                    Alignment(const Alignment &) = delete;
                    Alignment(Alignment &&) = delete;
                    Alignment & operator = (const Alignment &) = delete;
                    Alignment & operator = (Alignment &&) = delete;

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::Alignment::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::Alignment::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::Alignment::bind(property, style, vAtoms, DESC, &sListener); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::Alignment::unbind(vAtoms, DESC, &sListener); };

                    inline void         listener(prop::Listener *listener)              { pListener = listener;                     }
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */



#endif /* INCLUDE_LSP_PLUG_IN_TK_PROP_MULTI_ALIGNMENT_H_ */
