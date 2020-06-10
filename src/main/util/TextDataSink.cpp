/*
 * TextDataSink.cpp
 *
 *  Created on: 10 июн. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace tk
    {
        static const char * const mimes[] =
        {
            "text/plain;charset=utf-8",
            "UTF8_STRING",
            "text/plain;charset=UTF-16LE",
            "text/plain;charset=UTF-16BE",
            "text/plain;charset=US-ASCII",
            "text/plain",
            NULL
        };

        TextDataSink::TextDataSink()
        {
            nMime   = -1;
            pMime   = NULL;
        }

        TextDataSink::~TextDataSink()
        {
            clear();
        }

        void TextDataSink::clear()
        {
            sOS.drop();
            nMime   = -1;
            pMime   = NULL;
        }

        ssize_t TextDataSink::open(const char * const *mime_types)
        {
            const char *mime = NULL;
            ssize_t found = -1, idx = 0;

            for (const char *const *p = mimes; (*p != NULL) && (found < 0); ++p, ++idx)
            {
                for (const char *const *v = mime_types; (*v != NULL) && (found < 0); ++v)
                {
                    if (!::strcasecmp(*p, *v))
                    {
                        found   = idx;
                        break;
                    }
                }
            }

            if (found < 0)
                return -STATUS_UNSUPPORTED_FORMAT;

            nMime   = found;
            pMime   = mimes[found];
            lsp_trace("Selected mime type: %s, index=%d", pMime, found);
            return STATUS_OK;
        }

        status_t TextDataSink::write(const void *buf, size_t count)
        {
            if (pMime == NULL)
                return STATUS_CLOSED;
            ssize_t written = sOS.write(buf, count);
            return (written >= ssize_t(count)) ? STATUS_OK : STATUS_UNKNOWN_ERR;
        }

        status_t TextDataSink::close(status_t code)
        {
            lsp_trace("code: %x", int(code));
            if (pMime == NULL)
            {
                clear();
                return STATUS_OK;
            }

            // Commit data
            if (code == STATUS_OK)
            {
                LSPString tmp;

                bool ok = false;
                switch (nMime)
                {
                    case 0: // text/plain;charset=utf-8
                    case 1: // UTF8_STRING
                        ok      = tmp.set_utf8(reinterpret_cast<const char *>(sOS.data()), sOS.size());
                        break;
                    case 2: // text/plain;charset=UTF-16LE
                        ok      =
                            __IF_LEBE(
                                tmp.set_utf16(reinterpret_cast<const lsp_utf16_t *>(sOS.data())),
                                tmp.set_native(reinterpret_cast<const char *>(sOS.data()), "UTF16-LE")
                            );
                        break;
                    case 3: // text/plain;charset=UTF-16BE
                        ok      =
                            __IF_LEBE(
                                tmp.set_native(reinterpret_cast<const char *>(sOS.data()), "UTF16-BE"),
                                tmp.set_utf16(reinterpret_cast<const lsp_utf16_t *>(sOS.data()))
                            );
                        break;
                    case 4: // text/plain;charset=US-ASCII
                        ok      = tmp.set_ascii(reinterpret_cast<const char *>(sOS.data()));
                        break;
                    case 5: // text/plain
                        ok      = tmp.set_native(reinterpret_cast<const char *>(sOS.data()), sOS.size());
                        break;
                    default:
                        code    = STATUS_UNSUPPORTED_FORMAT;
                        break;
                }

                // Successful set?
                if (ok)
                    code    = receive(&tmp, pMime);
            }

            // Drop data
            clear();

            return code;
        }

        status_t TextDataSink::receive(const LSPString *text, const char *mime)
        {
            return STATUS_OK;
        }
    }
}


