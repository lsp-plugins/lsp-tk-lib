/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 17 мая 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_ENUM_SCROLLING_H_
#define LSP_PLUG_IN_TK_PROP_ENUM_SCROLLING_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Scrolling: public Enum
        {
            protected:
                static const prop::enum_t ENUM[];

            protected:
                explicit Scrolling(prop::Listener *listener = NULL): Enum(ENUM, SCROLL_NONE, listener) {};
                Scrolling(const Scrolling &) = delete;
                Scrolling(Scrolling &&) = delete;
                Scrolling & operator = (const Scrolling &) = delete;
                Scrolling & operator = (Scrolling &&) = delete;

            public:
                inline scrolling_t          get() const                 { return scrolling_t(nValue);           }
                inline scrolling_t          set(scrolling_t v)          { return scrolling_t(Enum::set(v));     }

                inline bool                 none() const                { return nValue == SCROLL_NONE;         }
                inline bool                 clip() const                { return nValue == SCROLL_CLIP;         }
                inline bool                 optional() const            { return nValue == SCROLL_OPTIONAL;     }
                inline bool                 always() const              { return nValue == SCROLL_ALWAYS;       }


                inline scrolling_t          set_none()                  { return set(SCROLL_NONE);              }
                inline scrolling_t          set_clip()                  { return set(SCROLL_CLIP);              }
                inline scrolling_t          set_optional()              { return set(SCROLL_OPTIONAL);          }
                inline scrolling_t          set_always()                { return set(SCROLL_ALWAYS);            }
        };
    
        namespace prop
        {
            class Scrolling: public tk::Scrolling
            {
                public:
                    explicit Scrolling(prop::Listener *listener = NULL): tk::Scrolling(listener) {};
                    Scrolling(const Scrolling &) = delete;
                    Scrolling(Scrolling &&) = delete;
                    Scrolling & operator = (const Scrolling &) = delete;
                    Scrolling & operator = (Scrolling &&) = delete;

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::Scrolling::bind(property, style); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::Scrolling::bind(property, style); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::Scrolling::bind(property, style); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::Scrolling::unbind(); };

                    inline void         listener(prop::Listener *listener)              { pListener = listener;                     }
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_ENUM_SCROLLING_H_ */
