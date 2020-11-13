/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 21 авг. 2019 г.
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
