/*
 * Schema.h
 *
 *  Created on: 6 мая 2020 г.
 *      Author: sadko
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
                context_t           sCtx;
                mutable Atoms      *pAtoms;

            protected:
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

                static status_t     parse_style_class(LSPString *cname, const LSPString *text);
                static status_t     parse_style_parents(style_t *style, const LSPString *text);
                static status_t     parse_property_type(property_type_t *pt, const LSPString *text);
                static status_t     parse_property_value(property_value_t *v, const LSPString *text, property_type_t pt);

                style_t            *get_style(const LSPString *id);

            public:
                explicit Schema(Atoms *atoms);
                virtual ~Schema();

            public:
                status_t            parse_file(const char *path, const char *charset = NULL);
                status_t            parse_file(const LSPString *path, const char *charset = NULL);
                status_t            parse_file(const io::Path *path, const char *charset = NULL);

                status_t            parse_data(io::IInStream *is, size_t flags = WRAP_NONE, const char *charset = NULL);
                status_t            parse_data(const char *str, const char *charset = NULL);
                status_t            parse_data(const LSPString *str);
                status_t            parse_data(io::IInSequence *seq, size_t flags = WRAP_NONE);

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
                Style              *get(const LSPString *id, style_init_t init, void *args = NULL);\

                /**
                 * Get style by class identifier.
                 * If style does not exists, it will be automatically created and bound to the root style
                 * @return style or NULL on error
                 */
                Style              *get(const char *id, IStyleInitializer *init = NULL);

                /**
                 * Get style by class identifier.
                 * If style does not exists, it will be automatically created and bound to the root style
                 * @return style or NULL on error
                 */
                Style              *get(const LSPString *id, IStyleInitializer *init = NULL);

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
