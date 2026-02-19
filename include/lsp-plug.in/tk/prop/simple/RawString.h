/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 19 февр. 2026 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_SIMPLE_RAWSTRING_H_
#define LSP_PLUG_IN_TK_PROP_SIMPLE_RAWSTRING_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Raw string value property interface
         */
        class RawString: public SimpleProperty
        {
            protected:
                LSPString           sValue;

            protected:
                virtual void        commit(atom_t property) override;
                virtual void        push() override;

            protected:
                explicit RawString(prop::Listener *listener = NULL);
                RawString(const RawString &) = delete;
                RawString(RawString &&) = delete;
                virtual ~RawString() override;

                RawString & operator = (const RawString &) = delete;
                RawString & operator = (RawString &&) = delete;

            public:
                /**
                 * Get value of the string property
                 * @return value of the string property
                 */
                inline const LSPString &get() const         { return sValue;                }

                /**
                 * Set value of the float property
                 * @param v value of the float property
                 * @return true on success
                 */
                bool                set(const LSPString & value);
                bool                set(const LSPString *value);
                bool                set(const char *value, const char *charset = NULL);

                /**
                 * Swap contents
                 * @param dst destination property to perform swap
                 */
                void                swap(RawString *dst);
                inline void         swap(RawString &dst)    { swap(&dst);   }

                /**
                 * Swap contents
                 * @param dst destination property to perform swap
                 */
                void                swap(LSPString *dst);
                inline void         swap(LSPString &dst)    { swap(&dst);   }
        };

        namespace prop
        {
            /**
             * Raw string value property implementation
             */
            class RawString: public tk::RawString
            {
                public:
                    explicit RawString(prop::Listener *listener = NULL): tk::RawString(listener) {};
                    RawString(const RawString &) = delete;
                    RawString(RawString &&) = delete;

                    RawString & operator = (const RawString &) = delete;
                    RawString & operator = (RawString &&) = delete;

                public:
                    bool                commit_value(const LSPString & value);
                    bool                commit_value(const LSPString *value);
                    bool                commit_value(const char *value, const char *charset = NULL);

                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return SimpleProperty::bind(property, style, PT_STRING, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return SimpleProperty::bind(property, style, PT_STRING, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return SimpleProperty::bind(property, style, PT_STRING, &sListener); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return SimpleProperty::unbind(&sListener); };

                    inline void         listener(prop::Listener *listener)              { pListener = listener;                     }
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */




#endif /* LSP_PLUG_IN_TK_PROP_SIMPLE_RAWSTRING_H_ */
