/*
 * Layout.h
 *
 *  Created on: 15 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_LAYOUT_H_
#define LSP_PLUG_IN_TK_PROP_LAYOUT_H_

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
                Layout & operator = (const Layout &);

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

                class Listener: public IStyleListener
                {
                    private:
                        Layout        *pValue;

                    public:
                        inline Listener(Layout *color)   { pValue = color; };

                    public:
                        virtual void    notify(atom_t property);
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                float               hAlign;             // Horizontal alignment
                float               vAlign;             // Vertical alignment
                float               hScale;             // Horizontal scale
                float               vScale;             // Vertical scale
                Listener            sListener;          // Listener

            protected:
                void                sync();
                void                commit(atom_t property);
                void                parse(const LSPString *s);

            protected:
                explicit Layout(prop::Listener *listener = NULL);
                ~Layout();

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
                private:
                    Layout & operator = (const Layout &);

                public:
                    explicit Layout(prop::Listener *listener = NULL): tk::Layout(listener) {};

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

                    /**
                     * Initialize default values
                     * @return status of operation
                     */
                    status_t            init(Style *style, float halign, float valign, float hscale, float vscale);
            };
        }
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_LAYOUT_H_ */
