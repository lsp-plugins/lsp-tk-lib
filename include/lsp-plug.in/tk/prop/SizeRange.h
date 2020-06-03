/*
 * SizeRange.h
 *
 *  Created on: 29 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_SIZERANGE_H_
#define LSP_PLUG_IN_TK_PROP_SIZERANGE_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class SizeRange: public MultiProperty
        {
            protected:
                SizeRange & operator = (const SizeRange &);

            protected:
                enum property_t
                {
                    P_VALUE,
                    P_MIN,
                    P_MAX,

                    P_COUNT
                };

                class Listener: public IStyleListener
                {
                    private:
                        SizeRange        *pValue;

                    public:
                        inline Listener(SizeRange *color)   { pValue = color; };

                    public:
                        virtual void    notify(atom_t property);
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                ssize_t             nMin;               // Minimum size
                ssize_t             nMax;               // Maximum size
                Listener            sListener;          // Listener

            protected:
                void                sync();
                void                commit(atom_t property);

            protected:
                explicit SizeRange(prop::Listener *listener = NULL);
                ~SizeRange();

            public:
                inline void         set_default()           { MultiProperty::set_default(vAtoms, DESC); };

            public:
                inline ssize_t min() const                  { return nMin;                          }
                inline ssize_t max() const                  { return nMax;                          }

                inline void get(ssize_t *min, ssize_t *max) const       { *min = nMin; *max = nMax;     }
                inline void get(ssize_t &min, ssize_t &max) const       { min = nMin; max = nMax;       }

                ssize_t     set_min(ssize_t value);
                ssize_t     set_max(ssize_t value);

                void        set(ssize_t min, ssize_t max);
                void        set(const SizeRange *src);
                inline void set(ssize_t value)              { set(value, value);                    }

                void        compute(ssize_t *min, ssize_t *max, float scale) const;
                void        compute(ws::size_limit_t *r, float scale) const;
        };

        namespace prop
        {
            /**
             * SizeRange property implementation
             */
            class SizeRange: public tk::SizeRange
            {
                private:
                    SizeRange & operator = (const SizeRange &);

                public:
                    explicit SizeRange(prop::Listener *listener = NULL): tk::SizeRange(listener) {};

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::SizeRange::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::SizeRange::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::SizeRange::bind(property, style, vAtoms, DESC, &sListener); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::SizeRange::unbind(vAtoms, DESC, &sListener); };

                    /**
                     * Initialize
                     * @param style style
                     * @return status of operation
                     */
                    status_t            init(Style *style, ssize_t min, ssize_t max);
                    inline status_t     init(Style *style, ssize_t value)               { return init(style, value, value); }
            };
        }

    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_SIZERANGE_H_ */
