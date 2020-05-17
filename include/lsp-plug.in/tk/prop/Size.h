/*
 * Size.h
 *
 *  Created on: 10 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_SIZE_H_
#define LSP_PLUG_IN_TK_PROP_SIZE_H_

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
                Size & operator = (const Size &);

            protected:
                enum property_t
                {
                    P_VALUE,
                    P_WIDTH,
                    P_HEIGHT,

                    P_COUNT
                };

                class Listener: public IStyleListener
                {
                    private:
                        Size        *pValue;

                    public:
                        inline Listener(Size *color)   { pValue = color; };

                    public:
                        virtual void    notify(atom_t property);
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                size_t              nWidth;             // Width
                size_t              nHeight;            // Height
                Listener            sListener;          // Listener

            protected:
                void                sync(bool notify);
                void                commit(atom_t property);

            protected:
                explicit Size(prop::Listener *listener = NULL);
                ~Size();

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
        };

        namespace prop
        {
            /**
             * Size property implementation
             */
            class Size: public tk::Size
            {
                private:
                    Size & operator = (const Size *);

                public:
                    explicit Size(prop::Listener *listener = NULL): tk::Size(listener) {};

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
                     * Initialize default size
                     * @param style style
                     * @param width width
                     * @param height height
                     * @return status of operation
                     */
                    status_t            init(Style *style, size_t width, size_t height);

                    /**
                     * Commit new size
                     * @param width new width
                     * @param height new height
                     * @param scale scaling factor
                     */
                    void                commit(size_t width, size_t height, float scale);
                    void                commit(const ws::rectangle_t *rect, float scale);
            };
        }
    }
}



#endif /* LSP_PLUG_IN_TK_PROP_SIZE_H_ */
