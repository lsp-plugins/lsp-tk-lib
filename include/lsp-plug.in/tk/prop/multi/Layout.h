/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 15 мая 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_MULTI_LAYOUT_H_
#define LSP_PLUG_IN_TK_PROP_MULTI_LAYOUT_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Layout property.
         * Allows to align widget within parent widget
         * from the most left/top (-1) position to the
         * most right/bottom (1) position.
         *
         * Additionally, it allows to control the scaling of
         * the widget relative to the rest empty space if possible
         */
        class Layout: public MultiProperty
        {
            protected:
                enum property_t
                {
                    P_VALUE,
                    P_HALIGN,
                    P_VALIGN,
                    P_HSCALE,
                    P_VSCALE,

                    P_COUNT
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                float               hAlign;             // Horizontal alignment
                float               vAlign;             // Vertical alignment
                float               hScale;             // Horizontal scale
                float               vScale;             // Vertical scale

            protected:
                virtual void        push() override;
                virtual void        commit(atom_t property) override;

                void                parse(const LSPString *s);

            protected:
                explicit Layout(prop::Listener *listener = NULL);
                Layout(const Layout &) = delete;
                Layout(Layout &&) = delete;
                virtual ~Layout() override;

                Layout & operator = (const Layout &) = delete;
                Layout & operator = (Layout &&) = delete;

            public:
                inline float        halign() const              { return hAlign;    }
                inline float        valign() const              { return vAlign;    }
                inline float        hscale() const              { return hScale;    }
                inline float        vscale() const              { return vScale;    }

                float               set_halign(float v);
                float               set_valign(float v);
                float               set_hscale(float v);
                float               set_vscale(float v);

                void                set_align(float h, float v);
                void                set_scale(float h, float v);
                void                set(float halign, float valign, float hscale, float vscale);

                inline void         set_align(float value)      { set_align(value, value);  }
                inline void         set_scale(float value)      { set_scale(value, value);  }
                inline void         set(float align, float scale)   { set(align, align, scale, scale);      }

                inline void         set_default()               { MultiProperty::set_default(vAtoms, DESC); }

            public:
                inline void         apply(ws::rectangle_t *dst, const ws::size_limit_t *req) { return apply(dst, dst, req); }
                void                apply(ws::rectangle_t *dst, const ws::rectangle_t *src, const ws::size_limit_t *req);

        };

        namespace prop
        {
            /**
             * Layout property implementation
             */
            class Layout: public tk::Layout
            {
                public:
                    explicit Layout(prop::Listener *listener = NULL): tk::Layout(listener) {};
                    Layout(const Layout &) = delete;
                    Layout(Layout &&) = delete;

                    Layout & operator = (const Layout &) = delete;
                    Layout & operator = (Layout &&) = delete;

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::Layout::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::Layout::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::Layout::bind(property, style, vAtoms, DESC, &sListener); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::Layout::unbind(vAtoms, DESC, &sListener); };

                    inline void         listener(prop::Listener *listener)              { pListener = listener;                     }
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */


#endif /* LSP_PLUG_IN_TK_PROP_MULTI_LAYOUT_H_ */
