/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/tk/helpers/mime.h>
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace tk
    {
        URLSink::URLSink()
        {
            pOS         = NULL;
            sProtocol   = NULL;
            nCtype      = -1;
        }

        URLSink::URLSink(const char *protocol)
        {
            pOS         = NULL;
            sProtocol   = ::strdup(protocol);
            nCtype      = -1;
        }

        URLSink::~URLSink()
        {
            if (pOS != NULL)
            {
                pOS->close();
                delete pOS;
                pOS     = NULL;
            }
            if (sProtocol != NULL)
            {
                ::free(sProtocol);
                sProtocol = NULL;
            }
        }

        const char * const URLSink::acceptMime[] =
        {
            "text/uri-list",
            "text/x-moz-url",
            "application/x-kde4-urilist",
            "text/plain",
            "application/x-windows-filenamew",
            "application/x-windows-filename",
            NULL
        };

        ssize_t URLSink::get_mime_index(const char *mime)
        {
            ssize_t idx = 0;
            for (const char * const *p = acceptMime; *p != NULL; ++p, ++idx)
            {
                if (!::strcasecmp(*p, mime))
                    return idx;
            }
            return -1;
        }

        ssize_t URLSink::select_mime_type(const char * const *mime_types)
        {
            for (const char * const *p = acceptMime; *p != NULL; ++p)
            {
                ssize_t idx = 0;
                for (const char * const *q = mime_types; *q != NULL; ++q, ++idx)
                {
                    if (!::strcasecmp(*p, *q))
                        return idx;
                }
            }
            return -1;
        }

        ssize_t URLSink::open(const char * const *mime_types)
        {
            if (pOS != NULL)
                return -STATUS_BAD_STATE;

            ssize_t idx = select_mime_type(mime_types);
            if (idx < 0)
                return -STATUS_UNSUPPORTED_FORMAT;
            ssize_t ctype = get_mime_index(mime_types[idx]);
            if (ctype < 0)
                return -STATUS_UNSUPPORTED_FORMAT;

            pOS         = new io::OutMemoryStream();
            if (pOS == NULL)
                return -STATUS_NO_MEM;

            nCtype      = ctype;
            return idx; // Return the index from mime_types array
        }

        status_t URLSink::write(const void *buf, size_t count)
        {
            if (pOS == NULL)
                return STATUS_CLOSED;
            return pOS->write(buf, count);
        }

        status_t URLSink::close(status_t code)
        {
            if (pOS == NULL)
                return STATUS_OK;
            pOS->close();

            // Commit the content
            const uint8_t *raw_data = pOS->data();
            size_t raw_size = pOS->size();

            status_t res = STATUS_NOT_FOUND;
            LSPString data;

            if ((raw_data != NULL) && (raw_size > 0))
            {
                lsp_dumpb("Content dump", raw_data, raw_size);

                switch (nCtype)
                {
                    case TEXT_URI_LIST:
                    case APPLICATION_X_KDE4_URILIST:
                        res = fetch_text_uri_list_item(&data, sProtocol, raw_data, raw_size, "UTF-8");
                        break;
                    case TEXT_X_MOZ_URL:
                        res = fetch_text_uri_list_item(&data, sProtocol, raw_data, raw_size, __IF_LEBE("UTF-16LE", "UTF-16BE"));
                        break;
                    case TEXT_PLAIN:
                        if (data.set_native(reinterpret_cast<const char *>(raw_data), raw_size))
                            res = STATUS_OK;
                        break;
                    case APPLICATION_X_WINDOWS_FILENAMEW:
                        res = fetch_win_filenamew(&data, sProtocol, raw_data, raw_size);
                        break;
                    case APPLICATION_X_WINDOWS_FILENAME:
                        res = fetch_win_filenamea(&data, sProtocol, raw_data, raw_size);
                        break;
                }

                // Remove file:// prefix
                if (data.ends_with('\n'))
                    data.remove_last();
                if (data.ends_with('\r'))
                    data.remove_last();
            }
            else
                lsp_trace("Empty content");

            // Drop allocated data
            pOS->drop();
            delete pOS;
            pOS     = NULL;
            nCtype  = -1;

            // Need to update file name?
            if (res == STATUS_OK)
                res     = commit_url(&data);

            return STATUS_OK;
        }

        status_t URLSink::commit_url(const LSPString *url)
        {
            return STATUS_OK;
        }

        status_t URLSink::set_protocol(const char *protocol)
        {
            if (protocol != NULL)
            {
                char *proto = ::strdup(protocol);
                if (proto == NULL)
                    return STATUS_NO_MEM;
                if (sProtocol != NULL)
                    ::free(sProtocol);
                sProtocol   = proto;
            }
            else if (sProtocol != NULL)
            {
                ::free(sProtocol);
                sProtocol = NULL;
            }
            return STATUS_OK;
        }


    } /* namespace tk */
} /* namespace lsp */


