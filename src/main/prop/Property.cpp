/*
 * Property.cpp
 *
 *  Created on: 8 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/prop.h>
#include <lsp-plug.in/io/InStringSequence.h>
#include <lsp-plug.in/expr/Tokenizer.h>

namespace lsp
{
    namespace tk
    {

        size_t Property::parse_ints(ssize_t *dst, size_t max, const LSPString *s)
        {
            // Wrap string with sequence
            io::InStringSequence is;
            if (is.wrap(s) != STATUS_OK)
                return 0;

            // Parse values
            size_t n = 0;
            expr::Tokenizer tok(&is);
            status_t res = STATUS_OK;

            while ((res = tok.get_token()) != STATUS_EOF)
            {
                if ((tok.current() != expr::TT_IVALUE) || (n >= max))
                    return 0;
                dst[n++] = tok.int_value();
            }

            return n;
        }

        ssize_t Property::find_enum(ssize_t *dst, const LSPString *s, const prop::enum_t *xenum)
        {
            for (size_t i=0; (xenum != NULL) && (xenum->name != NULL); ++i, ++xenum)
            {
                if (s->equals_ascii_nocase(xenum->name))
                {
                    if (dst != NULL)
                        *dst = xenum->value;
                    return i;
                }
            }
            return -1;
        }

        size_t Property::parse_enums(ssize_t *dst, size_t max, const LSPString *s, const prop::enum_t *xenum)
        {
            // Wrap string with sequence
            io::InStringSequence is;
            if (is.wrap(s) != STATUS_OK)
                return 0;

            // Parse values
            size_t n=0;
            expr::Tokenizer tok(&is);
            status_t res = STATUS_OK;

            while ((res = tok.get_token()) != STATUS_EOF)
            {
                if (n > 0)
                {
                    if (tok.current() != expr::TT_SEMICOLON)
                        return 0;
                    if ((res = tok.get_token()) == STATUS_EOF)
                        return 0;
                }

                if ((tok.current() != expr::TT_BAREWORD) || (n >= max))
                    return 0;

                if (find_enum(&dst[n++], tok.text_value(), xenum) < 0)
                    return 0;
            }

            return n;
        }

        size_t Property::parse_unique_enums(ssize_t *dst, size_t max, const LSPString *s, const prop::enum_t *xenum)
        {
            // Wrap string with sequence
            io::InStringSequence is;
            if (is.wrap(s) != STATUS_OK)
                return 0;

            // Parse values
            size_t n=0;
            bool semicolon = false, matched;
            expr::Tokenizer tok(&is);
            status_t res = STATUS_OK;

            while ((res = tok.get_token()) != STATUS_EOF)
            {
                if (semicolon)
                {
                    if (tok.current() != expr::TT_SEMICOLON)
                        return 0;
                    if ((res = tok.get_token()) == STATUS_EOF)
                        return 0;
                }

                if ((tok.current() != expr::TT_BAREWORD) || (n >= max))
                    return 0;

                if (find_enum(&dst[n], tok.text_value(), xenum) < 0)
                    return 0;

                // Add unique value
                matched = false;
                for (size_t i=0; i<n; ++i)
                    if (dst[i] == dst[n])
                    {
                        matched = true;
                        break;
                    }
                if (!matched)
                    ++n;

                semicolon = true;
            }

            return n;
        }

        size_t Property::parse_enums(lltl::darray<ssize_t> *dst, const LSPString *s, const prop::enum_t *xenum)
        {
            // Wrap string with sequence
            io::InStringSequence is;
            if (is.wrap(s) != STATUS_OK)
                return 0;

            // Parse values
            ssize_t v;
            expr::Tokenizer tok(&is);
            status_t res = STATUS_OK;

            while ((res = tok.get_token()) != STATUS_EOF)
            {
                if (dst->size() > 0)
                {
                    if (tok.current() != expr::TT_SEMICOLON)
                        return 0;
                    if ((res = tok.get_token()) == STATUS_EOF)
                        return 0;
                }

                if (tok.current() != expr::TT_BAREWORD)
                    return 0;

                if (find_enum(&v, tok.text_value(), xenum) < 0)
                    return 0;

                if (!dst->add(&v))
                    return 0;
            }

            return dst->size();
        }

        size_t Property::parse_unique_enums(lltl::darray<ssize_t> *dst, const LSPString *s, const prop::enum_t *xenum)
        {
            // Wrap string with sequence
            io::InStringSequence is;
            if (is.wrap(s) != STATUS_OK)
                return 0;

            // Parse values
            size_t n=0;
            ssize_t v;
            bool semicolon = false, matched;
            expr::Tokenizer tok(&is);
            status_t res = STATUS_OK;

            while ((res = tok.get_token()) != STATUS_EOF)
            {
                if (semicolon)
                {
                    if (tok.current() != expr::TT_SEMICOLON)
                        return 0;
                    if ((res = tok.get_token()) == STATUS_EOF)
                        return 0;
                }

                if (tok.current() != expr::TT_BAREWORD)
                    return 0;

                if (find_enum(&v, tok.text_value(), xenum) < 0)
                    return 0;

                // Add unique value
                matched = false;
                for (size_t i=0; i<n; ++i)
                    if (*(dst->uget(i)) == v)
                    {
                        matched = true;
                        break;
                    }

                if (!matched)
                {
                    if (!dst->add(&v))
                        return 0;
                }

                semicolon = true;
            }

            return dst->size();
        }
    }
}
