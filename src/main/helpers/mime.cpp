/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#include <lsp-plug.in/tk/helpers/mime.h>
#include <lsp-plug.in/io/InSequence.h>
#include <lsp-plug.in/io/InStringSequence.h>
#include <lsp-plug.in/io/InMemoryStream.h>

namespace lsp
{
    namespace tk
    {
        status_t fetch_text_uri_list_item(LSPString *dst, const char *protocol, const LSPString *src)
        {
            LSPString tmp;
            io::InStringSequence seq;
            status_t res = seq.wrap(src);
            if (res != STATUS_OK)
                return res;
            res = fetch_text_uri_list_item(&tmp, protocol, &seq);
            if (res != STATUS_OK)
            {
                seq.close();
                return res;
            }
            res = seq.close();
            if (res == STATUS_OK)
                tmp.swap(dst);
            return res;
        }

        status_t fetch_text_uri_list_item(LSPString *dst, const char *protocol, io::IInStream *is, const char *charset)
        {
            LSPString tmp;
            io::InSequence seq;
            status_t res = seq.wrap(is, WRAP_NONE, charset);
            if (res != STATUS_OK)
                return res;
            res = fetch_text_uri_list_item(&tmp, protocol, &seq);
            if (res != STATUS_OK)
            {
                seq.close();
                return res;
            }
            res = seq.close();
            if (res == STATUS_OK)
                tmp.swap(dst);
            return res;
        }

        status_t fetch_text_uri_list_item(LSPString *dst, const char *protocol, const void *src, size_t bytes, const char *charset)
        {
            LSPString tmp;
            io::InMemoryStream is;
            is.wrap(const_cast<void *>(src), bytes);
            status_t res = fetch_text_uri_list_item(&tmp, protocol, &is, charset);
            if (res != STATUS_OK)
            {
                is.close();
                return res;
            }
            res = is.close();
            if (res == STATUS_OK)
                tmp.swap(dst);
            return res;
        }

        status_t fetch_text_uri_list_item(LSPString *dst, const char *protocol, io::IInSequence *is)
        {
            LSPString line;
            while (true)
            {
                status_t res = is->read_line(&line, true);
                if (res == STATUS_EOF)
                    return STATUS_NOT_FOUND;
                if (line.starts_with('#'))
                    continue;
                if (protocol == NULL)
                    break;
                if (line.starts_with_ascii(protocol))
                    break;
            }

            // URL-decode the string
            line.swap(dst);
            return STATUS_OK;
        }

        status_t fetch_text_x_moz_url_item(LSPString *dst, const char *protocol, const LSPString *src)
        {
            LSPString tmp;
            io::InStringSequence seq;
            status_t res = seq.wrap(src);
            if (res != STATUS_OK)
                return res;
            res = fetch_text_x_moz_url_item(&tmp, protocol, &seq);
            if (res != STATUS_OK)
            {
                seq.close();
                return res;
            }
            res = seq.close();
            if (res == STATUS_OK)
                tmp.swap(dst);
            return res;
        }

        status_t fetch_text_x_moz_url_item(LSPString *dst, const char *protocol, io::IInStream *is, const char *charset)
        {
            LSPString tmp;
            io::InSequence seq;
            status_t res = seq.wrap(is, WRAP_NONE, charset);
            if (res != STATUS_OK)
                return res;
            res = fetch_text_x_moz_url_item(&tmp, protocol, &seq);
            if (res != STATUS_OK)
            {
                seq.close();
                return res;
            }
            res = seq.close();
            if (res == STATUS_OK)
                tmp.swap(dst);
            return res;
        }

        status_t fetch_text_x_moz_url_item(LSPString *dst, const char *protocol, const void *src, size_t bytes, const char *charset)
        {
            LSPString tmp;
            io::InMemoryStream is;
            is.wrap(const_cast<void *>(src), bytes);
            status_t res = fetch_text_x_moz_url_item(&tmp, protocol, &is, charset);
            if (res != STATUS_OK)
            {
                is.close();
                return res;
            }
            res = is.close();
            if (res == STATUS_OK)
                tmp.swap(dst);
            return res;
        }

