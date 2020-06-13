/*
 * String.cpp
 *
 *  Created on: 2 мар. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/expr/format.h>

namespace lsp
{
    namespace tk
    {
        void String::Params::modified()
        {
            pString->sCache.truncate();
            pString->nFlags &= ~F_MATCHING;

            pString->sync();
        }

        void String::Params::notify(atom_t property)
        {
            pString->commit(property);
        }

        String::String(prop::Listener *listener):
            SimpleProperty(listener),
            sParams(this)
        {
            pDict       = NULL;
            nFlags      = 0;
        }
        
        String::~String()
        {
            SimpleProperty::unbind(&sParams);
        }

        status_t String::bind(atom_t property, Style *style, i18n::IDictionary *dict)
        {
            if ((style == NULL) || (property < 0) || (dict == NULL))
                return STATUS_BAD_ARGUMENTS;

            // Unbind first
            status_t res;
            if ((pStyle != NULL) && (nAtom >= 0))
            {
                res = pStyle->unbind(nAtom, &sParams);
                if (res != STATUS_OK)
                    return res;
                pStyle      = NULL;
                nAtom       = -1;
            }

            // Bind to new handler
            style->begin();
            {
                res = style->bind(property, PT_STRING, &sParams);
                if (res == STATUS_OK)
                {
                    pDict       = dict;
                    pStyle      = style;
                    nAtom       = property;
                }
            }
            style->end();

            if (pListener != NULL)
                pListener->notify(this);

            return res;
        }

        status_t String::bind(const char *property, Style *style, i18n::IDictionary *dict)
        {
            if ((style == NULL) || (property == NULL))
                return STATUS_BAD_ARGUMENTS;
            atom_t atom = style->atom_id(property);
            return (atom >= 0) ? bind(atom, style, dict) : STATUS_UNKNOWN_ERR;
        }

        status_t String::bind(const LSPString *property, Style *style, i18n::IDictionary *dict)
        {
            if ((style == NULL) || (property == NULL))
                return STATUS_BAD_ARGUMENTS;
            atom_t atom = style->atom_id(property);
            return (atom >= 0) ? bind(atom, style, dict) : STATUS_UNKNOWN_ERR;
        }

        status_t String::unbind()
        {
            status_t res = SimpleProperty::unbind(&sParams);
            if (res == STATUS_OK)
                pDict       = NULL;

            return res;
        }

        void String::commit(atom_t property)
        {
            if (pStyle != NULL)
            {
                const char *lang;
                if ((property == nAtom) && (pStyle->get_string(property, &lang) == STATUS_OK))
                {
                    sCache.truncate();
                    nFlags &= ~F_MATCHING;
                }
            }

            if (pListener != NULL)
                pListener->notify(this);
        }

        void String::sync()
        {
            if (pListener != NULL)
                pListener->notify(this);
        }

        status_t String::set_raw(const LSPString *value)
        {
            if (value == NULL)
                sText.truncate();
            else if (!sText.set(value))
                return STATUS_NO_MEM;

            nFlags      = 0;
            sCache.truncate();
            sParams.clear();

            sync();
            return STATUS_OK;
        }

        status_t String::set_raw(const char *value)
        {
            if (value == NULL)
                sText.truncate();
            else if (!sText.set_utf8(value))
                return STATUS_NO_MEM;

            nFlags      = 0;
            sCache.truncate();
            sParams.clear();

            sync();
            return STATUS_OK;
        }

        status_t String::set_params(const expr::Parameters *params)
        {
            if (params != NULL)
                return sParams.set(params); // will call sync();

            sParams.clear(); // will call sync();
            return STATUS_OK;
        }

        status_t String::set(const LSPString *key, const expr::Parameters *params)
        {
            if (key == NULL)
            {
                clear();
                return STATUS_OK;
            }

            LSPString ts;
            expr::Parameters tp;

            // Create copies
            if (!ts.set(key))
                return STATUS_NO_MEM;
            if (params != NULL)
            {
                status_t res = tp.set(params);
                if (res != STATUS_OK)
                    return res;
            }
            else
                tp.clear();

            // Apply
            nFlags      = F_LOCALIZED;
            sText.swap(&ts);
            sParams.swap(&tp); // will call sync()

            return STATUS_OK;
        }

        status_t String::set_key(const char *key)
        {
            if (key == NULL)
            {
                sText.clear();
                sync();
                return STATUS_OK;
            }
            else if (!sText.set_utf8(key))
                return STATUS_NO_MEM;

            // Apply
            nFlags      = F_LOCALIZED;
            sync();
            return STATUS_OK;
        }

        status_t String::set_key(const LSPString *key)
        {
            if (key == NULL)
            {
                sText.clear();
                sync();
                return STATUS_OK;
            }
            else if (!sText.set(key))
                return STATUS_NO_MEM;

            // Apply
            nFlags      = F_LOCALIZED;
            sync();
            return STATUS_OK;
        }

        status_t String::set(const char *key, const expr::Parameters *params)
        {
            if (key == NULL)
            {
                clear();
                return STATUS_OK;
            }

            LSPString ts;
            expr::Parameters tp;

            // Create copies
            if (!ts.set_utf8(key))
                return STATUS_NO_MEM;
            if (params != NULL)
            {
                status_t res = tp.set(params);
                if (res != STATUS_OK)
                    return res;
            }
            else
                tp.clear();

            // Apply
            nFlags      = F_LOCALIZED;
            sText.swap(&ts);
            sParams.swap(&tp); // Will call sync()

            return STATUS_OK;
        }

        status_t String::set(const String *value)
        {
            if (value == NULL)
            {
                clear();
                return STATUS_OK;
            }

            LSPString ts;
            expr::Parameters tp;

            // Create copies
            if (!ts.set(&value->sText))
                return STATUS_NO_MEM;
            status_t res = tp.set(&value->sParams);
            if (res != STATUS_OK)
                return res;

            // Apply
            nFlags      = value->nFlags;
            sText.swap(&ts);
            sParams.swap(&tp); // Will call sync()

            return STATUS_OK;
        }

        void String::clear()
        {
            sText.truncate();
            sCache.truncate();
            sParams.clear();
            nFlags      = 0;

            sync();
        }

        status_t String::lookup_template(LSPString *templ, const LSPString *lang) const
        {
            if (pDict == NULL)
                return STATUS_NOT_FOUND;

            LSPString path;

            status_t res = STATUS_NOT_FOUND;
            if (lang != NULL)
            {
                if (!path.append(lang))
                    return STATUS_NO_MEM;
                if (!path.append('.'))
                    return STATUS_NO_MEM;
                if (!path.append(&sText))
                    return STATUS_NO_MEM;

                res = pDict->lookup(&path, templ);
            }

            // Now search in default language
            if (res == STATUS_NOT_FOUND)
            {
                path.clear();
                if (!path.append_ascii("default"))
                    return STATUS_NO_MEM;
                if (!path.append('.'))
                    return STATUS_NO_MEM;
                if (!path.append(&sText))
                    return STATUS_NO_MEM;

                res = pDict->lookup(&path, templ);
            }

            return res;
        }

        status_t String::fmt_internal(LSPString *out, const LSPString *lang) const
        {
            // Check that string is not localized
            if (!(nFlags & F_LOCALIZED))
            {
                sCache.truncate();
                return (out->set(&sText)) ? STATUS_OK : STATUS_NO_MEM;
            }

            // Check that value has been cached
            const char *xlang;
            if (pStyle != NULL)
                pStyle->get_string(nAtom, &xlang);

            bool caching = ((lang != NULL) && (xlang != NULL) && (lang->equals_ascii(xlang)));
            if ((caching) && (nFlags & F_MATCHING))
                return (out->set(&sCache)) ? STATUS_OK : STATUS_NO_MEM;

            // Lookup template
            LSPString templ;
            status_t res = lookup_template(&templ, lang);

            // Still no template? Leave
            if (res == STATUS_NOT_FOUND)
                res = (out->set(&sText)) ? STATUS_OK : STATUS_NO_MEM; // By default output dictionary key
            else if (res != STATUS_OK)
                return res;
            else
                res = expr::format(out, &templ, &sParams);

            // Format the template
            if ((res == STATUS_OK) && (caching))
            {
                if (sCache.set(out))
                    nFlags     |= F_MATCHING;
            }
            return res;
        }

        LSPString *String::fmt_for_update()
        {
            // Check that value is not localized
            if (!(nFlags & F_LOCALIZED))
            {
                sCache.truncate();
                return &sText;
            }
            else if (nFlags & F_MATCHING)
                return &sCache;

            // Lookup template
            LSPString templ;
            status_t res;

            if (pStyle != NULL)
            {
                LSPString lang;
                if (pStyle->get_string(nAtom, &lang) == STATUS_OK)
                    res = lookup_template(&templ, &lang);
                else
                    res = lookup_template(&templ, NULL);
            }
            else
                res = lookup_template(&templ, NULL);

            // Still no template? Leave
            if (res == STATUS_NOT_FOUND)
                res = (sCache.set(&sText)) ? STATUS_OK : STATUS_NO_MEM; // By default output dictionary key
            else if (res != STATUS_OK)
                return &sCache;
            else
                res = expr::format(&sCache, &templ, &sParams);

            // Format the template
            if (res == STATUS_OK)
                nFlags     |= F_MATCHING;

            return &sCache;
        }

        status_t String::format(LSPString *out, const char *lang) const
        {
            if (out == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (lang == NULL)
                return fmt_internal(out, NULL);

            LSPString tlang;
            if (!tlang.set_ascii(lang))
                return STATUS_NO_MEM;
            return fmt_internal(out, &tlang);
        }

        status_t String::format(LSPString *out, const LSPString *lang) const
        {
            if (out == NULL)
                return STATUS_BAD_ARGUMENTS;
            return fmt_internal(out, lang);
        }

        status_t String::format(LSPString *out) const
        {
            if (out == NULL)
                return STATUS_BAD_ARGUMENTS;
            return fmt_internal(out, NULL);
        }

        void String::swap(String *dst)
        {
            if (this == dst)
                return;

            lsp::swap(nFlags, dst->nFlags);
            sText.swap(&dst->sText);
            sParams.swap(&dst->sParams); // will call sync()
        }

        namespace prop
        {
            bool String::invalidate()
            {
                // Invalidate cache to text and reset LOCALIZED flag
                if (nFlags & F_MATCHING)
                {
                    sText.swap(&sCache);
                    sCache.truncate();
                    nFlags  = 0;
                }
                else if (nFlags & F_LOCALIZED) // Make localized string as non-localized
                {
                    // Format value into sCache
                    if (fmt_for_update() == NULL)
                        return false;

                    sText.swap(&sCache);
                    sCache.truncate();
                    nFlags  = 0;
                }

                // This is raw string, just return
                sync();
                return true;
            }
        }

    } /* namespace tk */
} /* namespace lsp */
