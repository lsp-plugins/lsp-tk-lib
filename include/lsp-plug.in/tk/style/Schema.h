/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 6 мая 2020 г.
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

#ifndef LSP_PLUG_IN_TK_STYLE_SCHEMA_H_
#define LSP_PLUG_IN_TK_STYLE_SCHEMA_H_

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
        class Atoms;

        class Schema
        {
            private:
                Schema & operator = (const Schema &);

            protected:
                typedef struct style_t
                {
                    Style                       sStyle;
                    lltl::parray<LSPString>     vParents;
                    bool                        bInitialized;

                    style_t(Schema *schema);
                    ~style_t();
                } style_t;

                typedef struct context_t
                {
                    lltl::pphash<LSPString, lsp::Color> vColors;    // Color aliases
                    lltl::pphash<LSPString, style_t>    vStyles;    // Styles
                    style_t                            *pRoot;      // Root style
                } context_t;

                typedef struct property_value_t
                {
                    property_type_t     type;
                    union
                    {
                        bool            bvalue;
                        int             ivalue;
                        float           fvalue;
                    };
                    LSPString           svalue;
                } property_value_t;

            protected:
                context_t                           sCtx; // TODO: remove me

                mutable Atoms                      *pAtoms;
                bool                                bInitialized;
                Style                              *pRoot;
                lltl::pphash<LSPString, Style>      vStyles;

                prop::Float                         sScaling;

            protected:
                status_t            create_style(StyleInitializer *init);

                status_t            apply_context(context_t *ctx);
                static void         init_context(context_t *ctx);
                static void         swap_context(context_t *a, context_t *b);
                static void         destroy_context(context_t *ctx);

                status_t            parse_document(xml::PullParser *p);
                status_t            parse_schema(xml::PullParser *p, context_t *ctx);
                status_t            parse_colors(xml::PullParser *p, context_t *ctx);
                status_t            parse_style(xml::PullParser *p, context_t *ctx, bool root);
                status_t            parse_color(xml::PullParser *p, lsp::Color *color);
                status_t            parse_property(xml::PullParser *p, style_t *style, const LSPString *name);
                static status_t     apply_settings(Style *s, StyleSheet::style_t *xs);
                status_t            apply_relations(Style *s, StyleSheet::style_t *xs);

                static status_t     parse_style_class(LSPString *cname, const LSPString *text);
                static status_t     parse_style_parents(style_t *style, const LSPString *text);
                static status_t     parse_property_type(property_type_t *pt, const LSPString *text);
                static status_t     parse_property_value(property_value_t *v, const LSPString *text, property_type_t pt);

                style_t            *get_style(const LSPString *id);

                void                bind(Style *root);

            public:
                explicit Schema(Atoms *atoms);
                virtual ~Schema();

                /**
                 * Initialize schema with the specified list of styles
                 * Can be run only once after the schema is instantiated. Otherwise
                 * it will return error
                 *
                 * @param list list of styles
                 * @return status of operation
                 */
                status_t            init(lltl::parray<StyleInitializer> *list);
                status_t            init(lltl::parray<StyleInitializer> &list);

                /**
                 * Initialize schema with the specified list of styles
                 * Can be run only once after the schema is instantiated. Otherwise
                 * it will return error
                 *
                 * @param list array of pointers to style initializers
                 * @param n number of elements in array
                 * @return status of operation
                 */
                status_t            init(StyleInitializer **list, size_t n);

                /**
                 * Apply stylesheet settings to the schema
                 * @param sheet style sheet
                 * @return status of operation
                 */
                status_t            apply(StyleSheet *sheet);

            public:
                status_t            parse_file(const char *path, const char *charset = NULL);
                status_t            parse_file(const LSPString *path, const char *charset = NULL);
                status_t            parse_file(const io::Path *path, const char *charset = NULL);

                status_t            parse_data(io::IInStream *is, size_t flags = WRAP_NONE, const char *charset = NULL);
                status_t            parse_data(const char *str, const char *charset = NULL);
                status_t            parse_data(const LSPString *str);
                status_t            parse_data(io::IInSequence *seq, size_t flags = WRAP_NONE);

            public:
                LSP_TK_PROPERTY(Float,          scaling,            &sScaling)

            public:
                /**
                 * Get color by identifier
                 * @param id color identifier
                 * @return color or NULL if not present
                 */
                lsp::Color         *color(const char *id);

                /**
                 * Get color by identifier
                 * @param id color identifier
                 * @return color or NULL if not present
                 */
                lsp::Color         *color(const LSPString *id);

                /**
                 * Get root style
                 * @return root style or NULL on error
                 */
                Style              *root();

                /**
                 * Get style by class identifier.
                 * If style does not exists, it will be automatically created and bound to the root style
                 * @return style or NULL on error
                 */
                Style              *get(const char *id, style_init_t init, void *args = NULL);

                /**
                 * Get style by class identifier.
                 * If style does not exists, it will be automatically created and bound to the root style
                 * @return style or NULL on error
                 */
                Style              *get(const LSPString *id, style_init_t init, void *args = NULL);

                /**
                 * Get style by class identifier.
                 * If style does not exists, it will be automatically created and bound to the root style
                 * @return style or NULL on error
                 */
                Style              *get(const char *id, StyleInitializer *init = NULL);

                /**
                 * Get style by class identifier.
                 * If style does not exists, it will be automatically created and bound to the root style
                 * @return style or NULL on error
                 */
                Style              *get(const LSPString *id, StyleInitializer *init = NULL);

                /**
                 * Get atom identifier by name
                 * @param name atom name
                 * @return atom identifier or negative error code
                 */
                atom_t              atom_id(const char *name) const;

                /**
                 * Get atom identifier by name
                 * @param name atom name
                 * @return atom identifier or negative error code
                 */
                atom_t              atom_id(const LSPString *name) const;

                /**
                 * Get atom name by identifier
                 * @param name atom name or NULL
                 * @return atom identifier
                 */
                const char         *atom_name(atom_t id) const;
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_STYLE_SCHEMA_H_ */
