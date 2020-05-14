/*
 * String.h
 *
 *  Created on: 2 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_STRING_H_
#define LSP_PLUG_IN_TK_PROP_STRING_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/expr/Parameters.h>
#include <lsp-plug.in/i18n/IDictionary.h>

namespace lsp
{
    namespace tk
    {
        class String: public SimpleProperty
        {
            private:
                enum flags_t
                {
                    F_LOCALIZED     = 1 << 0
                };

            protected:
                class Params: public expr::Parameters, public IStyleListener
                {
                    private:
                        String  *pString;

                    public:
                        inline Params(String *ps) { pString = ps; }

                    protected:
                        virtual void        modified();
                        virtual void        notify(atom_t property);
                };

            protected:
                LSPString           sText;      // Text used for rendering
                Params              sParams;    // Parameters
                size_t              nFlags;     // Different flags
                i18n::IDictionary  *pDict;      // Related dictionary

            protected:
                status_t            fmt_internal(LSPString *out, i18n::IDictionary *dict, const LSPString *lang) const;

            protected:
                status_t            bind(atom_t property, Style *style, i18n::IDictionary *dict);
                status_t            bind(const char *property, Style *style, i18n::IDictionary *dict);
                status_t            bind(const LSPString *property, Style *style, i18n::IDictionary *dict);
                status_t            unbind();
                inline void         sync();

            protected:
                explicit String(prop::Listener *listener = NULL);
                ~String();

            public:
                /**
                 * Check wheter the string is localized
                 * @return true if the string is localized
                 */
                inline bool localized() const               { return nFlags & F_LOCALIZED;    }

                /**
                 * Get raw text
                 * @return raw text or NULL if string is localized
                 */
                inline const LSPString *raw() const         { return (nFlags & F_LOCALIZED) ? NULL : &sText;        }

                /**
                 * Get localization key
                 * @return localization key or NULL if string is not localized
                 */
                inline const LSPString *key() const         { return (nFlags & F_LOCALIZED) ? &sText : NULL;        }

                /**
                 * Get formatting parameters for localized string
                 * @return parameters
                 */
                inline const expr::Parameters *params() const { return &sParams; }

                /**
                 * Get formatting parameters for localized string
                 * @return parameters
                 */
                inline expr::Parameters *params()           { return &sParams; }

                /**
                 * Check whether string contains localized data or not
                 * @return true if string contains localized data
                 */
                inline bool is_localized() const            { return nFlags & F_LOCALIZED; }

                /**
                 * Check for emptiness
                 * @return true if string is empty
                 */
                inline bool is_empty() const                { return sText.is_empty(); }

            public:
                /**
                 * Set raw (non-localized) value
                 * @param value value to set
                 * @return status of operation
                 */
                status_t set_raw(const LSPString *value);

                /**
                 * Set raw (non-localized) value
                 * @param value UTF-8 text to set
                 * @return status of operation
                 */
                status_t set_raw(const char *value);

                /**
                 * Set key to the localized string
                 * @param value value to set
                 * @return status of operation
                 */
                status_t set_key(const LSPString *value);

                /**
                 * Set key to the localized string
                 * @param value value to set
                 * @return status of operation
                 */
                status_t set_key(const char *value);

                /**
                 * Set parameters only to the localized string
                 * @param params parameters to set
                 * @return status of operation
                 */
                status_t set_params(const expr::Parameters *params);

                /**
                 * Set localized value
                 * @param key the bundle key to use as a text source
                 * @param params additional optional parameters for message formatting
                 * @return status of operation
                 */
                status_t set(const LSPString *key, const expr::Parameters *params);
                inline status_t set(const LSPString *key)   { return set(key, (expr::Parameters *)NULL); };

                /**
                 * Set localized value
                 * @param key the UTF8 bundle key to use as a text source
                 * @param params additional optional parameters for message formatting
                 * @return status of operation
                 */
                status_t set(const char *key, const expr::Parameters *params);
                inline status_t set(const char *key)        { return set(key, (expr::Parameters *)NULL); };

                /**
                 * Make a copy of data from the source local string to this local string
                 * @param value source string value
                 * @return status of operation
                 */
                status_t set(const String *value);

                /**
                 * Clear the localized string
                 */
                void clear();

                /**
                 * Output the formatted message to the string
                 * @param out output string
                 * @param dict dictionary that stores localization data
                 * @param lang the target language to use
                 * @return status of operation
                 */
                status_t format(LSPString *out, i18n::IDictionary *dict, const char *lang) const;

                /**
                 * Output the formatted message to the string
                 * @param out output string
                 * @param dict dictionary that stores localization data
                 * @param lang the target language to use (UTF-8 encoded)
                 * @return status of operation
                 */
                status_t format(LSPString *out, i18n::IDictionary *dict, const LSPString *lang) const;

                /**
                 * Output the formatted message to the string
                 * @param out output string
                 * @param dpy display to use as dictionary source
                 * @param style the style to take language identifier from
                 * @return status of operation
                 */
                status_t format(LSPString *out, Display *dpy, const Style *style) const;

                /**
                 * Output the formatted message to the string
                 * @param out output string
                 * @param widget LSP widget
                 * @return status of operation
                 */
                status_t format(LSPString *out, Widget *widget) const;

                /**
                 * Format the message using dictionary and style from derived widget
                 * @param out output string
                 * @return status of operation
                 */
                status_t format(LSPString *out) const;

                /**
                 * Swap contents
                 * @param dst destination string to perform swap
                 */
                void swap(String *dst);
        };

        namespace prop
        {
            class String: public tk::String
            {
                private:
                    String & operator = (const String *);

                public:
                    explicit String(prop::Listener *listener = NULL): tk::String(listener) {};

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t id, Style *style, i18n::IDictionary *dict)              { return tk::String::bind(id, style, dict); }
                    inline status_t     bind(const LSPString *id, Style *style, i18n::IDictionary *dict)    { return tk::String::bind(id, style, dict); }
                    inline status_t     bind(const char *id, Style *style, i18n::IDictionary *dict)         { return tk::String::bind(id, style, dict); }
                    inline status_t     bind(Style *style, i18n::IDictionary *dict)                         { return tk::String::bind(LSP_TK_PROP_LANGUAGE, style, dict); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                    { return tk::String::unbind(); };
            };
        }
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_STRING_H_ */
