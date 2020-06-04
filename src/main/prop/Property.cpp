/*
 * Property.cpp
 *
 *  Created on: 8 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/io/InStringSequence.h>
#include <lsp-plug.in/expr/Tokenizer.h>

namespace lsp
{
    namespace tk
    {
        size_t Property::parse_ints(ssize_t *dst, size_t max, const LSPString *s)
        {
            // Wrap string with sequence
            size_t n = 0;
            io::InStringSequence is(s);
            expr::Tokenizer tok(&is);
            status_t res = STATUS_OK;

            while ((res = tok.get_token(expr::TF_GET)) != expr::TT_EOF)
            {
                if ((tok.current() != expr::TT_IVALUE) || (n >= max))
                    return 0;
                dst[n++] = tok.int_value();
            }

            return n;
        }

        size_t Property::parse_floats(float *dst, size_t max, const LSPString *s)
        {
            // Wrap string with sequence
            size_t n = 0;
            io::InStringSequence is(s);
            expr::Tokenizer tok(&is);
            status_t res = STATUS_OK;

            while ((res = tok.get_token(expr::TF_GET)) != expr::TT_EOF)
            {
                expr::token_t t = tok.current();
                if (n >= max)
                    return 0;
                if (t == expr::TT_IVALUE)
                    dst[n++] = tok.int_value();
                else if (t == expr::TT_FVALUE)
                    dst[n++] = tok.float_value();
                else
                    return 0;
            }

            return n;
        }

        const prop::enum_t *Property::find_enum(const LSPString *s, const prop::enum_t *xenum)
        {
            for (size_t i=0; (xenum != NULL) && (xenum->name != NULL); ++i, ++xenum)
            {
                if (s->equals_ascii_nocase(xenum->name))
                    return xenum;
            }
            return NULL;
        }

        size_t Property::parse_enums(ssize_t *dst, size_t max, const LSPString *s, const prop::enum_t *xenum)
        {
            // Parse values
            size_t n = 0;
            io::InStringSequence is(s);
            expr::Tokenizer tok(&is);
            status_t res = STATUS_OK;
            const prop::enum_t *xe;

            while ((res = tok.get_token(expr::TF_GET | expr::TF_XKEYWORDS)) != expr::TT_EOF)
            {
                if (n > 0)
                {
                    if (tok.current() != expr::TT_COMMA)
                        return 0;
                    if ((res = tok.get_token(expr::TF_GET | expr::TF_XKEYWORDS)) == expr::TT_EOF)
                        return 0;
                }

                if ((tok.current() != expr::TT_BAREWORD) || (n >= max))
                    return 0;

                if ((xe = find_enum(tok.text_value(), xenum)) == NULL)
                    return 0;

                dst[n++] = xe->value;
            }

            return n;
        }

        ssize_t Property::parse_bit_enums(size_t *dst, const LSPString *s, const prop::enum_t *xenum)
        {
            // Parse values
            size_t n = 0;
            io::InStringSequence is(s);
            expr::Tokenizer tok(&is);
            status_t res = STATUS_OK;
            const prop::enum_t *xe;
            size_t xdst  = 0;

            while ((res = tok.get_token(expr::TF_GET | expr::TF_XKEYWORDS)) != expr::TT_EOF)
            {
                if (n > 0)
                {
                    if (tok.current() != expr::TT_COMMA)
                        return -1;
                    if ((res = tok.get_token(expr::TF_GET | expr::TF_XKEYWORDS)) == expr::TT_EOF)
                        return -1;
                }

                if (tok.current() != expr::TT_BAREWORD)
                    return -1;

                if ((xe = find_enum(tok.text_value(), xenum)) == NULL)
                    return -1;

                xdst   |= xe->value;
                ++n;
            }

            *dst    = xdst;

            return n;
        }

        size_t Property::parse_unique_enums(ssize_t *dst, size_t max, const LSPString *s, const prop::enum_t *xenum)
        {
            // Parse values
            size_t n = 0;
            bool semicolon = false;
            io::InStringSequence is(s);
            expr::Tokenizer tok(&is);
            status_t res = STATUS_OK;
            const prop::enum_t *xe;

            while ((res = tok.get_token(expr::TF_GET | expr::TF_XKEYWORDS)) != expr::TT_EOF)
            {
                if (semicolon)
                {
                    if (tok.current() != expr::TT_COMMA)
                        return 0;
                    if ((res = tok.get_token(expr::TF_GET | expr::TF_XKEYWORDS)) == expr::TT_EOF)
                        return 0;
                }

                if ((tok.current() != expr::TT_BAREWORD) || (n >= max))
                    return 0;

                if ((xe = find_enum(tok.text_value(), xenum)) == NULL)
                    return 0;

                // Add unique value
                for (size_t i=0; i<n; ++i)
                    if (dst[i] == xe->value)
                    {
                        xe = NULL;
                        break;
                    }
                if (xe)
                    dst[n++] = xe->value;

                semicolon = true;
            }

            return n;
        }

        size_t Property::parse_enums(lltl::darray<ssize_t> *dst, const LSPString *s, const prop::enum_t *xenum)
        {
            // Parse values
            io::InStringSequence is(s);
            expr::Tokenizer tok(&is);
            status_t res = STATUS_OK;
            const prop::enum_t *xe;

            while ((res = tok.get_token(expr::TF_GET | expr::TF_XKEYWORDS)) != expr::TT_EOF)
            {
                if (dst->size() > 0)
                {
                    if (tok.current() != expr::TT_COMMA)
                        return 0;
                    if ((res = tok.get_token(expr::TF_GET | expr::TF_XKEYWORDS)) == expr::TT_EOF)
                        return 0;
                }

                if (tok.current() != expr::TT_BAREWORD)
                    return 0;

                if ((xe = find_enum(tok.text_value(), xenum)) == NULL)
                    return 0;

                if (!dst->add(&xe->value))
                    return 0;
            }

            return dst->size();
        }

        size_t Property::parse_unique_enums(lltl::darray<ssize_t> *dst, const LSPString *s, const prop::enum_t *xenum)
        {
            // Parse values
            size_t n = 0;
            bool semicolon = false;
            io::InStringSequence is(s);
            expr::Tokenizer tok(&is);
            status_t res = STATUS_OK;
            const prop::enum_t *xe;

            while ((res = tok.get_token(expr::TF_GET | expr::TF_XKEYWORDS)) != expr::TT_EOF)
            {
                if (semicolon)
                {
                    if (tok.current() != expr::TT_COMMA)
                        return 0;
                    if ((res = tok.get_token(expr::TF_GET | expr::TF_XKEYWORDS)) == expr::TT_EOF)
                        return 0;
                }

                if (tok.current() != expr::TT_BAREWORD)
                    return 0;

                if ((xe = find_enum(tok.text_value(), xenum)) == NULL)
                    return 0;

                // Add unique value
                for (size_t i=0; i<n; ++i)
                    if (*(dst->uget(i)) == xe->value)
                    {
                        xe = NULL;
                        break;
                    }
                if (xe)
                {
                    if (!dst->add(&xe->value))
                        return 0;
                }

                semicolon = true;
            }

            return dst->size();
        }

        bool Property::fmt_bit_enums(LSPString *dst, const prop::enum_t *xe, size_t v)
        {
            bool success = true;
            LSPString tmp;

            for (const prop::enum_t *e = xe; (e != NULL) && (e->name != NULL); ++e)
            {
                if (!(v & e->value))
                    continue;
                if (tmp.length() > 0)
                {
                    if (!(success = tmp.append(',')))
                        break;
                }
                if (!(success = tmp.append_ascii(e->name)))
                    break;
            }

            if (success)
                tmp.swap(dst);

            return success;
        }
    }
}
