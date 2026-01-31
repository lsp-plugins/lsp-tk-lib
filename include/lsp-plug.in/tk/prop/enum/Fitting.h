/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 31 янв. 2026 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_ENUM_FITTING_H_
#define LSP_PLUG_IN_TK_PROP_ENUM_FITTING_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Fitting: public Enum
        {
            protected:
                static const prop::enum_t ENUM[];

            protected:
                explicit Fitting(prop::Listener *listener = NULL): Enum(ENUM, FIT_NONE, listener) {};
                Fitting(const Fitting &) = delete;
                Fitting(Fitting &&) = delete;
                Fitting & operator = (const Fitting &) = delete;
                Fitting & operator = (Fitting &&) = delete;

            public:
                inline fitting_t        get() const                 { return fitting_t(nValue); }
                inline fitting_t        set(fitting_t v)            { return fitting_t(Enum::set(v)); }

                inline bool             none() const                { return nValue == FIT_NONE;    }
                inline bool             width() const               { return nValue == FIT_WIDTH;   }
                inline bool             height() const              { return nValue == FIT_HEIGHT;  }
                inline bool             cover() const               { return nValue == FIT_COVER;   }
                inline bool             contain() const             { return nValue == FIT_CONTAIN; }
                inline bool             hstretch() const            { return nValue == FIT_HSTRETCH;}
                inline bool             vstretch() const            { return nValue == FIT_VSTRETCH; }
                inline bool             stretch() const             { return nValue == FIT_STRETCH; }

                inline fitting_t        set_none()                  { return set(FIT_NONE);         }
                inline fitting_t        set_width()                 { return set(FIT_WIDTH);        }
                inline fitting_t        set_height()                { return set(FIT_HEIGHT);       }
                inline fitting_t        set_cover()                 { return set(FIT_COVER);        }
                inline fitting_t        set_contain()               { return set(FIT_CONTAIN);      }
                inline fitting_t        set_hstretch()              { return set(FIT_HSTRETCH);     }
                inline fitting_t        set_vstretch()              { return set(FIT_VSTRETCH);     }
                inline fitting_t        set_stretch()               { return set(FIT_STRETCH);      }
        };

        namespace prop
        {
            class Fitting: public tk::Fitting
            {
                public:
                    explicit Fitting(prop::Listener *listener = NULL): tk::Fitting(listener) {};
                    Fitting(const Fitting &) = delete;
                    Fitting(Fitting &&) = delete;
                    Fitting & operator = (const Fitting &) = delete;
                    Fitting & operator = (Fitting &&) = delete;

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::Fitting::bind(property, style); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::Fitting::bind(property, style); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::Fitting::bind(property, style); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::Fitting::unbind(); };

                    inline void         listener(prop::Listener *listener)              { pListener = listener;                     }
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_PROP_ENUM_FITTING_H_ */
