/*
 * Schema.cpp
 *
 *  Created on: 6 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/style/Schema.h>

namespace lsp
{
    namespace tk
    {
        
        Schema::Schema()
        {
        }
        
        Schema::~Schema()
        {
        }

        status_t Schema::parse_file(const char *path, const char *charset)
        {
            xml::PullParser parser;
            status_t res = parser.open(path, charset);
            if (res == STATUS_OK)
                res = parse_document(&parser);
            if (res == STATUS_OK)
                res = parser.close();
            else
                parser.close();
            return res;
        }

        status_t Schema::parse_file(const LSPString *path, const char *charset)
        {
            xml::PullParser parser;
            status_t res = parser.open(path, charset);
            if (res == STATUS_OK)
                res = parse_document(&parser);
            if (res == STATUS_OK)
                res = parser.close();
            else
                parser.close();
            return res;
        }

        status_t Schema::parse_file(const io::Path *path, const char *charset)
        {
            xml::PullParser parser;
            status_t res = parser.open(path, charset);
            if (res == STATUS_OK)
                res = parse_document(&parser);
            if (res == STATUS_OK)
                res = parser.close();
            else
                parser.close();
            return res;
        }

        status_t Schema::parse_data(io::IInStream *is, size_t flags, const char *charset)
        {
            xml::PullParser parser;
            status_t res = parser.wrap(is, flags, charset);
            if (res == STATUS_OK)
                res = parse_document(&parser);
            if (res == STATUS_OK)
                res = parser.close();
            else
                parser.close();
            return res;
        }

        status_t Schema::parse_data(const char *str, const char *charset)
        {
            xml::PullParser parser;
            status_t res = parser.wrap(str, charset);
            if (res == STATUS_OK)
                res = parse_document(&parser);
            if (res == STATUS_OK)
                res = parser.close();
            else
                parser.close();
            return res;
        }

        status_t Schema::parse_data(const LSPString *str)
        {
            xml::PullParser parser;
            status_t res = parser.wrap(str);
            if (res == STATUS_OK)
                res = parse_document(&parser);
            if (res == STATUS_OK)
                res = parser.close();
            else
                parser.close();
            return res;
        }

        status_t Schema::parse_data(io::IInSequence *seq, size_t flags)
        {
            xml::PullParser parser;
            status_t res = parser.wrap(seq, flags);
            if (res == STATUS_OK)
                res = parse_document(&parser);
            if (res == STATUS_OK)
                res = parser.close();
            else
                parser.close();
            return res;
        }

        status_t Schema::parse_document(xml::PullParser *p)
        {
            status_t res;
            bool read = false;

            while ((res = p->read_next()) > 0)
            {
                switch (res)
                {
                    case xml::XT_START_ELEMENT:
                        if ((read) || (p->name() == NULL) || (!p->name()->equals_ascii("schema")))
                            return STATUS_CORRUPTED;

//                        if ((res = read_schema(p)) != STATUS_OK)
//                            return res;

                        read = true;
                        break;

                    default:
                        continue;
                }
            }

            if (res != -STATUS_EOF)
                return -res;

            return (read) ? STATUS_OK : STATUS_CORRUPTED;
        }
    
    } /* namespace tk */
} /* namespace lsp */
