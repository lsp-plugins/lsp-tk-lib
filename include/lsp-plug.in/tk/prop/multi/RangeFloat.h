/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 4 июн. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_MULTI_RANGEFLOAT_H_
#define LSP_PLUG_IN_TK_PROP_MULTI_RANGEFLOAT_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Floating-point property interface
         */
        class RangeFloat: public MultiProperty
        {
            private:
                RangeFloat & operator = (const RangeFloat &);

            protected:
                enum property_t
                {
                    P_VALUE,
                    P_RVALUE,
                    P_MIN,
                    P_MAX,

                    P_COUNT
                };

                enum flags_t
                {
                    F_RANGE_LOCK    = 1 << 0,
                    F_AUTO_LIMIT    = 1 << 1
                };

                class Listener: public IStyleListener
                {
                    protected:
                        RangeFloat      *pValue;

                    public:
                        inline Listener(RangeFloat *ptr)             { pValue = ptr;     }

                    public:
                        virtual void notify(atom_t property);
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                float               fValue;
                float               fMin;
                float               fMax;
                size_t              nFlags;
                Listener            sListener;

            protected:
                void                sync();
                void                commit(atom_t property);
                float               climited(float v) const;
                float               change(float k, float step);
                float               do_limit(float v) const;
                status_t            init();
                status_t            override();

            protected:
                explicit RangeFloat(prop::Listener *listener = NULL);
                ~RangeFloat();

            public:
                inline float        get() const             { return do_limit(fValue);      }
                inline float        min() const             { return fMin;                  }
                inline float        max() const             { return fMax;                  }
                inline float        range() const           { return fMax - fMin;           }
                inline float        abs_range() const       { return (fMax > fMin) ? fMax - fMin : fMin - fMax; }
                inline bool         range_locked() const    { return nFlags & F_RANGE_LOCK; }
                inline bool         auto_limit() const      { return nFlags & F_AUTO_LIMIT; }

                float               set(float v);
                float               set_all(float v, float min, float max);
                float               set_min(float v);
                float               set_max(float v);
                void                set_range(float min, float max);

                float               add(float value, bool cyclic = false);
                float               sub(float value, bool cyclic = false);

                float               get_normalized(float value) const           { return Property::normalized(value, fMin, fMax);   }
                inline float        get_normalized() const                      { return Property::normalized(fValue, fMin, fMax);  }
                float               set_normalized(float value, bool cyclic = false);

                static inline float limit(float value, float min, float max)    { return Property::limit(value, min, max);          }
                inline float        limit(float v) const                        { return Property::limit(v, fMin, fMax);            }
                static inline bool  matches(float v, float min, float max)      { return Property::matches(v, min, max);            }
                inline bool         matches(float v) const                      { return Property::matches(v, fMin, fMax);          }
        };

        namespace prop
        {
            /**
             * RangeFloat property implementation
             */
            class RangeFloat: public tk::RangeFloat
            {
                private:
                    RangeFloat & operator = (const RangeFloat &);

                public:
                    explicit RangeFloat(prop::Listener *listener = NULL): tk::RangeFloat(listener) {};

                protected:
                    using               tk::RangeFloat::init;
                    using               tk::RangeFloat::override;

                public:
                    bool                lock_range(bool lock = true);
                    inline bool         unlock_range()                                  { return lock_range(false);     }
                    bool                set_auto_limit(bool enable = true);
                    float               set_min(float v);
                    float               set_max(float v);
                    void                set_range(float min, float max);
                    float               set_all(float v, float min, float max);

                    float               commit(float v);

                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::RangeFloat::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::RangeFloat::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::RangeFloat::bind(property, style, vAtoms, DESC, &sListener); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::RangeFloat::unbind(vAtoms, DESC, &sListener); };

                    /**
                     * Initialize
                     * @param style style
                     * @return status of operation
                     */
                    status_t            init(Style *style, float value, float min, float max);
                    inline status_t     init(Style *style, float value)                 { return init(style, value, 0.0f, 1.0f); }

                    static status_t            init(const char *name, Style *style, float value, float min, float max);
                    static inline status_t     init(const char *name, Style *style, float value)                 { return init(name, style, value, 0.0f, 1.0f); }

                    static status_t            override(const char *name, Style *style, float value, float min, float max);
                    static inline status_t     override(const char *name, Style *style, float value)             { return override(name, style, value, 0.0f, 1.0f); }
            };
        }

    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_PROP_MULTI_RANGEFLOAT_H_ */
