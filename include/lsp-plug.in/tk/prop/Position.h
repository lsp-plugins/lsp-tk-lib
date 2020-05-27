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
                void                sync(bool notify);
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

                inline void get(ssize_t *left, ssize_t *top) const  { *left = nLeft; *top = nTop;               }
                inline void get(ssize_t &left, ssize_t &top) const  { left = nLeft; top = nTop;                 }
                inline void get(ws::rectangle_t *rect) const        { rect->nLeft = nLeft; rect->nTop = nTop;   }

                ssize_t         set_left(ssize_t value);
                ssize_t         set_top(ssize_t value);
                void            set(ssize_t left, ssize_t top);
                void            set(const Position *p);

                static bool     inside(const ws::rectangle_t *rect, ssize_t left, ssize_t top);
                inline bool     inside(const ws::rectangle_t *rect) { return inside(rect, nLeft, nTop);         }
        };

        namespace prop
        {
            /**
             * Position property implementation
             */
            class Position: public tk::Position
            {
                private:
                    Position & operator = (const Position &);

                public:
                    explicit Position(prop::Listener *listener = NULL): tk::Position(listener) {};

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::Position::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::Position::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::Position::bind(property, style, vAtoms, DESC, &sListener); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::Position::unbind(vAtoms, DESC, &sListener); };

                    /**
                     * Init default position
                     * @param style style
                     * @param left left
                     * @param top top
                     * @return status of operation
                     */
                    status_t            init(Style *style, ssize_t left, ssize_t top);

                    /**
                     * Commit new position
                     * @param left left
                     * @param top top
                     * @param scale
                     */
                    void                commit(ssize_t left, ssize_t top);
            };
        }
    }
}

#endif /* LSP_PLUG_IN_TK_PROP_POSITION_H_ */