        status_t fetch_text_x_moz_url_item(LSPString *dst, const char *protocol, io::IInSequence *is)
        {
            LSPString line;
            for (size_t index=0; ; ++index)
            {
                status_t res = is->read_line(&line, true);
                if (res == STATUS_EOF)
                    return STATUS_NOT_FOUND;
                if (index & 1) // Skip odd lines
                    continue;
                if (protocol == NULL)
                    break;
                if (line.starts_with_ascii(protocol))
                    break;
            }

            line.swap(dst);
            return STATUS_OK;
        }

        status_t fetch_application_x_kde4_urilist_item(LSPString *dst, const char *protocol, const LSPString *src)
        {
            LSPString tmp;
            io::InStringSequence seq;
            status_t res = seq.wrap(src);
            if (res != STATUS_OK)
                return res;
            res = fetch_application_x_kde4_urilist_item(&tmp, protocol, &seq);
            if (res != STATUS_OK)
            {
                seq.close();
                return res;
            }
            res = seq.close();
            if (res == STATUS_OK)
                tmp.swap(dst);
            return res;
        }

        status_t fetch_application_x_kde4_urilist_item(LSPString *dst, const char *protocol, io::IInStream *is, const char *charset)
        {
            LSPString tmp;
            io::InSequence seq;
            status_t res = seq.wrap(is, WRAP_NONE, charset);
            if (res != STATUS_OK)
                return res;
            res = fetch_application_x_kde4_urilist_item(&tmp, protocol, &seq);
            if (res != STATUS_OK)
            {
                seq.close();
                return res;
            }
            res = seq.close();
            if (res == STATUS_OK)
                tmp.swap(dst);
            return res;
        }

        status_t fetch_application_x_kde4_urilist_item(LSPString *dst, const char *protocol, const void *src, size_t bytes, const char *charset)
        {
            LSPString tmp;
            io::InMemoryStream is;
            is.wrap(const_cast<void *>(src), bytes);
            status_t res = fetch_application_x_kde4_urilist_item(&tmp, protocol, &is, charset);
            if (res != STATUS_OK)
            {
                is.close();
                return res;
            }
            res = is.close();
            if (res == STATUS_OK)
                tmp.swap(dst);
            return res;
        }

        status_t fetch_application_x_kde4_urilist_item(LSPString *dst, const char *protocol, io::IInSequence *is)
        {
            LSPString line;
            while (true)
            {
                status_t res = is->read_line(&line, true);
                if (res == STATUS_EOF)
                    return STATUS_NOT_FOUND;
                if (line.starts_with('#'))
                    continue;
                if (protocol == NULL)
                    break;
                if (line.starts_with_ascii(protocol))
                    break;
            }

            line.swap(dst);
            return STATUS_OK;
        }

        status_t fetch_win_filenamew(LSPString *dst, const char *protocol, const void *src, size_t bytes)
        {
            LSPString line;

            const lsp_utf16_t *str = reinterpret_cast<const lsp_utf16_t *>(src);
            size_t nlen = bytes / sizeof(lsp_utf16_t);
            while ((nlen > 0) && (str[nlen-1] == 0))
                --nlen;

            if (!line.append_ascii("file://"))
                return STATUS_NO_MEM;
            if (!line.append_utf16(str, nlen))
                return STATUS_NO_MEM;
            if (!line.starts_with_ascii(protocol))
                return STATUS_NOT_FOUND;

            line.swap(dst);
            return STATUS_OK;
        }

        status_t fetch_win_filenamea(LSPString *dst, const char *protocol, const void *src, size_t bytes)
        {
            LSPString line;

            const char *str = reinterpret_cast<const char *>(src);
            size_t nlen = bytes / sizeof(char);
            while ((nlen > 0) && (str[nlen-1] == 0))
                --nlen;

            if (!line.append_ascii("file://"))
                return STATUS_NO_MEM;
            if (!line.append_ascii(str, nlen))
                return STATUS_NO_MEM;
            if (!line.starts_with_ascii(protocol))
                return STATUS_NOT_FOUND;

            line.swap(dst);
            return STATUS_OK;
        }
    } /* namespace lsp */
} /* namespace tk */


