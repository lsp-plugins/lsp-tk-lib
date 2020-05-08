/*
 * MultiProperty.cpp
 *
 *  Created on: 8 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/prop.h>
#include <lsp-plug.in/tk/sys/Display.h>
#include <lsp-plug.in/tk/base.h>
#include <lsp-plug.in/io/InStringSequence.h>
#include <lsp-plug.in/expr/Tokenizer.h>

namespace lsp
{
    namespace tk
    {
        status_t MultiProperty::unbind(atom_t *atoms, const prop::desc_t *desc, IStyleListener *listener)
        {
            if (pStyle == NULL)
                return STATUS_NOT_BOUND;

            // Unbind all atoms
            for ( ; desc->postfix != NULL; ++atoms, ++desc)
            {
                if (*atoms < 0)
                    continue;
                pStyle->unbind(*atoms, listener);
                *atoms      = -1;
            }

            pStyle      = NULL;
            return STATUS_OK;
        }

        status_t MultiProperty::bind(
                atom_t *atoms, const prop::desc_t *desc, IStyleListener *listener,
                const char *property, Style *style, Display *dpy
        )
        {
            if ((style == NULL) || (dpy == NULL) || (property == NULL))
                return STATUS_BAD_ARGUMENTS;

            if (pStyle == style)
                return STATUS_OK;

            // Unbind from previously used style
            unbind(atoms, desc, listener);

            LSPString key;
            if (!key.set_utf8(property))
                return STATUS_NO_MEM;
            size_t len = key.length();

            // Bind all ports
            style->begin();

            status_t res = STATUS_OK;
            for ( ; desc->postfix != NULL; ++atoms, ++desc)
            {
                key.set_length(len);
                if (!key.append_ascii(desc->postfix))
                {
                    res = STATUS_NO_MEM;
                    break;
                }
                atom_t atom = dpy->atom_id(key.get_utf8());
                if (atom < 0)
                {
                    res = STATUS_NO_MEM;
                    break;
                }
                res = style->bind(atom, desc->type, listener);
                if (res != STATUS_OK)
                    break;
                *atoms      = atom;
            }

            if (res == STATUS_OK)
                pStyle      = style;
            else
                unbind(atoms, desc, listener);

            style->end();

            return res;
        }

        void MultiProperty::set_default(atom_t *atoms, const prop::desc_t *desc)
        {
            if (pStyle == NULL)
                return;

            pStyle->begin();

            for ( ; desc->postfix != NULL; ++atoms, ++desc)
            {
                if (*atoms >= 0)
                    pStyle->set_default(*atoms);
            }

            pStyle->end();
        }

        size_t MultiProperty::parse_ints(ssize_t *dst, size_t max, const LSPString *s)
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
    }
}


