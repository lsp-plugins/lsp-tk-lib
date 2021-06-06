/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 27 мая 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_MULTI_FONT_H_
#define LSP_PLUG_IN_TK_PROP_MULTI_FONT_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Font property that holds font parameters
         */
        class Font: public MultiProperty
        {
            protected:
                Font & operator = (const Font &);
                Font(const Font &);

            protected:
                enum property_t
                {
                    P_NAME,
                    P_SIZE,
                    P_FLAGS,
                    P_BOLD,
                    P_ITALIC,
                    P_UNDERLINE,
                    P_ANTIALIAS,
                    P_SCALING,

                    P_COUNT
                };

                enum over_flags_t
                {
                    O_NAME      = 1 << P_NAME,
                    O_SIZE      = 1 << P_SIZE,
                    O_FLAGS     = 1 << P_FLAGS,
                    O_BOLD      = 1 << P_BOLD,
                    O_ITALIC    = 1 << P_ITALIC,
                    O_UNDERLINE = 1 << P_UNDERLINE,
                    O_ANTIALIAS = 1 << P_ANTIALIAS,
                    O_SCALING   = 1 << P_SCALING,

                    O_ALL_FLAGS = O_FLAGS | O_BOLD | O_ITALIC | O_UNDERLINE | O_ANTIALIAS | O_SCALING,
                    O_ALL       = O_ALL_FLAGS | O_NAME | O_SIZE
                };

            protected:
                static const prop::desc_t   DESC[];
                static const prop::enum_t   FLAGS[];
                static const prop::enum_t   ANTIALIAS[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                ws::Font            sValue;             // Font parameters
                size_t              nOverride;          // Override flags
                mutable ws::font_parameters_t   sFP;    // Cached font parameters

            protected:
                void                push_masked(size_t mask);
                virtual void        push();
                virtual void        commit(atom_t property);

            protected:
                explicit Font(prop::Listener *listener = NULL);
                virtual ~Font();

            public:
                inline void         set_default()           { MultiProperty::set_default(vAtoms, DESC); }

            public:
                inline const char          *name() const            { return sValue.get_name();                 }
                inline float                size() const            { return sValue.get_size();                 }
                inline bool                 bold() const            { return sValue.bold();                     }
                inline bool                 italic() const          { return sValue.italic();                   }
                inline bool                 underline() const       { return sValue.underline();                }
                inline ws::font_antialias_t antialiasing() const    { return sValue.antialiasing();             }
                inline ws::font_antialias_t antialias() const       { return sValue.antialiasing();             }
                inline size_t               flags() const           { return sValue.flags();                    }
                inline void                 get(ws::Font *f) const  { f->set(&sValue);                          }
                inline void                 get(ws::Font *f, float scaling) const;

                void                        set_name(const char *name);
                inline void                 set_name(const LSPString *name)     { set_name((name != NULL) ? name->get_utf8() : NULL );      }
                float                       set_size(float size);
                bool                        set_bold(bool on = true);
                bool                        set_italic(bool on = true);
                bool                        set_underline(bool on = true);
                ws::font_antialias_t        set_antialiasing(ws::font_antialias_t value = ws::FA_ENABLED);
                ws::font_antialias_t        set_antialiasing(const char *value);
                ws::font_antialias_t        set_antialiasing(const LSPString *value);
                ws::font_antialias_t        set_antialias(ws::font_antialias_t value = ws::FA_ENABLED);
                ws::font_antialias_t        set_antialias(const char *value);
                ws::font_antialias_t        set_antialias(const LSPString *value);

                size_t                      set_flags(size_t flags);
                void                        set(const char *name, size_t size, size_t flags = 0);
                void                        set_params(size_t size, size_t flags = 0);
                void                        set(const ws::Font *f);

            public:
                virtual void                override();

                bool get_parameters(ws::ISurface *s, float scaling, ws::font_parameters_t *fp) const;
                bool get_parameters(Display *dpy, float scaling, ws::font_parameters_t *fp) const;

                bool get_multitext_parameters(ws::ISurface *s, ws::text_parameters_t *tp, float scaling, const LSPString *text) const;
                bool get_multitext_parameters(ws::ISurface *s, ws::text_parameters_t *tp, float scaling, const LSPString *text, ssize_t first) const;
                bool get_multitext_parameters(ws::ISurface *s, ws::text_parameters_t *tp, float scaling, const LSPString *text, ssize_t first, ssize_t last) const;

                bool get_multitext_parameters(Display *dpy, ws::text_parameters_t *tp, float scaling, const LSPString *text) const;
                bool get_multitext_parameters(Display *dpy, ws::text_parameters_t *tp, float scaling, const LSPString *text, ssize_t first) const;
                bool get_multitext_parameters(Display *dpy, ws::text_parameters_t *tp, float scaling, const LSPString *text, ssize_t first, ssize_t last) const;

                bool get_text_parameters(ws::ISurface *s, ws::text_parameters_t *tp, float scaling, const LSPString *text) const;
                bool get_text_parameters(ws::ISurface *s, ws::text_parameters_t *tp, float scaling, const LSPString *text, ssize_t first) const;
                bool get_text_parameters(ws::ISurface *s, ws::text_parameters_t *tp, float scaling, const LSPString *text, ssize_t first, ssize_t last) const;

                bool get_text_parameters(ws::ISurface *s, ws::text_parameters_t *tp, float scaling, const char *text) const;
                bool get_text_parameters(ws::ISurface *s, ws::text_parameters_t *tp, float scaling, const char *text, ssize_t first) const;
                bool get_text_parameters(ws::ISurface *s, ws::text_parameters_t *tp, float scaling, const char *text, ssize_t first, ssize_t last) const;

                bool get_text_parameters(Display *dpy, ws::text_parameters_t *tp, float scaling, const LSPString *text) const;
                bool get_text_parameters(Display *dpy, ws::text_parameters_t *tp, float scaling, const LSPString *text, ssize_t first) const;
                bool get_text_parameters(Display *dpy, ws::text_parameters_t *tp, float scaling, const LSPString *text, ssize_t first, ssize_t last) const;

                bool get_text_parameters(Display *dpy, ws::text_parameters_t *tp, float scaling, const char *text) const;
                bool get_text_parameters(Display *dpy, ws::text_parameters_t *tp, float scaling, const char *text, ssize_t first) const;
                bool get_text_parameters(Display *dpy, ws::text_parameters_t *tp, float scaling, const char *text, ssize_t first, ssize_t last) const;

                void draw(ws::ISurface *s, const lsp::Color &c, float x, float y, float scaling, const LSPString *text) const;
                void draw(ws::ISurface *s, const lsp::Color &c, float x, float y, float scaling, const LSPString *text, size_t first) const;
                void draw(ws::ISurface *s, const lsp::Color &c, float x, float y, float scaling, const LSPString *text, size_t first, size_t last) const;
        };

        namespace prop
        {
            class Font: public tk::Font
            {
                private:
                    Font & operator = (const Font &);
                    Font(const Font &);

                public:
                    explicit Font(prop::Listener *listener = NULL): tk::Font(listener) {};

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::Font::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::Font::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::Font::bind(property, style, vAtoms, DESC, &sListener); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::Font::unbind(vAtoms, DESC, &sListener); };
            };
        }

    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_PROP_MULTI_FONT_H_ */
