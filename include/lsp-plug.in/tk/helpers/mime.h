/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 27 авг. 2019 г.
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

#ifndef UI_TK_HELPERS_MIME_H_
#define UI_TK_HELPERS_MIME_H_

#include <lsp-plug.in/runtime/LSPString.h>
#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/io/IInStream.h>
#include <lsp-plug.in/io/IInSequence.h>

namespace lsp
{
    namespace tk
    {
        status_t fetch_text_uri_list_item(LSPString *dst, const char *protocol, const LSPString *src);
        status_t fetch_text_uri_list_item(LSPString *dst, const char *protocol, io::IInStream *is, const char *charset = NULL);
        status_t fetch_text_uri_list_item(LSPString *dst, const char *protocol, io::IInSequence *is);
        status_t fetch_text_uri_list_item(LSPString *dst, const char *protocol, const void *src, size_t bytes, const char *charset = NULL);

        status_t fetch_text_x_moz_url_item(LSPString *dst, const char *protocol, const LSPString *src);
        status_t fetch_text_x_moz_url_item(LSPString *dst, const char *protocol, io::IInStream *is, const char *charset = NULL);
        status_t fetch_text_x_moz_url_item(LSPString *dst, const char *protocol, io::IInSequence *is);
        status_t fetch_text_x_moz_url_item(LSPString *dst, const char *protocol, const void *src, size_t bytes, const char *charset = NULL);

        status_t fetch_application_x_kde4_urilist_item(LSPString *dst, const char *protocol, const LSPString *src);
        status_t fetch_application_x_kde4_urilist_item(LSPString *dst, const char *protocol, io::IInStream *is, const char *charset = NULL);
        status_t fetch_application_x_kde4_urilist_item(LSPString *dst, const char *protocol, io::IInSequence *is);
        status_t fetch_application_x_kde4_urilist_item(LSPString *dst, const char *protocol, const void *src, size_t bytes, const char *charset = NULL);

        status_t fetch_win_filenamew(LSPString *dst, const char *protocol, const void *src, size_t bytes);
        status_t fetch_win_filenamea(LSPString *dst, const char *protocol, const void *src, size_t bytes);
    }
}



#endif /* UI_TK_HELPERS_MIME_H_ */
