/*
 * Padding.h
 *
 *  Created on: 12 июл. 2017 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_PADDING_H_
#define LSP_PLUG_IN_TK_PROP_PADDING_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Widget;
        class Display;
        class Style;
        
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
                inline size_t left() const              { return sValue.nLeft;                  }
                inline size_t right() const             { return sValue.nRight;                 }
                inline size_t top() const               { return sValue.nTop;                   }
                inline size_t bottom() const            { return sValue.nBottom;                }
                inline size_t horizontal() const        { return sValue.nLeft + sValue.nRight;  }
                inline size_t vertical() const          { return sValue.nTop + sValue.nBottom;  }

                inline void get(padding_t *p) const     { *p = sValue;                          }
                inline void get(padding_t &p) const     { p = sValue;                           }
                void get(size_t *left, size_t *right, size_t *top, size_t *bottom) const;
                void get(size_t &left, size_t &right, size_t &top, size_t &bottom) const;
                inline void get_horizontal(size_t *left, size_t *right)    { *left = sValue.nLeft; *right = sValue.nRight; }
                inline void get_vertical(size_t *top, size_t *bottom)      { *top = sValue.nTop; *bottom = sValue.nBottom; }

                size_t  set_left(size_t value);
                size_t  set_right(size_t value);
                size_t  set_top(size_t value);
                size_t  set_bottom(size_t value);
                void    set_all(size_t value);
                void    set_horizontal(size_t left, size_t right);
                void    set_vertical(size_t top, size_t bottom);

                void    set(size_t left, size_t right, size_t top, size_t bottom);
                void    set(const padding_t *p);
                void    set(const Padding *p);

                void    compute(padding_t *padding, float scale);
        };

        namespace prop
        {
            /**
             * Padding property implementation
             */
            class Padding: public tk::Padding
            {
                private:
                    Padding & operator = (const Padding *);

                public:
                    explicit Padding(prop::Listener *listener = NULL): tk::Padding(listener) {};

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    status_t            bind(const char *property, Widget *widget);
                    status_t            bind(atom_t property, Widget *widget);
                    status_t            bind(const char *property, Style *style, Display *dpy);
                    status_t            bind(atom_t property, Style *style, Display *dpy);

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()            { return MultiProperty::unbind(vAtoms, DESC, &sListener); };
            };
        }
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_PADDING_H_ */
