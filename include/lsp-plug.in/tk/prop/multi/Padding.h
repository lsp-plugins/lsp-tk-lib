/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 12 июл. 2017 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_MULTI_PADDING_H_
#define LSP_PLUG_IN_TK_PROP_MULTI_PADDING_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Padding: public MultiProperty
        {
            protected:
                Padding & operator = (const Padding &);

            protected:
                enum property_t
                {
                    P_VALUE,
                    P_LEFT,
                    P_RIGHT,
                    P_TOP,
                    P_BOTTOM,
                    P_CSS,

                    P_COUNT
                };

                class Listener: public IStyleListener
                {
                    private:
                        Padding        *pValue;

                    public:
                        inline Listener(Padding *color)   { pValue = color; };

                    public:
                        virtual void    notify(atom_t property);
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                padding_t           sValue;             // Padding value
                Listener            sListener;          // Listener

            protected:
                void                sync();
                void                commit(atom_t property);
                void                parse(const LSPString *s);
                void                parse_css(const LSPString *s);

            protected:
                explicit Padding(prop::Listener *listener = NULL);
                ~Padding();

            public:
                inline void         set_default()       { MultiProperty::set_default(vAtoms, DESC); };

            public:
                inline size_t   left() const                { return sValue.nLeft;                  }
                inline size_t   right() const               { return sValue.nRight;                 }
                inline size_t   top() const                 { return sValue.nTop;                   }
                inline size_t   bottom() const              { return sValue.nBottom;                }
                inline size_t   horizontal() const          { return sValue.nLeft + sValue.nRight;  }
                inline size_t   vertical() const            { return sValue.nTop + sValue.nBottom;  }

                inline void     get(padding_t *p) const     { *p = sValue;                          }
                inline void     get(padding_t &p) const     { p = sValue;                           }
                void            get(size_t *left, size_t *right, size_t *top, size_t *bottom) const;
                void            get(size_t &left, size_t &right, size_t &top, size_t &bottom) const;
                inline void     get_horizontal(size_t *left, size_t *right)    { *left = sValue.nLeft; *right = sValue.nRight; }
                inline void     get_vertical(size_t *top, size_t *bottom)      { *top = sValue.nTop; *bottom = sValue.nBottom; }

                size_t          set_left(size_t value);
                size_t          set_right(size_t value);
                size_t          set_top(size_t value);
                size_t          set_bottom(size_t value);
                void            set_all(size_t value);
                void            set_horizontal(size_t left, size_t right);
                void            set_vertical(size_t top, size_t bottom);

                inline void     set_horizontal(size_t h)    { set_horizontal(h, h);                 }
                inline void     set_vertical(size_t v)      { set_vertical(v, v);                   }
                inline void     set(size_t value)           { set_all(value);                       }

                void            set(size_t left, size_t right, size_t top, size_t bottom);
                void            set(const padding_t *p);
                void            set(const Padding *p);

                void            compute(padding_t *padding, float scale);
                void            add(ws::size_limit_t *dst, float scale);
                void            add(ws::rectangle_t *dst, const ws::rectangle_t *src, float scale);
                void            sub(ws::rectangle_t *dst, const ws::rectangle_t *src, float scale);
                void            enter(ws::rectangle_t *dst, const ws::rectangle_t *src, float scale);
                void            leave(ws::rectangle_t *dst, const ws::rectangle_t *src, float scale);

                inline void     add(ws::rectangle_t *dst, float scale)              { add(dst, dst, scale);     }
                inline void     sub(ws::rectangle_t *dst, float scale)              { sub(dst, dst, scale);     }
                inline void     enter(ws::rectangle_t *dst, float scale)            { enter(dst, dst, scale);   }
                inline void     leave(ws::rectangle_t *dst, float scale)            { leave(dst, dst, scale);   }

                static void             add(ws::rectangle_t *dst, const ws::rectangle_t *src, const padding_t *pad);
                static void             sub(ws::rectangle_t *dst, const ws::rectangle_t *src, const padding_t *pad);
                static void             enter(ws::rectangle_t *dst, const ws::rectangle_t *src, const padding_t *pad);
                static void             leave(ws::rectangle_t *dst, const ws::rectangle_t *src, const padding_t *pad);

                static void             add(ws::size_limit_t *dst, const ws::size_limit_t *src, const padding_t *pad);

                static inline void      add(ws::rectangle_t *dst, const padding_t *pad)         { add(dst, dst, pad);       }
                static inline void      sub(ws::rectangle_t *dst, const padding_t *pad)         { sub(dst, dst, pad);       }
                static inline void      enter(ws::rectangle_t *dst, const padding_t *pad)       { enter(dst, dst, pad);     }
                static inline void      leave(ws::rectangle_t *dst, const padding_t *pad)       { leave(dst, dst, pad);     }

                static inline void      add(ws::size_limit_t *dst, const padding_t *pad)        { add(dst, dst, pad);       }
        };

        namespace prop
        {
            /**
             * Padding property implementation
             */
            class Padding: public tk::Padding
            {
                private:
                    Padding & operator = (const Padding &);

                public:
                    explicit Padding(prop::Listener *listener = NULL): tk::Padding(listener) {};

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::Padding::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::Padding::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::Padding::bind(property, style, vAtoms, DESC, &sListener); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::Padding::unbind(vAtoms, DESC, &sListener); };

                    /**
                     * Initialize default values
                     * @return status of operation
                     */
                    status_t            init(Style *style, size_t left, size_t right, size_t top, size_t bottom);
                    status_t            init(Style *style, const padding_t *p);
                    inline status_t     init(Style *style, size_t all)                  { return init(style, all, all, all, all);               }

                    status_t            override(Style *style, size_t left, size_t right, size_t top, size_t bottom);
                    status_t            override(Style *style, const padding_t *p);
                    inline status_t     override(Style *style, size_t all)              { return override(style, all, all, all, all);           }
            };
        }
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_MULTI_PADDING_H_ */
