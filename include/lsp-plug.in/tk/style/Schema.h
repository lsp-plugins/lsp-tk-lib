/*
 * Schema.h
 *
 *  Created on: 6 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_STYLE_SCHEMA_H_
#define LSP_PLUG_IN_TK_STYLE_SCHEMA_H_

#include <lsp-plug.in/tk/version.h>
#include <lsp-plug.in/tk/types.h>
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
        class Style;

        class Schema
        {
            protected:
                lltl::pphash<LSPString, Color>  vAlias;     // Color aliases
                lltl::pphash<LSPString, Style>  vStyles;    // Styles

            protected:
                status_t            parse_document(xml::PullParser *p);

            public:
                explicit Schema();
                virtual ~Schema();

            public:
                status_t            parse_file(const char *path, const char *charset = NULL);
                status_t            parse_file(const LSPString *path, const char *charset = NULL);
                status_t            parse_file(const io::Path *path, const char *charset = NULL);

                status_t            parse_data(io::IInStream *is, size_t flags = WRAP_NONE, const char *charset = NULL);
                status_t            parse_data(const char *str, const char *charset = NULL);
                status_t            parse_data(const LSPString *str);
                status_t            parse_data(io::IInSequence *seq, size_t flags = WRAP_NONE);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_STYLE_SCHEMA_H_ */
