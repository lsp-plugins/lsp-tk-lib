/*
 * LocalString.cpp
 *
 *  Created on: 2 мар. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/prop.h>
#include <lsp-plug.in/tk/sys/Display.h>
#include <lsp-plug.in/tk/base.h>
#include <lsp-plug.in/expr/format.h>

#define LANG_ATOM_NAME      "language"

namespace lsp
{
    namespace tk
    {
        void String::Params::modified()
        {
            pString->sync();
        }

        void String::Listener::notify(atom_t property)
        {
            if (property == pString->nAtom)
                pString->sync();
        }

        String::String():
            sParams(this),
            sListener(this)
        {
            nAtom       = -1;
            pDict       = NULL;
            nFlags      = 0;
        }
        
        String::~String()
        {
            unbind();
        }

        status_t String::bind(prop::Listener *listener, atom_t property, Style *style, i18n::IDictionary *dict)
        {
            if ((style == NULL) || (property < 0) || (dict == NULL))
                return STATUS_BAD_ARGUMENTS;

            // Unbind first
            status_t res;
            if ((pStyle != NULL) && (nAtom >= 0))
            {
                res = pStyle->unbind(nAtom, &sListener);
                if (res != STATUS_OK)
                    return res;
            }

            // Bind to new handler
            style->begin();
            res = style->bind(property, PT_FLOAT, &sListener);
            if (res == STATUS_OK)
            {
                pDict       = dict;
                pStyle      = style;
                pListener   = listener;
                nAtom       = property;
            }
            style->end();

            return res;
        }

        status_t String::unbind()
        {
            if ((pStyle != NULL) && (nAtom >= 0))
            {
                status_t res = pStyle->unbind(nAtom, &sListener);
                if (res != STATUS_OK)
                    return res;
            }

            pDict       = NULL;
            pStyle      = NULL;
            pListener   = NULL;
            nAtom       = -1;

            return STATUS_NOT_BOUND;
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
            nFlags      = /* F_DIRTY | */ F_LOCALIZED;
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
            sParams.clear();
            nFlags      = 0;
            sync();
        }

        status_t String::fmt_internal(LSPString *out, i18n::IDictionary *dict, const LSPString *lang) const
        {
            LSPString path, templ;
            status_t res = STATUS_NOT_FOUND;

            // Search first template in target language if target language specified
            if (lang != NULL)
            {
                if (!path.append(lang))
                    return STATUS_NO_MEM;
                if (!path.append('.'))
                    return STATUS_NO_MEM;
                if (!path.append(&sText))
                    return STATUS_NO_MEM;

                res = dict->lookup(&path, &templ);
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

                res = dict->lookup(&path, &templ);
            }

            // Still no template? Leave
            if (res == STATUS_NOT_FOUND)
                return (out->set(&sText)) ? STATUS_OK : STATUS_NO_MEM; // By default output dictionary key
            else if (res != STATUS_OK)
                return res;

            // Format the template
            return expr::format(out, &templ, &sParams);
        }

        status_t String::format(LSPString *out, i18n::IDictionary *dict, const char *lang) const
        {
            if (out == NULL)
                return STATUS_BAD_ARGUMENTS;

            if (!(nFlags & F_LOCALIZED))
                return (out->set(&sText)) ? STATUS_OK : STATUS_NO_MEM;

            if (dict == NULL)
            {
                out->clear();
                return STATUS_OK;
            }

            LSPString xlang;
            if (!xlang.set_utf8(lang))
                return STATUS_NO_MEM;

            return fmt_internal(out, dict, &xlang);
        }

        status_t String::format(LSPString *out, i18n::IDictionary *dict, const LSPString *lang) const
        {
            if (out == NULL)
                return STATUS_BAD_ARGUMENTS;

            if (!(nFlags & F_LOCALIZED))
                return (out->set(&sText)) ? STATUS_OK : STATUS_NO_MEM;

            if (dict == NULL)
            {
                out->clear();
                return STATUS_OK;
            }

            return fmt_internal(out, dict, lang);
        }

        status_t String::format(LSPString *out, Display *dpy, const Style *style) const
        {
            if ((dpy == NULL) || (style == NULL))
                return format(out, (i18n::IDictionary *)NULL, (const char *)NULL);

            // Get identifier of atom that describes language
            ssize_t atom = dpy->atom_id(LANG_ATOM_NAME);
            if (atom < 0)
                return format(out, (i18n::IDictionary *)NULL, (const char *)NULL);

            // Get language name from style property
            LSPString lang;
            status_t res = style->get_string(atom, &lang);
            if (res != STATUS_OK)
                return format(out, (i18n::IDictionary *)NULL, (const char *)NULL);

            // Perform formatting
            return format(out, dpy->dictionary(), &lang);
        }

        status_t String::format(LSPString *out, Widget *widget) const
        {
            if (widget == NULL)
                return format(out, (i18n::IDictionary *)NULL, (const char *)NULL);
            return format(out, widget->display(), widget->style());
        }

        status_t String::format(LSPString *out) const
        {
            return format(out, pDict, (const char *)NULL);
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
            String::String() :
                tk::String()
            {
            }

            String::~String()
            {
            }

            status_t String::bind(prop::Listener *listener, const char *property, Widget *widget)
            {
                if ((widget == NULL) || (property == NULL))
                    return STATUS_BAD_ARGUMENTS;

                Display *dpy    = widget->display();
                atom_t id       = (dpy != NULL) ? dpy->atom_id(property) : -1;
                if (id < 0)
                    return STATUS_UNKNOWN_ERR;

                return tk::String::bind(listener, id, widget->style(), dpy->dictionary());
            }

            status_t String::bind(prop::Listener *listener, atom_t property, Widget *widget)
            {
                Display *dpy = widget->display();
                if (dpy == NULL)
                    return STATUS_BAD_ARGUMENTS;
                return tk::String::bind(listener, property, widget->style(), dpy->dictionary());
            }

            status_t String::bind(prop::Listener *listener, const char *property, Display *dpy, Style *style)
            {
                if ((dpy == NULL) || (style == NULL) || (property < 0))
                    return STATUS_BAD_ARGUMENTS;

                atom_t id       = dpy->atom_id(property);
                if (id < 0)
                    return STATUS_UNKNOWN_ERR;

                return tk::String::bind(listener, id, style, dpy->dictionary());
            }

            status_t String::bind(prop::Listener *listener, atom_t property, i18n::IDictionary *dict, Style *style)
            {
                return tk::String::bind(listener, property, style, dict);
            }

            status_t String::bind(prop::Listener *listener, Widget *widget)
            {
                return bind(listener, LANG_ATOM_NAME, widget);
            }

            status_t String::bind(prop::Listener *listener, Display *dpy, Style *style)
            {
                return bind(listener, LANG_ATOM_NAME, dpy, style);
            }

            status_t String::unbind()
            {
                return tk::String::unbind();
            }
        }

    } /* namespace tk */
} /* namespace lsp */
