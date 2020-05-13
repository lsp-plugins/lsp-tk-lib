/*
 * Position.h
 *
 *  Created on: 10 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_POSITION_H_
#define LSP_PLUG_IN_TK_PROP_POSITION_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Display;
        class Widget;
        class Style;

        class Position: public MultiProperty
        {
            protected:
                Position & operator = (const Position &);

            protected:
                enum property_t
                {
                    P_VALUE,
                    P_LEFT,
                    P_TOP,

                    P_COUNT
                };

                class Listener: public IStyleListener
                {
                    private:
                        Position        *pValue;

                    public:
                        inline Listener(Position *color)   { pValue = color; };

                    public:
                        virtual void    notify(atom_t property);
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                ssize_t             nLeft;              // Left
                ssize_t             nTop;               // Top
                Listener            sListener;          // Listener

            protected:
                void                sync();
                void                commit(atom_t property);
                void                parse(const LSPString *s);

            protected:
                explicit Position(prop::Listener *listener = NULL);
                ~Position();

            public:
                inline void         set_default()       { MultiProperty::set_default(vAtoms, DESC); };

            public:
                inline ssize_t left() const             { return nLeft;                        }
                inline ssize_t top() const              { return nTop;                       }

                inline void get(ssize_t *left, ssize_t *top) const  { *left = nLeft; *top = nTop;           }
                inline void get(ssize_t &left, ssize_t &top) const  { left = nLeft; top = nTop;             }

                ssize_t set_left(ssize_t value);
                ssize_t set_top(ssize_t value);
                void    set(ssize_t left, ssize_t top);
                void    set(const Position *p);
        };

        namespace prop
        {
            /**
             * Position property implementation
             */
            class Position: public tk::Position
            {
                private:
                    Position & operator = (const Position *);

                public:
                    explicit Position(prop::Listener *listener = NULL): tk::Position(listener) {};

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    status_t            bind(const char *property, Widget *widget);
                    status_t            bind(atom_t property, Widget *widget);
                    status_t            bind(const char *property, Style *style, Atoms *atoms);
                    status_t            bind(atom_t property, Style *style, Atoms *atoms);

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()            { return MultiProperty::unbind(vAtoms, DESC, &sListener); };
            };
        }
    }
}

#endif /* LSP_PLUG_IN_TK_PROP_POSITION_H_ */
