/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 1 нояб. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_STYLE_STYLESHEET_H_
#define LSP_PLUG_IN_TK_STYLE_STYLESHEET_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/lltl/parray.h>
#include <lsp-plug.in/lltl/pphash.h>
#include <lsp-plug.in/runtime/LSPString.h>
#include <lsp-plug.in/runtime/Color.h>
#include <lsp-plug.in/io/Path.h>
#include <lsp-plug.in/io/IInStream.h>
#include <lsp-plug.in/io/IInSequence.h>
#include <lsp-plug.in/fmt/xml/PullParser.h>

namespace lsp
{
    namespace tk
    {
        /**
         * Style sheet class
         *
         */
        class StyleSheet
        {
            private:
                StyleSheet & operator = (const StyleSheet &);
                StyleSheet(const StyleSheet &);

                friend class Schema;

            protected:
                typedef struct style_t
                {
                    LSPString                               name;       // Name of style
                    lltl::parray<LSPString>                 parents;    // List of parents
                    lltl::pphash<LSPString, LSPString>      properties; // properties

                    style_t();
                    ~style_t();
                } style_t;

                typedef struct font_t
                {
                    LSPString                               name;       // Name of the font
                    LSPString                               path;       // Path to font
                    bool                                    alias;      // Is an alias
                } font_t;

                typedef struct path_t
                {
                    lltl::parray<style_t>                   visited;
                    style_t                                *curr;
                } path_t;

            protected:
                LSPString                           sTitle;     // Schema title
                style_t                            *pRoot;      // Root style
                lltl::pphash<LSPString, style_t>    vStyles;    // Additional named styles
                lltl::pphash<LSPString, font_t>     vFonts;     // Additional fonts
                lltl::pphash<LSPString, lsp::Color> vColors;    // Color map
                lltl::pphash<LSPString, LSPString>  vConstants; // Global constants
                LSPString                           sError;     // Error text

            public:
                explicit StyleSheet();
                ~StyleSheet();

            protected:
                status_t            parse_document(xml::PullParser *p);
                status_t            parse_schema(xml::PullParser *p);
                status_t            parse_colors(xml::PullParser *p);
                status_t            parse_fonts(xml::PullParser *p);
                status_t            parse_constants(xml::PullParser *p);

                status_t            parse_metadata(xml::PullParser *p);
                status_t            parse_style(xml::PullParser *p, bool root);
                status_t            parse_color(xml::PullParser *p, const LSPString *color_name, lsp::Color *color);
                status_t            parse_constant(xml::PullParser *p, LSPString *value);
                status_t            parse_font(xml::PullParser *p, font_t *font);
                status_t            parse_property(xml::PullParser *p, style_t *style, const LSPString *name);

                status_t            parse_style_class(LSPString *cname, const LSPString *text);
                status_t            parse_string_value(xml::PullParser *p, LSPString *value);
                status_t            parse_style_parents(style_t *style, const LSPString *text);
                status_t            parse_property_type(property_type_t *pt, const LSPString *text);
                status_t            add_parent(style_t *style, const LSPString *text);

                status_t            validate();
                status_t            validate_style(style_t *s);
                static void         drop_paths(lltl::parray<path_t> *paths);

            public:
                status_t            parse_file(const char *path, const char *charset = NULL);
                status_t            parse_file(const LSPString *path, const char *charset = NULL);
                status_t            parse_file(const io::Path *path, const char *charset = NULL);

                status_t            parse_data(io::IInStream *is, size_t flags = WRAP_NONE, const char *charset = NULL);
                status_t            parse_data(const char *str, const char *charset = NULL);
                status_t            parse_data(const LSPString *str);
                status_t            parse_data(io::IInSequence *seq, size_t flags = WRAP_NONE);

            public:
                inline const LSPString *title() const                               { return &sTitle;       }
                status_t            enum_colors(lltl::parray<LSPString> *names) const;
                status_t            enum_styles(lltl::parray<LSPString> *names) const;
                status_t            enum_fonts(lltl::parray<LSPString> *names) const;
                status_t            enum_constants(lltl::parray<LSPString> *names) const;
                status_t            enum_properties(const LSPString *style, lltl::parray<LSPString> *names) const;
                status_t            enum_properties(const char *style, lltl::parray<LSPString> *names) const;
                status_t            enum_parents(const LSPString *style, lltl::parray<LSPString> *names) const;
                status_t            enum_parents(const char *style, lltl::parray<LSPString> *names) const;

                ssize_t             get_property(const LSPString *style, const LSPString *property, LSPString *dst) const;
                ssize_t             get_property(const char *style, const char *property, LSPString *dst) const;
                ssize_t             get_color(const LSPString *color, lsp::Color *dst) const;
                ssize_t             get_color(const char *color, lsp::Color *dst) const;
                ssize_t             get_font(const LSPString *font, LSPString *path, bool *alias) const;
                ssize_t             get_font(const char *font, LSPString *path, bool *alias) const;
                ssize_t             get_constant(const LSPString *name, LSPString *dst) const;
                ssize_t             get_constant(const char *name, LSPString *dst) const;

            public:
                const LSPString    *error() const { return &sError;     }
        };
    }
}



#endif /* LSP_PLUG_IN_TK_STYLE_STYLESHEET_H_ */
