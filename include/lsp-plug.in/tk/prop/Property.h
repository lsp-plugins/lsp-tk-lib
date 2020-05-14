/*
 * Property.h
 *
 *  Created on: 7 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_PROPERTY_H_
#define LSP_PLUG_IN_TK_PROP_PROPERTY_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        namespace prop
        {
            class Listener;
        }

        /**
         * Floating-point property interface
         */
        class Property
        {
            private:
                Property & operator = (const Property *);

            protected:
                Style              *pStyle;
                prop::Listener     *pListener;

            protected:
                explicit inline Property(prop::Listener *listener = NULL)
                {
                    pStyle          = NULL;
                    pListener       = listener;
                }

            protected:
                static const prop::enum_t *find_enum(const LSPString *s, const prop::enum_t *xenum);

                static size_t       parse_ints(ssize_t *dst, size_t max, const LSPString *s);
                static size_t       parse_enums(ssize_t *dst, size_t max, const LSPString *s, const prop::enum_t *xenum);
                static ssize_t      parse_bit_enums(size_t *dst, const LSPString *s, const prop::enum_t *xenum);
                static size_t       parse_unique_enums(ssize_t *dst, size_t max, const LSPString *s, const prop::enum_t *xenum);

                static size_t       parse_enums(lltl::darray<ssize_t> *dst, const LSPString *s, const prop::enum_t *xenum);
                static size_t       parse_unique_enums(lltl::darray<ssize_t> *dst, const LSPString *s, const prop::enum_t *xenum);

            public:
                /**
                 * Check that property matches another property
                 * @param prop pointer to property to check
                 * @return true if property matches
                 */
                inline bool         is(const Property *prop) const { return prop == this; }

                /**
                 * Check that property matches another property
                 * @param prop reference to property to check
                 * @return true if property matches
                 */
                inline bool         is(const Property &prop) const { return &prop == this; }
        };

    }
}


#endif /* LSP_PLUG_IN_TK_PROP_PROPERTY_H_ */
