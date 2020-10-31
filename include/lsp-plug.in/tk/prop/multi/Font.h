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

                    P_COUNT
                };

                class Listener: public IStyleListener
                {
                    private:
                        Font           *pValue;

                    public:
                        inline Listener(Font *font)     { pValue = font;    }

                    public:
                        virtual void    notify(atom_t property);
                };

            protected:
                static const prop::desc_t   DESC[];
                static const prop::enum_t   FLAGS[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                ws::Font            sValue;             // Font parameters
                Listener            sListener;          // Listener
                mutable ws::font_parameters_t   sFP;    // Cached font parameters

            protected:
                void                sync();
                void                commit(atom_t property);
                status_t            init();
                status_t            override();

            protected:
                explicit Font(prop::Listener *listener = NULL);
                ~Font();

            public:
                inline void         set_default()           { MultiProperty::set_default(vAtoms, DESC); }

            public:
                inline const char  *name() const            { return sValue.get_name();                 }
                inline float        size() const            { return sValue.get_size();                 }
                inline bool         bold() const            { return sValue.is_bold();                  }
                inline bool         italic() const          { return sValue.is_italic();                }
                inline bool         underline() const       { return sValue.is_underline();             }
                inline bool         antialiasing() const    { return sValue.is_antialiasing();          }
                inline size_t       flags() const           { return sValue.flags();                    }
                inline void         get(ws::Font *f) const  { f->set(&sValue);                          }
                inline void         get(ws::Font *f, float scaling) const;

                void                set_name(const char *name);
                inline void         set_name(const LSPString *name)     { set_name((name != NULL) ? name->get_utf8() : NULL );      }
                float               set_size(float size);
                bool                set_bold(bool on = true);
                bool                set_italic(bool on = true);
                bool                set_underline(bool on = true);
                bool                set_antialiasing(bool on = true);
                size_t              set_flags(size_t flags);
                void                set(const char *name, size_t size, size_t flags = 0);
                void                set(const ws::Font *f);

            public:
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

                public:
                    explicit Font(prop::Listener *listener = NULL): tk::Font(listener) {};

                protected:
                    using               tk::Font::init;
                    using               tk::Font::override;

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

                    /**
                     * Initialize default values
                     * @return status of operation
                     */
                    status_t            init(Style *style);
                    status_t            init(Style *style, const char *name, float size, size_t flags = 0);
                    status_t            init(Style *style, float size, size_t flags = 0);

                    static status_t     init(const char *name, Style *style);
                    static status_t     init(const char *name, Style *style, const char *fname, float size, size_t flags = 0);
                    static status_t     init(const char *name, Style *style, float size, size_t flags = 0);

                    static status_t     override(const char *name, Style *style);
                    static status_t     override(const char *name, Style *style, const char *fname, float size, size_t flags = 0);
                    static status_t     override(const char *name, Style *style, float size, size_t flags = 0);
            };
        }

    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_PROP_MULTI_FONT_H_ */
