/*
 * TextDataSource.h
 *
 *  Created on: 21 авг. 2019 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_UTIL_TEXTDATASOURCE_H_
#define LSP_PLUG_IN_TK_UTIL_TEXTDATASOURCE_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Text data source object for clipboard data
         */
        class TextDataSource: public ws::IDataSource
        {
            protected:
                LSPString   sText;

            public:
                explicit TextDataSource();
                virtual ~TextDataSource();

            public:
                /**
                 * Set UTF-8 encoded text
                 * @param text UTF-8 encoded text
                 * @return status of operation
                 */
                status_t set_text(const char *text);

                /**
                 * Set text from the LSPString object
                 * @param text text
                 * @return status of operation
                 */
                status_t set_text(const LSPString *text);

                /**
                 * Set text from the LSPString object
                 * @param text text
                 * @param first the first character of the source string to use
                 * @return status of operation
                 */
                status_t set_text(const LSPString *text, ssize_t first);

                /**
                 * Set text from the LSPString object
                 * @param text text
                 * @param first the first character to start from
                 * @param last the last character to end
                 * @return status of operation
                 */
                status_t set_text(const LSPString *text, ssize_t first, ssize_t last);

            public:
                virtual io::IInStream   *open(const char *mime);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_UTIL_TEXTDATASOURCE_H_ */
