/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 23 окт. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_UTIL_URLSINK_H_
#define LSP_PLUG_IN_TK_UTIL_URLSINK_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/io/OutMemoryStream.h>

namespace lsp
{
    namespace tk
    {
        /**
         * Text data sink
         */
        class URLSink: public ws::IDataSink
        {
            protected:
                static const char * const acceptMime[];

            protected:
                enum ctype_t
                {
                    TEXT_URI_LIST,
                    TEXT_X_MOZ_URL,
                    APPLICATION_X_KDE4_URILIST,
                    TEXT_PLAIN,
                    APPLICATION_X_WINDOWS_FILENAMEW,
                    APPLICATION_X_WINDOWS_FILENAME,
                };

            protected:
                io::OutMemoryStream    *pOS;
                char                   *sProtocol;
                ssize_t                 nCtype;

            protected:
                ssize_t                 get_mime_index(const char *mime);

            public:
                explicit URLSink();
                explicit URLSink(const char *protocol);
                virtual ~URLSink();

            public:
                ssize_t                 select_mime_type(const char * const *mime_types);
                inline const char      *get_protocol() const { return sProtocol; }
                virtual status_t        commit_url(const LSPString *url);
                status_t                set_protocol(const char *protocol);

            public:
                void                    clear();

                virtual ssize_t         open(const char * const *mime_types);
                virtual status_t        write(const void *buf, size_t count);
                virtual status_t        close(status_t code);

        };
    }
}



#endif /* LSP_PLUG_IN_TK_UTIL_URLSINK_H_ */
