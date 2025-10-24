/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 23 сент. 2025 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_MULTI_RANGE_H_
#define LSP_PLUG_IN_TK_PROP_MULTI_RANGE_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif /* LSP_PLUG_IN_TK_IMPL */

namespace lsp
{
    namespace tk
    {
        /**
         * Floating-point range property
         */
        class Range: public MultiProperty
        {
            protected:
                enum property_t
                {
                    P_VALUE,
                    P_MIN,
                    P_MAX,

                    P_COUNT
                };

                enum flags_t
                {
                    F_RANGE_LOCK    = 1 << 0
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                float               fMin;
                float               fMax;
                size_t              nFlags;
                float_transform_t   pTransform;
                mutable void       *pTransformArg;

            protected:
                virtual void        push() override;
                virtual void        commit(atom_t property) override;

                float               do_limit(float v) const;
                float               transform(float v) const;

            protected:
                explicit Range(prop::Listener *listener = NULL);
                Range(const Range &) = delete;
                Range(Range &&) = delete;
                virtual ~Range() override;

                Range & operator = (const Range &) = delete;
                Range & operator = (Range &&) = delete;

            public:
                inline void         set_transform(float_transform_t func, void *arg = NULL) { pTransform = func; pTransformArg = arg; }
                inline void         clear_transform()       { set_transform(NULL, NULL);    }

            public:
                inline float        min() const             { return fMin;                  }
                inline float        max() const             { return fMax;                  }
                inline float        range() const           { return fMax - fMin;           }
                inline float        abs_range() const       { return (fMax > fMin) ? fMax - fMin : fMin - fMax; }
                inline bool         range_locked() const    { return nFlags & F_RANGE_LOCK; }
                inline bool         inversed() const        { return fMin > fMax;           }

                float               set_min(float v);
                float               set_max(float v);
                void                set(float min, float max);

                float               get_normalized(float value) const           { return Property::normalized(value, fMin, fMax);   }
                static inline float limit(float value, float min, float max)    { return Property::limit(value, min, max);          }
                inline float        limit(float v) const                        { return Property::limit(v, fMin, fMax);            }
                static inline bool  matches(float v, float min, float max)      { return Property::matches(v, min, max);            }
                inline bool         matches(float v) const                      { return Property::matches(v, fMin, fMax);          }
        };

        namespace prop
        {
            /**
             * Range property implementation
             */
            class Range: public tk::Range
            {
                public:
                    explicit Range(prop::Listener *listener = NULL): tk::Range(listener) {};
                    Range(const Range &) = delete;
                    Range(Range &&) = delete;

                    Range & operator = (const Range &) = delete;
                    Range & operator = (Range &&) = delete;

                public:
                    bool                lock_range(bool lock = true);
                    inline bool         unlock_range()                                  { return lock_range(false);     }
                    float               set_min(float v);
                    float               set_max(float v);
                    void                set(float min, float max);

                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::Range::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::Range::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::Range::bind(property, style, vAtoms, DESC, &sListener); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::Range::unbind(vAtoms, DESC, &sListener); };
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */


#endif /* INCLUDE_LSP_PLUG_IN_TK_PROP_MULTI_RANGE_H_ */
