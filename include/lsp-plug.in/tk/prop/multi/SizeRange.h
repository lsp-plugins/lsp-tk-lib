/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 29 мая 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_MULTI_SIZERANGE_H_
#define LSP_PLUG_IN_TK_PROP_MULTI_SIZERANGE_H_

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
                enum property_t
                {
                    P_VALUE,
                    P_MIN,
                    P_MAX,

                    P_COUNT
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                ssize_t             nMin;               // Minimum size
                ssize_t             nMax;               // Maximum size

            protected:
                virtual void        push() override;
                virtual void        commit(atom_t property) override;

            protected:
                explicit SizeRange(prop::Listener *listener = NULL);
                SizeRange(const SizeRange &) = delete;
                SizeRange(SizeRange &&) = delete;
                virtual ~SizeRange() override;

                SizeRange & operator = (const SizeRange &) = delete;
                SizeRange & operator = (SizeRange &&) = delete;

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
                public:
                    explicit SizeRange(prop::Listener *listener = NULL): tk::SizeRange(listener) {};
                    SizeRange(const SizeRange &) = delete;
                    SizeRange(SizeRange &&) = delete;

                    SizeRange & operator = (const SizeRange &) = delete;
                    SizeRange & operator = (SizeRange &&) = delete;

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

            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */


#endif /* LSP_PLUG_IN_TK_PROP_MULTI_SIZERANGE_H_ */
