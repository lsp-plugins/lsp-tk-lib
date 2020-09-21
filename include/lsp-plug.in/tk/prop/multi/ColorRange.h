/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 21 сент. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_MULTI_COLORRANGE_H_
#define LSP_PLUG_IN_TK_PROP_MULTI_COLORRANGE_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/expr/Tokenizer.h>

namespace lsp
{
    namespace tk
    {
        class ColorRange: public MultiProperty
        {
            private:
                ColorRange & operator = (const ColorRange &);

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
                    F_LOCK_EVENT    = 1 << 0,
                    F_CHANGED       = 1 << 1
                };

                class ColorListener: public prop::Listener
                {
                    protected:
                        ColorRange     *pValue;

                    public:
                        explicit inline ColorListener(ColorRange *ptr)  { pValue   = ptr;       }

                    public:
                        virtual void    notify(Property *prop);
                };

                class Listener: public IStyleListener
                {
                    protected:
                        ColorRange      *pValue;

                    public:
                        inline Listener(ColorRange *ptr)                { pValue = ptr;         }

                    public:
                        virtual void    notify(atom_t property);
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t                  vAtoms[P_COUNT];    // Atom bindings
                prop::Color             sColor;             // Actual color
                float                   fMin;               // Minimum value
                float                   fMax;               // Maximum value
                size_t                  nFlags;             // Flags
                ColorListener           sColorListener;     // Listener
                Listener                sListener;

            protected:
                void                    sync();
                void                    commit(atom_t property);
                void                    color_changed();
                status_t                do_parse(expr::Tokenizer *t);

            public:
                explicit ColorRange(prop::Listener *listener = NULL);
                ~ColorRange();

            public:
                inline Color           *color()             { return &sColor;       }
                inline const Color     *color() const       { return &sColor;       }
                inline float            min() const         { return fMin;          }
                inline float            max() const         { return fMax;          }
                inline float            range() const       { return fMax - fMin;           }
                inline float            abs_range() const   { return (fMax > fMin) ? fMax - fMin : fMin - fMax; }

                float                   set_min(float v);
                float                   set_max(float v);
                void                    set_range(float min, float max);
                status_t                set(const char *s);
                status_t                set(const LSPString *s);

                static inline float     limit(float v, float min, float max)        { return Property::limit(v, min, max);      }
                inline float            limit(float v) const                        { return Property::limit(v, fMin, fMax);    }
                static inline bool      matches(float v, float min, float max)      { return Property::matches(v, min, max);    }
                inline bool             matches(float v) const                      { return Property::matches(v, fMin, fMax);  }
        };
    }
}



#endif /* LSP_PLUG_IN_TK_PROP_MULTI_COLORRANGE_H_ */
