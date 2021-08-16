/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 10 июн. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_UTIL_TEXTDATASINK_H_
#define LSP_PLUG_IN_TK_UTIL_TEXTDATASINK_H_

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
        class TextDataSink: public ws::IDataSink
        {
            protected:
                io::OutMemoryStream     sOS;
                ssize_t                 nMime;
                const char             *pMime;

            protected:
                virtual status_t        receive(const LSPString *text, const char *mime);
                virtual status_t        error(status_t code);

            public:
                explicit TextDataSink();
                virtual ~TextDataSink();

            public:
                inline const char      *mime() const    { return pMime;     }

            public:
                void                    clear();

                virtual ssize_t         open(const char * const *mime_types);
                virtual status_t        write(const void *buf, size_t count);
                virtual status_t        close(status_t code);
        };
    }
}


#endif /* LSP_PLUG_IN_TK_UTIL_TEXTDATASINK_H_ */
