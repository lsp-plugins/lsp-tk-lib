/*
 * TextDataSink.h
 *
 *  Created on: 10 июн. 2020 г.
 *      Author: sadko
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
