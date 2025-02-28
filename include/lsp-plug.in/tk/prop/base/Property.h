/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 7 мая 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_BASE_PROPERTY_H_
#define LSP_PLUG_IN_TK_PROP_BASE_PROPERTY_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/common/variadic.h>

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
            protected:
                class Listener: public IStyleListener
                {
                    private:
                        Property   *pProperty;
                        ssize_t     nLocks;

                    public:
                        explicit Listener(Property *p);

                    public:
                        virtual void    notify(atom_t property);

                        inline ssize_t  lock()      { return ++nLocks; }
                        inline ssize_t  unlock()    { return --nLocks; }
                };

            protected:
                Style              *pStyle;                     // Bound style
                prop::Listener     *pListener;                  // Nested client listener
                Listener            sListener;                  // Listener

            protected:
                void                sync(bool notify = true);   // Save property to style
                virtual void        push();                     // Push implementation-specific data to style
                virtual void        commit(atom_t property);    // Commit changes from style

            protected:
                explicit Property(prop::Listener *listener = NULL);

            public:
                Property(const Property &) = delete;
                Property(Property &&) = delete;
                virtual ~Property();

                Property & operator = (const Property &) = delete;
                Property & operator = (Property &&) = delete;

            protected:
                static const prop::enum_t *find_enum(const LSPString *s, const prop::enum_t *xenum);
                static const prop::enum_t *find_enum(const char *s, const prop::enum_t *xenum);
                static const prop::enum_t *find_enum(ssize_t value, const prop::enum_t *xenum);

                static size_t       parse_bools(bool *dst, size_t max, const LSPString *s);
                static size_t       parse_ints(ssize_t *dst, size_t max, const LSPString *s);
                static size_t       parse_floats(float *dst, size_t max, const LSPString *s);
                static size_t       parse_enums(ssize_t *dst, size_t max, const LSPString *s, const prop::enum_t *xenum);
                static ssize_t      parse_bit_enums(size_t *dst, const LSPString *s, const prop::enum_t *xenum);
                static size_t       parse_unique_enums(ssize_t *dst, size_t max, const LSPString *s, const prop::enum_t *xenum);

                static size_t       parse_enums(lltl::darray<ssize_t> *dst, const LSPString *s, const prop::enum_t *xenum);
                static size_t       parse_unique_enums(lltl::darray<ssize_t> *dst, const LSPString *s, const prop::enum_t *xenum);

                static bool         fmt_bit_enums(LSPString *dst, const prop::enum_t *xe, size_t v);

                static float        limit(float value, float min, float max);
                static float        normalized(float value, float min, float max);
                static bool         matches(float value, float min, float max);

                constexpr inline bool one_of() const { return false; }

            public:
                /**
                 * Override property for the style
                 */
                virtual void        override();                 // Override property

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

                /**
                 * Check that property matches one of the
                 * @param prop reference to property to check
                 * @return true if property matches one of the passed properties in the list
                 */
                template <class... T>
                inline bool         one_of(const Property &prop, T && ... args) const
                {
                    if (is(prop))
                        return true;
                    return one_of(lsp::forward<T>(args)...);
                }
        };

    } /* namespace tk */
} /* namespace lsp */


#endif /* LSP_PLUG_IN_TK_PROP_BASE_PROPERTY_H_ */
