/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 2 июн. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_ENUM_DRAWMODE_H_
#define LSP_PLUG_IN_TK_PROP_ENUM_DRAWMODE_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class DrawMode: public Enum
        {
            private:
                DrawMode & operator = (const DrawMode &);
                DrawMode(const DrawMode &);

            protected:
                static const prop::enum_t ENUM[];

            protected:
                explicit DrawMode(prop::Listener *listener = NULL): Enum(ENUM, DM_CLASSIC, listener) {};

            public:
                inline draw_mode_t      get() const                 { return draw_mode_t(nValue); }
                inline draw_mode_t      set(draw_mode_t v)          { return draw_mode_t(Enum::set(v)); }

                inline bool             classic() const             { return nValue == DM_CLASSIC;  }
                inline bool             modern() const              { return nValue == DM_MODERN;   }

                inline draw_mode_t      set_classic()               { return set(DM_CLASSIC);       }
                inline draw_mode_t      set_modern()                { return set(DM_MODERN);        }
        };

        namespace prop
        {
            class DrawMode: public tk::DrawMode
            {
                private:
                    DrawMode & operator = (const DrawMode &);
                    DrawMode(const DrawMode &);

                public:
                    explicit DrawMode(prop::Listener *listener = NULL): tk::DrawMode(listener) {};

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::DrawMode::bind(property, style); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::DrawMode::bind(property, style); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::DrawMode::bind(property, style); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::DrawMode::unbind(); };

                    inline void         listener(prop::Listener *listener)              { pListener = listener;                     }
            };
        }
    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_PROP_ENUM_DRAWMODE_H_ */
