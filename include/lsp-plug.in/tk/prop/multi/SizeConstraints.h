/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef LSP_PLUG_IN_TK_PROP_MULTI_SIZECONSTRAINTS_H_
#define LSP_PLUG_IN_TK_PROP_MULTI_SIZECONSTRAINTS_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class SizeConstraints: public MultiProperty
        {
            protected:
                SizeConstraints & operator = (const SizeConstraints &);

            protected:
                enum property_t
                {
                    P_VALUE,
                    P_MIN_WIDTH,
                    P_MIN_HEIGHT,
                    P_MAX_WIDTH,
                    P_MAX_HEIGHT,
                    P_CSS,

                    P_COUNT
                };

                class Listener: public IStyleListener
                {
                    private:
                        SizeConstraints        *pValue;

                    public:
                        inline Listener(SizeConstraints *color)   { pValue = color; };

                    public:
                        virtual void    notify(atom_t property);
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                ws::size_limit_t    sValue;             // Value
                Listener            sListener;          // Listener

            protected:
                void                sync();
                void                commit(atom_t property);
                status_t            init();
                status_t            override();

            protected:
                explicit SizeConstraints(prop::Listener *listener = NULL);
                ~SizeConstraints();

            public:
                inline void         set_default()           { MultiProperty::set_default(vAtoms, DESC); };

            public:
                inline ssize_t min_width() const            { return sValue.nMinWidth;              }
                inline ssize_t min_height() const           { return sValue.nMinHeight;             }
                inline ssize_t max_width() const            { return sValue.nMaxWidth;              }
                inline ssize_t max_height() const           { return sValue.nMaxHeight;             }

                inline void get(ws::size_limit_t *p) const  { *p = sValue;                          }
                inline void get(ws::size_limit_t &p) const  { p = sValue;                           }
                void get(ssize_t *min_width, ssize_t *min_height, ssize_t *max_width, ssize_t *max_height) const;
                void get(ssize_t &min_width, ssize_t &min_height, ssize_t &max_width, ssize_t &max_height) const;

                inline void get_min(ssize_t *width, ssize_t *height) const  { *width = sValue.nMinWidth; *height = sValue.nMinHeight;   }
                inline void get_min(ssize_t &width, ssize_t &height) const  { width = sValue.nMinWidth; height = sValue.nMinHeight;     }
                inline void get_max(ssize_t *width, ssize_t *height) const  { *width = sValue.nMaxWidth; *height = sValue.nMaxHeight;   }
                inline void get_max(ssize_t &width, ssize_t &height) const  { width = sValue.nMaxWidth; height = sValue.nMaxHeight;     }

                inline void get_width(ssize_t *min, ssize_t *max) const     { *min = sValue.nMinWidth; *max = sValue.nMaxWidth;         }
                inline void get_width(ssize_t &min, ssize_t &max) const     { min = sValue.nMinWidth; max = sValue.nMaxWidth;           }
                inline void get_height(ssize_t *min, ssize_t *max) const    { *min = sValue.nMinHeight; *max = sValue.nMaxHeight;       }
                inline void get_height(ssize_t &min, ssize_t &max) const    { min = sValue.nMinHeight; max = sValue.nMaxHeight;         }

                ssize_t     set_min_width(ssize_t value);
                ssize_t     set_min_height(ssize_t value);
                ssize_t     set_max_width(ssize_t value);
                ssize_t     set_max_height(ssize_t value);

                void        set_min(ssize_t width, ssize_t height);
                void        set_max(ssize_t width, ssize_t height);
                void        set_width(ssize_t min, ssize_t max);
                void        set_height(ssize_t min, ssize_t max);

                inline void set_width(ssize_t value)                        { set_width(value, value);              }
                inline void set_height(ssize_t value)                       { set_height(value, value);             }

                void        set(ssize_t min_width, ssize_t min_height, ssize_t max_width, ssize_t max_height);
                void        set(const ws::size_limit_t *p);
                void        set(const SizeConstraints *p);

                void        set_fixed(ssize_t width, ssize_t height)        { set(width, height, width, height);    }
                inline void set_fixed(ssize_t size)                         { set(size, size, size, size);          }

                void        compute(ws::size_limit_t *limit, float scale);

                static void apply(ws::size_limit_t *dst, const ws::size_limit_t *src, const ws::size_limit_t *rule);

                void        apply(ws::size_limit_t *dst, const ws::size_limit_t *src, float scale);
                inline void apply(ws::size_limit_t *dst, float scale)           { apply(dst, dst, scale);   }

                void        apply(ws::rectangle_t *sc, float scale);
                static void apply(ws::rectangle_t *dst, const ws::rectangle_t *src, const ws::size_limit_t *sc);
                static void apply(ws::rectangle_t *dst, const ws::size_limit_t *sc);

                // Transposed versions with swapped roles of Width and Height
                void        tcompute(ws::size_limit_t *limit, float scale);

                void        tapply(ws::size_limit_t *dst, const ws::size_limit_t *src, float scale);
                inline void tapply(ws::size_limit_t *dst, float scale)          { tapply(dst, dst, scale);   }

                void        tapply(ws::rectangle_t *sc, float scale);
                static void tapply(ws::rectangle_t *dst, const ws::rectangle_t *src, const ws::size_limit_t *sc);
                static void tapply(ws::rectangle_t *dst, const ws::size_limit_t *sc);
        };

        namespace prop
        {
            /**
             * SizeConstraints property implementation
             */
            class SizeConstraints: public tk::SizeConstraints
            {
                private:
                    SizeConstraints & operator = (const SizeConstraints &);

                public:
                    explicit SizeConstraints(prop::Listener *listener = NULL): tk::SizeConstraints(listener) {};

                protected:
                    using               tk::SizeConstraints::init;
                    using               tk::SizeConstraints::override;

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::SizeConstraints::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::SizeConstraints::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::SizeConstraints::bind(property, style, vAtoms, DESC, &sListener); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::SizeConstraints::unbind(vAtoms, DESC, &sListener); };

                    /**
                     * Initialize
                     * @param style style
                     * @return status of operation
                     */
                    status_t            init(Style *style, ssize_t min_width, ssize_t min_height, ssize_t max_width, ssize_t max_height);
                    status_t            init(Style *style, const ws::size_limit_t *p);
                    status_t            init(Style *style);

                    status_t            override(Style *style, ssize_t min_width, ssize_t min_height, ssize_t max_width, ssize_t max_height);
                    status_t            override(Style *style, const ws::size_limit_t *p);
                    status_t            override(Style *style);

                    static status_t     init(const char *name, Style *style, ssize_t min_width, ssize_t min_height, ssize_t max_width, ssize_t max_height);
                    static status_t     init(const char *name, Style *style, const ws::size_limit_t *p);
                    static status_t     init(const char *name, Style *style);

                    static status_t     override(const char *name, Style *style, ssize_t min_width, ssize_t min_height, ssize_t max_width, ssize_t max_height);
                    static status_t     override(const char *name, Style *style, const ws::size_limit_t *p);
                    static status_t     override(const char *name, Style *style);
            };
        }

    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_MULTI_SIZECONSTRAINTS_H_ */
